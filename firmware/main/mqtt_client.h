#ifndef SL5G_MQTT_CLIENT_H
#define SL5G_MQTT_CLIENT_H

#include <stdbool.h>

void mqtt_client_init(void);

bool mqtt_client_subscribe(char *topic);

bool mqtt_client_unsubscribe(char *topic);

void mqtt_publish_group_state(char *groups);

#endif
