# Custom MenuConfig

## Kconfig.projbuild
```c
menu "My Custom Menu"

config MY_BOOL
    bool "select yes or no"

config MY_STRING
    string "enter a string"
    default "hello world!"
        help
            this is my help text description

config MY_INT
    int "in int"
    range 1 15

config MY_CHOICE
    choice
        bool "select a choice"
            default OPTION_1
        config OPTION_1
            bool "this is option 1"
        config OPTION_2
            ...
    endchoice    


endmenu```

```c

void app_main(void)
{
  ESP_LOGI(TAG, "MY_INT %d", CONFIG_MY_INT);
  ESP_LOGI(TAG, "MY_STRING %s", CONFIG_MY_STRING);

  bool my_bool = false;
#ifdef CONFIG_MY_BOOL
  my_bool = true;
#else
  my_bool = false;
#endif
  ESP_LOGI(TAG, "MY_BOOL %s", my_bool ? "yes" : "no");
  int option = 0;
#ifdef CONFIG_OPTION_1
  option = 1;
#elif CONFIG_OPTION_2
  option = 2;
#else
  option = 3;
#endif
  ESP_LOGI(TAG, "MY_OPTION %d", option);
}

```