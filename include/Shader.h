#pragma once

#include <string>

#include <GL/glew.h>

// Includes GLM
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
    public:
        Shader(std::string shaderName = "default");
        virtual ~Shader();

        bool load();

        GLuint getProgramID() { return m__programID; };

        void sendMat4Uniform(std::string nom, glm::mat4 matrice);
        void sendMat3Uniform(std::string nom, glm::mat3 matrice);
        void sendVec4Uniform(std::string nom, glm::vec4 vecteur);
        void sendVec3Uniform(std::string nom, glm::vec3 vecteur);
        void sendVec2Uniform(std::string nom, glm::vec2 vecteur);
        void sendIntUniform(std::string nom, GLuint id);
        void sendFloatUniform(std::string nom, float val);

    private:
        GLuint m__programID;

        GLuint m__vertexID;
        GLuint m__fragmentID;

        std::string m__vertexShaderFile;
        std::string m__fragmentShaderFile;

        bool compile(GLuint &shader, GLenum type, std::string const &fichierSource);
};
