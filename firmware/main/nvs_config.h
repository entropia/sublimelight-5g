#ifndef SL5G_NVS_CONFIG_H
#define SL5G_NVS_CONFIG_H

/*
 * MQTT namespace
 *
 * cmnd/$device/WARM 123
 * cmnd/$device/COLD 456
 *
 * stat/$device/...
 */

#include <esp_event.h>

typedef struct {
	char *device_name;
	char *mqtt_broker_uri;
} nvs_config_t;

ESP_EVENT_DECLARE_BASE(NVS_CONFIG_EVENT);

enum {
	NVS_CONFIG_EVENT_CHANGED,
};

void nvs_config_init(void);

nvs_config_t *nvs_config_get(void);

void nvs_config_free(nvs_config_t *config);

void nvs_config_update(nvs_config_t *new_config);

#endif
