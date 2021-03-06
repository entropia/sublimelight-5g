#include "nvs_config.h"

#include <string.h>

#include <esp_log.h>
#include <nvs_flash.h>

static const char *TAG = "sl5g_nvs_config";

static const char *NVS_NAMESPACE = "sl5g";
// For backward compatibility
static const char *NVS_KEY_DEVICE_ID = "device_name";
static const char *NVS_KEY_ROOM_NAME = "room_name";
static const char *NVS_KEY_MQTT_BROKER_URI = "mqtt_uri";

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

	new_config->device_id = strdup(CONFIG_SL5G_DEFAULT_DEVICE_ID);
	new_config->room_name = strdup("");
	new_config->mqtt_broker_uri = strdup(CONFIG_SL5G_DEFAULT_MQTT_BROKER_URI);

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
	char *device_id = get_str_alloc(nvsh, NVS_KEY_DEVICE_ID);
	if (!device_id) {
		return nvs_new_empty_config();
	}

	char *room_name = get_str_alloc(nvsh, NVS_KEY_ROOM_NAME);
	if (!room_name) {
		free(device_id);
		return nvs_new_empty_config();
	}

	char *mqtt_broker_uri = get_str_alloc(nvsh, NVS_KEY_MQTT_BROKER_URI);
	if (!mqtt_broker_uri) {
		free(device_id);
		free(room_name);
		return nvs_new_empty_config();
	}

	nvs_close(nvsh);

	nvs_config_t *result = malloc(sizeof(nvs_config_t));
	assert(result);
	*result = (nvs_config_t) {
		.device_id = device_id,
		.room_name = room_name,
		.mqtt_broker_uri = mqtt_broker_uri,
	};

	return result;
}

void nvs_config_update(nvs_config_t *new_config)
{
	nvs_handle_t nvsh;
	ESP_ERROR_CHECK(nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvsh));

	ESP_ERROR_CHECK(nvs_set_str(nvsh, NVS_KEY_DEVICE_ID, new_config->device_id));
	ESP_ERROR_CHECK(nvs_set_str(nvsh, NVS_KEY_ROOM_NAME, new_config->room_name));
	ESP_ERROR_CHECK(nvs_set_str(nvsh, NVS_KEY_MQTT_BROKER_URI, new_config->mqtt_broker_uri));

	ESP_ERROR_CHECK(nvs_commit(nvsh));
	nvs_close(nvsh);

	esp_event_post(NVS_CONFIG_EVENT, NVS_CONFIG_EVENT_CHANGED, new_config, sizeof(nvs_config_t), portMAX_DELAY);
}

void nvs_config_free(nvs_config_t *config)
{
	free(config->device_id);
	free(config->room_name);
	free(config->mqtt_broker_uri);
	free(config);
}
