# Build Environment

## Mac:

To build, install the XCode development tools and place the [SDL development framework](https://www.libsdl.org/release/SDL2-2.0.3.dmg) in `/Library/Frameworks/`.

## Raspberry Pi:

Use a [Raspbian](http://www.raspberrypi.org/downloads/) image, and install the SDL2 development libraries. My recommended way is to use [this script](https://gist.github.com/blacktm/8268a468f933eba46f80), this operation should take just under an hour to complete.

# Building

    make

# Usage

    anscat [file ...]
