```c
  ESP_ERROR_CHECK(nvs_flash_init_partition("MyNvs"));

  nvs_handle handle;
  ESP_ERROR_CHECK(nvs_open_from_partition("MyNvs", "store", NVS_READWRITE, &handle));
  ```