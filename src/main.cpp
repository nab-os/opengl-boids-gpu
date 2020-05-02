#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <fstream>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

// Includes GLM
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Quad.h"
#include "Shader.h"
#include "ComputeShader.h"

using namespace std;
using namespace glm;

int window_width = 1920;
int window_height = 1080;
int viewport_width = 1920;
int viewport_height = 1080;
bool resized = false;

void framebuffer_size_callback(GLFWwindow* window, int w, int h) {
    window_width = w;
    window_height = h;
    if (w < h) {
        viewport_width = w;
        viewport_height = (9 * w) / 16;
    } else {
        viewport_width = (16 * h) / 9;
        viewport_height = h;
    }
    glViewport((window_width - viewport_width) / 2, (window_height - viewport_height) / 2, viewport_width, viewport_height);
    resized = true;
}

int main(int argc, char **argv) {
    int bird_number(200);
    if (argc == 2) {
        bird_number = atoi(argv[1]);
    }

    int err = glfwInit();
    if (!err) {
        std::cout << "Failed to initialize GLFW: " << err << "\n";
        exit(EXIT_FAILURE);
    }

    //=====================
    int width = 1920;
    int height = 1080;
    std::cout << "Screen resolution: " << width << "x" << height << std::endl;

    GLFWwindow* window = glfwCreateWindow(width, height, "Test boids", NULL, NULL);
    if (!window) {
        std::cout << "Erreur lors de la creation de la fenetre" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetWindowSizeCallback(window, framebuffer_size_callback);
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);

    //=====================
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); //OpenGL 4.6
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // On initialise GLEW
    GLenum initialisationGLEW(glewInit());

    // Si l'initialisation a échoué :
    if (initialisationGLEW != GLEW_OK) {
        // On affiche l'erreur grâce à la fonction : glewGetErrorString(GLenum code)
        std::cout << "Erreur d'initialisation de GLEW : " << glewGetErrorString(initialisationGLEW) << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetWindowShouldClose(window, false);

    //===========INITIALIZATION===========
    Quad quad;
    quad.load();

    //====Shaders====
    Shader render("final");
    render.load();

    Shader background("background");
    background.load();

    ComputeShader bird_init("bird_init");
    bird_init.load();

    ComputeShader heatmaps("heatmaps");
    heatmaps.load();

    ComputeShader bird_update("bird_update");
    bird_update.load();


    //====Shader data====

    GLuint positions;
    GLuint movements;
    glGenBuffers(1, &positions);
    glGenBuffers(1, &movements);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, positions);
        glBufferData(GL_SHADER_STORAGE_BUFFER, bird_number * sizeof(vec2), 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_ARRAY_BUFFER, positions);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, movements);
        glBufferData(GL_SHADER_STORAGE_BUFFER, bird_number * sizeof(vec2), 0, GL_DYNAMIC_COPY);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, positions);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, movements);

    //====Texture====
    ivec2 grid_size(1920, 1080);
    GLuint center_heatmap;
    GLuint repel_heatmap;
    glGenTextures(1, &center_heatmap);
    glGenTextures(1, &repel_heatmap);

    glBindTexture(GL_TEXTURE_2D, center_heatmap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, grid_size.x, grid_size.y, 0, GL_RGBA, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glBindImageTexture(2, center_heatmap, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

    glBindTexture(GL_TEXTURE_2D, repel_heatmap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, grid_size.x, grid_size.y, 0, GL_RGBA, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glBindImageTexture(3, repel_heatmap, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    glBindTexture(GL_TEXTURE_2D, 0);

    //====================================
    int framerate = 1000/60;

    GLuint clearColor[4] = {0, 0, 0, 0};

    bird_init.compute(bird_number);
    glfwMakeContextCurrent(window);
    glViewport(0, 0, window_width, window_height);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    cout << "Running: " << endl;
    while(!glfwWindowShouldClose(window)) {
        int startTime = int(glfwGetTime() * 1000);

        if (resized) {
            glClearTexImage(center_heatmap, 0, GL_BGRA, GL_UNSIGNED_BYTE, &clearColor);
            glClearTexImage(repel_heatmap, 0, GL_BGRA, GL_UNSIGNED_BYTE, &clearColor);
            heatmaps.compute(bird_number);
            bird_update.compute(bird_number);
        }

        glClearColor(0.1, 0.1, 0.1, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(background.getProgramID());
            background.sendIntUniform("center_heatmap", 0);
            background.sendIntUniform("repel_heatmap", 1);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, center_heatmap);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, repel_heatmap);
            quad.render();
        glUseProgram(render.getProgramID());
            glDrawArrays(GL_POINTS, 0, bird_number);
        glUseProgram(0);

        glfwSwapBuffers(window);
        glfwPollEvents();

        int endTime = int(glfwGetTime() * 1000);
        int elapsedTime = endTime - startTime;

        if(elapsedTime < framerate)
            this_thread::sleep_for(std::chrono::milliseconds(framerate - elapsedTime));

        int endTime2 = int(glfwGetTime() * 1000);
        int elapsedTime2 = endTime2 - startTime;
        cout << '\r' << 1000/elapsedTime2 << " fps" << std::flush;
    };
    cout << endl;

    return EXIT_SUCCESS;
}
