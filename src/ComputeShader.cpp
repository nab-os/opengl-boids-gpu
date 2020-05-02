#include "ComputeShader.h"

#include <iostream>
#include <fstream>

using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::ifstream;

ComputeShader::ComputeShader(string shaderName):
    m__computeShaderFile("Shaders/" + shaderName + "/shader.comp") {
	cout << this << " [ComputeShader] constructor" << endl;
}


bool ComputeShader::load() {
	if (glIsShader(m__computeID) == GL_TRUE)
		glDeleteShader(m__computeID);

    if (!compilerShader(m__computeID, GL_COMPUTE_SHADER, m__computeShaderFile))
		return false;

    // Création du programme
	m__programID = glCreateProgram();

	// Linkage du programme
	glAttachShader(m__programID, m__computeID);
	glLinkProgram(m__programID);

    // Vérification du linkage
	GLint erreurLink(0);
	glGetProgramiv(m__programID, GL_LINK_STATUS, &erreurLink);

	// S'il y a eu une erreur
	if (erreurLink != GL_TRUE) {
		// Récupération de la taille de l'erreur
		GLint tailleErreur = 0;
		glGetProgramiv(m__programID, GL_INFO_LOG_LENGTH, &tailleErreur);

		// Allocation de mémoire
		char *erreur = new char[tailleErreur + 1];

		// Récupération de l'erreur
		glGetShaderInfoLog(m__programID, tailleErreur, &tailleErreur, erreur);
		erreur[tailleErreur] = '\0';

		// Affichage de l'erreur
		cout << erreur << endl;

		// Libération de la mémoire et retour du booléen false
		delete[] erreur;
		glDeleteProgram(m__programID);

		return false;
	} else { // Sinon c'est que tout s'est bien passé
		return true;
	}
}

ComputeShader::~ComputeShader() {
	cout << this << " [ComputeShader] destructor" << endl;
}

bool ComputeShader::compilerShader(GLuint &shader, GLenum type, string const &fichierSource) {
	// Création du shader
	shader = glCreateShader(type);
    cout << this << " [ComputeShader] compiling: " << fichierSource << endl;

	// Vérification du shader
	if (shader == 0) {
		cout << "Erreur, le type de shader (" << type << ") n'existe pas" << endl;
		return false;
	}

	// Flux de lecture
	ifstream fichier(fichierSource.c_str());

	// Test d'ouverture
	if (!fichier) {
		cout << "Erreur le fichier " << fichierSource << " est introuvable" << endl;
		glDeleteShader(shader);

		return false;
	}

	// Strings permettant de lire le code source
	string ligne;
	string codeSource;

	// Lecture
	while (getline(fichier, ligne))
		codeSource += ligne + '\n';

	// Fermeture du fichier
	fichier.close();

	// Récupération de la chaine C du code source
	const GLchar* chaineCodeSource = codeSource.c_str();

	// Envoi du code source au shader
	glShaderSource(shader, 1, &chaineCodeSource, 0);

	// Compilation du shader
	glCompileShader(shader);

	// Vérification de la compilation
	GLint erreurCompilation(0);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &erreurCompilation);

	// S'il y a eu une erreur
	if (erreurCompilation != GL_TRUE) {
		// Récupération de la taille de l'erreur
		GLint tailleErreur(0);
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &tailleErreur);

		// Allocation de mémoire
		char *erreur = new char[tailleErreur + 1];

		// Récupération de l'erreur
		glGetShaderInfoLog(shader, tailleErreur, &tailleErreur, erreur);
		erreur[tailleErreur] = '\0';

		// Affichage de l'erreur
		cout << erreur << endl;

		// Libération de la mémoire et retour du booléen false
		delete[] erreur;
		glDeleteShader(shader);

		return false;
	} else {
		return true;
    }
}

void ComputeShader::compute(int width, int height) {
    glUseProgram(m__programID);
        int localisation = glGetUniformLocation(m__programID, "grid_size");
        glUniform2i(localisation, width, height);
        glDispatchCompute(width, height, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}
