#include <stdio.h>
#include <iostream>
#include <GL/glut.h>
#include "stb_image.h"

// Carregar a textura
void carregaTextura(GLuint tex_id, std::string filePath){
    unsigned char* imgData;
    int largura, altura, canais;
    
    filePath = "imagens/" + filePath + ".jpg";
    stbi_set_flip_vertically_on_load(true);
    imgData = stbi_load(filePath.c_str(), &largura, &altura, &canais, 4);
    
    if (imgData){
        glBindTexture(GL_TEXTURE_2D, tex_id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, largura, altura, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgData);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        stbi_image_free(imgData);
    }
}

// Funcao para desenhar uma face com a textura
void desenhaFace(float v1[3], float v2[3], float v3[3], float v4[3], GLuint texid) {
    glBindTexture(GL_TEXTURE_2D, texid);

    glBegin(GL_QUADS);
    glTexCoord2f(0, 1); glVertex3fv(v1); 
    glTexCoord2f(0, 0); glVertex3fv(v2); 
    glTexCoord2f(1, 0); glVertex3fv(v3); 
    glTexCoord2f(1, 1); glVertex3fv(v4);   
    glEnd();
}

// Funcao para desenhar o cubo com diferentes texturas nas faces
void desenhaCubo(float x, float y, float z, float tamanho, GLuint texIDSup, GLuint texIDOut) {
    float d = tamanho / 2;

    float v1[3] = {x - d, y + d, z + d};
    float v2[3] = {x - d, y - d, z + d};
    float v3[3] = {x + d, y - d, z + d};
    float v4[3] = {x + d, y + d, z + d};
    float v5[3] = {x + d, y + d, z - d};
    float v6[3] = {x + d, y - d, z - d};
    float v7[3] = {x - d, y - d, z - d};
    float v8[3] = {x - d, y + d, z - d};

    desenhaFace(v1, v4, v5, v8, texIDSup); // Face superior
    desenhaFace(v1, v2, v3, v4, texIDOut); // Face frontal
    desenhaFace(v4, v3, v6, v5, texIDOut); // Face direita
    desenhaFace(v5, v6, v7, v8, texIDOut); // Face traseira
    desenhaFace(v8, v7, v2, v1, texIDOut); // Face esquerda
    //desenhaFace(v2, v7, v6, v3, texIDOut); // Face inferior (desnecessário?)
}

// Funcao para desenhar muros e caixas acima do chão do terreno
void desenhaObstaculos(int linhas, int colunas, float tamanhoCubo, GLuint texID[], int matrizMapa[11][16]) {
    float inicioX = -colunas * tamanhoCubo / 2;
    float inicioZ = -linhas * tamanhoCubo / 2;

    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            float x = inicioX + j * tamanhoCubo;
            float z = inicioZ + i * tamanhoCubo;
            float y = tamanhoCubo;  // Desenha acima do terreno base

            if (matrizMapa[i][j] == 1){
				desenhaCubo(x, y, z, tamanhoCubo, texID[2], texID[2]); // desenha o muro	
			}else if(matrizMapa[i][j] == 2) {
				desenhaCubo(x, y, z, tamanhoCubo, texID[3], texID[3]); // desenha a caixa
			}
        }
    }
}

// Funcao para desenhar o terreno com cubos
void desenhaBaseTerreno(int linhas, int colunas, float tamanhoCubo, GLuint texID[]) {
    float inicioX = -colunas * tamanhoCubo / 2;
    float inicioZ = -linhas * tamanhoCubo / 2;

    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            float x = inicioX + j * tamanhoCubo;
            float z = inicioZ + i * tamanhoCubo;
            desenhaCubo(x, 0, z, tamanhoCubo, texID[0], texID[1]); // texID[0] para a face superior e texID[1] para as outras faces
        }
    }
}

// Funcao de iluminacao
void configurarIluminacao() {
    GLfloat light_position[] = {-1, 1, 1, 0};
    GLfloat light_color[] = {1, 1, 1, 0};
    glLightfv(GL_LIGHT0, GL_AMBIENT_AND_DIFFUSE, light_color);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat mat_ambient_diffuse[] = {1, 1, 1, 1};
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_ambient_diffuse);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
}
