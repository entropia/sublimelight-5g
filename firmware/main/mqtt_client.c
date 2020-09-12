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

static void publish_current_ip()
{
	stat_event_t event = STAT_IP;
	char *stat_topic_ip = stat_topic_lookup(event);

	int msg_id = esp_mqtt_client_publish(client, stat_topic_ip, current_ip, 0, 1, true);
	if (msg_id == -1) {
		ESP_LOGW(TAG, "Could not publish to IP topic");
	} else {
		ESP_LOGI(TAG, "IP update published. Address is %s", current_ip);
	}
}

static void publish_current_state(light_manager_state_t *state, int32_t event_id)
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

	int msg_id = esp_mqtt_client_publish(client, topic, value, 0, 1, true);
	if (msg_id == -1) {
		ESP_LOGW(TAG, "Could not publish value update. Topic is %s. Value is %s", topic, value);
	} else {
		ESP_LOGI(TAG, "Value update published. Topic is %s. Value is %s", topic, value);
	}

	free(value);
}

static void on_mqtt_connected(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
	ESP_LOGI(TAG, "MQTT client connected, subscribing to initial topics");
	subscribe_to_initial_topics();
	publish_current_ip();
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
	light_manager_state_t *state = (light_manager_state_t*) event_data;
	publish_current_state(state, event_id);
}

static void start_mqtt_client()
{
	rebuild_stat_topics();

	nvs_config_t *config = nvs_config_get();

	esp_mqtt_client_config_t client_config = {
		// No need to copy the URL, the MQTT client internally does a strdup
		.uri = config->mqtt_broker_uri,
	};

	client = esp_mqtt_client_init(&client_config);
	assert(client);
	esp_mqtt_client_register_event(client, MQTT_EVENT_CONNECTED, on_mqtt_connected, NULL);
	esp_mqtt_client_register_event(client, MQTT_EVENT_DATA, on_mqtt_received, NULL);
	esp_mqtt_client_start(client);
	ESP_LOGI(TAG, "MQTT client started");

	nvs_config_free(config);
}

static void stop_mqtt_client()
{
	ESP_LOGI(TAG, "Shutting down MQTT client");
	esp_mqtt_client_stop(client);
	esp_mqtt_client_destroy(client);
	client = NULL;
	led_fallback();
}

static void on_dhcp_got_ip(void *arg, esp_event_base_t base, int32_t event_id, void *event_data)
{
	ip_event_got_ip_t *event = (ip_event_got_ip_t*) event_data;

	free(current_ip);
	current_ip = NULL;
	esp_ip4_addr_t ip = event->ip_info.ip;
	asprintf(&current_ip, IPSTR, IP2STR(&ip));

	if (client == NULL) {
		ESP_LOGI(TAG, "Starting MQTT client");
		start_mqtt_client();
		// The client will publish our IP after startup
	} else {
		ESP_LOGW(TAG, "Got new IP %s but MQTT client already running", current_ip);
		publish_current_ip();
	}
}

static void on_wifi_disconnect(void *arg, esp_event_base_t base, int32_t event_id, void *event_data)
{
	if (client != NULL) {
		ESP_LOGI(TAG, "Stopping MQTT client after Wifi disconnect");
		stop_mqtt_client(client);
	}
}

static void on_nvs_config_changed(void *arg, esp_event_base_t base, int32_t event_id, void *event_data)
{
	if (client != NULL) {
		ESP_LOGI(TAG, "Stopping MQTT client after NVS config change");
		stop_mqtt_client(client);
		ESP_LOGI(TAG, "Restarting MQTT client after NVS config change");
		start_mqtt_client(client);
	}
}

void mqtt_client_init(void)
{
	ESP_ERROR_CHECK(esp_event_handler_register(NVS_CONFIG_EVENT, NVS_CONFIG_EVENT_CHANGED, &on_nvs_config_changed, NULL));
	ESP_ERROR_CHECK(esp_event_handler_register(LIGHT_MANAGER_EVENT, ESP_EVENT_ANY_ID, &on_light_manager_state_changed, NULL));

	ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &on_dhcp_got_ip, NULL));
	ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &on_wifi_disconnect, NULL));
}
