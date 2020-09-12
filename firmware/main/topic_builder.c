#include "topic_builder.h"

#include <esp_log.h>
#include <string.h>

#include "mqtt_client.h"
#include "nvs_config.h"

static const char *TAG = "SL5G_TOPIC_BUILDER";

static const char *CMND_TOPIC_PREFIX_ID = "cmnd/sl5g/id";
static const char *CMND_TOPIC_PREFIX_ROOM = "cmnd/sl5g/room";
static const char *STAT_TOPIC_PREFIX = "stat/sl5g/id";

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

static const char* const CMND_MAP[CMND_END] = {
	"ENABLE",
	"WARM",
	"COLD",
	"BRIGHTNESS_AUTO",
	"BRIGHTNESS",
	"TEMPERATURE",
};

char *stat_map[STAT_END];

char* stat_topic_lookup(stat_event_t event)
{
	return stat_map[event];
}

cmnd_event_t cmnd_topic_lookup(char *topic)
{
	char *suffix = strrchr(topic, '/');
	if (suffix == NULL) {
		// No '/' anywhere -> We have a malformed topic
		return -1;
	}
	suffix += 1;

	for (int i = 0; i < CMND_END; i++)
	{
		if(!strcmp(suffix, CMND_MAP[i])) {
			return (cmnd_event_t)i;
		}
	}
	return -1;
}

void subscribe_to_initial_topics()
{
	nvs_config_t *config = nvs_config_get();
	char *topic = NULL;

	for (int i = 0; i < CMND_END; i++) {
		asprintf(&topic, "%s/%s/%s", CMND_TOPIC_PREFIX_ID, config->device_id, CMND_MAP[i]);
		mqtt_client_subscribe(topic);
		asprintf(&topic, "%s/%s/%s", CMND_TOPIC_PREFIX_ROOM, config->device_id, CMND_MAP[i]);
		mqtt_client_subscribe(topic);
	}

	free(topic);
	nvs_config_free(config);
}

void rebuild_stat_topics()
{
	for (int i = 0; i < STAT_END; i++) {
		free(stat_map[i]);
		stat_map[i] = NULL;
	}

	nvs_config_t *config = nvs_config_get();
	for (int i = 0; i < STAT_END; i++) {
		asprintf(&stat_map[i], "%s/%s/%s", STAT_TOPIC_PREFIX, config->device_id, STAT_TOPIC_SUFFIXES[i]);
	}

	nvs_config_free(config);
}
