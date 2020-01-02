#ifndef SL5G_WLAN_H
#define SL5G_WLAN_H

#include <esp_err.h>

/**
 * Initiate connection with WLAN.
 *
 * This function returns before the connection is complete. Use event
 * handlers to wait for the connection.
 */
esp_err_t wlan_connect(void);

#endif
