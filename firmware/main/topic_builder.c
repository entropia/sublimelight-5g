#include "topic_builder.h"

#include <esp_log.h>
#include <string.h>

#include "mqtt_client.h"
#include "nvs_config.h"

static const char *TAG = "SL5G_TOPIC_BUILDER";

static const char *CMND_TOPIC_PREFIX_ID = "cmnd/sl5g/id";
static const char *CMND_TOPIC_PREFIX_ROOM = "cmnd/sl5g/room";
static const char *STAT_TOPIC_PREFIX = "stat/sl5g/id";

static const char *CMND_TOPIC_SUFFIXES[CMND_END] =
{
		"ENABLE",
		"WARM",
		"COLD",
		"BRIGHTNESS_AUTO",
		"BRIGHTNESS",
		"TEMPERATURE"
};

static const char *STAT_TOPIC_SUFFIXES[STAT_END] =
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

#define CMND_MAP_ENTRIES (2 * CMND_END)

cmnd_map_entry_t cmnd_map[CMND_MAP_ENTRIES];
stat_map_entry_t stat_map[STAT_END];

char* stat_topic_lookup(stat_event_t event)
{
	return stat_map[event].topic;
}

cmnd_event_t cmnd_topic_lookup(char *topic)
{
	for (int i = 0; i < CMND_MAP_ENTRIES; i++)
	{
		if(!strcmp(topic, cmnd_map[i].topic)) {
			return cmnd_map[i].event;
		}
	}
	return -1;
}

void subscribe_to_initial_topics()
{
	for (int i = 0; i < CMND_MAP_ENTRIES; i++) {
		mqtt_client_subscribe(cmnd_map[i].topic);
	}
}

void rebuild_initial_topics()
{
	for (int i = 0; i < STAT_END; i++) {
		free(stat_map[i].topic);
		stat_map[i].topic = NULL;
	}

	for (int i = 0; i < CMND_MAP_ENTRIES; i++) {
		free(cmnd_map[i].topic);
		cmnd_map[i].topic = NULL;
	}

	nvs_config_t *config = nvs_config_get();
	for (int i = 0; i < STAT_END; i++) {
		//stat_map_entry_t *entry = malloc(sizeof(stat_map_entry_t));
		//stat_map[i] = *entry;
		//entry->event = i;
		stat_map[i].event = i; // Why does this work?
		asprintf(&stat_map[i].topic, "%s/%s/%s", STAT_TOPIC_PREFIX, config->device_id, STAT_TOPIC_SUFFIXES[i]);
	}

	int i = 0;
	for (int cmnd = 0; cmnd < CMND_END; cmnd++, i++) {
		cmnd_map[i].event = cmnd;
		asprintf(&cmnd_map[i].topic, "%s/%s/%s", CMND_TOPIC_PREFIX_ID, config->device_id, CMND_TOPIC_SUFFIXES[cmnd]);
	}
	for (int cmnd = 0; cmnd < CMND_END; cmnd++, i++) {
		cmnd_map[i].event = cmnd;
		asprintf(&cmnd_map[i].topic, "%s/%s/%s", CMND_TOPIC_PREFIX_ROOM, config->room_name, CMND_TOPIC_SUFFIXES[cmnd]);
	}
	assert(i == CMND_MAP_ENTRIES);
}
