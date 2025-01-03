#include <math.h>
#include <stdio.h>
#include <iostream>
#include <GL/glut.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "cenario.h"

#define LINHAS_MAPA 11
#define COLUNAS_MAPA 16
#define QTD_TEXTURAS 5

#define N 1 	// muro normal
#define M 2 	// muro com musgo
#define C 3 	// caixote

int matrizMapa[LINHAS_MAPA][COLUNAS_MAPA] = {
    {M, N, M, N, M, N, M, N, M, N, M, N, M, N, M, N},
	{N, 0, 0, 0, N, 0, C, 0, 0, C, C, 0, 0, C, M, N},
	{M, C, M, 0, 0, 0, M, 0, 0, 0, N, C, N, 0, C, M},
	{N, 0, M, 0, M, C, N, C, N, 0, M, 0, N, 0, 0, N},
	{M, C, M, 0, 0, 0, N, 0, N, C, C, 0, C, 0, C, M},
	{N, 0, M, N, M, C, M, 0, 0, 0, M, C, N, N, C, N},
	{M, 0, M, 0, 0, 0, M, 0, N, C, M, 0, 0, 0, C, M},
	{N, 0, C, 0, 0, 0, C, 0, C, 0, 0, 0, C, 0, 0, N},
	{M, 0, M, 0, N, 0, C, 0, 0, 0, N, 0, M, N, 0, M},
	{M, C, C, 0, N, C, M, 0, N, C, M, 0, 0, C, 0, M},
	{N, M, N, N, M, M, N, N, M, M, N, M, N, M, N, N}
};

// Variaveis para controlar a camera
bool visaoCima = false;
float eyeX = -2, eyeY = 41, eyeZ = 47;   	   // Posicao inicial da camera
float centerX = -2, centerY = 0, centerZ = -2; // Ponto de foco inicial

// Vetor de texturas
GLuint texID[QTD_TEXTURAS];  

// Funcao de redimensionamento
void redimensiona(int w, int h) {
    glViewport(0, 0, w, h);
    float aspect = (float)w / (float)h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, aspect, 0.1, 500);
    glMatrixMode(GL_MODELVIEW);
}

// Funcao de desenho
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(eyeX, eyeY, eyeZ,
              centerX, centerY, centerZ,
              0, 1, 0);

    // Desenhar o mapa
    desenhaTerreno(LINHAS_MAPA, COLUNAS_MAPA, 4, texID, matrizMapa);

    glFlush();
    glutSwapBuffers();
}

// Funcao para capturar entradas do teclado
void teclado(unsigned char key, int x, int y) {
    const float movimento = 1.0; // Define a quantidade de movimento
    switch (key) {
        case 'w': case 'W': // Mover a camera para frente (ao longo do eixo Z)
            eyeZ -= movimento;
            break;
        case 's': case 'S': // Mover a camera para tras
            eyeZ += movimento;
            break;
        case 'a': case 'A': // Mover a camera para a esquerda (ao longo do eixo X)
            eyeX -= movimento;
            break;
        case 'd': case 'D': // Mover a camera para a direita
            eyeX += movimento;
            break;
        case 'q': case 'Q': // Mover a camera para cima (ao longo do eixo Y)
            eyeY += movimento;
            break;
        case 'e': case 'E': // Mover a camera para baixo
            eyeY -= movimento;
            break;
        case 'i': case 'I': // Mover o ponto de foco para frente (no eixo Z)
            centerZ -= movimento;
            break;
        case 'k': case 'K': // Mover o ponto de foco para tras
            centerZ += movimento;
            break;
        case 'j': case 'J': // Mover o ponto de foco para a esquerda (no eixo X)
            centerX -= movimento;
            break;
        case 'l': case 'L': // Mover o ponto de foco para a direita
            centerX += movimento;
            break;
        /*
        case 'p': case 'P':
            visaoCima = !visaoCima;
            if (visaoCima) {
                eyeY = 51;
                eyeZ = -1;
            } else {
                eyeY = 41;
                eyeZ = 35;
            }
            break;
  		*/
        default:
            break;
    }

    glutPostRedisplay();
}


void init() {
    glClearColor(0.4, 0.7, 1, 1); // Cor de fundo azul claro
    //configurarIluminacao();         // Configura a iluminacao
    
    // Configurar as texturas
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glGenTextures(QTD_TEXTURAS, texID);
    
    // Carrega as texturas no vetor
    carregaTextura(texID[0], "grama_cima.jpg");     // Textura 0 = parte de cima da grama
    carregaTextura(texID[1], "grama_lateral.jpg");  // Textura 1 = lateral da grama
    carregaTextura(texID[2], "pedra.jpg");  		// Textura 2 = muro normal
    carregaTextura(texID[3], "pedra_musgo.png");  	// Textura 2 = muro com musgo
	carregaTextura(texID[4], "caixa.jpg");  		// Textura 3 = caixa
    
    glEnable(GL_DEPTH_TEST);  // Ativa o teste de profundidade
}

int main(int argc, char** argv) {
	// Inicializa o GLUT e configura a janela
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 500);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Bomberman 3D");
    glutDisplayFunc(display);
    glutReshapeFunc(redimensiona);                           
    glutKeyboardFunc(teclado);
	                                
	printf("W/w: Mover para frente\n");
	printf("S/s: Mover para tras\n");
	printf("A/a: Mover para a esquerda\n");
	printf("D/d: Mover para a direita\n");
	printf("Q/q: Mover para cima\n");
	printf("E/e: Mover para baixo\n");
	printf("I/i: Mover o ponto de foco para frente\n");
	printf("K/k: Mover o ponto de foco para tras\n");
	printf("J/j: Mover o ponto de foco para a esquerda\n");
	printf("L/l: Mover o ponto de foco para a direita\n");
	
	init();
	glutMainLoop();
}
