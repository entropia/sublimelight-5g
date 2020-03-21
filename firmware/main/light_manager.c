#include "light_manager.h"

#include "led.h"

ESP_EVENT_DEFINE_BASE(LIGHT_MANAGER_EVENT);

light_manager_state_t state;

void light_manager_init()
{
	state.warm_value = 0;
	state.cold_value = 0;
}

void set_warm(unsigned intensity)
{
	led_set(WARM_WHITE, intensity);
	state.warm_value = intensity;
	update_state(LIGHT_MANAGER_EVENT_WARM_CHANGED);
}

void set_cold(unsigned intensity)
{
	led_set(COLD_WHITE, intensity);
	state.cold_value = intensity;
	update_state(LIGHT_MANAGER_EVENT_COLD_CHANGED);
}

void update_state(light_manager_event_t event)
{
	esp_event_post(LIGHT_MANAGER_EVENT, event, &state, sizeof(light_manager_state_t), portMAX_DELAY);
}
