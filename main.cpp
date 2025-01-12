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

#include <set>
#include <utility>
#include <cstdio>

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

std::set<std::pair<int, int> > muro;  // Conjunto para armazenar as posições de colisão

bool temColisao(int x, int y, const std::set< std::pair<int, int> >& colisoes) {
    // Iterar por todos os pares de coordenadas no set
    for (std::set< std::pair<int, int> >::iterator it = colisoes.begin(); it != colisoes.end(); ++it) {
        // Comparar coordenadas
        if (it->first == x && it->second == y) {
            return true;
        }
    }
    return false;
}


// coordenadas reais (no mapa) da matriz
void testaMatriz(){
    int inicioX = -4;  // Valor inicial de X
    int inicioZ = -22; // Valor inicial de Z
    int razaoX = 4;    // Razão de incremento para X
    int razaoZ = 4;    // Razão de incremento para Z

    // Iterar por todas as linhas e colunas da matriz
    for (int z = 0; z < LINHAS_MAPA; z++) {
        for (int x = 0; x < COLUNAS_MAPA; x++) {
            // Se o valor na matriz for 'M' ou 'N', calcular todas as coordenadas dentro da célula
            if (matrizMapa[z][x] == M || matrizMapa[z][x] == N) {
                // Para cada célula de 'M' ou 'N', iterar dentro dela (4x4 unidades)
                for (int i = 0; i < 4; ++i) {
                    for (int j = 0; j < 4; ++j) {
                        // Calcular as coordenadas de cada célula
                        int posX = inicioX + x * razaoX + i;  // Incrementar por 1 unidade dentro da célula
                        int posZ = inicioZ + z * razaoZ + j;  // Incrementar por 1 unidade dentro da célula

                        // Adicionar as coordenadas no set
                        muro.insert(std::make_pair(posX, posZ));

                        // Imprimir as coordenadas de cada posição dentro da célula
                        printf("mapaX: %d, mapaZ: %d\n", posX, posZ);
                    }
                }
            }
        }
    }
}

// Protótipos das funções
void drawSphere(float radius, float r, float g, float b);
void drawCube(float size, float r, float g, float b);
void moverPersonagem(float novoX, float novoZ);
void mousePassiveMotion(int x, int y);

float personagemX = 0.0f, personagemZ = 0.0f;
float anguloRotacao = 0.0f; 

// Função para desenhar o personagem Bomberman
void drawBomberman(){
	glPushMatrix();
		// mexer na posição do player
		glTranslatef(-28+personagemX, 3.7, 0.0f+personagemZ);
		//glRotated(10,anguloRotacao,anguloRotacao,anguloRotacao); > talvez fique bom quando adicionar as animacoes
		float e = 2;
		glScaled(e, e, e);
		
		// Função para desenhar o personagem Bomberman
		glPushMatrix();
		// mexer no rosto + cabeca + olhos
		glTranslatef(0, -0.2, 0);
		glScaled(1.1, 1.1, 1.1);

		//Cabeça
		glPushMatrix();
		glTranslatef(0.0f, 1.2f, 0.0f);
		drawSphere(0.5f, 1.0f, 1.0f, 1.0f); // Cabeça branca
		glPopMatrix();

		// rosto
		glPushMatrix();
		glTranslatef(0.0f, 1.21f, 0.08);
		e = 0.9;
		glScaled(e, e, e);
		drawSphere(0.5, 0.961, 0.741, 0.569); // rosto
		glPopMatrix();

		// olhos
		glPushMatrix();
		glTranslatef(-0.15, 1.21f, 0.32);
		e = 0.6;
		glScaled(0.2, 1, 1);
		drawCube(0.4f, 0, 0, 0); // Perna esquerda vermelha
		glPopMatrix();

		glPushMatrix();
		glTranslatef(0.15, 1.21f, 0.32);
		e = 0.6;
		glScaled(0.2, 1, 1);
		drawCube(0.4f, 0, 0, 0); // Perna esquerda vermelha
		glPopMatrix();
		glPopMatrix();
		
		// Corpo
		glPushMatrix();
		glTranslatef(0.0f, 0.3, 0.0f);
		glScaled(1, 1, 1);
		drawCube(0.7f, 0.0f, 0.0f, 1.0f); // Corpo azul
		glPopMatrix();
	
		// fundo do cinto
		glPushMatrix();
		glTranslatef(0.0f, 0.1, 0.35f);
		e = 0.6;
		glScaled(1.8, 0.3, 0.1);
		drawCube(0.4f, 0, 0, 0); // Perna esquerda vermelha
		glPopMatrix();
	
		// parte dourada do cinto cireita
		glPushMatrix();
		glTranslatef(0.1, 0.1, 0.38f);
		e = 0.2;
		glScaled(0.05, 0.2, e);
		drawCube(0.4f, 1, 1, 0); // Perna esquerda vermelha
		glPopMatrix();
	
		// parte dourada do cinto esquerda
		glPushMatrix();
		glTranslatef(-0.1, 0.1, 0.38f);
		e = 0.2;
		glScaled(0.05, 0.2, e);
		drawCube(0.4f, 1, 1, 0); // Perna esquerda vermelha
		glPopMatrix();
	
		// parte dourada do cinto cima
		glPushMatrix();
		glTranslatef(0, 0.13, 0.38f);
		e = 0.2;
		glScaled(0.45, 0.05, e);
		drawCube(0.4f, 1, 1, 0); // Perna esquerda vermelha
		glPopMatrix();
	
		// parte dourada do cinto cima
		glPushMatrix();
		glTranslatef(0, 0.07, 0.38f);
		e = 0.2;
		glScaled(0.45, 0.05, e);
		drawCube(0.4f, 1, 1, 0); // Perna esquerda vermelha
		glPopMatrix();
	
	
		// anteperna esquerda
		glPushMatrix();
		glTranslatef(-0.2, -0.2, 0.0f);
		e = 0.9;
		glScaled(e, 1.25, e);
		drawCube(0.3f, 1, 1, 1); // Braço esquerdo amarelo
		glPopMatrix();
	
		// Perna  esquerda
		glPushMatrix();
		glTranslatef(-0.2f, -0.5, 0.0f);
		e = 0.9;
		glScaled(e, e, e);
		drawCube(0.4f, 1.0f, 0.0f, 0.0f); // Perna esquerda vermelha
		glPopMatrix();
	
		// anteperna direita
		glPushMatrix();
		glTranslatef(0.2, -0.2, 0.0f);
		e = 0.9;
		glScaled(e, 1.25, e);
		drawCube(0.3f, 1, 1, 1); // Braço esquerdo amarelo
		glPopMatrix();
	
		// Perna  direita
		glPushMatrix();
		glTranslatef(0.2f, -0.5, 0.0f);
		glScaled(e, e, e);
		drawCube(0.4f, 1.0f, 0.0f, 0.0f); // Perna esquerda vermelha
		glPopMatrix();
	
	
	// mexer nos braços
		glPushMatrix();
		glTranslatef(0, -0.34, 0);
		// antebraço esquerdo
		glPushMatrix();
		glTranslatef(-0.5f, 0.8f, 0.0f);
		e = 0.9;
		glScaled(e, e, e);
		drawCube(0.3f, 1, 1, 1); // Braço esquerdo amarelo
		glPopMatrix();
	
		// Braço esquerdo
		glPushMatrix();
		glTranslatef(-0.8, 0.8f, 0.0f);
		drawCube(0.3f, 1, 0, 0); // Braço esquerdo amarelo
		glPopMatrix();
	
		// antebraço direito
		glPushMatrix();
		glTranslatef(0.5f, 0.8f, 0.0f);
		e = 0.9;
		glScaled(e, e, e);
		drawCube(0.3f, 1, 1, 1); // Braço esquerdo amarelo
		glPopMatrix();
	
		// Braço direito
		glPushMatrix();
		glTranslatef(0.8, 0.8f, 0.0f);
		drawCube(0.3f, 1, 0, 0); // Braço esquerdo amarelo
		glPopMatrix();
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
void redimensiona(int w, int h)
{
	glViewport(0, 0, w, h);
	height = h;
	float aspect = (float)w / (float)h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, aspect, 0.1, 500);
	glMatrixMode(GL_MODELVIEW);
}

void display()
{
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




void drawCube(float size, float r, float g, float b)
{
	glColor3f(r, g, b);
	glutSolidCube(size);
}

void drawSphere(float radius, float r, float g, float b)
{
	glColor3f(r, g, b);
	glutSolidSphere(radius, 20, 20);
}

// Controle do teclado
// Funcao para capturar entradas do teclado
void teclado(unsigned char key, int x, int y)
{
	const float movimento = 1.0; // Define a quantidade de movimento
	switch (key)
	{
	case '1':
		eyeX = personagemX-2;
		eyeY = 41;
		eyeZ = personagemZ+47;
		centerX = personagemX-2;
		centerY = 0;
		centerZ = personagemZ-2;
		break ;
	case '2':
		eyeZ = personagemZ+0;
		eyeY = 64;
		eyeX = personagemX-2-28;
		centerX = personagemX-2-28;
		centerY = 0;
		centerZ = personagemZ-2;
		break;
	case '3':
		eyeX = personagemX-26;
		eyeY = 22;
		eyeZ = personagemZ-25;
		centerX = personagemX+-23;
		centerY = 0;
		centerZ = personagemZ+18;
		break;

	case '4':
		eyeX = personagemX-28-1;
		eyeY = 41;
		eyeZ = personagemZ-64;
		centerX = personagemX-28-2,
		centerY = 0,
		centerZ = personagemZ-2;
		break;

	case 'w':
	case 'W': // Mover o personagem pra frente
		if (!temColisao(personagemX, personagemZ - 1, muro)){
			centerZ -= movimento;
			eyeZ-=movimento;
			
			personagemZ -= movimento;
			printf("(%.0f, %.0f)\n", personagemX, personagemZ);
			
			anguloRotacao = 180.0f;
		}
		break;
	case 's':
	case 'S': // Mover o personagem pra tras
		if (!temColisao(personagemX, personagemZ + 4, muro)){
			centerZ+= movimento;
			eyeZ+=movimento;
			
			personagemZ += movimento;
			printf("(%.0f, %.0f)\n", personagemX, personagemZ);

			anguloRotacao = 0.0f;			
		}
		break;
	case 'a':
	case 'A': // Mover o personagem pra esquerda
		if (!temColisao(personagemX - 1, personagemZ, muro)){
			eyeX -= movimento;
			centerX -= movimento;
			
			personagemX -= movimento;
			printf("(%.0f, %.0f)\n", personagemX, personagemZ);
			
			anguloRotacao = 270.0f;
		}
		break;
	case 'd':
	case 'D': // Mover o personagem pra direita
		if (!temColisao(personagemX + 4, personagemZ, muro)){
			eyeX += movimento;
			centerX += movimento;
			
			personagemX += movimento;
			printf("(%.0f, %.0f)\n", personagemX, personagemZ);
			
			anguloRotacao = 90.0f;
		}
		break;
	case 't':
	case 'T': // Mover a camera para frente (ao longo do eixo Z)
		eyeZ -= movimento;
		break;
	case 'g':
	case 'G': // Mover a camera para tras
		eyeZ += movimento;
		break;
	case 'f':
	case 'F': // Mover a camera para a esquerda (ao longo do eixo X)
		eyeX -= movimento;
		break;
	case 'h':
	case 'H': // Mover a camera para a direita
		eyeX += movimento;
		break;
	case 'q':
	case 'Q': // Mover a camera para cima (ao longo do eixo Y)
		eyeY += movimento;
		break;
	case 'e':
	case 'E': // Mover a camera para baixo
		eyeY -= movimento;
		break;
	case 'i':
	case 'I': // Mover o ponto de foco para frente (no eixo Z)
		centerZ -= movimento;
		break;
	case 'k':
	case 'K': // Mover o ponto de foco para tras
		centerZ += movimento;
		break;
	case 'j':
	case 'J': // Mover o ponto de foco para a esquerda (no eixo X)
		centerX -= movimento;
		break;
	case 'l':
	case 'L': // Mover o ponto de foco para a direita
		centerX += movimento;
		break;

	case 'p':
	case 'P':

		printf("Acabou");
		return;
		break;

	default:
		break;
	}

	glutPostRedisplay();
}



void moverPersonagem(float novoX, float novoZ)
{
	//int mapaX = (int)(novoX / 4); // Dividir por 4, que é a escala do terreno
	//int mapaZ = (int)(novoZ / 4);

	// Verificar se a nova posição está dentro dos limites do mapa e não é um muro

	personagemX = novoX;
	personagemZ = novoZ;

}


void init()
{
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


void mousePassiveMotion(int x, int y)
{
	m_x = x;
	m_y = height - y - 1;
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
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
	testaMatriz();

	init();
	glutMainLoop();
}