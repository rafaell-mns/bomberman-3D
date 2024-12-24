#include <GL/glut.h>
#include "cenario.h"
#include <math.h>

// Cores
color verde = {0.0, 0.8, 0.0};
color preto = {0.0, 0.0, 0.0};

// Função para calcular a normal de uma face com base nos vértices
void calculaNormal(float v1[3], float v2[3], float v3[3], float normal[3]) {
    // Vetores AB e AC
    float AB[3] = {v2[0] - v1[0], v2[1] - v1[1], v2[2] - v1[2]};
    float AC[3] = {v3[0] - v1[0], v3[1] - v1[1], v3[2] - v1[2]};

    // Produto vetorial (normal = AB x AC)
    normal[0] = AB[1] * AC[2] - AB[2] * AC[1];
    normal[1] = AB[2] * AC[0] - AB[0] * AC[2];
    normal[2] = AB[0] * AC[1] - AB[1] * AC[0];

    // Normaliza a normal
    float magnitude = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
    normal[0] /= magnitude;
    normal[1] /= magnitude;
    normal[2] /= magnitude;
}

void configurarIluminacao() {
    // Inicia a iluminação
    GLfloat light_position[] = {-1.0, 1.0, 1.0, 0.0};
    GLfloat light_color[] = {1.0, 1.0, 1.0, 0.0};
    glLightfv(GL_LIGHT0, GL_AMBIENT_AND_DIFFUSE, light_color);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable(GL_LIGHTING); // Ativa o modelo de iluminação
    glEnable(GL_LIGHT0); // Ativa a Luz 0. O OpenGL suporta pelo menos 8 pontos de luz

    // Inicia as características gerais dos materiais
    GLfloat mat_ambient_diffuse[] = {1.0, 1.0, 1.0, 1.0};
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_ambient_diffuse);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL); // Um ou mais parâmetros do material rastreiam a cor atual do glColor.

    // Ativa o modelo de sombreamento de Gouraud.
    glShadeModel(GL_SMOOTH);

    // Ativa o z-buffering, de modo a remover as superfícies escondidas
    glEnable(GL_DEPTH_TEST);
}

// Desenha uma face do cubo com normal
void desenhaFace(float v1[3], float v2[3], float v3[3], float v4[3]) {
    // Calcula a normal da face
    float normal[3];
    calculaNormal(v1, v2, v3, normal);
    glNormal3fv(normal);  // Aplica a normal calculada à face

    // Desenha a face
    glBegin(GL_QUADS);
    glColor3fv(verde);  // Cor da face
    glVertex3fv(v1);
    glVertex3fv(v2);
    glVertex3fv(v3);
    glVertex3fv(v4);
    glEnd();

    // Desenha o contorno preto
    glBegin(GL_LINE_LOOP);
    glColor3fv(preto);  // Contorno preto
    glVertex3fv(v1);
    glVertex3fv(v2);
    glVertex3fv(v3);
    glVertex3fv(v4);
    glEnd();
}

// Desenha um cubo com a luz difusa e calculando as normais
void desenhaCubo(float x, float y, float z, float tamanho) {
    float d = tamanho / 2.0;

    // Vértices do cubo
    float v1[3] = {x - d, y + d, z + d};
    float v2[3] = {x - d, y - d, z + d};
    float v3[3] = {x + d, y - d, z + d};
    float v4[3] = {x + d, y + d, z + d};
    float v5[3] = {x + d, y + d, z - d};
    float v6[3] = {x + d, y - d, z - d};
    float v7[3] = {x - d, y - d, z - d};
    float v8[3] = {x - d, y + d, z - d};

    // Desenha as faces do cubo
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
