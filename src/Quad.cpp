#include "Quad.h"

#include <iostream>
#include <vector>

// Includes GLM
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef BUFFER_OFFSET
#define BUFFER_OFFSET(offset) ((char*)NULL + (offset))
#endif

using std::cout;
using std::endl;
using std::vector;
using glm::vec3;

Quad::Quad() {
	cout << this << " [Quad] constructor" << endl;
}

Quad::~Quad() {
	cout << this << " [Quad] destructor" << endl;
}

void Quad::load() {
	int sizeVerticesBytes = sizeof(float) * 12;

	float vertices[] = {-1.0, -1.0,   -1.0, 1.0,    1.0, -1.0,
                        -1.0, 1.0,    1.0, 1.0,     1.0, -1.0};

	if (glIsBuffer(m__idVBO) == GL_TRUE)
		glDeleteBuffers(1, &m__idVBO);

	glGenBuffers(1, &m__idVBO);

	glBindBuffer(GL_ARRAY_BUFFER, m__idVBO);

		glBufferData(GL_ARRAY_BUFFER, sizeVerticesBytes, 0, GL_STATIC_DRAW);

		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeVerticesBytes, vertices);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	if (glIsVertexArray(m__idVAO) == GL_TRUE)
		glDeleteVertexArrays(1, &m__idVAO);

	glGenVertexArrays(1, &m__idVAO);

	glBindVertexArray(m__idVAO);

		glBindBuffer(GL_ARRAY_BUFFER, m__idVBO);

			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
			glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void Quad::render() {
    glBindVertexArray(m__idVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
