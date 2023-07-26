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

    spi_bus_config_t spi_bus_config = {
        .mosi_io_num = PIN_MOSI,
        .miso_io_num = PIN_MISO,
        .sclk_io_num = PIN_CLK,
        .quadhd_io_num = -1,
        .quadwp_io_num = -1};

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    ESP_ERROR_CHECK(spi_bus_initialize(host.slot, &spi_bus_config, SDSPI_DEFAULT_DMA));

    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = PIN_CS;
    slot_config.host_id = host.slot;

    sdmmc_card_t *card;
    ESP_ERROR_CHECK(esp_vfs_fat_sdspi_mount(BASE_PATH, &host, &slot_config, &mount_config, &card));
    sdmmc_card_print_info(stdout, card);

    //////////////////
    write_file("/store/text.txt", "Hello world!");
    read_file("/store/text.txt");
    /////////////////

    esp_vfs_fat_sdcard_unmount(BASE_PATH, card);

    spi_bus_free(host.slot);
```