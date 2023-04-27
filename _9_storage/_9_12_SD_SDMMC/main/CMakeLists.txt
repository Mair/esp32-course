idf_component_register(SRCS "main.c"
    INCLUDE_DIRS ".")

# SPIFFS
# spiffs_create_partition_image(storage ../flash-loaded FLASH_IN_PROJECT)

# FAT WL
# fatfs_create_spiflash_image(storage ../flash-loaded FLASH_IN_PROJECT)

# FAT RO
# fatfs_create_rawflash_image(storage ../flash-loaded FLASH_IN_PROJECT)
