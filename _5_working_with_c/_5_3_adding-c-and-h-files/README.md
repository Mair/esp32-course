# adding c and h files

# the following is defined in the CMakeLists.txt
```c
idf_component_register(
    SRCS 
        "main.c"
        "calc/calc.c"
    INCLUDE_DIRS 
        "."
        "calc"
)

```