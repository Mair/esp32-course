# commands

## To generate a bin file 
`
spiffsgen.py <size> <folder> <out bin file>
`
```
spiffsgen.py 0x100000 folder_to_download to_flash.bin 
```
## to flash
`
esptool.py --chip esp32 --port <port> --baud 115200 write_flash -z <start address> <the bin file>
`
```
esptool.py --chip esp32 --port COM5 --baud 115200 write_flash -z 0x110000 to_flash.bin
```