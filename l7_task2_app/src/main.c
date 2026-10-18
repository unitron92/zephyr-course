#include <errno.h>
#include <stdlib.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/shell/shell.h>
#include <zephyr/drivers/sensor.h>
#include <led_sensor.h>

#define ON_TIME_MIN_MS 1UL
#define ON_TIME_MAX_MS 10000UL

static const struct device *const sensor_dev = DEVICE_DT_GET(DT_NODELABEL(led_sensor0));

static int check_ready(const struct shell *sh)
{
if (!device_is_ready(sensor_dev)) {
shell_error(sh, "device %s is not ready", sensor_dev->name);
return -ENODEV;
}

return 0;
}

static int cmd_fetch(const struct shell *sh, size_t argc, char **argv)
{
int ret;

ARG_UNUSED(argc);
ARG_UNUSED(argv);

ret = check_ready(sh);
if (ret < 0) {
return ret;
}

ret = sensor_sample_fetch(sensor_dev);
if (ret < 0) {
shell_error(sh, "sample_fetch failed: %d", ret);
return ret;
}

shell_print(sh, "sample fetched (LED on)");
return 0;
}

static int cmd_read(const struct shell *sh, size_t argc, char **argv)
{
struct sensor_value val;
int ret;

ARG_UNUSED(argc);
ARG_UNUSED(argv);

ret = check_ready(sh);
if (ret < 0) {
return ret;
}

ret = sensor_channel_get(sensor_dev, LED_SENSOR_CHAN_STATE, &val);
if (ret < 0) {
shell_error(sh, "channel_get failed: %d", ret);
return ret;
}

shell_print(sh, "state: %d, on_time_ms: %d (LED off)", val.val1, val.val2);
return 0;
}

static int cmd_info(const struct shell *sh, size_t argc, char **argv)
{
ARG_UNUSED(argc);
ARG_UNUSED(argv);

shell_print(sh, "device: %s", sensor_dev->name);
shell_print(sh, "ready: %s", device_is_ready(sensor_dev) ? "yes" : "no");
return 0;
}

/* sensorroot set <on_time_ms> : calls the L06 extension API */
static int cmd_set(const struct shell *sh, size_t argc, char **argv)
{
unsigned long value;
char *end;
int ret;

if (argc < 2) {
shell_error(sh, "missing argument. Usage: sensorroot set <on_time_ms>");
return -EINVAL;
}

ret = check_ready(sh);
if (ret < 0) {
return ret;
}

errno = 0;
value = strtoul(argv[1], &end, 10);
if (argv[1][0] == '\0' || *end != '\0' || errno == ERANGE) {
shell_error(sh, "invalid number: '%s'", argv[1]);
return -EINVAL;
}

if (value < ON_TIME_MIN_MS || value > ON_TIME_MAX_MS) {
shell_error(sh, "value %lu out of range (%lu..%lu ms)", value,
    ON_TIME_MIN_MS, ON_TIME_MAX_MS);
return -ERANGE;
}

ret = led_sensor_set_on_time(sensor_dev, (uint32_t)value);
if (ret < 0) {
shell_error(sh, "set_on_time failed: %d", ret);
return ret;
}

shell_print(sh, "on_time_ms set to %lu", value);
return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_sensorroot,
SHELL_CMD(fetch, NULL, "Fetch a sample (LED on)", cmd_fetch),
SHELL_CMD(read, NULL, "Read the channel (LED off)", cmd_read),
SHELL_CMD(info, NULL, "Show device name and ready state", cmd_info),
SHELL_CMD_ARG(set, NULL, "Set LED on-time in ms (1-10000): set <ms>", cmd_set, 2, 0),
SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(sensorroot, &sub_sensorroot, "Sensor commands", NULL);

int main(void)
{
return 0;
}