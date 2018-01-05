# RelayControl

This basic program controls relays with the FTDI FT245R chip.
Simply turn each relay on/off by parameter.

# Requirements
- A free usb port
- A usb-relay module like [this](https://images-na.ssl-images-amazon.com/images/I/71XYWP4-5dL._SL1500_.jpg) (mine is from Quimat, bought on amazon)
- Linux x64 operating system

# Background

The FT245R chip is an usb to serial converter with some special features.
You can use it as normal serial connection to your device with the driver from [here](http://www.ftdichip.com/FTSupport.htm).
Or with no driver installed you can set the chip in bitbanging mode, where all serial pins become an independently controlable 
in-/output (what we need for this).

I used this the documents provided by FTDI:
- [Software guide](http://www.ftdichip.com/Support/Documents/ProgramGuides/D2XX_Programmer%27s_Guide(FT_000071).pdf)
- [Driver & Examples](http://www.ftdichip.com/Drivers/D2XX/Linux/libftd2xx-x86_64-1.4.6.tgz)

# Build

Simply run `make`. I included the static library from FTDI.

# Usage

RelayControl       (Created by V-Modder)

Usage:	relaycontol OPTION...

Options:
+ p [Pin]:[State]	Using pin mode, where each pin and state is set in the next argument
+ s 0xH			Using single mode, where all pins are set at once by the given hex-mask


If -s parameter is set no other parameters are excepted
In pin mode you can have n arguments.
