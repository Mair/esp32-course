#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"

void app_main(void)
{
  ESP_LOGI("CONFIG - ", "MY_BOOL = %s", CONFIG_MY_BOOL? "yes" : "no");
  ESP_LOGI("CONFIG - ", "MY_INT = %d", CONFIG_MY_INT);
  ESP_LOGI("CONFIG - ", "MY_STRING = %s", CONFIG_MY_STRING);

 int option = 0;

# ifdef CONFIG_OPTION_1
  option = 1;
# elif CONFIG_OPTION_2
   option = 2;
# else
   option = 3;
# endif

 ESP_LOGI("CONFIG - ", "My choice was %d", option);
}