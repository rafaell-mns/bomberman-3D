#include <stdio.h>
#include <iostream>
#include <GL/glut.h>
#include "stb_image.h"

// Carregar a textura
// Fonte: https://youtu.be/AKz52bZb-9I?si=0dvG-fuKZ7fxSz9s
void carregaTextura(GLuint tex_id, std::string filePath){
    unsigned char* imgData;
    int largura, altura, canais;

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
    glTexCoord2f(0.0f, 0.0f); glVertex3fv(v1);  
    glTexCoord2f(1.0f, 0.0f); glVertex3fv(v2);  
    glTexCoord2f(1.0f, 1.0f); glVertex3fv(v3);  
    glTexCoord2f(0.0f, 1.0f); glVertex3fv(v4);  
    glEnd();
}

// Funcao para desenhar o cubo com diferentes texturas
void desenhaCubo(float x, float y, float z, float tamanho, GLuint texID[]) {
    float d = tamanho / 2.0;

    float v1[3] = {x - d, y + d, z + d};
    float v2[3] = {x - d, y - d, z + d};
    float v3[3] = {x + d, y - d, z + d};
    float v4[3] = {x + d, y + d, z + d};
    float v5[3] = {x + d, y + d, z - d};
    float v6[3] = {x + d, y - d, z - d};
    float v7[3] = {x - d, y - d, z - d};
    float v8[3] = {x - d, y + d, z - d};

	// Desenha a face superior (com textura 1) rotacionada
    glBindTexture(GL_TEXTURE_2D, texID[1]);
	glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex3fv(v1);  
    glTexCoord2f(0.0f, 0.0f); glVertex3fv(v2);  
    glTexCoord2f(1.0f, 0.0f); glVertex3fv(v3);  
    glTexCoord2f(1.0f, 1.0f); glVertex3fv(v4);  
    glEnd();

	// Desenhar as demais faces com a textura lateral (textura 0)
    glBindTexture(GL_TEXTURE_2D, texID[0]);
    desenhaFace(v4, v3, v6, v5, texID[0]);
    desenhaFace(v5, v6, v7, v8, texID[0]);
    desenhaFace(v8, v7, v2, v1, texID[0]);
    desenhaFace(v2, v7, v6, v3, texID[0]);
    desenhaFace(v1, v4, v5, v8, texID[0]);
    //desenhaFace(v2, v7, v6, v3, texID[0]);
}

// Funcao para desenhar o terreno com cubos
void desenhaTerreno(int linhas, int colunas, float tamanhoCubo, GLuint texID[]) {
    float inicioX = -colunas * tamanhoCubo / 2.0;
    float inicioZ = -linhas * tamanhoCubo / 2.0;

    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            float x = inicioX + j * tamanhoCubo;
            float z = inicioZ + i * tamanhoCubo;
            desenhaCubo(x, 0, z, tamanhoCubo, texID);
        }
    }
}

// Funcao de iluminacao
void configurarIluminacao() {
    GLfloat light_position[] = {-1.0, 1.0, 1.0, 0.0};
    GLfloat light_color[] = {1.0, 1.0, 1.0, 0.0};
    glLightfv(GL_LIGHT0, GL_AMBIENT_AND_DIFFUSE, light_color);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat mat_ambient_diffuse[] = {1.0, 1.0, 1.0, 1.0};
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_ambient_diffuse);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
}
