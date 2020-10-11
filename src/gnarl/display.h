typedef enum {
	CONNECTED,
	PHONE_RSSI,
	PUMP_RSSI,
	COMMAND_TIME,
	SHOW_STATUS,
} display_op_t;

//Description of the commands

void display_init(void);
/*
It starts the display and habilitates the display_update command
*/

void display_update(display_op_t op, int arg);
/*
It updates values to be displayed.

The values of the input "op" could be:
	typedef enum {
		CONNECTED,
		PHONE_RSSI,
		PUMP_RSSI,
		COMMAND_TIME,
		SHOW_STATUS,
	} display_op_t;

and the arg correspond to an auxiliar value to op.

For example:
	If i want to display that the new phone RSSI is 10,
	I call the command "display_update(PHONE_RSSI, 10);"
*/