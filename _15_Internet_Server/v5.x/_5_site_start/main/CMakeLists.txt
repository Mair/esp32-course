idf_component_register(SRCS "pushBtn.c" "toggleLed.c" "main.c"
                    INCLUDE_DIRS ".")

fatfs_create_rawflash_image(storage ../site/dist FLASH_IN_PROJECT)