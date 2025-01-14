#include <set>
#include <math.h>
#include <cstdio>
#include <vector>
#include <utility>
#include <conio.h> 
#include <stdio.h>
#include <iostream>
#include <GL/glut.h>

#define STB_IMAGE_IMPLEMENTATION
#include "cenario.h"
#include "stb_image.h"
#include "glut_text.h"

#define LINHAS_MAPA 11
#define COLUNAS_MAPA 16
#define QTD_TEXTURAS 5

#define N 1 	// muro normal
#define M 2 	// muro com musgo
#define C 3 	// caixote

//Largura e altura da janela
int width = 800, height = 500;
float flag = true;
//Coordenadas da posicao atual do mouse
int m_x, m_y;

struct Bomba {
    float x, z;   // Posição da bomba (X e Z)
};
std::vector<Bomba> bombas;

/*
void atualizarBombas(float deltaTempo) {
    for (auto& bomba : bombas) {
        bomba.tempo -= deltaTempo; // Diminui o tempo de vida
    }

    // Remove bombas com tempo <= 0
    bombas.erase(std::remove_if(bombas.begin(), bombas.end(),
                                [](const Bomba& bomba) { return bomba.tempo <= 0.0f; }),
                 bombas.end());
}
*/

// Função para desenhar todas as bombas
void desenhaBombas() {
    for (std::vector<Bomba>::iterator it = bombas.begin(); it != bombas.end(); ++it) { // Usando iterador explícito
        glPushMatrix();
        glColor3f(0.0f, 0.0f, 0.0f); // Cor da bomba
        glTranslated(it->x, 3.7, it->z); // Posição da bomba (X, Y fixo, Z)
        glutSolidSphere(1.0, 50, 50); // Desenha a esfera
        glPopMatrix();
    }
}


int matrizMapa[LINHAS_MAPA][COLUNAS_MAPA] = {
	{M, N, M, N, M, N, M, N, M, N, M, N, M, N, M, N},
	{N, 0, 0, 0, N, 0, C, 0, 0, C, C, 0, C, C, 0, N},
	{M, C, M, 0, 0, 0, M, 0, 0, 0, N, C, N, M, 0, M},
	{N, 0, M, 0, M, C, N, C, N, 0, M, 0, N, 0, 0, N},
	{M, C, M, 0, 0, 0, N, 0, N, C, C, 0, C, 0, C, M},
	{N, 0, M, N, M, C, M, 0, 0, 0, M, C, N, N, C, N},
	{M, 0, M, 0, 0, 0, M, 0, N, C, M, 0, 0, 0, C, M},
	{N, 0, C, 0, 0, 0, C, 0, C, 0, 0, 0, C, 0, 0, N},
	{M, 0, M, 0, N, 0, C, 0, 0, 0, N, 0, M, N, 0, M},
	{M, 0, 0, 0, N, C, M, 0, N, C, M, 0, 0, C, 0, M},
	{N, M, N, N, M, M, N, N, M, M, N, M, N, M, N, N}
};

std::set<std::pair<int, int> > muro;   // Conjunto para armazenar as posicoes de colisao com o muro
std::set<std::pair<int, int> > caixa;  // Conjunto para armazenar as posicoes de colisao com as caixas

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

// captura coordenadas que geram colisao na matriz
void colisoesMatriz(){
    int inicioX = -4;  // Valor inicial de X
    int inicioZ = -22; // Valor inicial de Z

    // Iterar por todas as linhas e colunas da matriz
    for (int z = 0; z < LINHAS_MAPA; z++) {
        for (int x = 0; x < COLUNAS_MAPA; x++) {
            // Se o valor na matriz for 'M' ou 'N', calcular todas as coordenadas dentro da c?lula
            if (matrizMapa[z][x] == M || matrizMapa[z][x] == N) {
                // Para cada bloco de muro, iterar dentro dele (4x4 unidades)
                for (int i = 0; i < 4; ++i) {
                    for (int j = 0; j < 4; ++j) {
                        int posX = inicioX + x * 4 + i;
                        int posZ = inicioZ + z * 4 + j;

                        muro.insert(std::make_pair(posX, posZ));
                    }
                }
            }else if(matrizMapa[z][x] == C){
				// Para cada caixote, iterar dentro dele (4x4 unidades)
                for (int i = 0; i < 4; ++i) {
                    for (int j = 0; j < 4; ++j) {
                        int posX = inicioX + x * 4 + i;
                        int posZ = inicioZ + z * 4 + j;

                        caixa.insert(std::make_pair(posX, posZ));
                    }
                }
			}
        }
    }
}

// Prot?tipos das fun??es
void drawSphere(float radius, float r, float g, float b);
void drawCube(float size, float r, float g, float b);
void moverPersonagem(float novoX, float novoZ);
void mousePassiveMotion(int x, int y);

float personagemX = 0.0f, personagemZ = 0.0f;
float anguloRotacao = 0.0f; 
bool andando = false;
bool valorW = true;
std::vector<float> v;

void preencherVetor(std::vector<float>& v) {
    for (float i = 0.27f; i >= -0.27f; i -= 0.05f) {
        v.push_back(i);
    }
}

float k = 0;
float t = 0;

// FunÃ§Ã£o para desenhar o personagem Bomberman
void andarBomberman(float t) {
		// alternar  = true
		// perna direita pra tras e braÃ§o esquerdo pra frente
		// alternar = false
		// perna esquerda pra tras e braÃ§o direito pra frente
		
 glPushMatrix();
		// mexer na posi??o do player
		glTranslatef(-28+personagemX, 3.7, 0.0f+personagemZ);
		//glRotated(10,anguloRotacao,anguloRotacao,anguloRotacao); > talvez fique bom quando adicionar as animacoes
		float e = 2;
		
		glRotated(anguloRotacao,0,1,0);
		glScaled(e, e, e);
		
		// Fun??o para desenhar o personagem Bomberman
		glPushMatrix();
		// mexer no rosto + cabeca + olhos
		glTranslatef(0, -0.2, 0);
		glScaled(1.1, 1.1, 1.1);
		
		//Cabe?a
		glPushMatrix();
		glTranslatef(0.0f, 1.2f, 0.0f);
		drawSphere(0.5f, 1.0f, 1.0f, 1.0f); // Cabe?a branca
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
		glRotated(45,-1,0,0);
		drawCube(0.3f, 1, 1, 1); // Bra?o esquerdo amarelo
		glPopMatrix();
		
		// Perna  esquerda
		glPushMatrix();
		glTranslatef(-0.2f, -0.4, -t);
		glRotated(45,-1,0,0);
		e = 0.9;
		glScaled(e, e, e);
		drawCube(0.4f, 1.0f, 0.0f, 0.0f); // Perna esquerda vermelha
		glPopMatrix();
	
		// anteperna direita
		glPushMatrix();
		glTranslatef(0.2, -0.2, 0.0f);
		e = 0.9;
		glScaled(e, 1.25, e);
		glRotated(45,-1,0,0);
		drawCube(0.3f, 1, 1, 1); // Bra?o esquerdo amarelo
		glPopMatrix();
		
		// Perna  direita
		glPushMatrix();
		glTranslatef(0.2f, -0.4, t);
		glRotated(45,-1,0,0);
		glScaled(e, e, e);
		drawCube(0.4f, 1.0f, 0.0f, 0.0f); // Perna esquerda vermelha
		glPopMatrix();

		// mexer nos bra?os
glPushMatrix();
		glTranslatef(0.0f, -0.34, 0.0f);

		// antebra?o esquerdo

		glPushMatrix();
		
	
		glTranslatef(-0.5f, 0.8f, 0.0f);
		e = 0.9;
		glRotated(45, -1, 0, 0);      
		glScaled(e, e, e);
		drawCube(0.3f, 1, 1, 1); // Bra?o esquerdo amarelo
		glPopMatrix();
	
		// Bra?o esquerdo
		glPushMatrix();
		glTranslatef(-0.5, 0.63, t);
		glRotated(45, -1, 0, 0);        
		drawCube(0.3f, 1, 0, 0); // Bra?o esquerdo amarelo
		glPopMatrix();

		// antebra?o direito
		glPushMatrix();
		glTranslatef(0.5f, 0.8f, 0.0f);
		e = 0.9;
		glRotated(45, -1, 0, 0);  
		glScaled(e, e, e);
		drawCube(0.3f, 1, 1, 1); // Bra?o esquerdo amarelo
		glPopMatrix();
	
		// Bra?o direito
		glPushMatrix();
		glTranslatef(0.5, 0.63, -t);
		glRotated(45, -1, 0, 0);  
		drawCube(0.3f, 1, 0, 0); // Bra?o esquerdo amarelo
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


// Fun??o para desenhar o personagem Bomberman
void drawBomberman(){
	
glPushMatrix();
		// mexer na posi??o do player
		glTranslatef(-28+personagemX, 3.7, 0.0f+personagemZ);
		//glRotated(10,anguloRotacao,anguloRotacao,anguloRotacao); > talvez fique bom quando adicionar as animacoes
		float e = 2;
		glRotated(anguloRotacao,0,1,0);
		glScaled(e, e, e);
		
		// Fun??o para desenhar o personagem Bomberman
		glPushMatrix();
		// mexer no rosto + cabeca + olhos
		glTranslatef(0, -0.2, 0);
		glScaled(1.1, 1.1, 1.1);

		//Cabe?a
		glPushMatrix();
		glTranslatef(0.0f, 1.2f, 0.0f);
		drawSphere(0.5f, 1.0f, 1.0f, 1.0f); // Cabe?a branca
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
		//glRotated(45, -1,0,0);
		e = 0.9;
		glScaled(e, 1.25, e);
		drawCube(0.3f, 1, 1, 1); // Bra?o esquerdo amarelo
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
		drawCube(0.3f, 1, 1, 1); // Bra?o esquerdo amarelo
		glPopMatrix();
	
		// Perna  direita
		glPushMatrix();
		glTranslatef(0.2f, -0.5, 0.0f);
		glScaled(e, e, e);
		drawCube(0.4f, 1.0f, 0.0f, 0.0f); // Perna esquerda vermelha
		glPopMatrix();
	
	
// mexer nos bra?os
glPushMatrix();
		glTranslatef(0.0f, -0.34, 0.0f);
		// antebra?o esquerdo
		glPushMatrix();
		glTranslatef(-0.5f, 0.8f, 0.0f);
		e = 0.9;
		//glRotated(45, -1, 0, 0);      
		glScaled(e, e, e);
		drawCube(0.3f, 1, 1, 1); // Bra?o esquerdo amarelo
		glPopMatrix();
	
	
		// Bra?o esquerdo
		glPushMatrix();
		glTranslatef(-0.5, 0.63, 0);
		//glRotated(45, -1, 0, 0);        
		drawCube(0.3f, 1, 0, 0); // Bra?o esquerdo amarelo
		glPopMatrix();
	
		// antebra?o direito
		glPushMatrix();
		glTranslatef(0.5f, 0.8f, 0.0f);
		e = 0.9;
		//glRotated(45, -1, 0, 0);  
		glScaled(e, e, e);
		drawCube(0.3f, 1, 1, 1); // Bra?o esquerdo amarelo
		glPopMatrix();
	
		// Bra?o direito
		glPushMatrix();
		glTranslatef(0.5, 0.63, 0);
		//glRotated(45, -1, 0, 0);  
		drawCube(0.3f, 1, 0, 0); // Bra?o esquerdo amarelo
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

void spawnBomba() {
    Bomba novaBomba;
    novaBomba.x = personagemX - 28.3f; // Posição X da bomba (relativa ao personagem)
    novaBomba.z = personagemZ;        // Posição Z da bomba
    //novaBomba.tempo = TEMPO_DE_VIDA_BOMBA; // Tempo de vida inicial
    bombas.push_back(novaBomba);      // Adiciona ao vetor de bombas
}

void display()
{
	// Limpar os buffers de cor e profundidade
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Configurar a cÃ¢mera
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
	
	desenhaBombas();



	if (andando){
		andarBomberman(t);	
	}		
	else{
		drawBomberman();
	}
	    
		
     
			
	
		
	
	
	
	glEnable(GL_TEXTURE_2D);    // Reativar texturas caso necessÃ¡rio
	glPopMatrix();

	// Finalizar a renderizaÃ§Ã£o
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
	const float movimento = 0.4; // Define a quantidade de movimento
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
		eyeX = personagemX-21;
		eyeY = 16; // 14
		eyeZ = personagemZ+24;
		centerX = personagemX-21;
		centerY = 0;
		centerZ = personagemZ-3;
		break;

	case '4':
		eyeX = personagemX-28-1;
		eyeY = 41;
		eyeZ = personagemZ-64;
		centerX = personagemX-28-2,
		centerY = 0,
		centerZ = personagemZ-2;
		break;
	case 32:
		spawnBomba();
		break;
	case 'w':
	case 'W': // Mover o personagem pra frente
		if (!temColisao(personagemX, personagemZ - 1, muro) && !temColisao(personagemX, personagemZ - 1, caixa)){
			centerZ -= movimento;
			eyeZ-=movimento;
			personagemZ -= movimento;
			andando = true;
			if (k >= v.size())
				flag = false;
			if (k <= 0)
				flag = true;
			t = v[k];
			printf("t: %f ",t);
			if (flag)
				k++;	
            else
				k--;
			
			printf("(%.0f, %.0f)\n", personagemX, personagemZ);
			
			
			anguloRotacao = 180;
		}
		break;
	case 's':
	case 'S': // Mover o personagem pra tras
		if (!temColisao(personagemX, personagemZ + 4, muro) && !temColisao(personagemX, personagemZ + 4, caixa)){
			centerZ+= movimento;
			eyeZ+=movimento;
			personagemZ += movimento;
			
			andando = true;
			if (k >= v.size())
				flag = false;
			if (k <= 0)
				flag = true;
			t = v[k];
			printf("t: %f ",t);
			if (flag)
				k++;	
            else
				k--;
			
			printf("(%.0f, %.0f)\n", personagemX, personagemZ);

			anguloRotacao = 0.0f;			
		}
		break;
	case 'a':
	case 'A': // Mover o personagem pra esquerda
		if (!temColisao(personagemX - 1, personagemZ, muro) && !temColisao(personagemX - 1, personagemZ, caixa)){
			eyeX -= movimento;
			centerX -= movimento;
			personagemX -= movimento;
			
			andando = true;
			if (k >= v.size())
				flag = false;
			if (k <= 0)
				flag = true;
			t = v[k];
			printf("t: %f ",t);
			if (flag)
				k++;	
            else
				k--;
			printf("(%.0f, %.0f)\n", personagemX, personagemZ);
			
			anguloRotacao = 270.0f;
		}
		break;
	case 'd':
	case 'D': // Mover o personagem pra direita
		if (!temColisao(personagemX + 4, personagemZ, muro) && !temColisao(personagemX + 4, personagemZ, caixa)){
			eyeX += movimento;
			centerX += movimento;
			personagemX += movimento;
			
			andando = true;
			if (k >= v.size())
				flag = false;
			if (k <= 0)
				flag = true;
			t = v[k];
			printf("t: %f ",t);
			if (flag)
				k++;	
            else
				k--;
			
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


void tecladoSolta(unsigned char key, int x, int y){
	
		switch (key)
	{

	case 'w':
	case 'W': // Mover o personagem pra frente
		andando = false;

	case 's':
	case 'S': // Mover o personagem pra tras
		andando = false;
		break;
	case 'a':
	case 'A': // Mover o personagem pra esquerda
		andando = false;
	case 'd':
	case 'D': // Mover o personagem pra direita
		andando = false;

	default:
		
		
		break;
	}
	
	

	glutPostRedisplay();
	
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
	
	colisoesMatriz();
}


void mousePassiveMotion(int x, int y)
{
	m_x = x;
	m_y = height - y - 1;
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	preencherVetor(v);
	// Inicializa o GLUT e configura a janela
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Bomberman 3D");
	glutDisplayFunc(display);
	glutReshapeFunc(redimensiona);
	glutKeyboardFunc(teclado);
	glutKeyboardUpFunc(tecladoSolta); 
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

	init();
	glutMainLoop();
}
