#include <math.h>
#include <stdio.h>
#include <iostream>
#include <GL/glut.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "cenario.h"
#include "glut_text.h"
#define LINHAS_MAPA 11
#define COLUNAS_MAPA 16
#define QTD_TEXTURAS 5

#define N 1 	// muro normal
#define M 2 	// muro com musgo
#define C 3 	// caixote
//Largura e altura da janela
int width = 800, height = 500;

//Coordenadas da posicao atual do mouse
int m_x, m_y;

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


// Prototipos das funcoes
void drawSphere(float radius, float r, float g, float b);
void drawCube(float size, float r, float g, float b);
void moverPersonagem(float novoX, float novoZ);
void mousePassiveMotion(int x, int y);


float personagemX = -28.0f, personagemZ = 0.0f;
float anguloRotacao = 0.0f; 


// Funcao para desenhar o personagem Bomberman
void drawBomberman() {
	glPushMatrix();
	
		// mexer na posicao do player (fixo no eixo y)
		glTranslatef(personagemX, 3.7, personagemZ);
		glRotatef(anguloRotacao, 0.0f, 1.0f, 0.0f);
		float e = 2.5;
		glScaled(e,e,e);
		 
   	    // Cabeca
	    glPushMatrix();
	    glTranslatef(0.0f, 1.2f, 0.0f);
	    drawSphere(0.5f, 1.0f, 1.0f, 1.0f); // Cabe�a branca
	    glPopMatrix();
	    
	    // Corpo
	    glPushMatrix();
	    glTranslatef(0.0f, 0.5f, 0.0f);
	    drawCube(0.7f, 0.0f, 0.0f, 1.0f); // Corpo azul
	    glPopMatrix();
	    
	    // Pernas
	    glPushMatrix();
	    glTranslatef(-0.2f, -0.5f, 0.0f);
	    drawCube(0.4f, 1.0f, 0.0f, 0.0f); // Perna esquerda vermelha
	    glPopMatrix();
	    
   	    glPushMatrix();
	    glTranslatef(0.2f, -0.5f, 0.0f);
	    drawCube(0.4f, 1.0f, 0.0f, 0.0f); // Perna direita vermelha
	    glPopMatrix();
	    
	    // Bracos
	    glPushMatrix();
	    glTranslatef(-0.5f, 0.8f, 0.0f);
	    drawCube(0.3f, 1.0f, 0.8f, 0.0f); // Bra�o esquerdo amarelo
	    glPopMatrix();
	    glPushMatrix();
	    glTranslatef(0.5f, 0.8f, 0.0f);
	    drawCube(0.3f, 1.0f, 0.8f, 0.0f); // Bra�o direito amarelo
	    glPopMatrix();
	    
	    // Antena
	    glPushMatrix();
	    glTranslatef(0.0f, 1.8f, 0.0f);
	    drawSphere(0.1f, 1.0f, 0.0f, 0.0f); // Bolinha vermelha na antena
	    glPopMatrix();
	    
	    glPushMatrix();
	    glTranslatef(0.0f, 1.5f, 0.0f);
	    glScalef(0.1f, 0.5f, 0.1f);
	    drawCube(1.0f, 0.5f, 0.5f, 0.5f); // Haste da antena cinza
	    glPopMatrix();
    
    glPopMatrix();
}




// Variaveis para controlar a camera
bool visaoCima = false;
float eyeX = -2, eyeY = 41, eyeZ = 47;   	   // Posicao inicial da camera
float centerX = -2, centerY = 0, centerZ = -2; // Ponto de foco inicial

// Vetor de texturas
GLuint texID[QTD_TEXTURAS];  

// Funcao de redimensionamento
void redimensiona(int w, int h) {
    glViewport(0, 0, w, h);
    height = h;
    float aspect = (float)w / (float)h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, aspect, 0.1, 500);
    glMatrixMode(GL_MODELVIEW);
}

void display() {
    // Limpar os buffers de cor e profundidade
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Configurar a câmera
    glLoadIdentity();
    gluLookAt(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, 0, 1, 0);

    // Desenhar o terreno
    glPushMatrix(); // Salvar o estado da matriz atual
    desenhaTerreno(LINHAS_MAPA, COLUNAS_MAPA, 4, texID, matrizMapa);
    glPopMatrix(); // Restaurar o estado da matriz


   	   	   	    
    


    // Desenhar o texto das coordenadas do mouse
    glPushMatrix();
    glDisable(GL_TEXTURE_2D);   // Desativar texturas para o texto
    glColor3f(1.0f, 1.0f, 1.0f); // Garantir cor branca para o texto
      	draw_text_stroke(-20, 20, "(" + to_string(m_x) + "," + to_string(m_y) + ")", 0.01);
      	drawBomberman();
    glEnable(GL_TEXTURE_2D);    // Reativar texturas caso necessário
    glPopMatrix();

    // Finalizar a renderização
    glFlush();
    glutSwapBuffers();
}




void drawCube(float size, float r, float g, float b) {
    glColor3f(r, g, b);
    glutSolidCube(size);
}

void drawSphere(float radius, float r, float g, float b) {
    glColor3f(r, g, b);
    glutSolidSphere(radius, 20, 20);
}

// Controle do teclado
// Funcao para capturar entradas do teclado
void teclado(unsigned char key, int x, int y) {
    const float movimento = 1.0; // Define a quantidade de movimento
    switch (key) {
        case 'w': case 'W': // Mover o personagem pra frente
            personagemZ -= movimento;
            anguloRotacao = 180.0f;
            break;
        case 's': case 'S': // Mover o personagem pra tras
            personagemZ += movimento;
            anguloRotacao = 0.0f; 
            break;
        case 'a': case 'A': // Mover o personagem pra esquerda
            personagemX -= movimento;
            anguloRotacao = 270.0f;
            break;
        case 'd': case 'D': // Mover o personagem pra direita
            personagemX += movimento;
            anguloRotacao = 90.0f;
            break;
        case 't': case 'T': // Mover a camera para frente (ao longo do eixo Z)
            eyeZ -= movimento;
            break;
        case 'g': case 'G': // Mover a camera para tras
            eyeZ += movimento;
            break;
        case 'f': case 'F': // Mover a camera para a esquerda (ao longo do eixo X)
            eyeX -= movimento;
            break;
        case 'h': case 'H': // Mover a camera para a direita
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



void moverPersonagem(float novoX, float novoZ) {
    int mapaX = (int)(novoX / 4); // Dividir por 4, que é a escala do terreno
    int mapaZ = (int)(novoZ / 4);

    // Verificar se a nova posição está dentro dos limites do mapa e não é um muro

        personagemX = novoX;
        personagemZ = novoZ;
    
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
 
 


void mousePassiveMotion(int x, int y){
    m_x = x; m_y = height - y - 1;
    glutPostRedisplay();
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
    glutPassiveMotionFunc(mousePassiveMotion); //fucao callback do movimento passivo do mouse                                
	printf("T/F/G/H: translada a camera pras 4 direcoes\n");
	printf("Q/q: Mover para cima\n");
	printf("E/e: Mover para baixo\n");
	printf("I/i: Mover o ponto de foco para frente\n");
	printf("K/k: Mover o ponto de foco para tras\n");
	printf("J/j: Mover o ponto de foco para a esquerda\n");
	printf("L/l: Mover o ponto de foco para a direita\n");
	
	init();
	glutMainLoop();
}