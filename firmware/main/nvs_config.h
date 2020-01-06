#ifndef SL5G_NVS_CONFIG_H
#define SL5G_NVS_CONFIG_H

#include <esp_event.h>

typedef struct {
	char *board_name;
	char *mqtt_broker_uri;
	char *mqtt_topic_prefix;
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
