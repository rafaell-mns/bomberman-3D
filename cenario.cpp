#include <stdio.h>
#include <iostream>
#include <GL/glut.h>
#include "stb_image.h"

// Carregar a textura
void carregaTextura(GLuint tex_id, std::string filePath){
    unsigned char* imgData;
    int largura, altura, canais;
    
    filePath = "imagens/" + filePath;
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

// Funcao para desenhar o terreno e obstaculos
void desenhaTerreno(int linhas, int colunas, float tamanhoCubo, GLuint texID[], int matrizMapa[11][16]) {
    float inicioX = -colunas * tamanhoCubo / 2;
    float inicioZ = -linhas * tamanhoCubo / 2;

    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            float x = inicioX + j * tamanhoCubo;
            float z = inicioZ + i * tamanhoCubo;

            // Desenha a base do terreno
            desenhaCubo(x, 0, z, tamanhoCubo, texID[0], texID[1]); // texID[0] para a face superior e texID[1] para as outras faces

            // Desenha os obstaculos (muros e caixas) acima do terreno
            if (matrizMapa[i][j] == 1) {
                desenhaCubo(x, tamanhoCubo, z, tamanhoCubo, texID[2], texID[2]); // muro normal
            } else if (matrizMapa[i][j] == 2) {
                desenhaCubo(x, tamanhoCubo, z, tamanhoCubo, texID[3], texID[3]); // muro com musgo
            } else if (matrizMapa[i][j] == 3) {
                desenhaCubo(x, tamanhoCubo, z, tamanhoCubo, texID[4], texID[4]); // caixote
            } else if (matrizMapa[i][j] == 9) {
            	printf("desenha o 9\n");
                desenhaCubo(x, tamanhoCubo, z, tamanhoCubo, texID[9], texID[9]); // power up
            } else if (matrizMapa[i][j] == 10) {
            	printf("desenha o 10\n");
                desenhaCubo(x, tamanhoCubo, z, tamanhoCubo, texID[10], texID[10]); // power up
            } else if (matrizMapa[i][j] == 11) {
            	printf("desenha o 11\n");
                desenhaCubo(x, tamanhoCubo, z, tamanhoCubo, texID[11], texID[11]); // power up
            }
        }
    }
}