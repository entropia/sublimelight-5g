#include "mqtt_client.h"

#include <esp_log.h>
#include <mqtt_client.h>

#include "led.h"
#include "nvs_config.h"
#include "light_manager.h"
#include "topic_builder.h"

static const char *TAG = "SL5G_MQTT_CLIENT";

static esp_mqtt_client_handle_t client = NULL;
static char *topic_warm = NULL;
static char *topic_cold = NULL;
static char *state_topic_warm = NULL;
static char *state_topic_cold = NULL;

static char *topic_ip = NULL;
static char *current_ip = NULL;

static void subscribe_to_topics(esp_mqtt_client_handle_t client)
{
	int msg_id = esp_mqtt_client_subscribe(client, topic_warm, 1);
	if (msg_id == -1) {
		ESP_LOGW(TAG, "Could not subscribe to warm-white topic");
	}
	msg_id = esp_mqtt_client_subscribe(client, topic_cold, 1);
	if (msg_id == -1) {
		ESP_LOGW(TAG, "Could not subscribe to cold-white topic");
	}
	ESP_LOGI(TAG, "Topic subscription requests sent");
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
	char *value;
	char *topic;
	switch (event_id)
	{
		case LIGHT_MANAGER_EVENT_WARM_CHANGED:
			asprintf(&value, "%d", state->warm_value);
			topic = stat_topic_lookup(STAT_WARM);
			ESP_LOGI(TAG, "Warm value update publish scheduled. Value is %s", value);
			break;
		case LIGHT_MANAGER_EVENT_COLD_CHANGED:
			asprintf(&value, "%d", state->cold_value);
			topic = stat_topic_lookup(STAT_COLD);
			ESP_LOGI(TAG, "Cold value update publish scheduled. Value is %s", value);
			break;
		default:
			asprintf(&value, "shutupcompiler");
			asprintf(&topic, "shutupcompiler");
	}
	// TODO: Error handling
	esp_mqtt_client_publish(client, topic, value, 0, 1, true);
}

static void on_mqtt_connected(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
	esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t) event_data;
	subscribe_to_topics(event->client);
	publish_current_ip(event->client);
}

static void on_mqtt_received(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
	esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t) event_data;

	ESP_LOGI(TAG, "MQTT packet received. Topic: %.*s, Data: %.*s",
		 event->topic_len, event->topic, event->data_len, event->data);

	// We canot rely on event->data being NULL-terminated, so copy it into another buffer
	char *data = malloc(event->data_len + 1);
	if (!data) {
		ESP_LOGI(TAG, "MQTT packet ignored as data is too large.");
		return;
	}
	memcpy(data, event->data, event->data_len);
	data[event->data_len] = '\0';

	unsigned int intensity;
	sscanf(data, "%u", &intensity);

	if (strncmp(event->topic, topic_warm, strlen(topic_warm)) == 0) {
// 		led_set(WARM_WHITE, intensity);
		set_warm(intensity);
	} else if (strncmp(event->topic, topic_cold, strlen(topic_cold)) == 0) {
// 		led_set(COLD_WHITE, intensity);
		set_cold(intensity);
	} else {
		ESP_LOGI(TAG, "Not handled");
	}

	free(data);
}

static void on_light_manager_state_changed(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
	esp_mqtt_client_handle_t *client = (esp_mqtt_client_handle_t*) handler_args;
	light_manager_state_t *state = (light_manager_state_t*) event_data;
	publish_current_state(*client, state, event_id);
}

static esp_mqtt_client_handle_t start_mqtt_client()
{
	nvs_config_t *config = nvs_config_get();

	free(topic_warm);
	topic_warm = NULL;

	free(topic_cold);
	topic_cold = NULL;

	free(state_topic_warm);
	state_topic_warm = NULL;

	free(state_topic_cold);
	state_topic_cold = NULL;

	free(topic_ip);
	topic_cold = NULL;

	asprintf(&topic_warm, "cmnd/%s/WARM", config->device_name);
	asprintf(&topic_cold, "cmnd/%s/COLD", config->device_name);
	asprintf(&state_topic_warm, "stat/%s/WARM", config->device_name);
	asprintf(&state_topic_cold, "stat/%s/COLD", config->device_name);
	asprintf(&topic_ip,   "stat/%s/IP",   config->device_name);

	esp_mqtt_client_config_t client_config = {
		// No need to the URL, the MQTT client internally does a strdup
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
