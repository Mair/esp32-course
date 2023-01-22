# LED Strip Driver

[![Component Registry](https://components.espressif.com/components/espressif/led_strip/badge.svg)](https://components.espressif.com/components/espressif/led_strip)

This driver is designed for addressable LEDs like [WS2812](http://www.world-semi.com/Certifications/WS2812B.html), where each LED is controlled by a single data line.

## Backend Controllers

### The [RMT](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/rmt.html) Peripheral

This is the most economical way to drive the LEDs because it only consumes one RMT channel, leaving other channels free to use. However, the memory usage increases dramatically with the number of LEDs. If the RMT hardware can't be assist by DMA, the driver will going into interrupt very frequently, thus result in a high CPU usage. What's worse, if the RMT interrupt is delayed or not serviced in time (e.g. if Wi-Fi interrupt happens on the same CPU core), the RMT transaction will be corrupted and the LEDs will display incorrect colors. If you want to use RMT to drive a large number of LEDs, you'd better to enable the DMA feature if possible. [^1]

#### Allocate LED Strip Object with RMT Backend

```c
#define BLINK_GPIO 0

led_strip_handle_t led_strip;

/* LED strip initialization with the GPIO and pixels number*/
led_strip_config_t strip_config = {
    .strip_gpio_num = BLINK_GPIO, // The GPIO that connected to the LED strip's data line
    .max_leds = 1, // The number of LEDs in the strip,
    .led_pixel_format = LED_PIXEL_FORMAT_GRB, // Pixel format of your LED strip
    .led_model = LED_MODEL_WS2812, // LED strip model
    .flags.invert_out = false, // whether to invert the output signal (useful when your hardware has a level inverter)
};

led_strip_rmt_config_t rmt_config = {
    .clk_src = RMT_CLK_SRC_DEFAULT, // different clock source can lead to different power consumption
    .resolution_hz = 10 * 1000 * 1000, // 10MHz
    .flags.with_dma = false, // whether to enable the DMA feature
};
ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
```

[^1]: The DMA feature is not available on all ESP chips. Please check the data sheet before using it.
