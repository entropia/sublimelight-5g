#include "mqtt_client.h"

#include <esp_log.h>
#include <mqtt_client.h>

#include "led.h"
#include "nvs_config.h"
#include "light_manager.h"
#include "topic_builder.h"

static const char *TAG = "SL5G_MQTT_CLIENT";

static esp_mqtt_client_handle_t client = NULL;

static char *current_ip = NULL;

bool mqtt_client_subscribe(char *topic)
{
	ESP_LOGI(TAG, "Subscribing to %s", topic);
	int msg_id = esp_mqtt_client_subscribe(client, topic, 1);
	if (msg_id == -1) {
		ESP_LOGW(TAG, "Could not subscribe to topic %s", topic);
	}

	return (msg_id != -1);
}

bool mqtt_client_unsubscribe(char *topic)
{
	ESP_LOGI(TAG, "Unsubscribing from %s", topic);
	int msg_id = esp_mqtt_client_unsubscribe(client, topic);
	if (msg_id == -1) {
		ESP_LOGW(TAG, "Could not unsubscribe from topic %s", topic);
	}

	return (msg_id != -1);
}

static void publish_current_ip(esp_mqtt_client_handle_t client)
{
	stat_event_t event = STAT_IP;
	char *stat_topic_ip = stat_topic_lookup(event);

	int msg_id = esp_mqtt_client_publish(client, stat_topic_ip, current_ip, 0, 1, true);
	if (msg_id == -1) {
		ESP_LOGW(TAG, "Could not publish to IP topic");
	}
	ESP_LOGI(TAG, "IP update published. Address is %s", current_ip);
}

static void publish_current_state(esp_mqtt_client_handle_t client, light_manager_state_t *state, int32_t event_id)
{
	char *value = NULL;
	char *topic = NULL;
	switch (event_id) {
	case LIGHT_MANAGER_EVENT_ENABLED_CHANGED:
		if (state->enabled) {
			value = strdup("ON");
		} else {
			value = strdup("OFF");
		}
		topic = stat_topic_lookup(STAT_ENABLE);
		break;
	case LIGHT_MANAGER_EVENT_WARM_CHANGED:
		asprintf(&value, "%d", state->warm_value);
		topic = stat_topic_lookup(STAT_WARM);
		break;
	case LIGHT_MANAGER_EVENT_COLD_CHANGED:
		asprintf(&value, "%d", state->cold_value);
		topic = stat_topic_lookup(STAT_COLD);
		break;
	case LIGHT_MANAGER_EVENT_BRIGHTNESS_CHANGED:
		asprintf(&value, "%d", state->brightness);
		topic = stat_topic_lookup(STAT_BRIGHTNESS);
		break;
	case LIGHT_MANAGER_EVENT_TEMPERATURE_CHANGED:
		asprintf(&value, "%f", state->temperature);
		topic = stat_topic_lookup(STAT_TEMPERATURE);
		break;
	default:
		ESP_LOGE(TAG, "Ignoring unknown state update from light manager: %d", event_id);
		return;
	}
	// TODO: Error handling
	esp_mqtt_client_publish(client, topic, value, 0, 1, true);
	ESP_LOGI(TAG, "Value update published. Topic is %s. Value is %s", topic, value);

	free(value);
}

static void on_mqtt_connected(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
	esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t) event_data;
	subscribe_to_initial_topics();
	publish_current_ip(event->client);
}

static void on_mqtt_received(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
	esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t) event_data;

	ESP_LOGI(TAG, "MQTT packet received. Topic: %.*s, Data: %.*s",
		 event->topic_len, event->topic, event->data_len, event->data);

	// We cannot rely on event->data and event->topic being NULL-terminated, so copy them into another buffer
	char *data  = strndup(event->data, event->data_len);
	if (!data) {
		ESP_LOGI(TAG, "MQTT packet ignored as data is too large.");
		return;
	}

	char *topic = strndup(event->topic, event->topic_len);
	if (!topic) {
		ESP_LOGI(TAG, "MQTT packet ignored as topic is too large.");
		free(data);
		return;
	}

	cmnd_event_t cmnd_event = cmnd_topic_lookup(topic);

	// Special case for temperature as it carries a double instead of unsigned payload
	if (cmnd_event == CMND_TEMPERATURE) {
		double temperature;
		bool temperature_valid = (sscanf(data, "%lf", &temperature) == 1);

		if (!temperature_valid) {
			ESP_LOGW(TAG, "Received malformed MQTT (temperature): Topic %s, data %s\n", topic, data);
			goto out;
		}

		set_temperature(temperature);
		goto out;
	}

	// Special case for enabled as it carries an "on"/"off" instead of unsigned payload
	if (cmnd_event == CMND_ENABLE) {
		bool enabled;
		if (strcasecmp(data, "on") == 0) {
			enabled = true;
		} else if (strcasecmp(data, "off") == 0) {
			enabled = false;
		} else {
			ESP_LOGW(TAG, "Received malformed MQTT (enabled): Topic %s, data %s\n", topic, data);
			goto out;
		}

		set_enabled(enabled);
		goto out;
	}

	unsigned int intensity;
	bool intensity_valid = (sscanf(data, "%u", &intensity) == 1);

	if (!intensity_valid) {
		ESP_LOGW(TAG, "Received malformed MQTT: Topic %s, data %s\n", topic, data);
		goto out;
	}

	switch (cmnd_event)
	{
		case CMND_WARM:
			set_warm(intensity);
			break;
		case CMND_COLD:
			set_cold(intensity);
			break;
		case CMND_BRIGHTNESS_AUTO:
			set_brightness_auto(intensity);
			break;
		case CMND_BRIGHTNESS:
			set_brightness(intensity);
			break;
		default:
			ESP_LOGI(TAG, "Not handled");
	}

out:
	free(data);
	free(topic);
}

static void on_light_manager_state_changed(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
	esp_mqtt_client_handle_t *client = (esp_mqtt_client_handle_t*) handler_args;
	light_manager_state_t *state = (light_manager_state_t*) event_data;
	publish_current_state(*client, state, event_id);
}

static esp_mqtt_client_handle_t start_mqtt_client()
{
	rebuild_initial_topics();

	nvs_config_t *config = nvs_config_get();

	esp_mqtt_client_config_t client_config = {
		// No need to copy the URL, the MQTT client internally does a strdup
		.uri = config->mqtt_broker_uri,
	};

	esp_mqtt_client_handle_t client = esp_mqtt_client_init(&client_config);
	assert(client);
	esp_mqtt_client_register_event(client, MQTT_EVENT_CONNECTED, on_mqtt_connected, client);
	esp_mqtt_client_register_event(client, MQTT_EVENT_DATA, on_mqtt_received, client);
	esp_mqtt_client_start(client);

	nvs_config_free(config);

	return client;
}

static void stop_mqtt_client(esp_mqtt_client_handle_t client)
{
	ESP_LOGI(TAG, "Shutting down MQTT client");
	esp_mqtt_client_stop(client);
	esp_mqtt_client_destroy(client);
	led_fallback();
}

static void on_dhcp_got_ip(void *arg, esp_event_base_t base, int32_t event_id, void *event_data)
{
	esp_mqtt_client_handle_t *client = (esp_mqtt_client_handle_t*) arg;
	ip_event_got_ip_t *event = (ip_event_got_ip_t*) event_data;

	free(current_ip);
	current_ip = NULL;
	esp_ip4_addr_t ip = event->ip_info.ip;
	asprintf(&current_ip, IPSTR, IP2STR(&ip));

	if (*client == NULL) {
		ESP_LOGI(TAG, "Starting MQTT client");
		*client = start_mqtt_client();
		// The client will publish our IP after startup
	} else {
		ESP_LOGW(TAG, "Got new IP %s but MQTT client already running", current_ip);
		publish_current_ip(*client);
	}
}

static void on_wifi_disconnect(void *arg, esp_event_base_t base, int32_t event_id, void *event_data)
{
	esp_mqtt_client_handle_t *client = (esp_mqtt_client_handle_t*) arg;

	if (*client != NULL) {
		ESP_LOGI(TAG, "Stopping MQTT client after Wifi disconnect");
		stop_mqtt_client(*client);
	}
}

static void on_nvs_config_changed(void *arg, esp_event_base_t base, int32_t event_id, void *event_data)
{
	esp_mqtt_client_handle_t *client = (esp_mqtt_client_handle_t*) arg;

	if (*client != NULL) {
		stop_mqtt_client(*client);
		start_mqtt_client(*client);
	}
}

void mqtt_client_init(void)
{
	ESP_ERROR_CHECK(esp_event_handler_register(NVS_CONFIG_EVENT, NVS_CONFIG_EVENT_CHANGED, &on_nvs_config_changed, &client));
	ESP_ERROR_CHECK(esp_event_handler_register(LIGHT_MANAGER_EVENT, ESP_EVENT_ANY_ID, &on_light_manager_state_changed, &client));

	ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &on_dhcp_got_ip, &client));
	ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &on_wifi_disconnect, &client));
}
