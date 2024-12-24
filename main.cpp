#include <GL/glut.h>
#include <stdio.h>
#include "cenario.h"

#define LINHAS_MAPA 7
#define COLUNAS_MAPA 14 

// Vari�veis globais para controlar a posi��o da c�mera
float eyeX = 0.0, eyeY = -20.0, eyeZ = 20.0;   		// Posi��o da c�mera
float centerX = 0.0, centerY = 0.0, centerZ = 0.0;  // Ponto de foco
float upX = 0.0, upY = 1.0, upZ = 0.0;           	// Vetor "cima"

// Fun��o de redimensionamento
void redimensiona(int w, int h) {
    glViewport(0, 0, w, h);
    float aspect = (float)w / (float)h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, aspect, 0.1, 500.0);
    glMatrixMode(GL_MODELVIEW);
}

// Fun��o de desenho
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();

    // Define a posi��o da c�mera e o ponto de foco
    gluLookAt(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ);

    desenhaTerreno(LINHAS_MAPA, COLUNAS_MAPA, 4.0);

    glFlush();
    glutSwapBuffers();
}

// Fun��o para desenhar o terreno (cubos)
void desenhaTerreno(float alturaBase, int linhas, int colunas, float tamanhoCubo) {
    float inicioX = -colunas * tamanhoCubo / 2.0; 
    float inicioZ = -linhas * tamanhoCubo / 2.0;  

    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            float x = inicioX + j * tamanhoCubo;
            float z = inicioZ + i * tamanhoCubo;
            desenhaCubo(x, alturaBase, z, tamanhoCubo);
        }
    }
}

// Fun��o para capturar entradas do teclado
void teclado(unsigned char key, int x, int y) {
    const float movimento = 1.0; // Define a quantidade de movimento

    switch (key) {
        case 'w': // Mover a c�mera para frente (ao longo do eixo Z)
            eyeZ -= movimento;
            break;
        case 's': // Mover a c�mera para tr�s
            eyeZ += movimento;
            break;
        case 'a': // Mover a c�mera para a esquerda (ao longo do eixo X)
            eyeX -= movimento;
            break;
        case 'd': // Mover a c�mera para a direita
            eyeX += movimento;
            break;
        case 'q': // Mover a c�mera para cima (ao longo do eixo Y)
            eyeY += movimento;
            break;
        case 'e': // Mover a c�mera para baixo
            eyeY -= movimento;
            break;
        case 'i': // Mover o ponto de foco para frente (no eixo Z)
            centerZ -= movimento;
            break;
        case 'k': // Mover o ponto de foco para tr�s
            centerZ += movimento;
            break;
        case 'j': // Mover o ponto de foco para a esquerda (no eixo X)
            centerX -= movimento;
            break;
        case 'l': // Mover o ponto de foco para a direita
            centerX += movimento;
            break;
        case 'u': // Mover o vetor "cima" para cima (ao longo do eixo Y)
            upY += 0.1;
            break;
        case 'o': // Mover o vetor "cima" para baixo
            upY -= 0.1;
            break;
        default:
            break;
    }

    // Exibe as coordenadas no console
    printf("Camera 		(%.2f, %.2f, %.2f)\n", eyeX, eyeY, eyeZ);
    printf("Ponto de foco 	(%.2f, %.2f, %.2f)\n", centerX, centerY, centerZ);
    printf("Vetor 'Cima' 	(%.2f, %.2f, %.2f)\n\n", upX, upY, upZ);

    glutPostRedisplay(); // Atualiza a tela
}

int main(int argc, char** argv) {
    glutInit(&argc, argv); 										// Inicializa o GLUT
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); 	// Modo de exibi��o com duplo buffer, RGB e Depth
    glutInitWindowSize(800, 500); 								// Tamanho da janela
    glutInitWindowPosition(100, 100); 							// Posi��o inicial da janela
    glutCreateWindow("Bomberman 3D"); 							// Cria a janela
    glClearColor(0.4, 0.7, 1, 1.0); 							// Cor de fundo azul claro
    glutDisplayFunc(display);								 	// Fun��o para desenhar a janela
    glutReshapeFunc(redimensiona); 								// Fun��o para redimensionar a janela
    glutKeyboardFunc(teclado); 									// Registra a fun��o de teclado
    glEnable(GL_DEPTH_TEST); 									// Ativa o teste de profundidade
    glEnable(GL_CULL_FACE);        								// Ativa o face culling
    glCullFace(GL_BACK);           								// Descarta as faces traseiras
    glFrontFace(GL_CCW);           								// Define sentido anti-hor�rio como padr�o
    glutMainLoop(); 											// Inicia o loop principal
    return 0;
}
