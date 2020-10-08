#include <stdio.h>

#include <esp_task_wdt.h>

#include "display.h"
#include "gnarl.h"
#include "rfm95.h"

#define PUMP_FREQUENCY 916400000

void app_main(void) {
	ESP_LOGD(TAG, "enable watchdog with a timeout of %d seconds", WDT_TIMEOUT_SECONDS); //A monitor message appear with the Watch Dog Timer,
																						//in this case, this time is about 300 seconds = 5 min
																						//All the ESP_LOG* commands could be find in:
																						//esp/esp-idf/components/spi_flash/sim/stubs/log/include/esp_log.h

	esp_task_wdt_init(WDT_TIMEOUT_SECONDS, true);										//Initialization WDT command with the time of 5 mins
																						//The command could be found in:
																						//~/esp/esp-idf/components/esp_common/include/esp_task_wdt.h

	ESP_LOGI(TAG, "%s", SUBG_RFSPY_VERSION);											//It shows in monitor "subg_rfspy 2.2" with a green color
																						//the color depend of the ESP_LOG message
																						//for example "I" message are green and
																						//E messages are red

	rfm95_init();																		//The RF is started, nothing is shown in monitor
																						
	uint8_t v = read_version();															//The radio version is read and it's saved in the "v" variable
																						//the meaning of uint8_t is "unsigned 8 bits int"

	ESP_LOGD(TAG, "radio version %d.%d", version_major(v), version_minor(v));			//The monitor shows: "radio version 1.2".
																						//In the libraries
																						//<esp_wifi.h> o <esp_task_wdt.h> should exists a  
																						//command version_mayor and version_minor which 
																						//convert the v value in the monitor message values
																						//I guess that this command it's not huge important

	set_frequency(PUMP_FREQUENCY);														//The pump frequency was setted, this value was changed from
																						//916600000 to 916400000 y "magically" all works
																						//This change was done in line number 9

	ESP_LOGD(TAG, "frequency set to %d Hz", read_frequency());							//The setted frequency is shown

	display_init();																		//The display get started

	gnarl_init();																		//The gnarl get started
}

/*
This main.c is located in:
$IDK_somewhere/gnarl/src/gnarl/main.c

All the commands ESP_LOG* shows monitor message
The * character could be changed for:
	-ESP_LOGE = Error (lowest)
	-ESP_LOGW = Warning
	-ESP_LOGI = Info
	-ESP_LOGD = Debug
	-ESP_LOGV = Verbose (highest)

I still without understanding the "TAG" parametr in the function.
For simplification the command ESP_LOG* works like a printf in the monitor
*/

/*
The codes display.h and gnarl.h have to be looked to	 search of info
about the all gnarl performance 
*/