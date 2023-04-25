![sd pinout](https://raw.githubusercontent.com/Mair/esp32-course/master/_9_storage/_9_11_SD_SPI/sd%20card%20numbers.png)

SD

| Pin | SD MMC  |    SPI    |
| :-: | :-----: | :-------: |
|  1  | CC/DAT3 |    CS     |
|  2  |   CMD   | D1 / MOSI |
|  3  |  VSS1   |   VSS1    |
|  4  |   VDD   |    VDD    |
|  5  |   CLK   |    CLK    |
|  6  |  VSS2   |   VSS2    |
|  7  |  DAT0   | DO / MISO |
|  8  |  DAT1   |     X     |
|  9  |  DAT2   |     X     |

Micro SD

| Pin | SD MMC  |    SPI    |
| :-: | :-----: | :-------: |
|  1  |  DAT2   |     x     |
|  2  | CC/DAT3 |    CS     |
|  3  |   CMD   | D1 / MOSI |
|  4  |   VDD   |    VDD    |
|  5  |   CLK   |    CLK    |
|  6  |   VSS   |    VSS    |
|  7  |  DAT0   | DO / MISO |
|  8  |  DAT1   |     x     |
