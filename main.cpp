#include <GL/glut.h>
#include <stdio.h>
#include "cenario.h"

#define LINHAS_MAPA 7
#define COLUNAS_MAPA 14 

bool visaoCima = false;
int cameraY, cameraZ;

// Função de redimensionamento
void redimensiona(int w, int h) {
    glViewport(0, 0, w, h);
    float aspect = (float)w / (float)h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, aspect, 0.1, 500.0);
    glMatrixMode(GL_MODELVIEW);
}

// Função de desenho
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

    desenhaTerreno(LINHAS_MAPA, COLUNAS_MAPA, 4.0);

    glFlush();
    glutSwapBuffers();
}

// Função para capturar entradas do teclado
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

int main(int argc, char** argv) {
    glutInit(&argc, argv); 										// Inicializa o GLUT
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); 	// Modo de exibição com duplo buffer, RGB e Depth
    glutInitWindowSize(800, 500); 								// Tamanho da janela
    glutInitWindowPosition(100, 100); 							// Posição inicial da janela
    glutCreateWindow("Bomberman 3D"); 							// Cria a janela
    glClearColor(0.4, 0.7, 1, 1.0); 							// Cor de fundo azul claro
    configurarIluminacao();										// Configura a iluminação
    glutDisplayFunc(display);								 	// Função para desenhar a janela
    glutReshapeFunc(redimensiona); 								// Função para redimensionar a janela
    glutKeyboardFunc(teclado); 									// Registra a função de teclado
    glEnable(GL_DEPTH_TEST); 									// Ativa o teste de profundidade
    glEnable(GL_CULL_FACE);        								// Ativa o face culling
    glCullFace(GL_BACK);           								// Descarta as faces traseiras
    glFrontFace(GL_CCW);           								// Define sentido anti-horário como padrão
    glutMainLoop(); 											// Inicia o loop principal
    return 0;
}