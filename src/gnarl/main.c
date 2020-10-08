#include <stdio.h>

#include <esp_task_wdt.h>

#include "display.h"
#include "gnarl.h"
#include "rfm95.h"

#define PUMP_FREQUENCY 916400000

void app_main(void) {
	ESP_LOGD(TAG, "enable watchdog with a timeout of %d seconds", WDT_TIMEOUT_SECONDS); //Se muestra en monitor el tiempo con el que trabajará
																						//el perro guardian TWDT, en este caso, corresponde a un
																						//tiempo de 300 segundos equivalentes a cinco minutos
																						//Todos estos comandos ESP_LOG* se encuentran en:
																						//esp/esp-idf/components/spi_flash/sim/stubs/log/include/esp_log.h

	esp_task_wdt_init(WDT_TIMEOUT_SECONDS, true);										//Comando para inicializar el TWDT con los valores
																						//mostrados anteriormente se encuentra en 
																						//~/esp/esp-idf/components/esp_common/include/esp_task_wdt.h

	ESP_LOGI(TAG, "%s", SUBG_RFSPY_VERSION);											//Escribe "subg_rfspy 2.2" en el monitor con un color
																						//verde, ya que es del tipo info

	rfm95_init();																		//Se inicia la radiofrecuencia, no arroja nada en pantalla
																						
	uint8_t v = read_version();															//Se lee la versión del radio y se almacena en la variable
																						//v, uint8_t es un entero sin signo que se almacena
																						//en 8 bits

	ESP_LOGD(TAG, "radio version %d.%d", version_major(v), version_minor(v));			//Se imprime en monitor un Debug con el mensaje:
																						//"radio version 1.2", debe de existir en las librerias
																						//<esp_wifi.h> o <esp_task_wdt.h> el comando 
																						//version_mayor y version_minor que transforme el 
																						//valor de v en los datos mostrados en el monitor
																						//Pero este en principio no es importante

	set_frequency(PUMP_FREQUENCY);														//Se establece la frecuencia de la bomba, se varió este
																						//valor de 916600000 a 916400000 y funcionó todo

	ESP_LOGD(TAG, "frequency set to %d Hz", read_frequency());							//Se imprime en monitor la frecuencia que se eligió

	display_init();																		//Se inicia el display

	gnarl_init();																		//Se inicia el gnarl
}

/*
Este main.c corresponde al que se encuentra en la localizacion:
$dondeSeaQueTengasGnarl/gnarl/src/gnarl/main.c

Todos los comandos ESP_LOG* son encargados de mostrar los mensajes en el monitor
depende de la denominación que se dé en *, que puede ser del tipo:
	-ESP_LOGE = Error (lowest)
	-ESP_LOGW = Warning
	-ESP_LOGI = Info
	-ESP_LOGD = Debug
	-ESP_LOGV = Verbose (highest)

Aún no termino de entender la función del parámetro "TAG", pero de resto
funciona como un printf
*/

/*
Se debe de revisar entonces display.h y gnarl.h en búsqueda de datos 
para entender como funciona más a profundidad el programa
*/