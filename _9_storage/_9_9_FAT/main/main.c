#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
// #include "esp_spiffs.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"

static const char *TAG = "store";
static const char *BASE_PATH = "/store";

void write_file(char *path, char *content);
void read_file(char *path);

void app_main(void)
{
    wl_handle_t wl_handle;
    /// SPIFFS
    // esp_vfs_spiffs_conf_t config = {
    //     .base_path = BASE_PATH,
    //     .partition_label = NULL,
    //     .max_files = 5,
    //     .format_if_mount_failed = true,
    // };
    // esp_vfs_spiffs_register(&config);

    esp_vfs_fat_mount_config_t esp_vfs_fat_mount_config = {
        .allocation_unit_size = CONFIG_WL_SECTOR_SIZE,
        .max_files = 5,
        .format_if_mount_failed = true,
    };
    esp_vfs_fat_spiflash_mount_rw_wl(BASE_PATH, "storage", &esp_vfs_fat_mount_config, &wl_handle);

    //////////////////
    read_file("/store/flash-loaded.txt");
    write_file("/store/text.txt", "Hello world!");
    read_file("/store/text.txt");
    /////////////////

    // esp_vfs_spiffs_unregister(NULL);
    esp_vfs_fat_spiflash_unmount_rw_wl(BASE_PATH, wl_handle);
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
