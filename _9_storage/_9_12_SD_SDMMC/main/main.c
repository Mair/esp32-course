#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "esp_timer.h"
#include "driver/sdmmc_host.h"

#define PIN_CS 48
#define PIN_MOSI 9
#define PIN_CLK 14
#define PIN_MISO 8


#define ESP_SD_PIN_CLK            GPIO_NUM_14
#define ESP_SD_PIN_CMD            GPIO_NUM_15
#define ESP_SD_PIN_D0             GPIO_NUM_2
#define ESP_SD_PIN_D1             GPIO_NUM_4
#define ESP_SD_PIN_D2             GPIO_NUM_12
#define ESP_SD_PIN_D3             GPIO_NUM_13

static const char *TAG = "store";
static const char *BASE_PATH = "/store";

void write_file(char *path, char *content);
void read_file(char *path);

void app_main(void)
{
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024};
    sdmmc_card_t *card;
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t sdmmc_slot = SDMMC_SLOT_CONFIG_DEFAULT();
    sdmmc_slot.width = 4;
    sdmmc_slot.clk = ESP_SD_PIN_CLK;
    sdmmc_slot.cmd = ESP_SD_PIN_CMD;
    sdmmc_slot.d0 = ESP_SD_PIN_D0;
    sdmmc_slot.d1 = ESP_SD_PIN_D1;
    sdmmc_slot.d2 = ESP_SD_PIN_D2;
    sdmmc_slot.d3 = ESP_SD_PIN_D3;
    sdmmc_slot.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;
    ESP_ERROR_CHECK(esp_vfs_fat_sdmmc_mount(BASE_PATH, &host, &sdmmc_slot, &mount_config, &card));

    // esp_vfs_fat_sdmmc_mount_config_t mount_config = {
    //     .format_if_mount_failed = true,
    //     .max_files = 5,
    //     .allocation_unit_size = 16 * 1024};

    // spi_bus_config_t spi_bus_config = {
    //     .mosi_io_num = PIN_MOSI,
    //     .miso_io_num = PIN_MISO,
    //     .sclk_io_num = PIN_CLK,
    //     .quadhd_io_num = -1,
    //     .quadwp_io_num = -1};

    // sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    // ESP_ERROR_CHECK(spi_bus_initialize(host.slot, &spi_bus_config, SDSPI_DEFAULT_DMA));

    // sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    // slot_config.gpio_cs = PIN_CS;
    // slot_config.host_id = host.slot;

    // sdmmc_card_t *card;
    // ESP_ERROR_CHECK(esp_vfs_fat_sdspi_mount(BASE_PATH, &host, &slot_config, &mount_config, &card));
    // sdmmc_card_print_info(stdout, card);

    ///////////////////////
    DMA_ATTR static char long_text[1024];
    memset(&long_text, 'c', 1023);

    int64_t start_write = esp_timer_get_time();

    FILE *file = fopen("/store/text.txt", "w");
    fputs(long_text, file);
    fclose(file);

    uint64_t start_read = esp_timer_get_time();

    file = fopen("/store/text.txt", "r");
    fgets(long_text, 1023, file);
    fclose(file);
    uint64_t end_read = esp_timer_get_time();

    printf("write time %lld, read time %lld\n", start_read - start_write, end_read - start_write);
    /////////////////

    esp_vfs_fat_sdcard_unmount(BASE_PATH, card);
    // spi_bus_free(host.slot);
}
