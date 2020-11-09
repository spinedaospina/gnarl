#ifndef _GNARL_H
#define _GNARL_H

#include <stdint.h>

#include "pump_config.h"
#include "version.h"

#define TAG		"GNARL"
#define LOG_LOCAL_LEVEL	ESP_LOG_DEBUG
#include <esp_log.h>

#define MILLISECONDS	1000
#define SECONDS		1000000

#define WDT_TIMEOUT_SECONDS (5*60)

#define MHz		1000000

// Track github.com/ps2/rileylink/firmware/ble113_rfspy/gatt.xml
#define BLE_RFSPY_VERSION  "ble_rfspy 2.0"

#define STATE_OK "OK"

void gnarl_init(void);
/*
	Call start_gnarl_task();
	Initializate the NimBLE configuration (NimBLE is a BLE Stack) and debug

	Set the function "sync_callback" like the function to be executed when a sync occurs
*/

void start_gnarl_task(void);
/*
start_gnarl_task()
	Start the gnarl_loop creating a high priority task
*/

void rfspy_command(const uint8_t *buf, int count, int rssi);
void send_code(const uint8_t code);
void send_bytes(const uint8_t *buf, int count);
void print_bytes(const char* msg, const uint8_t *buf, int count);

#endif // _GNARL_H

/*
In main.c only the command gnarl_init is called, It´s suggested that this command start the interesting code

But... what do the other commands in gnarl.h? Let´s search in gnarl.c

        *   start_gnarl_task(void);
        *   rfspy_command(const uint8_t *buf, int count, int rssi)
        *   send_code(const uint8_t code);
        *   send_bytes(const uint8_t *buf, int count);
        *   print_bytes(const char* msg, const uint8_t *buf, int count);
*/