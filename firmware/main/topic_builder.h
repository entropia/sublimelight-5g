#ifndef SL5G_TOPIC_BUILDER_H
#define SL5G_TOPIC_BUILDER_H

typedef enum {
	CMND_ENABLE,
	CMND_WARM,
	CMND_COLD,
	CMND_BRIGHNTESS_AUTO,
	CMND_BRIGHTNESS,
	CMND_TEMPERATURE,
} cmnd_event_t;

typedef enum {
	STAT_ENABLE,
	STAT_WARM,
	STAT_COLD,
	STAT_BRIGHNTESS_AUTO,
	STAT_BRIGHTNESS,
	STAT_TEMPERATURE,
	STAT_IP,
} stat_event_t;

char* stat_topic_lookup(stat_event_t event);

void topic_builder_init();

#endif
