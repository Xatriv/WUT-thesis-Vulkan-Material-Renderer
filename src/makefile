CC = g++
GLSLC = /usr/local/bin/glslc
CFLAGS = -std=c++17 -O3
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

vertSources = $(shell find ../shaders -type f -name "*.vert")
vertObjFiles = $(patsubst %.vert, %.vert.spv, $(vertSources))
fragSources = $(shell find ../shaders -type f -name "*.frag")
fragObjFiles = $(patsubst %.frag, %.frag.spv, $(fragSources))

VMR.out: $(vertObjFiles) $(fragObjFiles)
VMR.out: main.cpp
	$(CC) $(CFLAGS) -o $@ main.cpp $(LDFLAGS)

.PHONY: test clean

%.spv: %
	$(GLSLC) $< -o $@

test: VMR
	./VMR.out

clean:
	rm -f VMR.out
	rm -f shaders/*.spv