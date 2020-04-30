#pragma once

#include <GL/glew.h>

// Includes GLM
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>

class ComputeShader {
    private:
        GLuint m__programID;
        GLuint m__computeID;

        std::string m__computeShaderFile;

        bool compilerShader(GLuint &shader, GLenum type, std::string const &fichierSource);

    public:
        ComputeShader(std::string shaderName);
        ~ComputeShader();

        bool load();
        void compute(glm::ivec2 size);
};
