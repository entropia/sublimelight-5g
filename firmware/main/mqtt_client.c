#include "mqtt_client.h"

#include <esp_log.h>
#include <mqtt_client.h>

#include "led.h"
#include "nvs_config.h"

static const char *TAG = "SL5G_MQTT_CLIENT";

static esp_mqtt_client_handle_t client = NULL;
static char *topic_warm = NULL;
static char *topic_cold = NULL;

static void unsubscribe_from_topics(esp_mqtt_client_handle_t client)
{
	int msg_id = esp_mqtt_client_unsubscribe(client, topic_warm);
	if (msg_id == -1) {
		ESP_LOGW(TAG, "Could not unsubscribe from warm-white topic");
	}
	msg_id = esp_mqtt_client_unsubscribe(client, topic_cold);
	if (msg_id == -1) {
		ESP_LOGW(TAG, "Could not unsubscribe from cold-white topic");
	}
	ESP_LOGI(TAG, "Topic unsubscription requests sent");
}

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

static void on_mqtt_connected(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
	esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t) event_data;
	subscribe_to_topics(event->client);
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
		led_set(WARM_WHITE, intensity);
	} else if (strncmp(event->topic, topic_cold, strlen(topic_cold)) == 0) {
		led_set(COLD_WHITE, intensity);
	} else {
		ESP_LOGI(TAG, "Not handled");
	}

	free(data);
}

static esp_mqtt_client_handle_t start_mqtt_client()
{
	esp_mqtt_client_config_t config = {
		.uri = CONFIG_SL5G_DEFAULT_MQTT_BROKER_URI,
	};

	esp_mqtt_client_handle_t client = esp_mqtt_client_init(&config);
	assert(client);
	esp_mqtt_client_register_event(client, MQTT_EVENT_CONNECTED, on_mqtt_connected, client);
	esp_mqtt_client_register_event(client, MQTT_EVENT_DATA, on_mqtt_received, client);
	esp_mqtt_client_start(client);

	return client;
}

static void stop_mqtt_client(esp_mqtt_client_handle_t client)
{
	ESP_LOGI(TAG, "Shutting down MQTT client");
	led_fallback();
}

static void on_dhcp_got_ip(void *arg, esp_event_base_t base, int32_t event_id, void *event_data)
{
	esp_mqtt_client_handle_t *client = (esp_mqtt_client_handle_t*) arg;

	if (*client == NULL) {
		ESP_LOGI(TAG, "Starting MQTT client");
		*client = start_mqtt_client();
	} else {
		ESP_LOGW(TAG, "Got new IP but MQTT client already running");
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

static void load_topics_from_nvs_config(void)
{
	free(topic_warm);
	topic_warm = NULL;

	free(topic_cold);
	topic_cold = NULL;

	free(topic_ip);
	topic_cold = NULL;

	nvs_config_t *config = nvs_config_get();
	asprintf(&topic_warm, "cmnd/%s/WARM", config->device_name);
	asprintf(&topic_cold, "cmnd/%s/COLD", config->device_name);
	nvs_config_free(config);
}

static void on_nvs_config_changed(void *arg, esp_event_base_t base, int32_t event_id, void *event_data)
{
	esp_mqtt_client_handle_t *client = (esp_mqtt_client_handle_t*) arg;

	if (*client != NULL) {
		unsubscribe_from_topics(*client);
	}
	load_topics_from_nvs_config();
	if (*client != NULL) {
		subscribe_to_topics(*client);
	}
}

void mqtt_client_init(void)
{
	load_topics_from_nvs_config();
	ESP_ERROR_CHECK(esp_event_handler_register(NVS_CONFIG_EVENT, NVS_CONFIG_EVENT_CHANGED, &on_nvs_config_changed, &client));

	ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &on_dhcp_got_ip, &client));
	ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &on_wifi_disconnect, &client));
}
