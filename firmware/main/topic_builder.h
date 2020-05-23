#ifndef SL5G_TOPIC_BUILDER_H
#define SL5G_TOPIC_BUILDER_H

typedef enum {
	CMND_ENABLE,
	CMND_WARM,
	CMND_COLD,
	CMND_BRIGHTNESS_AUTO,
	CMND_BRIGHTNESS,
	CMND_TEMPERATURE,

	CMND_END,
} cmnd_event_t;

typedef enum {
	STAT_ENABLE,
	STAT_WARM,
	STAT_COLD,
	STAT_BRIGHTNESS_AUTO,
	STAT_BRIGHTNESS,
	STAT_TEMPERATURE,
	STAT_IP,

	STAT_END,
} stat_event_t;

char* stat_topic_lookup(stat_event_t event);

cmnd_event_t cmnd_topic_lookup(char *topic);

void topic_builder_init();

void subscribe_to_initial_topics();

#endif
