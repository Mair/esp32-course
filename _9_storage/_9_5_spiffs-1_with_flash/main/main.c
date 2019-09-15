#include <stdio.h>
#include "esp_spiffs.h"
#include "esp_log.h"

#define TAG "SPIFFS"

void app_main(void)
{
  esp_vfs_spiffs_conf_t config = {
      .base_path = "/spiffs",
      .partition_label = NULL,
      .max_files = 5,
      .format_if_mount_failed = true,
  };
  esp_vfs_spiffs_register(&config);

  FILE *file = fopen("/spiffs/sub/data.txt", "r");
  if(file ==NULL)
  {
    ESP_LOGE(TAG,"could not open file");
  }
  else 
  {
    char line[256];
    while(fgets(line, sizeof(line), file) != NULL)
    {
      printf(line);
    }
    fclose(file);
  }
  esp_vfs_spiffs_unregister(NULL);
}
