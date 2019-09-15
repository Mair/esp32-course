#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "bmp280.h"
#include "bmp280_defs.h"
#include "bmp280.c"
#include "esp_log.h"

#define SDA_GPIO 18
#define SCL_GPIO 19
#define RTOS_DELAY_1SEC 1000 / portTICK_PERIOD_MS
#define TAG "BPM2080"

void i2c_master_init()
{
    i2c_config_t i2c_config = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = SDA_GPIO,
        .scl_io_num = SCL_GPIO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000};
    i2c_param_config(I2C_NUM_0, &i2c_config);
    i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
}

void checkI2CPeripheralWorking()
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (BMP280_I2C_ADDR_SEC << 1) | I2C_MASTER_WRITE, true);
    i2c_master_stop(cmd);
    esp_err_t f_retval = i2c_master_cmd_begin(I2C_NUM_0, cmd, 50 / portTICK_PERIOD_MS);
    if (f_retval != ESP_OK)
    {
        printf("I2C slave NOT working or wrong I2C slave address - error (%i)\n", f_retval);
        vTaskDelay(3000 / portTICK_PERIOD_MS);
        esp_restart();
    }
    i2c_cmd_link_delete(cmd);
}

int8_t i2c_reg_read(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t length)
{
    i2c_cmd_handle_t cmd_handle = i2c_cmd_link_create();
    i2c_master_start(cmd_handle);
    i2c_master_write_byte(cmd_handle, (i2c_addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd_handle, reg_addr, true);
    i2c_master_start(cmd_handle);
    i2c_master_write_byte(cmd_handle, (i2c_addr << 1) | I2C_MASTER_READ, true);
    if (length > 1)
    {
        i2c_master_read(cmd_handle, reg_data, length - 1, I2C_MASTER_ACK);
    }
    i2c_master_read_byte(cmd_handle, reg_data + length - 1, I2C_MASTER_NACK);
    i2c_master_stop(cmd_handle);
    esp_err_t esp_err = i2c_master_cmd_begin(I2C_NUM_0, cmd_handle, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd_handle);
    return (esp_err == ESP_OK) ? 0 : -1;
}

int8_t i2c_reg_write(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t length)
{
    i2c_cmd_handle_t cmd_handle = i2c_cmd_link_create();
    i2c_master_start(cmd_handle);
    i2c_master_write_byte(cmd_handle, (i2c_addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd_handle, reg_addr, true);
    i2c_master_write(cmd_handle, reg_data, length, true);
    i2c_master_stop(cmd_handle);
    esp_err_t esp_err = i2c_master_cmd_begin(I2C_NUM_0, cmd_handle, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd_handle);
    return esp_err == ESP_OK ? 0 : -1;
}

void delay_ms(uint32_t period_ms)
{
    // vTaskDelay(period_ms / portTICK_PERIOD_MS);
    ets_delay_us(period_ms * 1000);
}

double getBMP280()
{

    int32_t temp32;
    double temp;
    uint32_t pres32, pres64;
    double pres;
    struct bmp280_dev bmp;
    struct bmp280_uncomp_data ucomp_data;
    struct bmp280_config config;
    bmp.delay_ms = delay_ms;
    /* Assign device I2C address based on the status of SDO pin (GND for PRIMARY(0x76) & VDD for SECONDARY(0x77)) */
    bmp.dev_id = BMP280_I2C_ADDR_SEC;
    bmp.intf = BMP280_I2C_INTF;
    bmp.read = i2c_reg_read;
    bmp.write = i2c_reg_write;

    int8_t bmpstatus = bmp280_init(&bmp);
    bmpstatus = bmp280_get_config(&config, &bmp);
    config.filter = BMP280_FILTER_COEFF_2;
    config.os_temp = BMP280_OS_1X;
    config.os_pres = BMP280_OS_8X;
    config.odr = BMP280_ODR_1000_MS;
    bmpstatus = bmp280_set_config(&config, &bmp);
    bmpstatus = bmp280_set_power_mode(BMP280_NORMAL_MODE, &bmp);

    vTaskDelay(50 / portTICK_PERIOD_MS);
    bmpstatus = bmp280_get_uncomp_data(&ucomp_data, &bmp);
    bmpstatus = bmp280_get_comp_temp_double(&temp, ucomp_data.uncomp_temp, &bmp);
    // ESP_LOGI(TAG, "Temperature is %f", temp);
    bmpstatus = bmp280_get_comp_pres_double(&pres, ucomp_data.uncomp_press, &bmp);
    // ESP_LOGI(TAG, "atmospheric pressure %f", pres);
    bmp.delay_ms(1);
    return temp;
}


void initi2c()
{
    ESP_ERROR_CHECK(nvs_flash_init());
    i2c_master_init();
    checkI2CPeripheralWorking();
   
}