zephyr_include_directories(include)

if(CONFIG_LED_SENSOR)
  zephyr_library()
  zephyr_library_sources(drivers/led_sensor.c)
endif()