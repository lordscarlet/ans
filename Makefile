UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
	CXX = gcc
	CXXFLAGS = -Wall -c -std=c99 `sdl2-config --cflags`
	LDFLAGS = -lm `sdl2-config --libs`
endif

ifeq ($(UNAME_S),Darwin)
	CXX = clang
	CXXFLAGS = -Wall -c -std=c99 -I/Library/Frameworks/SDL2.framework/Headers
	LDFLAGS = -F/Library/Frameworks -framework SDL2
endif

OUT = ans

all: $(OUT)

$(OUT): main.o file.o sauce.o palette.o font.o screen.o artworx.o ansi.o ansiedit.o ascii.o binary.o icedraw.o pcboard.o tundra.o xbin.o canvas.o window.o event.o text.o utf8.o
	$(CXX) $(LDFLAGS) $^ -o $@

main.o: src/main.c src/ui/window.h src/text/text.h
	$(CXX) $(CXXFLAGS) $< -o $@

file.o: src/file/file.c src/file/file.h src/file/formats/screen.h src/file/sauce.h src/file/formats/artworx.h src/file/formats/ansi.h src/file/formats/ansiedit.h src/file/formats/ascii.h src/file/formats/binary.h src/file/formats/icedraw.h src/file/formats/pcboard.h src/file/formats/tundra.h src/file/formats/xbin.h
	$(CXX) $(CXXFLAGS) $< -o $@

sauce.o: src/file/sauce.c src/file/sauce.h
	$(CXX) $(CXXFLAGS) $< -o $@

palette.o: src/file/formats/palette.c src/file/formats/palette.h src/file/formats/palettes/ascii_palette.h src/file/formats/palettes/binary_palette.h src/file/formats/palettes/ansi_palette.h src/file/formats/palettes/xterm256_palette.h
	$(CXX) $(CXXFLAGS) $< -o $@

font.o: src/file/formats/font.c src/file/file.h src/file/formats/screen.h src/file/formats/font.h src/file/formats/fonts/cp437_8x16.h
	$(CXX) $(CXXFLAGS) $< -o $@

screen.o: src/file/formats/screen.c src/file/formats/screen.h src/file/formats/palette.h src/file/formats/font.h src/file/sauce.h
	$(CXX) $(CXXFLAGS) $< -o $@

window.o: src/ui/window.c src/ui/window.h src/file/file.h src/ui/canvas.h src/ui/event.h
	$(CXX) $(CXXFLAGS) $< -o $@

event.o: src/ui/event.c src/ui/event.h src/ui/canvas.h
	$(CXX) $(CXXFLAGS) $< -o $@

text.o: src/text/text.c src/text/text.h src/text/utf8.h src/file/file.h src/file/formats/screen.h src/file/formats/palette.h
	$(CXX) $(CXXFLAGS) $< -o $@

utf8.o: src/text/utf8.c src/text/utf8.h
	$(CXX) $(CXXFLAGS) $< -o $@

artworx.o: src/file/formats/artworx.c src/file/formats/artworx.h src/file/file.h src/file/formats/screen.h src/file/formats/palette.h src/file/formats/font.h src/file/sauce.h
	$(CXX) $(CXXFLAGS) $< -o $@

ansi.o: src/file/formats/ansi.c src/file/formats/ansi.h src/file/file.h src/file/formats/screen.h src/file/formats/palette.h src/file/formats/font.h src/file/sauce.h
	$(CXX) $(CXXFLAGS) $< -o $@

ansiedit.o: src/file/formats/ansiedit.c src/file/formats/ansiedit.h src/file/file.h src/file/formats/screen.h src/file/formats/palette.h src/file/formats/font.h src/file/sauce.h
	$(CXX) $(CXXFLAGS) $< -o $@

ascii.o: src/file/formats/ascii.c src/file/formats/ascii.h src/file/file.h src/file/formats/screen.h src/file/formats/font.h src/file/sauce.h
	$(CXX) $(CXXFLAGS) $< -o $@

binary.o: src/file/formats/binary.c src/file/formats/binary.h src/file/file.h src/file/formats/screen.h src/file/formats/palette.h src/file/formats/font.h src/file/sauce.h
	$(CXX) $(CXXFLAGS) $< -o $@

icedraw.o: src/file/formats/icedraw.c src/file/formats/icedraw.h src/file/file.h src/file/formats/screen.h src/file/formats/palette.h src/file/formats/font.h src/file/sauce.h
	$(CXX) $(CXXFLAGS) $< -o $@

pcboard.o: src/file/formats/pcboard.c src/file/formats/pcboard.h src/file/file.h src/file/formats/screen.h src/file/formats/palette.h src/file/formats/font.h src/file/sauce.h
	$(CXX) $(CXXFLAGS) $< -o $@

tundra.o: src/file/formats/tundra.c src/file/formats/tundra.h src/file/file.h src/file/formats/screen.h src/file/formats/palette.h src/file/formats/font.h src/file/sauce.h
	$(CXX) $(CXXFLAGS) $< -o $@

xbin.o: src/file/formats/xbin.c src/file/formats/xbin.h src/file/file.h src/file/formats/screen.h src/file/formats/palette.h src/file/formats/font.h src/file/sauce.h
	$(CXX) $(CXXFLAGS) $< -o $@

canvas.o: src/ui/canvas.c src/ui/canvas.h src/file/file.h src/file/sauce.h src/file/formats/screen.h src/file/formats/palette.h src/file/formats/font.h
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f *.o
	rm -f $(OUT)