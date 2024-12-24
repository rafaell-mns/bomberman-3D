#include <GL/glut.h>
#include "cenario.h"

// Cores
color verde = {0.0, 0.8, 0.0};
color preto = {0.0, 0.0, 0.0};

// Desenha uma face do cubo
void desenhaFace(float v1[3], float v2[3], float v3[3], float v4[3]) {
    // Preenche a face
    glBegin(GL_QUADS);
    glColor3fv(verde);
    glVertex3fv(v1);
    glVertex3fv(v2);
    glVertex3fv(v3);
    glVertex3fv(v4);
    glEnd();

    // Contorno preto
    glBegin(GL_LINE_LOOP);
    glColor3fv(preto);
    glVertex3fv(v1);
    glVertex3fv(v2);
    glVertex3fv(v3);
    glVertex3fv(v4);
    glEnd();
}

// Desenha um cubo
void desenhaCubo(float x, float y, float z, float tamanho) {
    float d = tamanho / 2.0;

    // Vertices
    float v1[3] = {x - d, y + d, z + d};
    float v2[3] = {x - d, y - d, z + d};
    float v3[3] = {x + d, y - d, z + d};
    float v4[3] = {x + d, y + d, z + d};
    float v5[3] = {x + d, y + d, z - d};
    float v6[3] = {x + d, y - d, z - d};
    float v7[3] = {x - d, y - d, z - d};
    float v8[3] = {x - d, y + d, z - d};

    // Desenha as faces
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0, 1.0);

    desenhaFace(v1, v2, v3, v4);
    desenhaFace(v4, v3, v6, v5);
    desenhaFace(v5, v6, v7, v8);
    desenhaFace(v8, v7, v2, v1);
    desenhaFace(v1, v4, v5, v8);
    desenhaFace(v2, v7, v6, v3);

    glDisable(GL_POLYGON_OFFSET_FILL);
}

// Desenha o terreno com os cubos
void desenhaTerreno(int linhas, int colunas, float tamanhoCubo) {
    float inicioX = -colunas * tamanhoCubo / 2.0; // Centraliza o terreno no eixo X
    float inicioZ = -linhas * tamanhoCubo / 2.0;  // Centraliza o terreno no eixo Z

    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            float x = inicioX + j * tamanhoCubo;
            float z = inicioZ + i * tamanhoCubo;
            desenhaCubo(x, 0, z, tamanhoCubo);
        }
    }
}
