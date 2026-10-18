#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/printk.h>

#define LED_SENSOR_CHAN_STATE ((enum sensor_channel)SENSOR_CHAN_PRIV_START)

int main(void)
{
const struct device *dev = DEVICE_DT_GET(DT_NODELABEL(led_sensor0));
struct sensor_value val;

if (!device_is_ready(dev)) {
printk("LED sensor not ready\n");
return 0;
}

while (1) {
sensor_sample_fetch(dev);
k_sleep(K_MSEC(500));
sensor_channel_get(dev, LED_SENSOR_CHAN_STATE, &val);
printk("LED state when read: %d\n", val.val1);
k_sleep(K_MSEC(500));
}

return 0;
}