#include "topic_builder.h"

#include <esp_log.h>
#include <string.h>

#include "mqtt_client.h"
#include "nvs_config.h"

static const char *TAG = "SL5G_TOPIC_BUILDER";

static const char *CMND_TOPIC_PREFIX_ID = "cmnd/sl5g/id";
static const char *CMND_TOPIC_PREFIX_ROOM = "cmnd/sl5g/room";
static const char *CMND_TOPIC_PREFIX_GROUP = "cmnd/sl5g/group";
static const char *STAT_TOPIC_PREFIX = "stat/sl5g/id";

static const char *STAT_TOPIC_SUFFIXES[STAT_END] =
{
	"ENABLE",
	"WARM",
	"COLD",
	"BRIGHTNESS_AUTO",
	"BRIGHTNESS",
	"TEMPERATURE",
	"IP",
	"GROUPS",
};

static const char* const CMND_MAP[CMND_END] = {
	"ENABLE",
	"WARM",
	"COLD",
	"BRIGHTNESS_AUTO",
	"BRIGHTNESS",
	"TEMPERATURE",
	"GROUPS",
};

char *stat_map[STAT_END];

// current_groups points to a buffer containing the names of our
// current groups separated by '\0' (so we can easily pass around the
// individual names). In order to know when we have reached the last
// group name, current_groups_end points at end of the buffer.
char *current_groups = NULL;
char *current_groups_end = NULL;

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

		// CMND_GROUPS is only valid for a single board, not for a whole room.
		if (i != CMND_GROUPS) {
			asprintf(&topic, "%s/%s/%s", CMND_TOPIC_PREFIX_ROOM, config->room_name, CMND_MAP[i]);
			mqtt_client_subscribe(topic);
		}
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

static void foreach_current_group_topic(void (*action)(char *))
{
	char *group = current_groups;
	char *topic = NULL;

	while (group < current_groups_end) {
		// Skip over empty elements in the groups list.
		// Also catches the empty list and trailing separators.
		if (strlen(group) > 0) {
			for (int i = 0; i < CMND_END; i++) {
				if (i != CMND_GROUPS) {
					asprintf(&topic, "%s/%s/%s", CMND_TOPIC_PREFIX_GROUP, group, CMND_MAP[i]);
					action(topic);
				}
			}
		}
		group += strlen(group) + 1;
	}

	free(topic);
}

void set_group_membership(char *group_list)
{
	// For now, we handle membership by unsubscribing from all our
	// old groups and then subscribing to all new groups. We will
	// still not miss any updates, because they are usually
	// published as retained.

	// TODO: On the other hand, retained publishing may not be
	// what we want, because we will be flooded with (probably
	// stale) updates from all groups we subscribe to.

	// TODO: We do not filter out duplicate groups, as this also
	// requires more infrastructure in terms of counting which
	// groups we have already seen in the list. However, the
	// ensuing duplicate subscribe/unsubscribe calls do not seem
	// to cause problems.

	if (current_groups != NULL) {
		foreach_current_group_topic(mqtt_client_unsubscribe);
		free(current_groups);
	}

	current_groups = group_list;
	current_groups_end = current_groups + strlen(current_groups);

	// We are about to chop the group list to bits. Even though it
	// is cheating (we are not actually in the new groups just
	// yet), we publish our new group status *before* we do that
	// so we do not have to keep another copy of the list around.
	mqtt_publish_group_state(current_groups);

	for (char *p = current_groups; *p != '\0'; p++) {
		if (*p == ',') {
			*p = '\0';
		}
	}

	foreach_current_group_topic(mqtt_client_subscribe);
}
