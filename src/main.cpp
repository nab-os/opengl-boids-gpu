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

    //====Objects====
    Quad quad;
    quad.load();

    //====Shaders====
    Shader render("final");
    render.load();

    ComputeShader bird_init("bird_init");
    bird_init.load();

    ComputeShader bird_update("bird_update");
    bird_update.load();

    //====Texture====
    int bird_size = 70;
    cout << "Instanciating " << bird_size * bird_size << " birds" << endl;
    GLuint bird_positions;
    glGenTextures(1, &bird_positions);
    glBindTexture(GL_TEXTURE_2D, bird_positions);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, bird_size, bird_size, 0, GL_RGBA, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glBindImageTexture(0, bird_positions, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    glBindTexture(GL_TEXTURE_2D, 0);

    GLuint bird_movements;
    glGenTextures(1, &bird_movements);
    glBindTexture(GL_TEXTURE_2D, bird_movements);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, bird_size, bird_size, 0, GL_RGBA, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glBindImageTexture(1, bird_movements, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    glBindTexture(GL_TEXTURE_2D, 0);

    GLuint bird_final;
    glGenTextures(1, &bird_final);
    glBindTexture(GL_TEXTURE_2D, bird_final);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 1920, 1080, 0, GL_RGBA, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glBindImageTexture(2, bird_final, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    glBindTexture(GL_TEXTURE_2D, 0);

    //====================================
    int framerate = 1000/60;

    bird_init.compute(bird_size);
    glfwMakeContextCurrent(window);
    glViewport(0, 0, width, height);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    while(!glfwWindowShouldClose(window)) {
	    int startTime = int(glfwGetTime() * 1000);

        GLuint clearColor[4] = {0, 0, 0, 0};
        glClearTexImage(bird_final, 0, GL_BGRA, GL_UNSIGNED_BYTE, &clearColor);
        bird_update.compute(bird_size);

        glClearColor(0.2, 0.2, 0.2, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(render.getProgramID());
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, bird_final);
            quad.render();
        glUseProgram(0);

        glfwSwapBuffers(window);
        glfwPollEvents();

        int endTime = int(glfwGetTime() * 1000);
        int elapsedTime = endTime - startTime;

        if(elapsedTime < framerate)
            this_thread::sleep_for(std::chrono::milliseconds(framerate - elapsedTime));

        //int endTime2 = int(glfwGetTime() * 1000);
        //int elapsedTime2 = endTime2 - startTime;
        //cout << 1000/elapsedTime2 << " fps" << endl;
    };

    return EXIT_SUCCESS;
}
