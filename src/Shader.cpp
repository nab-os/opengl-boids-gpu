#include "Shader.h"

#include <iostream>
#include <fstream>

using std::cout;
using std::endl;
using std::string;
using std::ifstream;
using glm::ivec2;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

Shader::Shader(string shaderName): m__programID(),
										m__vertexID(),
										m__fragmentID(),
										m__vertexShaderFile("Shaders/" + shaderName + "/shader.vert"),
										m__fragmentShaderFile("Shaders/" + shaderName + "/shader.frag") {
	cout << this << " [Shader] constructor" << endl;
}

Shader::~Shader() {}

bool Shader::load() {
	cout << this << " [Shader] load" << endl;
	if (glIsShader(m__vertexID) == GL_TRUE)
		glDeleteShader(m__vertexID);

	if (glIsShader(m__fragmentID) == GL_TRUE)
		glDeleteShader(m__fragmentID);

	if (glIsProgram(m__programID) == GL_TRUE)
		glDeleteProgram(m__programID);

    cout << this << " [Shader]: " << m__programID << endl;
	cout << this << " [Shader] load() 1 : " << m__vertexShaderFile << endl;
	cout << this << " [Shader] load() 2 : " << m__fragmentShaderFile << endl;

	if (!compile(m__vertexID, GL_VERTEX_SHADER, m__vertexShaderFile))
		return false;

	if (!compile(m__fragmentID, GL_FRAGMENT_SHADER, m__fragmentShaderFile))
		return false;

	m__programID = glCreateProgram();

	glBindAttribLocation(m__programID, 0, "in_Vertex");
	glAttachShader(m__programID, m__vertexID);
	glAttachShader(m__programID, m__fragmentID);
	glLinkProgram(m__programID);

	GLint erreurLink(0);
	glGetProgramiv(m__programID, GL_LINK_STATUS, &erreurLink);

	if (erreurLink != GL_TRUE) {
		GLint tailleErreur = 0;
		glGetProgramiv(m__programID, GL_INFO_LOG_LENGTH, &tailleErreur);

		char *erreur = new char[tailleErreur + 1];
		glGetShaderInfoLog(m__programID, tailleErreur, &tailleErreur, erreur);
		erreur[tailleErreur] = '\0';
		cout << erreur << endl;
		delete[] erreur;

		glDeleteProgram(m__programID);
		return false;
	} else {
		return true;
	}
}

bool Shader::compile(GLuint &shader, GLenum type, string const &fichierSource) {
	shader = glCreateShader(type);
	if (shader == 0) {
		cout << "Erreur, le type de shader (" << type << ") n'existe pas" << endl;
		return false;
	}

	ifstream fichier(fichierSource.c_str());
	if (!fichier) {
		cout << "Erreur le fichier " << fichierSource << " est introuvable" << endl;
		glDeleteShader(shader);

		return false;
	}

	string ligne;
	string codeSource;
	while (getline(fichier, ligne))
		codeSource += ligne + '\n';
	fichier.close();

	const GLchar* chaineCodeSource = codeSource.c_str();
	glShaderSource(shader, 1, &chaineCodeSource, 0);
	glCompileShader(shader);

	GLint erreurCompilation(0);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &erreurCompilation);
	if (erreurCompilation != GL_TRUE) {
		GLint tailleErreur(0);
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &tailleErreur);

		char *erreur = new char[tailleErreur + 1];
		glGetShaderInfoLog(shader, tailleErreur, &tailleErreur, erreur);
		erreur[tailleErreur] = '\0';
		cout << erreur << endl;
		delete[] erreur;

		glDeleteShader(shader);
		return false;
	} else {
		return true;
    }
}

void Shader::sendMat4Uniform(string nom, mat4 matrice) {
	int localisation = glGetUniformLocation(m__programID, nom.c_str());
	glUniformMatrix4fv(localisation, 1, GL_FALSE, value_ptr(matrice));
}

void Shader::sendMat3Uniform(string nom, mat3 matrice) {
	int localisation = glGetUniformLocation(m__programID, nom.c_str());
	glUniformMatrix3fv(localisation, 1, GL_FALSE, value_ptr(matrice));
}

void Shader::sendVec4Uniform(string nom, vec4 vecteur) {
	int localisation = glGetUniformLocation(m__programID, nom.c_str());
	glUniform4f(localisation, vecteur.x, vecteur.y, vecteur.z, vecteur.w);
}

void Shader::sendVec3Uniform(string nom, vec3 vecteur) {
	int localisation = glGetUniformLocation(m__programID, nom.c_str());
	glUniform3f(localisation, vecteur.x, vecteur.y, vecteur.z);
}

void Shader::sendVec2Uniform(string nom, vec2 vecteur) {
	int localisation = glGetUniformLocation(m__programID, nom.c_str());
	glUniform2f(localisation, vecteur.x, vecteur.y);
}

void Shader::sendIvec2Uniform(string nom, ivec2 vecteur) {
	int localisation = glGetUniformLocation(m__programID, nom.c_str());
	glUniform2i(localisation, vecteur.x, vecteur.y);
}

void Shader::sendIntUniform(string nom, GLuint id) {
	int localisation = glGetUniformLocation(m__programID, nom.c_str());
	glUniform1i(localisation, id);
}

void Shader::sendFloatUniform(string nom, float val) {
	int localisation = glGetUniformLocation(m__programID, nom.c_str());
	glUniform1f(localisation, val);
}
