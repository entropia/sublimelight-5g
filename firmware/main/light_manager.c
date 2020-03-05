#include "light_manager.h"

#include "led.h"

ESP_EVENT_DEFINE_BASE(LIGHT_MANAGER_EVENT);

light_manager_state_t *state;

void light_manager_init()
{
	light_manager_state_t *new_state = malloc(sizeof(light_manager_state_t));
	assert(new_state);
	new_state->warm_value = 0;
	new_state->cold_value = 0;
	state = new_state;
}

void set_warm(unsigned intensity)
{
	led_set(WARM_WHITE, intensity);
	state->warm_value = intensity;
	update_state();
}

void set_cold(unsigned intensity)
{
	led_set(COLD_WHITE, intensity);
	state->cold_value = intensity;
	update_state();
}

void update_state()
{
	esp_event_post(LIGHT_MANAGER_EVENT, LIGHT_MANAGER_EVENT_STATE_CHANGED, state, sizeof(light_manager_state_t), portMAX_DELAY);
}
