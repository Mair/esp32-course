![sd pinout](https://raw.githubusercontent.com/Mair/esp32-course/master/_9_storage/_9_11_SD_SPI/sd%20card%20numbers.png)

SD

| Pin | SD MMC  |    SPI    |
| :-: | :-----: | :-------: |
|  1  | CC/DAT3 |    CS     |
|  2  |   CMD   | D1 / MOSI |
|  3  |  VSS1   |   VSS1    |
|  4  |   VDD   |    VDD    |
|  5  |   CLK   |    CLK    |
|  6  |  VSS2   |   VSS2    |
|  7  |  DAT0   | DO / MISO |
|  8  |  DAT1   |     X     |
|  9  |  DAT2   |     X     |

Micro SD

| Pin | SD MMC  |    SPI    |
| :-: | :-----: | :-------: |
|  1  |  DAT2   |     x     |
|  2  | CC/DAT3 |    CS     |
|  3  |   CMD   | D1 / MOSI |
|  4  |   VDD   |    VDD    |
|  5  |   CLK   |    CLK    |
|  6  |   VSS   |    VSS    |
|  7  |  DAT0   | DO / MISO |
|  8  |  DAT1   |     x     |

```c
esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024};
    sdmmc_card_t *card;
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t sdmmc_slot = SDMMC_SLOT_CONFIG_DEFAULT();
    sdmmc_slot.width = 4;
    sdmmc_slot.clk = 14;
    sdmmc_slot.cmd = 9;
    sdmmc_slot.d0 = 8;
    sdmmc_slot.d1 = 36;
    sdmmc_slot.d2 = 37;
    sdmmc_slot.d3 = 48;
    sdmmc_slot.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;
    ESP_ERROR_CHECK(esp_vfs_fat_sdmmc_mount(BASE_PATH, &host, &sdmmc_slot, &mount_config, &card));

    
    ...

    esp_vfs_fat_sdcard_unmount(BASE_PATH, card);
```