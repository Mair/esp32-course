# JTAG Debugging
```bash

openocd -f .\debug\ftdi_ft2322.cfg -f .\debug\esp-wroom-32.cfg
```
## esp-wroom-32.cfg

```bash
transport select jtag
adapter_khz 20000
source [find target/esp32.cfg]
```

## ftdi_ft2322.cfg

```bash
interface ftdi
ftdi_vid_pid 0x0403 0x6010
ftdi_layout_init 0x0038 0x003b
transport select jtag
adapter_khz 200
```
![esp32 jtag](<misc/ESP32 JTAG.svg>)
