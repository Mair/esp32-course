## main/CMakeLists.txt
```c
fatfs_create_rawflash_image(storage ../flash-loaded FLASH_IN_PROJECT)
```

```c
  wl_handle_t wl_handle;
    esp_vfs_fat_mount_config_t esp_vfs_fat_mount_config = {
        .allocation_unit_size = CONFIG_WL_SECTOR_SIZE,
        .max_files = 5,
        .format_if_mount_failed = true,
    };

    esp_vfs_fat_spiflash_mount_ro(BASE_PATH, "storage", &esp_vfs_fat_mount_config);
    read_file("/store/flash-loaded.txt");
    esp_vfs_fat_spiflash_unmount_ro(BASE_PATH, "storage");
```
