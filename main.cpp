#include <math.h>
#include <stdio.h>
#include <iostream>
#include <GL/glut.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "cenario.h"

#define LINHAS_MAPA 7
#define COLUNAS_MAPA 14

// Variaveis para controlar a camera
bool visaoCima = false;
int cameraY, cameraZ;

// Vetor de texturas
GLuint texID[2];  

// Funcao de redimensionamento
void redimensiona(int w, int h) {
    glViewport(0, 0, w, h);
    float aspect = (float)w / (float)h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, aspect, 0.1, 500.0);
    glMatrixMode(GL_MODELVIEW);
}

// Funcao de desenho
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();

    if(visaoCima){
        cameraY = 51;
        cameraZ = -1;
    } else{
        cameraY = 41;
        cameraZ = 35;
    }

    gluLookAt(-2, cameraY, cameraZ,
              -2, 0, -2,
              0, 1, 0);

    desenhaTerreno(LINHAS_MAPA, COLUNAS_MAPA, 4.0, texID);

    glFlush();
    glutSwapBuffers();
}

// Funcao para capturar entradas do teclado
void teclado(unsigned char key, int x, int y) {
    switch (key) {
        case 'p': case 'P':
            visaoCima = !visaoCima;
            break;
        default:
            break;
    }

    glutPostRedisplay();
}

void init() {
    glClearColor(0.4, 0.7, 1, 1.0); // Cor de fundo azul claro
    //configurarIluminacao();         // Configura a iluminacao
    
	// Configurar as texturas
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glGenTextures(2, texID);
    
    // Carrega as texturas no vetor
    carregaTextura(texID[0], "imagens/grama_cima.jpg");   	// Textura 0 = parte de cima de grama
    carregaTextura(texID[1], "imagens/grama_lateral.jpg");  // Textura 1 = lateral de grama
    
    glEnable(GL_DEPTH_TEST);  // Ativa o teste de profundidade
}

int main(int argc, char** argv) {
    glutInit(&argc, argv); 										// Inicializa o GLUT
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); 	// Modo de exibicao com duplo buffer, RGB e Depth
    glutInitWindowSize(800, 500); 								// Tamanho da janela
    glutInitWindowPosition(100, 100); 							// Posicao inicial da janela
    glutCreateWindow("Bomberman 3D"); 							// Cria a janela
    init();														// Configuracoes iniciais
    glutDisplayFunc(display);								 	// Funcao para desenhar a janela
    glutReshapeFunc(redimensiona); 								// Funcao para redimensionar a janela
    glutKeyboardFunc(teclado); 									// Registra a funcao de teclado
    glutMainLoop(); 											// Inicia o loop do GLUT
}

