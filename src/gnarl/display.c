#include "gnarl.h"

#include <string.h>
#include <unistd.h>

#include <esp_timer.h>
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

#include "display.h"
#include "module.h"
#include "oled.h"

typedef struct {
	display_op_t op;
	int arg;
} display_command_t;

#define QUEUE_LENGTH	100

static QueueHandle_t display_queue;

static int connected = false;
static int phone_rssi;
static int pump_rssi;
static int command_time;  // seconds

#define DISPLAY_TIMEOUT	5  // seconds


static void format_time_ago(char *buf) {
	int now = esp_timer_get_time() / 1000000;
	int delta = now - command_time;
	int min = delta / 60;
	if (min == 0) {
		sprintf(buf, "%ds", delta);
		return;
	}
	if (min < 60) {
		sprintf(buf, "%dm", min);
		return;
	}
	int hr = min / 60;
	min = min % 60;
	sprintf(buf, "%dh%dm", hr, min);
}


static void update(display_command_t cmd) {
	switch (cmd.op) {
	case PHONE_RSSI:
		phone_rssi = cmd.arg;
		ESP_LOGD(TAG, "phone RSSI = %d", phone_rssi);
		return;
	case PUMP_RSSI:
		pump_rssi = cmd.arg;
		ESP_LOGD(TAG, "pump RSSI = %d", pump_rssi);
		return;
	case COMMAND_TIME:
		command_time = cmd.arg;
		ESP_LOGD(TAG, "command time = %d", command_time);
		return;
	case CONNECTED:
		connected = cmd.arg;
		break;
	default:
		break;
	}
	oled_on();
	oled_clear();

	oled_font_medium();
        oled_align_center();
        oled_draw_string(64, 15, connected ? "Connected" : "Disconnected");

	oled_font_small();
	oled_align_left();
        oled_draw_string(5, 32, "Last command:");
        oled_draw_string(5, 46, "Phone RSSI:");
        oled_draw_string(5, 60, "Pump  RSSI:");

	oled_align_right();
	char buf[16];
	format_time_ago(buf);
	oled_draw_string(122, 32, buf);
	if (connected) {
		sprintf(buf, "%d", phone_rssi);
		oled_draw_string(122, 46, buf);
		sprintf(buf, "%d", pump_rssi);
		oled_draw_string(122, 60, buf);
	} else {
		oled_draw_string(122, 46, "--");
		oled_draw_string(122, 60, "--");
	}

        oled_update();
	usleep(DISPLAY_TIMEOUT*SECONDS);
	oled_off();
}

/*
display_loop

It´s a command task, display values and show them in monitor
*/
static void display_loop(void *unused) {
	for (;;) {																
		display_command_t cmd;
		if (!xQueueReceive(display_queue, &cmd, pdMS_TO_TICKS(100))) {
			continue;
		}
		ESP_LOGD(TAG, "display_loop: op %d arg %d", cmd.op, cmd.arg);
		update(cmd);
	}
}

/*
button_interrupt

Add the cmd value to the back of the queue
*/
static void button_interrupt(void *unused) {
	display_command_t cmd = { .op = SHOW_STATUS };
	xQueueSendFromISR(display_queue, &cmd, 0);
}

/*
display_update

	1) Create a cmd value with the inputs of the function
	2) Add the cmd to the display_queue
		-> If the queue is full print a message
*/
void display_update(display_op_t op, int arg) {
	display_command_t cmd = { .op = op, .arg = arg };
	if (!xQueueSend(display_queue, &cmd, 0)) {
		ESP_LOGE(TAG, "display_update: queue full");
	}
}


/*
display_init()

	1) Start the oled screen, 
	2) Create a queue (display_queue) which contains the info to be displayed
	3) Start a task which execute the command display_loop
	4) Execute the command display_update
	5) Enable interrupt on button press.
*/
void display_init(void) {
	oled_init();																// Oled started, command in oled.c 
	display_queue = xQueueCreate(QUEUE_LENGTH, sizeof(display_command_t));		// A queue is created with 100 components
	xTaskCreate(display_loop, "display", 2048, 0, 10, 0);						// Create the "display" task based on the 
																				// display_loop command

	display_update(SHOW_STATUS, 0);												//

	// Enable interrupt on button press.
	gpio_set_direction(BUTTON, GPIO_MODE_INPUT);
	gpio_set_intr_type(BUTTON, GPIO_INTR_NEGEDGE);
	gpio_isr_handler_add(BUTTON, button_interrupt, 0);
	gpio_intr_enable(BUTTON);
}

/*
All the variables cmd are an array of two values, "op" and "arg"

The values of "op" could be: (These are defined in display.h)
	typedef enum {
		CONNECTED,
		PHONE_RSSI,
		PUMP_RSSI,
		COMMAND_TIME,
		SHOW_STATUS,
	} display_op_t;

An the values of "arg" like the name indicates, are an argument. The meaning of this argument depend of the cmd.op, it could be an improve in the
understanding of this idea reading the "void update" command in the top of this code.


Unknow commands:
	- xQueueCreate
	http://esp32.info/docs/esp_idf/html/da/ded/group__xQueueCreate.html

		Syntaxis:
			display_queue = xQueueCreate(QUEUE_LENGTH, sizeof(display_command_t));
				^                 ^           ^                  ^
				|                 |           |                  |
			Name of the   -    Function - Length of   -  Size of each Queue
			  Queue						  the Queue			 component

		Description:
			Create a Queue in display_Queue

		Uses:
			1) Create a Queue of commands to be displayed (Save the memory, no info is added)
				Code:
					display_queue = xQueueCreate(QUEUE_LENGTH, sizeof(display_command_t));

	- xTaskCreate
	http://esp32.info/docs/esp_idf/html/dd/d3c/group__xTaskCreate.html

		Syntaxis:
			xTaskCreate(TaskCode, "NAME", STACK_SIZE, ParameterToPass, PRIORITY, &Handle );

		Description:
			Create a new task and add it to the list of tasks that are ready to run. 

		Uses:
			1) Create a task which display in the oled screen:
					- Connection
					- Time of the last command
					- Phone RSSI
        			- Pump  RSSI
				Code:
					xTaskCreate(display_loop, "display", 2048, 0, 10, 0);


	- xQueueReceived
	https://www.freertos.org/a00118.html

		Syntaxis:
			BaseType_t xQueueReceive(QueueHandle, *pvBuffer, xTicksToWait);

		Description:
			Copy an item from a Queue. If the item is copied returns TRUE, otherwise returns FALSE.
				- QueueHandle = The handle to the queue from which the item is to be received.
				- pvBuffer = Pointer to the buffer into which the received item will be copied.
				- xTicksToWait = The maximum amount of time the task should block waiting for an item

		Uses:
			1) If doesn´t copy the value from the Queue continue
				Code:
					if (!xQueueReceive(display_queue, &cmd, pdMS_TO_TICKS(100))) {
						continue;
					}			


	-xQueueSend

		Syntaxis:
			BaseType_t xQueueSend(QueueHandle, *pvItemToQueue, xTicksToWait);

		Description:
			Post an item to the **front** of a queue. The item is queued by copy, not by reference. 
			If the item is copied returns TRUE, otherwise returns FALSE.
				- QueueHandle = The handle to the queue which we will add an item.
				- pvItemToQueue = Pointer to the item that is to be placed on the Queue.
				- xTicksToWait = The maximum amount of time the task should block waiting
	                             for space to become available on the queue, should it already be full.

		Uses:
			1) Try to copy the value contained in cmd, if the Queue is full show an error message
				Code:
					if (!xQueueSend(display_queue, &cmd, 0)) {
						ESP_LOGE(TAG, "display_update: queue full");
					}		

	
	-xQueueSendFromISR

		Syntaxis:
			BaseType_t xQueueSendFromISR(QueueHandle,*pvItemToQueue,*pxHigherPriorityTaskWoken);

		Description:
			Post an item into the **back** of a queue.. The item is queued by copy, not by reference. 
			If the item is copied returns TRUE, otherwise returns FALSE.
				- QueueHandle = The handle to the queue which we will add an item.
				- pvItemToQueue = Pointer to the item that is to be placed on the Queue.
				- pxHigherPriorityTaskWoken = 

		Uses:
			1) Send the cmd value to the queue
				Code:
					xQueueSendFromISR(display_queue, &cmd, 0);	
*/