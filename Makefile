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

$(OUT): main.o xbin.o renderer.o window.o event.o canvas.o
	$(CXX) $(LDFLAGS) $^ -o $@

main.o: main.c xbin.h canvas.h window.h
	$(CXX) $(CXXFLAGS) $< -o $@

renderer.o: renderer.c canvas.h
	$(CXX) $(CXXFLAGS) $< -o $@

window.o: window.c canvas.h event.h
	$(CXX) $(CXXFLAGS) $< -o $@

xbin.o: xbin.c xbin.h canvas.h renderer.h
	$(CXX) $(CXXFLAGS) $< -o $@

canvas.o: canvas.c canvas.h
	$(CXX) $(CXXFLAGS) $< -o $@

event.o: event.c canvas.h
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm *.o
	rm $(OUT)