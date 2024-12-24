#ifndef CENARIO_H
#define CENARIO_H

// Cores
typedef float color[3];
extern color verde;
extern color preto;

// Funcoes
void calculaNormal(float v1[3], float v2[3], float v3[3], float normal[3]);
void configurarIluminacao();
void desenhaFace(float v1[3], float v2[3], float v3[3], float v4[3]);
void desenhaCubo(float x, float y, float z, float tamanho);
void desenhaTerreno(int linhas, int colunas, float tamanhoCubo);

#endif