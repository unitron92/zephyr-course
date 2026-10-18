#define DT_DRV_COMPAT our_led_sensor

#include <errno.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/sys/printk.h>
#include <led_sensor.h>

struct led_sensor_config {
struct gpio_dt_spec led;
};

struct led_sensor_data {
bool led_on;
uint32_t on_time_ms;
};

static int led_sensor_sample_fetch(const struct device *dev, enum sensor_channel chan)
{
const struct led_sensor_config *cfg = dev->config;
struct led_sensor_data *data = dev->data;
int ret;

if (chan != SENSOR_CHAN_ALL && chan != LED_SENSOR_CHAN_STATE) {
return -ENOTSUP;
}

ret = gpio_pin_set_dt(&cfg->led, 1);
if (ret < 0) {
return ret;
}

data->led_on = true;
return 0;
}

static int led_sensor_channel_get(const struct device *dev, enum sensor_channel chan,
  struct sensor_value *val)
{
const struct led_sensor_config *cfg = dev->config;
struct led_sensor_data *data = dev->data;
int ret;

if (chan != LED_SENSOR_CHAN_STATE) {
return -ENOTSUP;
}

val->val1 = data->led_on ? 1 : 0;
val->val2 = (int32_t)data->on_time_ms;

ret = gpio_pin_set_dt(&cfg->led, 0);
if (ret < 0) {
return ret;
}

data->led_on = false;
return 0;
}

/* Extension API: change a parameter in the dynamic data struct */
static int led_sensor_impl_set_on_time(const struct device *dev, uint32_t on_time_ms)
{
struct led_sensor_data *data = dev->data;

if (on_time_ms == 0) {
return -EINVAL;
}

data->on_time_ms = on_time_ms;
return 0;
}

static const struct led_sensor_driver_api led_sensor_api = {
.base = {
.sample_fetch = led_sensor_sample_fetch,
.channel_get = led_sensor_channel_get,
},
.set_on_time = led_sensor_impl_set_on_time,
};

static int led_sensor_init(const struct device *dev)
{
const struct led_sensor_config *cfg = dev->config;
struct led_sensor_data *data = dev->data;

if (!gpio_is_ready_dt(&cfg->led)) {
printk("led_sensor: GPIO not ready\n");
return -ENODEV;
}

data->led_on = false;
data->on_time_ms = 500;
return gpio_pin_configure_dt(&cfg->led, GPIO_OUTPUT_INACTIVE);
}

#define LED_SENSOR_DEFINE(inst)                                                \
static struct led_sensor_data led_sensor_data_##inst;                  \
static const struct led_sensor_config led_sensor_config_##inst = {     \
.led = GPIO_DT_SPEC_INST_GET(inst, led_gpios),                 \
};                                                                     \
DEVICE_DT_INST_DEFINE(inst, led_sensor_init, NULL,                     \
      &led_sensor_data_##inst,                         \
      &led_sensor_config_##inst,                       \
      POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY,        \
      &led_sensor_api);

DT_INST_FOREACH_STATUS_OKAY(LED_SENSOR_DEFINE)