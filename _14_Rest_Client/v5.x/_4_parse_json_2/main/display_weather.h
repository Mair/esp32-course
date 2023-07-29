#ifndef display_weather_h
#define display_weather_h

#include "parse_weather.h"

void weather_display_task(void *params);
void update_display(weather_t *weather);
#endif