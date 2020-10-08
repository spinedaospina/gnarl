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
void start_gnarl_task(void);
void rfspy_command(const uint8_t *buf, int count, int rssi);
void send_code(const uint8_t code);
void send_bytes(const uint8_t *buf, int count);
void print_bytes(const char* msg, const uint8_t *buf, int count);

#endif // _GNARL_H

/*
    De main.c únicamente se ejecuta el comando gnarl_init que en principio debería de comenzar a correr la programación como tal

    Pero ¿Qué hacen las demás funciones?
        *   start_gnarl_task(void);
        *   rfspy_command(const uint8_t *buf, int count, int rssi)
        *   send_code(const uint8_t code);
        *   send_bytes(const uint8_t *buf, int count);
        *   print_bytes(const char* msg, const uint8_t *buf, int count);
*/