#include <stdio.h>
#include "esp_spiffs.h"
#include "esp_log.h"

#include <stdio.h> 
#include <dirent.h>
#include <sys/unistd.h>
#include <sys/stat.h>

#define TAG "SPIFFS"

void app_main(void)
{
  esp_vfs_spiffs_conf_t config = {
      .base_path = "/spiffs",
      .partition_label = NULL,
      .max_files = 5,
  };
  esp_vfs_spiffs_register(&config);

  DIR *dir = opendir("/spiffs");
  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL)
  {
    char fullPath[300];
    sprintf(fullPath,"/spiffs/%s", entry->d_name);
    struct stat entryStat;
    if(stat(fullPath, &entryStat) == -1)
    {
      ESP_LOGE(TAG, "error getting stats for %s", fullPath);
    }
    else
    {
      ESP_LOGI(TAG,"full path = %s, file size = %ld", fullPath, entryStat.st_size);
    }
  }
  size_t total = 0, used = 0;
  esp_spiffs_info(NULL,&total, &used);
  ESP_LOGI(TAG,"total = %d, used = %d", total, used);
  

  FILE *file = fopen("/spiffs/index.html", "r");
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
