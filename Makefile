CXX = clang
CXXFLAGS = -Wall -c -std=c99 -I/Library/Frameworks/SDL2.framework/Headers
LDFLAGS = -F/Library/Frameworks -framework SDL2
OUT = anscat

all: $(OUT)

$(OUT): main.o xbin.o renderer.o window.o event.o canvas.o
	$(CXX) $(LDFLAGS) $^ -o $@

main.o: main.c xbin.h window.h canvas.h renderer.h
	$(CXX) $(CXXFLAGS) $< -o $@

renderer.o: renderer.c canvas.h xbin.h
	$(CXX) $(CXXFLAGS) $< -o $@

window.o: window.c renderer.h canvas.h event.h
	$(CXX) $(CXXFLAGS) $< -o $@

xbin.o: xbin.c xbin.h canvas.h
	$(CXX) $(CXXFLAGS) $< -o $@

canvas.o: canvas.c canvas.h
	$(CXX) $(CXXFLAGS) $< -o $@

event.o: event.c canvas.h
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm $(OUT)