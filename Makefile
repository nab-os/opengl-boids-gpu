EXEC=boids

SOURCES=main.cpp Quad.cpp Shader.cpp ComputeShader.cpp

OUT_DIR=bin
OBJ_DIR=obj

OBJECTS=${SOURCES:%.cpp=$(OBJ_DIR)/%.o}

ifeq ($(OS),Windows_NT)

    IDIR=-I include \
         -I C://Mingw/include \
         -I C://Mingw/glew/include \
         -I C://Mingw/glfw/include \
         -I C://Mingw/glm

    LDIR=-L C://Mingw/lib \
         -L C://MinGW/glew/lib/Release/Win32 \
         -L C://Mingw/glfw/lib-mingw

    CFLAGS=-Wall -Wextra -O3 -g3 -std=c++14 -Wno-unused-variable -Wno-unused-parameter
    LFLAGS=-lstdc++ \
           -lglfw3 -lopengl32 -lglu32 -lgdi32 -lglew32
else

    IDIR=-I include \
         -I /usr/include \
         -I /usr/include/libdrm

    LDIR=-L /usr/lib \
         -L /usr/local/lib \
         -L /usr/lib64 \
         -L /usr/lib/x86_64-linux-gnu

    CFLAGS=-Wall -Wextra -O3 -g3 -std=c++14 -Wno-unused-variable -Wno-unused-parameter
    LFLAGS=-lstdc++ \
           -lXi -lGLEW -lm -lGL -lm -lpthread -ldl -ldrm \
           -lXdamage -lX11-xcb -lxcb-glx -lxcb-dri2 -lXxf86vm \
           -lglfw -lrt -lm -ldl -lXrandr -lXinerama -lXext -lXcursor -lXrender -lXfixes -lX11 \
           -lpthread -lxcb -lXau -lXdmcp \
           -lpcre -lxml2 -lLLVM
endif

CC=g++

all: clean init $(OUT_DIR)/$(EXEC)

$(OUT_DIR)/$(EXEC): $(OBJECTS)
	@echo "\n=======LINKING=======\n"
	$(CC) -o $@ $^ $(LDIR) $(LFLAGS)

$(OBJ_DIR)/%.o: src/%.cpp
	@echo "\n=======COMPILING $@======="
	$(CC) -o $@ -c $< $(IDIR) $(CFLAGS)

init:
	@mkdir -p "$(OUT_DIR)"
	@mkdir -p "$(OBJ_DIR)"

clean:
	@rm -rf "$(OUT_DIR)"
	@rm -rf "$(OBJ_DIR)"
