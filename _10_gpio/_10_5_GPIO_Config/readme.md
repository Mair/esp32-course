```c 
gpio_config_t config = {
        .intr_type = GPIO_INTR_POSEDGE,
        .mode = GPIO_MODE_INPUT,
        .pull_down_en = 1,
        .pull_up_en = 0,
        .pin_bit_mask = (1ULL << PIN_SWITCH) | (1ULL << 12)};
    gpio_config(&config);
    ```