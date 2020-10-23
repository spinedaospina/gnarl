/*
This is the first code executed when the TTGO started.


The monitor messages when main.c is running are:

1 - (362) GNARL: enable watchdog with a timeout of 300 seconds		// white color
2 - (362) GNARL: subg_rfspy 2.2										// -> green color
3 - (362) GNARL: radio version 1.2									// white color
4 - (372) GNARL: frequency set to 916599975 Hz						// white color
5 - (672) GNARL: display_loop: op 4 arg 0							// white color
6 - (672) GNARL: starting gnarl_loop								// white color
*/

#include <stdio.h>

#include <esp_task_wdt.h>

#include "display.h"
#include "gnarl.h"
#include "rfm95.h"

#define PUMP_FREQUENCY 916400000	//this value was changed from 916600000 to 916400000 y "magically" all works

void app_main(void) {
	
	"(362) GNARL: enable watchdog with a timeout of 300 seconds"
	ESP_LOGD(TAG, "enable watchdog with a timeout of %d seconds", WDT_TIMEOUT_SECONDS); //Shows in monitor that WDT = 5 min = 300 s
	esp_task_wdt_init(WDT_TIMEOUT_SECONDS, true);										//Initialization of WDT
																						//The command could be found in:
																						//~/esp/esp-idf/components/esp_common/include/esp_task_wdt.h

	"(362) GNARL: subg_rfspy 2.2"
	ESP_LOGI(TAG, "%s", SUBG_RFSPY_VERSION);											//It shows in monitor "subg_rfspy 2.2"

	"(362) GNARL: radio version 1.2"
	rfm95_init();																		//The RF is started, nothing is shown in monitor
	uint8_t v = read_version();															//The radio version is saved in "v"
	ESP_LOGD(TAG, "radio version %d.%d", version_major(v), version_minor(v));			//The monitor shows: "radio version 1.2".

	"(372) GNARL: frequency set to 916599975 Hz"
	set_frequency(PUMP_FREQUENCY);														//The pump frequency is set
	ESP_LOGD(TAG, "frequency set to %d Hz", read_frequency());							//The setted frequency is shown


	"(672) GNARL: display_loop: op 4 arg 0"
	display_init();																		//The display is started, command in display.h

	"(672) GNARL: starting gnarl_loop"
	gnarl_init();																		//The gnarl started, command in ble.c
}


/*
All the ESP_LOG* commands could be find in:
//esp/esp-idf/components/spi_flash/sim/stubs/log/include/esp_log.h

This commands shows a monitor message
The * character could be changed for:
	-ESP_LOGE = Error (lowest)
	-ESP_LOGW = Warning
	-ESP_LOGI = Info
	-ESP_LOGD = Debug
	-ESP_LOGV = Verbose (highest)

I still without understanding the "TAG" parameter in the function.
For simplification the command ESP_LOG* works like a printf in the monitor
*/

/*
Continue the reading in gnarl.h code
*/