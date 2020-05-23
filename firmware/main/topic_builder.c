#include "topic_builder.h"

#include <esp_log.h>
#include <mqtt_client.h>

#include "nvs_config.h"

static const char *TAG = "SL5G_TOPIC_BUILDER";

static const char *CMND_TOPIC_PREFIX = "cmnd/sl5g";
static const char *STAT_TOPIC_PREFIX = "stat/sl5g/id";

static const char *CMND_TOPIC_SUFFIXES[6] =
{
		"ENABLE",
		"WARM",
		"COLD",
		"BRIGHTNESS_AUTO",
		"BRIGHTNESS",
		"TEMPERATURE"
};

static const char *STAT_TOPIC_SUFFIXES[7] =
{
		"ENABLE",
		"WARM",
		"COLD",
		"BRIGHTNESS_AUTO",
		"BRIGHTNESS",
		"TEMPERATURE",
		"IP"
};

typedef struct
{
	cmnd_event_t event;
	char *topic;
} cmnd_map_entry_t;

typedef struct
{
	stat_event_t event;
	char *topic;
} stat_map_entry_t;

cmnd_map_entry_t cmnd_map[6];
stat_map_entry_t stat_map[7];

char* stat_topic_lookup(stat_event_t event)
{
	return stat_map[event].topic;
}

cmnd_event_t cmnd_topic_lookup(char *topic)
{
	for (int i = 0; i < 6; i++)
	{
		if(!strcmp(topic, cmnd_map[i].topic)) {
			return cmnd_map[i].event;
		}
	}
	return -1;
}

static void build_topics()
{
	nvs_config_t *config = nvs_config_get();
	for (int i = 0; i < 7; i++)
	{
		//stat_map_entry_t *entry = malloc(sizeof(stat_map_entry_t));
		//stat_map[i] = *entry;
		//entry->event = i;
		stat_map[i].event = i; // Why does this work?
		asprintf(&stat_map[i].topic, "%s/%s/%s", STAT_TOPIC_PREFIX, config->device_id, STAT_TOPIC_SUFFIXES[i]);
	}

	for (int i = 0; i < 6; i++)
	{
		cmnd_map[i].event = i;
		asprintf(&cmnd_map[i].topic, "%s", CMND_TOPIC_SUFFIXES[i]);
	}
}

static void on_nvs_config_changed(void *arg, esp_event_base_t base, int32_t event_id, void *event_data)
{
	(void) arg;

	// TODO: free()
	build_topics();
}

void topic_builder_init()
{
	ESP_ERROR_CHECK(esp_event_handler_register(NVS_CONFIG_EVENT, NVS_CONFIG_EVENT_CHANGED, &on_nvs_config_changed, NULL));
	build_topics();
}
