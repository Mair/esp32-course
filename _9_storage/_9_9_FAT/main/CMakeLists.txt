idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS ".")

# SPIFFS
# spiffs_create_partition_image(storage ../flash-loaded FLASH_IN_PROJECT)

fatfs_create_spiflash_image(storage ../flash-loaded FLASH_IN_PROJECT)
