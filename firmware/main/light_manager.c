#include "light_manager.h"

#include <math.h>

#include "led.h"

ESP_EVENT_DEFINE_BASE(LIGHT_MANAGER_EVENT);

static const unsigned MAX_ONE_CHANNEL = 8191;
static const unsigned MAX_TWO_CHANNELS = 2 * MAX_ONE_CHANNEL;

light_manager_state_t state;

static void update_state(light_manager_event_t event)
{
	led_set(WARM_WHITE, state.warm_value);
	led_set(COLD_WHITE, state.cold_value);
	esp_event_post(LIGHT_MANAGER_EVENT, event, &state, sizeof(light_manager_state_t), portMAX_DELAY);
}

static void update_state_warm(unsigned intensity)
{
	state.warm_value = intensity;
	update_state(LIGHT_MANAGER_EVENT_WARM_CHANGED);
}

static void update_state_cold(unsigned intensity)
{
	state.cold_value = intensity;
	update_state(LIGHT_MANAGER_EVENT_COLD_CHANGED);
}

static void update_state_brightness(unsigned brightness)
{
	state.brightness = brightness;
	update_state(LIGHT_MANAGER_EVENT_BRIGHTNESS_CHANGED);
}

static void update_state_temperature(double temperature)
{
	state.temperature = temperature;
	update_state(LIGHT_MANAGER_EVENT_TEMPERATURE_CHANGED);
}

static void update_brightness_temperature_from_cold_warm()
{
	double w = (double) state.warm_value;
	double c = (double) state.cold_value;

	update_state_temperature(w / (w + c));
	update_state_brightness(w + c);
}

static unsigned clamp(unsigned x, unsigned  min, unsigned max)
{
	if (x < min) {
		return min;
	} else if (x > max) {
		return max;
	} else {
		return x;
	}
}

static double clamp_double(double x, double  min, double max)
{
	if (isnan(x)) {
		return 0;
	} else if (x < min) {
		return min;
	} else if (x > max) {
		return max;
	} else {
		return x;
	}
}

static void update_cold_warm_from_brightness_temperature()
{
	unsigned desired_warm = state.brightness * state.temperature;
	unsigned desired_cold = state.brightness * (1.0 - state.temperature);

	update_state_warm(clamp(desired_warm, 0, MAX_ONE_CHANNEL));
	update_state_cold(clamp(desired_cold, 0, MAX_ONE_CHANNEL));
}

void light_manager_init()
{
	state.warm_value = 0;
	state.cold_value = 0;
}

void set_warm(unsigned intensity)
{
	intensity = clamp(intensity, 0, MAX_ONE_CHANNEL);
	update_state_warm(intensity);
	update_brightness_temperature_from_cold_warm();
}

void set_cold(unsigned intensity)
{
	intensity = clamp(intensity, 0, MAX_ONE_CHANNEL);
	update_state_cold(intensity);
	update_brightness_temperature_from_cold_warm();
}

void set_brightness(unsigned brightness)
{
	brightness = clamp(brightness, 0, MAX_TWO_CHANNELS);
	update_state_brightness(brightness);
	update_cold_warm_from_brightness_temperature();
}

void set_temperature(double temperature)
{
	temperature = clamp_double(temperature, 0.0, 1.0);
	update_state_temperature(temperature);
	update_cold_warm_from_brightness_temperature();
}

void set_brightness_auto(unsigned brightness)
{
	brightness = clamp(brightness, 0, MAX_TWO_CHANNELS);
	update_state_brightness(brightness);
	update_state_temperature(0.5 * (((double) brightness) / ((double) MAX_TWO_CHANNELS)));
	update_cold_warm_from_brightness_temperature();
}
