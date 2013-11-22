#ifndef WLD_HALL_SENSOR_H
#define WLD_HALL_SENSOR_H

struct hall_sensor_pdata {
	int hall_sensor_gpio;
	int hall_sensor_active_type;
	int hall_sensor_code;
	void (*hall_sensor_io_init)(int);
	void (*action_for_hall_sensor)(int);
};

enum {
	HALL_SENSOR_ACTIVE_LOW,
	HALL_SENSOR_ACTIVE_HIGH,
};

#endif