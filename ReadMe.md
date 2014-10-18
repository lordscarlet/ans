# Build Environment

## Mac:

To build, install the XCode development tools and place the [SDL development framework](https://www.libsdl.org/release/SDL2-2.0.3.dmg) in `/Library/Frameworks/`.

## Raspberry Pi:

Use a [Raspbian](http://www.raspberrypi.org/downloads/) image.

Then install libudev-dev and the SDL2 development libraries. I would recommended using [this script](https://gist.github.com/andyherbert/f8682422a3b59bfda79d), the operation should take just under an hour to complete.

# Building

    make

# Usage

    ans [file ...]

At the moment Artworx, ASCII, Binary, Ice Draw, PCBoard, Tundra, and XBin (files with extensions adf, asc, bin, diz, idf, ion, nfo, pcb, tnd, txt, and xb) files are supported.

An SDL2-compatible game controller (such as an XBox controller) can be used to scroll the artwork with the analog stick. Scrolling can be performed by using the up and down keys. Pressing shift and using the cursor keys with shift the image slightly faster. Pressing space, and shift-space will scroll the image forward a page, and back a page respectively. Escape quits the program, or proceeds to the next file.