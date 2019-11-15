set(COMPONENT_SRCS "main.c"
                    "server.c"
                    "connect.c"
                    "getTemperature.c")

set(COMPONENT_ADD_INCLUDEDIRS ".")

set(COMPONENT_EMBED_TXTFILES
        "../client/esp32-demo/build/index.html")

register_component()