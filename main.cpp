#include <GL/glut.h>

typedef float color[3];
color verde = {0.0, 0.8, 0.0};
color preto = {0.0, 0.0, 0.0};

void redimensiona(int w, int h) {
    glViewport(0, 0, w, h);

    float aspect = (float)w / (float)h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, aspect, 0.1, 500.0);

    glMatrixMode(GL_MODELVIEW);
}

void desenhaFace(float v1[3], float v2[3], float v3[3], float v4[3]) {
	// face
    glBegin(GL_QUADS);
    glColor3fv(verde);
    glVertex3fv(v1);
    glVertex3fv(v2);
    glVertex3fv(v3);
    glVertex3fv(v4);
    glEnd();

	// borda preta
    glBegin(GL_LINE_LOOP);
    glColor3fv(preto);
    glVertex3fv(v1);
    glVertex3fv(v2);
    glVertex3fv(v3);
    glVertex3fv(v4);
    glEnd();
}

void desenhaCubo(float x, float y, float z, float tamanho) {
	// metade do tamanho (para posicionar os vértices)
    float d = tamanho / 2.0;

	// vértices
    float v1[3] = {x - d, y + d, z + d};
    float v2[3] = {x - d, y - d, z + d};
    float v3[3] = {x + d, y - d, z + d};
    float v4[3] = {x + d, y + d, z + d};
    float v5[3] = {x + d, y + d, z - d};
    float v6[3] = {x + d, y - d, z - d};
    float v7[3] = {x - d, y - d, z - d};
    float v8[3] = {x - d, y + d, z - d};

	// desenha as faces com contorno
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

void desenhaTerreno(float alturaBase, int linhas, int colunas, float tamanhoCubo) {
    float inicioX = -colunas * tamanhoCubo / 2.0; // Centraliza na direção X
    float inicioZ = -linhas * tamanhoCubo / 2.0;  // Centraliza na direção Z

    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            float x = inicioX + j * tamanhoCubo;
            float z = inicioZ + i * tamanhoCubo;
            desenhaCubo(x, alturaBase, z, tamanhoCubo);
        }
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Limpa a tela e o depth buffer

    glLoadIdentity();
    glTranslated(2, 20, -58); // Posiciona a câmera

    desenhaTerreno(-32.0, 7, 14, 4.0); // 7 linhas e 14 colunas

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
    glutMainLoop(); 											// Inicia o loop principal
    return 0;
}
