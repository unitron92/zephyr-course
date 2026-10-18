#ifndef LED_SENSOR_H_
#define LED_SENSOR_H_

#include <errno.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>

/* Custom channel: val1 = LED state, val2 = configured on-time in ms */
#define LED_SENSOR_CHAN_STATE ((enum sensor_channel)SENSOR_CHAN_PRIV_START)

typedef int (*led_sensor_set_on_time_t)(const struct device *dev, uint32_t on_time_ms);

/* Extended API: the standard sensor API must be the first member */
struct led_sensor_driver_api {
struct sensor_driver_api base;
led_sensor_set_on_time_t set_on_time;
};

/* Extension function: change the on-time parameter in the driver's data struct */
static inline int led_sensor_set_on_time(const struct device *dev, uint32_t on_time_ms)
{
const struct led_sensor_driver_api *api =
(const struct led_sensor_driver_api *)dev->api;

if (api->set_on_time == NULL) {
return -ENOSYS;
}

return api->set_on_time(dev, on_time_ms);
}

#endif /* LED_SENSOR_H_ */