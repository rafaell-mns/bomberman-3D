#include <GL/glut.h>
#include "cenario.h"

#define LINHAS 7
#define COLUNAS 14 

void redimensiona(int w, int h) {
    glViewport(0, 0, w, h);

    float aspect = (float)w / (float)h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, aspect, 0.1, 500.0);

    glMatrixMode(GL_MODELVIEW);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Limpa a tela e o depth buffer

    glLoadIdentity();
    glTranslated(2, 20, -58); // Posiciona a câmera

    desenhaTerreno(-32.0, LINHAS, COLUNAS, 4.0);

    glFlush(); 			// Executa os comandos OpenGL
    glutSwapBuffers();  // Troca os buffers (necessário para usar GLUT_DOUBLE)
}

int main(int argc, char** argv) {
    glutInit(&argc, argv); 										// Inicializa o GLUT
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); 	// Modo de exibição com duplo buffer, RGB e Depth
    glutInitWindowSize(800, 500); 								// Tamanho da janela
    glutInitWindowPosition(100, 100); 							// Posição inicial da janela
    glutCreateWindow("Bomberman 3D"); 							// Cria a janela
    glClearColor(0.4, 0.7, 1, 1.0); 							// Cor de fundo azul claro
    glutDisplayFunc(display);								 	// Função para desenhar a janela
    glutReshapeFunc(redimensiona); 								// Função para redimensionar a janela
    glEnable(GL_DEPTH_TEST); 									// Ativa o teste de profundidade

    glEnable(GL_CULL_FACE);        								// Ativa o face culling
    glCullFace(GL_BACK);           								// Descarta as faces traseiras
    glFrontFace(GL_CCW);           								// Define sentido anti-horário como padrão

    glutMainLoop(); 											// Inicia o loop principal
    return 0;
}
