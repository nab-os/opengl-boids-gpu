#pragma once

#include <string>

#include <GL/glew.h>

class Quad {
    private:
        GLuint m__programID;
        GLuint m__idVAO;
        GLuint m__idVBO;

    public:
        Quad();
        ~Quad();

        void load();
        void render();
};
