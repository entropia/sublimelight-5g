#ifndef SL5G_LIGHT_MANAGER_H
#define SL5G_LIGHT_MANAGER_H

#include <esp_event.h>

typedef struct {
	short int warm_value;
	short int cold_value;
} light_manager_state_t;

ESP_EVENT_DECLARE_BASE(LIGHT_MANAGER_EVENT);

typedef enum {
	LIGHT_MANAGER_EVENT_STATE_CHANGED,
	LIGHT_MANAGER_EVENT_WARM_CHANGED,
	LIGHT_MANAGER_EVENT_COLD_CHANGED,
} light_manager_event_t;

void light_manager_init();

void set_warm(unsigned intensity);

void set_cold(unsigned intensity);

void update_state();

#endif
