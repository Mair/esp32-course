# Embed with Flash

## main/CMakeLists.txt
```c
idf_component_register(SRCS
    "main.c"
    INCLUDE_DIRS
    "."
    EMBED_TXTFILES
    "files/index.html"
    "files/sample.txt"
    EMBED_FILES
    "files/pinout.jpg"
)
```