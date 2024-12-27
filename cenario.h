#ifndef CENARIO_H
#define CENARIO_H

#include <GL/glut.h>
#include <string>

// Funcoes de carregamento e desenho das texturas
void carregaTextura(GLuint tex_id, std::string filePath);
void desenhaFace(float v1[3], float v2[3], float v3[3], float v4[3], GLuint texid);
void desenhaCubo(float x, float y, float z, float tamanho, GLuint texIDSup, GLuint texIDOut);
void desenhaTerreno(int linhas, int colunas, float tamanhoCubo, GLuint texID[], int matrizMapa[11][16]);
void configurarIluminacao();

#endif
