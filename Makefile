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

$(OUT): main.o file.o sauce.o palette.o font.o screen.o artworx.o ascii.o binary.o icedraw.o pcboard.o tundra.o xbin.o window.o event.o canvas.o
	$(CXX) $(LDFLAGS) $^ -o $@

main.o: src/main.c src/file/file.h src/image/canvas.h src/ui/window.h
	$(CXX) $(CXXFLAGS) $< -o $@

file.o: src/file/file.c src/file/file.h src/file/formats/screen.h src/file/sauce.h src/image/canvas.h src/file/formats/artworx.h src/file/formats/ascii.h src/file/formats/binary.h src/file/formats/icedraw.h src/file/formats/pcboard.h src/file/formats/tundra.h src/file/formats/xbin.h
	$(CXX) $(CXXFLAGS) $< -o $@

sauce.o: src/file/sauce.c src/file/sauce.h
	$(CXX) $(CXXFLAGS) $< -o $@

palette.o: src/file/formats/palette.c src/file/formats/palette.h src/file/formats/palettes/ascii_palette.h src/file/formats/palettes/binary_palette.h
	$(CXX) $(CXXFLAGS) $< -o $@

font.o: src/file/formats/font.c src/file/file.h src/file/formats/screen.h src/file/formats/font.h src/file/formats/fonts/cp437_8x16.h
	$(CXX) $(CXXFLAGS) $< -o $@

screen.o: src/file/formats/screen.c src/file/formats/screen.h src/file/formats/palette.h src/file/formats/font.h src/file/sauce.h
	$(CXX) $(CXXFLAGS) $< -o $@

window.o: src/ui/window.c src/ui/window.h src/image/canvas.h src/ui/event.h
	$(CXX) $(CXXFLAGS) $< -o $@

artworx.o: src/file/formats/artworx.c src/file/file.h src/file/formats/screen.h src/file/formats/palette.h src/file/formats/font.h src/file/sauce.h
	$(CXX) $(CXXFLAGS) $< -o $@

ascii.o: src/file/formats/ascii.c src/file/file.h src/file/formats/screen.h src/file/formats/font.h src/file/sauce.h
	$(CXX) $(CXXFLAGS) $< -o $@

binary.o: src/file/formats/binary.c src/file/file.h src/file/formats/screen.h src/file/formats/palette.h src/file/formats/font.h src/file/sauce.h
	$(CXX) $(CXXFLAGS) $< -o $@

icedraw.o: src/file/formats/icedraw.c src/file/file.h src/file/formats/screen.h src/file/formats/palette.h src/file/formats/font.h src/file/sauce.h
	$(CXX) $(CXXFLAGS) $< -o $@

pcboard.o: src/file/formats/pcboard.c src/file/file.h src/file/formats/screen.h src/file/formats/palette.h src/file/formats/font.h src/file/sauce.h
	$(CXX) $(CXXFLAGS) $< -o $@

tundra.o: src/file/formats/tundra.c src/file/file.h src/file/formats/screen.h src/file/formats/palette.h src/file/formats/font.h src/file/sauce.h
	$(CXX) $(CXXFLAGS) $< -o $@

xbin.o: src/file/formats/xbin.c src/file/file.h src/file/formats/screen.h src/file/formats/palette.h src/file/formats/font.h src/file/sauce.h
	$(CXX) $(CXXFLAGS) $< -o $@

canvas.o: src/image/canvas.c src/image/canvas.h src/file/formats/screen.h
	$(CXX) $(CXXFLAGS) $< -o $@

event.o: src/ui/event.c src/image/canvas.h
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f *.o
	rm -f $(OUT)