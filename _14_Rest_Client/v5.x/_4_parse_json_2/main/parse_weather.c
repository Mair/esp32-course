#include <stdio.h>
#include <string.h>
#include "parse_weather.h"
#include "esp_log.h"
#include "cJSON.h"
#include "esp_heap_caps.h"

static const char *TAG = "PARSE WEATHER";

void * json_malloc(size_t size){
  return heap_caps_malloc(size, MALLOC_CAP_SPIRAM);
}

void json_free(void* ptr){
  heap_caps_free(ptr);
}

esp_err_t parse_weather(weather_t *weather, char *weatherStr)
{

  cJSON_Hooks hooks ={
      .malloc_fn =json_malloc,
      .free_fn = json_free
  };
  cJSON_InitHooks(&hooks);

  cJSON *weather_json = cJSON_Parse(weatherStr);
  if (weather_json == NULL)
  {
    const char *err = cJSON_GetErrorPtr();
    if (err)
    {
      ESP_LOGE(TAG, "Error parsing json before %s", err);
      return -1;
    }
  }
  cJSON *location = cJSON_GetObjectItemCaseSensitive(weather_json, "location");
  strcpy(weather->location.country, cJSON_GetObjectItemCaseSensitive(location, "country")->valuestring);
  strcpy(weather->location.region, cJSON_GetObjectItemCaseSensitive(location, "region")->valuestring);
  strcpy(weather->location.name, cJSON_GetObjectItemCaseSensitive(location, "name")->valuestring);

  cJSON *current = cJSON_GetObjectItemCaseSensitive(weather_json, "current");
  weather->current.temp_c = cJSON_GetObjectItemCaseSensitive(current, "temp_c")->valuedouble;

  cJSON *current_condition = cJSON_GetObjectItemCaseSensitive(current, "condition");
  strcpy(weather->current.condion_text, cJSON_GetObjectItemCaseSensitive(current_condition, "text")->valuestring);

  cJSON *forecast = cJSON_GetObjectItemCaseSensitive(weather_json, "forecast");
  cJSON *forecast_days = cJSON_GetObjectItemCaseSensitive(forecast, "forecastday");

  int i = 0;
  cJSON *forecast_day;
  cJSON_ArrayForEach(forecast_day, forecast_days)
  {
    cJSON *day = cJSON_GetObjectItemCaseSensitive(forecast_day, "day");
    weather->forecast[i++].avgtemp_c = cJSON_GetObjectItemCaseSensitive(day, "avgtemp_c")->valuedouble;

    cJSON *day_condition = cJSON_GetObjectItemCaseSensitive(day, "condition");
    strcpy(weather->forecast[i].condition_text, cJSON_GetObjectItemCaseSensitive(day_condition, "text")->valuestring);
  }
  cJSON_Delete(weather_json);

  return ESP_OK;
}