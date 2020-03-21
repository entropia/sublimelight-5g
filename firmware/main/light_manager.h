#ifndef SL5G_LIGHT_MANAGER_H
#define SL5G_LIGHT_MANAGER_H

#include <esp_event.h>

typedef struct {
	unsigned warm_value;
	unsigned cold_value;

	unsigned brightness;
	double temperature;
} light_manager_state_t;

ESP_EVENT_DECLARE_BASE(LIGHT_MANAGER_EVENT);

typedef enum {
	LIGHT_MANAGER_EVENT_STATE_CHANGED,
	LIGHT_MANAGER_EVENT_WARM_CHANGED,
	LIGHT_MANAGER_EVENT_COLD_CHANGED,
	LIGHT_MANAGER_EVENT_BRIGHTNESS_CHANGED,
	LIGHT_MANAGER_EVENT_TEMPERATURE_CHANGED,
} light_manager_event_t;

void light_manager_init();

void set_warm(unsigned intensity);

void set_cold(unsigned intensity);

void set_brightness(unsigned brightness);

void set_temperature(double temperature);

void set_brightness_auto(unsigned brightness);

#endif
