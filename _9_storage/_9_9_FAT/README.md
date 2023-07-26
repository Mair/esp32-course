## main/cmakelists.txt
```c
fatfs_create_spiflash_image(storage ../flash-loaded FLASH_IN_PROJECT)
```
```c
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
```