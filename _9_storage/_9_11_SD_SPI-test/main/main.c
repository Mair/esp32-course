#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
// #include "esp_spiffs.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"

static const char *TAG = "store";
static const char *BASE_PATH = "/store";

void write_file(char *path, char *content);
void read_file(char *path);

#define PIN_NUM_CS 4
#define PIN_NUM_MOSI 5
#define PIN_NUM_CLK 18
#define PIN_NUM_MISO 19

void app_main(void)
{
    // wl_handle_t wl_handle;
    /// SPIFFS
    // esp_vfs_spiffs_conf_t config = {
    //     .base_path = BASE_PATH,
    //     .partition_label = NULL,
    //     .max_files = 5,
    //     .format_if_mount_failed = true,
    // };
    // esp_vfs_spiffs_register(&config);

    // esp_vfs_fat_mount_config_t esp_vfs_fat_mount_config = {
    //     .allocation_unit_size = CONFIG_WL_SECTOR_SIZE,
    //     .max_files = 5,
    //     .format_if_mount_failed = true,
    // };

    // FAT WL
    // esp_vfs_fat_spiflash_mount_rw_wl(BASE_PATH, "storage", &esp_vfs_fat_mount_config, &wl_handle);

    // FAT RO
    // esp_vfs_fat_spiflash_mount_ro(BASE_PATH, "storage", &esp_vfs_fat_mount_config);

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024};

    spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.slot = SPI3_HOST;

    ESP_ERROR_CHECK(spi_bus_initialize(host.slot, &bus_cfg, SDSPI_DEFAULT_DMA));
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = PIN_NUM_CS;
    slot_config.host_id = host.slot;

    sdmmc_card_t *card;
    esp_vfs_fat_sdspi_mount(BASE_PATH, &host, &slot_config, &mount_config, &card);

    //////////////////
    // read_file("/store/flash-loaded.txt");
    write_file("/store/text.txt", "Hello world!");
    read_file("/store/text.txt");
    /////////////////

    // SPIFFS
    // esp_vfs_spiffs_unregister(NULL);

    // FAT WL
    // esp_vfs_fat_spiflash_unmount_rw_wl(BASE_PATH, wl_handle);

    // esp_vfs_fat_spiflash_unmount_ro(BASE_PATH, "storage");
}

void read_file(char *path)
{
    ESP_LOGI(TAG, "reading file %s", path);
    FILE *file = fopen(path, "r");
    char buffer[100];
    fgets(buffer, 99, file);
    fclose(file);
    ESP_LOGI(TAG, "file contains: %s", buffer);
}

void write_file(char *path, char *content)
{
    ESP_LOGI(TAG, "Writing \"%s\" to %s", content, path);
    FILE *file = fopen(path, "w");
    fputs(content, file);
    fclose(file);
}
