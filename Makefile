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

OUT = anscat

all: $(OUT)

$(OUT): main.o file.o sauce.o xbin.o artworx.o renderer.o window.o event.o canvas.o
	$(CXX) $(LDFLAGS) $^ -o $@

main.o: src/main.c src/file/file.h src/image/canvas.h src/ui/window.h
	$(CXX) $(CXXFLAGS) $< -o $@

file.o: src/file/file.c src/file/file.h src/image/canvas.h src/file/xbin.h src/file/artworx.h
	$(CXX) $(CXXFLAGS) $< -o $@

sauce.o: src/file/sauce.c src/file/sauce.h
	$(CXX) $(CXXFLAGS) $< -o $@

renderer.o: src/image/renderer.c src/image/canvas.h
	$(CXX) $(CXXFLAGS) $< -o $@

window.o: src/ui/window.c src/ui/window.h src/image/canvas.h src/ui/event.h
	$(CXX) $(CXXFLAGS) $< -o $@

xbin.o: src/file/xbin.c src/file/xbin.h src/file/sauce.h src/image/canvas.h src/image/renderer.h
	$(CXX) $(CXXFLAGS) $< -o $@

artworx.o: src/file/artworx.c src/file/artworx.h src/file/sauce.h src/image/canvas.h src/image/renderer.h
	$(CXX) $(CXXFLAGS) $< -o $@

canvas.o: src/image/canvas.c src/image/canvas.h
	$(CXX) $(CXXFLAGS) $< -o $@

event.o: src/ui/event.c src/image/canvas.h
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f *.o
	rm -f $(OUT)