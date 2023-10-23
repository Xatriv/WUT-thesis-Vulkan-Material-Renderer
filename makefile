CC = g++
BUILD_DIR = build
SRC_DIR = src
GLSLC = /usr/local/bin/glslc
CFLAGS = -std=c++17 -O3
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi #-I./include

cppSources = $(wildcard $(SRC_DIR)/*.cpp)
vertSources = $(shell find ./shaders -type f -name "*.vert")
vertObjFiles = $(patsubst %.vert, %.vert.spv, $(vertSources))
fragSources = $(shell find ./shaders -type f -name "*.frag")
fragObjFiles = $(patsubst %.frag, %.frag.spv, $(fragSources))

TARGET = $(BUILD_DIR)/VMR.out

$(TARGET): $(vertObjFiles) $(fragObjFiles)
$(TARGET): $(cppSources)
	$(CC) $(CFLAGS) -o $(TARGET) $(cppSources) $(LDFLAGS) 2> error.txt

%.spv: %
	$(GLSLC) $< -o $@

.PHONY: test clean

test: VMR
	./VMR.out


clean:
	rm -f $(TARGET)
	rm -f shaders/*.spv