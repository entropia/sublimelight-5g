#ifndef SL5G_HTTPD_H
#define SL5G_HTTPD_H

#include <esp_err.h>

/**
 * Initialize the web interface.
 *
 * This function sets the required event handlers to start the server
 * as soon as the network becomes available. It should be called
 * before initating the network connection.
 */
esp_err_t web_interface_init();

#endif
