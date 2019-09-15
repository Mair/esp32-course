#include <driver/i2c.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include "sdkconfig.h"
#include "u8g2_esp32_hal.h"

#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <string.h>
#include <u8g2.h>
#include "u8g2_esp32_hal.h"
#include "esp_log.h"

#define SDA_PIN 19
#define SCL_PIN 18
#define DISPLAY_ADDRESS 0x3C
static const char *TAG = "ssd1306";

void app_main()
{
	u8g2_esp32_hal_t u8g2_esp32_hal = U8G2_ESP32_HAL_DEFAULT;

	u8g2_esp32_hal.sda = SDA_PIN;
	u8g2_esp32_hal.scl = SCL_PIN;
	u8g2_esp32_hal_init(u8g2_esp32_hal);

	u8g2_t u8g2; // a structure which will contain all the data for one display
	u8g2_Setup_ssd1306_i2c_128x32_univision_f(
			&u8g2,
			U8G2_R0,
			//u8x8_byte_sw_i2c,
			u8g2_esp32_i2c_byte_cb,
			u8g2_esp32_gpio_and_delay_cb); // init u8g2 structure
	u8x8_SetI2CAddress(&u8g2.u8x8, 0x78);

	ESP_LOGI(TAG, "u8g2_InitDisplay");
	u8g2_InitDisplay(&u8g2); // send init sequence to the display, display is in sleep mode after this,

	ESP_LOGI(TAG, "u8g2_SetPowerSave");
	u8g2_SetPowerSave(&u8g2, 0); // wake up display
	u8g2_SetFont(&u8g2, u8g2_font_ncenB14_tr);

	int x = 0;
	 while (1)
	 {
		u8g2_ClearBuffer(&u8g2);
		u8g2_DrawStr(&u8g2, x, 17, "Hello World!!!!");
		x+=2;
		if(x > 128) x = -100;
		u8g2_SendBuffer(&u8g2);
	 	vTaskDelay(10 / portTICK_RATE_MS);
	 }
}