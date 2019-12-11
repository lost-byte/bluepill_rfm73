# RFM73 2.4GHz radio module library.
This project was started when I got some old RFM73 modules. Yes, I know it seems to be out of production. But I have some in very low price and it is still available.

There are several implementations suitable for this modules on github:
* [roboremo/ChiNRF](https://github.com/roboremo/ChiNRF) - looks like generally universal for nrf24-clones;
* [heye/rfm73](https://github.com/heye/rfm73) - Radioduino compartible;
...and some others, thats seems to be freezed many eyars ago.

But, my Idea - make library not for arduino platforms only.

Thats why fixed goals for this project:
* minimalistic realisation - less code, low universality (I mean nrf24 clones), only necessary functions;
* avoid using arduino functions - just pure C/C++, interconnection with HAL of any platform;

## Example - RC System.

My first Idea of using RFM73 modules is low-cost realisation of multichannel proportional RC-system.
Thats why 'Example' contains basic realisation of 2Ch (Car or boat) RC-system.

To implement  that example, you need:
* Hardware items:
  * 2x Bluepill (Also calls as 'Generic STM32F103') boards, for TX and RX devices;
  * 2x Potintiometers - input for TX;
  * 2x Servo-like actuatos, for example RC car contains 1 Servo for steer and ESC for motor;
* Software:
  * Arduino IDE;
  * [Arduino_STM32](https://github.com/rogerclarkmelbourne/Arduino_STM32) - installed over Arduino IDE;
  * [STM32duino-bootloader](https://github.com/rogerclarkmelbourne/STM32duino-bootloader) - bootloaders thats make STM32 boards flashable by Arduino IDE, see [instruction](https://github.com/rogerclarkmelbourne/Arduino_STM32/wiki/Uploading).

Great thanks to:
  * [Roger Clark](https://github.com/rogerclarkmelbourne) - for awesome work to enable cheap STM32 boards on Arduino;
  
