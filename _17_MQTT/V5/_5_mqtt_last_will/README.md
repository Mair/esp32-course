```c
 esp_mqtt_client_config_t esp_mqtt_client_config = {
        .broker.address.uri = "mqtt://mqtt.eclipseprojects.io:1883",
        .session.last_will = {
            .topic = "animal/on-chip-death",
            .msg = "I died ☠️",
            .msg_len = strlen("I died ☠️"),
            .qos = 1,
        }};

```
