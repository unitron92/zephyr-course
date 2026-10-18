#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/printk.h>
#include <led_sensor.h>

int main(void)
{
const struct device *dev = DEVICE_DT_GET(DT_NODELABEL(led_sensor0));
struct sensor_value val;
uint32_t on_time = 200;

if (!device_is_ready(dev)) {
printk("LED sensor not ready\n");
return 0;
}

while (1) {
/* Custom extension API: change on_time_ms in the driver data */
int ret = led_sensor_set_on_time(dev, on_time);

printk("set_on_time(%u) -> %d\n", on_time, ret);

sensor_sample_fetch(dev);          /* LED on */
k_sleep(K_MSEC(on_time));
sensor_channel_get(dev, LED_SENSOR_CHAN_STATE, &val); /* LED off */
printk("LED state: %d, on_time_ms in driver: %d\n", val.val1, val.val2);
k_sleep(K_MSEC(500));

on_time += 100;
if (on_time > 1000) {
on_time = 200;
}
}

return 0;
}