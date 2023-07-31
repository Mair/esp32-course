## NB. If you are on v5 this project will not compile as it is. Please see next video for MDNS on version 5

```c

void start_mdns_service()
{
  mdns_init();
  mdns_hostname_set("my-esp32");
  mdns_instance_name_set("LEARN esp32 thing");
}
```