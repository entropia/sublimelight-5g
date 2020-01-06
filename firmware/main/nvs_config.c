#include "nvs_config.h"

#include <string.h>

#include <esp_log.h>
#include <nvs_flash.h>

static const char *TAG = "sl5g_nvs_config";

static const char *NVS_NAMESPACE = "sl5g";
static const char *NVS_KEY_BOARD_NAME = "board_name";
static const char *NVS_KEY_MQTT_BROKER_URI = "mqtt_uri";
static const char *NVS_KEY_MQTT_TOPIC_PREFIX = "mqtt_topic";

ESP_EVENT_DEFINE_BASE(NVS_CONFIG_EVENT);

void nvs_config_init(void)
{
	esp_err_t err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_LOGW(TAG, "Could not initialize NVS, resetting configuration.");
		ESP_ERROR_CHECK(nvs_flash_erase());
		err = nvs_flash_init();
	}
	ESP_ERROR_CHECK(err);
}

static nvs_config_t *nvs_new_empty_config(void)
{
	nvs_config_t *new_config = malloc(sizeof(nvs_config_t));
	assert(new_config);

	new_config->board_name = strdup(CONFIG_SL5G_DEFAULT_BOARD_NAME);
	new_config->mqtt_broker_uri = strdup(CONFIG_SL5G_DEFAULT_MQTT_BROKER_URI);
	new_config->mqtt_topic_prefix = strdup(CONFIG_SL5G_DEFAULT_MQTT_TOPIC_PREFIX);

	return new_config;
}

static char *get_str_alloc(nvs_handle_t h, const char *key)
{
	size_t required_size = SIZE_MAX;
	esp_err_t ret = nvs_get_str(h, key, NULL, &required_size);
	if (ret == ESP_ERR_NVS_NOT_FOUND) {
		return NULL;
	}
	ESP_ERROR_CHECK(ret);
	char *result = malloc(required_size);
	assert(result);
	ESP_ERROR_CHECK(nvs_get_str(h, key, result, &required_size));
	return result;
}

nvs_config_t *nvs_config_get(void)
{
	nvs_handle_t nvsh;
	esp_err_t ret = nvs_open(NVS_NAMESPACE, NVS_READONLY, &nvsh);

	if (ret == ESP_ERR_NVS_NOT_FOUND) {
		return nvs_new_empty_config();
	}
	ESP_ERROR_CHECK(ret);

	// TODO: We may want to cache this
	char *board_name = get_str_alloc(nvsh, NVS_KEY_BOARD_NAME);
	if (!board_name) {
		return nvs_new_empty_config();
	}

	char *mqtt_broker_uri = get_str_alloc(nvsh, NVS_KEY_MQTT_BROKER_URI);
	if (!mqtt_broker_uri) {
		free(board_name);
		return nvs_new_empty_config();
	}

	char *mqtt_topic_prefix = get_str_alloc(nvsh, NVS_KEY_MQTT_TOPIC_PREFIX);
	if (!mqtt_topic_prefix) {
		free(mqtt_broker_uri);
		free(board_name);
		return nvs_new_empty_config();
	}

	nvs_close(nvsh);

	nvs_config_t *result = malloc(sizeof(nvs_config_t));
	assert(result);
	*result = (nvs_config_t) {
		.board_name = board_name,
		.mqtt_broker_uri = mqtt_broker_uri,
		.mqtt_topic_prefix = mqtt_topic_prefix,
	};

	return result;
}

void nvs_config_update(nvs_config_t *new_config)
{
	nvs_handle_t nvsh;
	ESP_ERROR_CHECK(nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvsh));

	ESP_ERROR_CHECK(nvs_set_str(nvsh, NVS_KEY_BOARD_NAME, new_config->board_name));
	ESP_ERROR_CHECK(nvs_set_str(nvsh, NVS_KEY_MQTT_BROKER_URI, new_config->mqtt_broker_uri));
	ESP_ERROR_CHECK(nvs_set_str(nvsh, NVS_KEY_MQTT_TOPIC_PREFIX, new_config->mqtt_topic_prefix));

	ESP_ERROR_CHECK(nvs_commit(nvsh));
	nvs_close(nvsh);

	esp_event_post(NVS_CONFIG_EVENT, NVS_CONFIG_EVENT_CHANGED, new_config, sizeof(nvs_config_t), portMAX_DELAY);
}

void nvs_config_free(nvs_config_t *config)
{
	free(config->board_name);
	free(config->mqtt_broker_uri);
	free(config->mqtt_topic_prefix);
	free(config);
}
