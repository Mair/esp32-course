#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

#define TAG "NVS"

typedef struct cat_struct
{
  char name[30];
  int age;
  int id;
} Cat;

void app_main()
{
  vTaskDelay(1000 / portTICK_PERIOD_MS);

  ESP_ERROR_CHECK(nvs_flash_init_partition("Mynvs"));

  nvs_handle handle;
  ESP_ERROR_CHECK(nvs_open_from_partition("Mynvs", "cat_store", NVS_READWRITE, &handle));

  char catKey[50];
  Cat cat;
  size_t catSize = sizeof(Cat);
  for (int i = 0; i < 5; i++)
  {
    sprintf(catKey, "cat_%d", i);
    esp_err_t result = nvs_get_blob(handle, catKey, (void *)&cat, &catSize);
    //printf("actual cat size=%d returned from NVS =%d\n", sizeof(Cat), catSize);
    
    switch (result)
    {
    case ESP_ERR_NOT_FOUND:
    case ESP_ERR_NVS_NOT_FOUND:
      ESP_LOGE(TAG, "Value not set yet");
      break;
    case ESP_OK:
      ESP_LOGI(TAG, "cat name: %s, age %d, id %d", cat.name, cat.age, cat.id);
      break;
    default:
      ESP_LOGE(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(result));
      break;
    }
  }
  for (int i = 0; i < 5; i++)
  {
    sprintf(catKey, "cat_%d", i);
    Cat newCat;
    sprintf(newCat.name,"Mr cat %d", i);
    newCat.age = i + 2;
    newCat.id = i;
    ESP_ERROR_CHECK(nvs_set_blob(handle, catKey, (void *) &newCat, sizeof(Cat) ));
    ESP_ERROR_CHECK(nvs_commit(handle));
  }

  nvs_close(handle);
}