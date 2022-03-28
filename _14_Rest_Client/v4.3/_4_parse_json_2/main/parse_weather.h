#ifndef parse_weather_h
#define parse_weather_h

#include "esp_err.h"

#define NUM_DAYS 3

typedef struct forecast_t{
  double avgtemp_c;
  char condition_text[50];
}forecast_t;

typedef struct current_t{
  double temp_c;
  char condion_text[50];
}current_t;

typedef struct location_t
{
  char name[50];
  char region[50];
  char country[50];
} location_t;

typedef struct weather_t
{
  location_t location;
  current_t current;
  forecast_t forecast[NUM_DAYS];
} weather_t;

esp_err_t parse_weather(weather_t * weather, char *weatherStr);
#endif