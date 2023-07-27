# components

## the following is defined in components/calc
```c
idf_component_register(
    SRCS 
        "calc.c"
    INCLUDE_DIRS 
        "."
    REQUIRES
        "driver"
)
```