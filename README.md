# NINA-W10X Board
Basic dev board for u-blox NINA-W10X (ESP32 chip). Features low power voltage regulator, header for USB<->UART and JTAG connector for easy flashing and debugging using ESP-PROG (recommended). Powering from micro USB or screw terminal to make it flexible for different types of projects. There are probably many design flaws/improvements, but it's working at least 🤷

<img src="/.github/finished.jpg" />
<img src="/.github/finished1.jpg" />
<img src="/PCB/board.PNG" />

## GPIO Testing

### Compiling
Follow instruction on [https://github.com/espressif/esp-idf](https://github.com/espressif/esp-idf) to set up the esp-idf, then just run `idf.py build` or use the [VSCode extension](https://github.com/espressif/vscode-esp-idf-extension). Any esp-idf version should work.

### Flashing precompiled test application
```
esptool.py -p COMX -b 921600 --after hard_reset write_flash --flash_mode dio --flash_freq 40m --flash_size detect 0x8000 partition-table.bin 0x1000 bootloader.bin 0x10000 gpio_test.bin
```
I created a "test rig" that connects all GPIOs in pairs to verify that soldering was successful. The PCB can be pressed into the rig for fast testing. Test pulls all GPIOs low and then goes through one by one of the outputs, setting it to HIGH and reading all input GPIOs, checking they are still LOW, except the pin connected to the output. This should give pretty good confidence all pads on the NINA-W10X was soldered correctly.

Example output:
```
I (543) GPIO_TEST_APP: Set 23 to 1 expects 34 to match
I (543) GPIO_TEST_APP: PASS

I (553) GPIO_TEST_APP: Set 22 to 1 expects 35 to match
I (553) GPIO_TEST_APP: PASS

I (563) GPIO_TEST_APP: Set 15 to 1 expects 33 to match
I (563) GPIO_TEST_APP: PASS

I (563) GPIO_TEST_APP: Set 2 to 1 expects 32 to match
I (573) GPIO_TEST_APP: PASS

I (573) GPIO_TEST_APP: Set 19 to 1 expects 25 to match
I (583) GPIO_TEST_APP: PASS

I (583) GPIO_TEST_APP: Set 18 to 1 expects 26 to match
I (593) GPIO_TEST_APP: PASS

I (593) GPIO_TEST_APP: Set 5 to 1 expects 12 to match
I (603) GPIO_TEST_APP: PASS

I (603) GPIO_TEST_APP: Set 13 to 1 expects 27 to match
I (613) GPIO_TEST_APP: PASS

I (613) GPIO_TEST_APP: Set 21 to 1 expects 14 to match
I (623) GPIO_TEST_APP: PASS

I (623) GPIO_TEST_APP: Set 4 to 1 expects 36 to match
I (633) GPIO_TEST_APP: PASS

I (633) GPIO_TEST_APP: Set 0 to 1 expects 39 to match
I (643) GPIO_TEST_APP: PASS

W (643) GPIO_TEST_APP: GPIO TEST PASSED
```

<img src="/.github/testing.jpg"/>