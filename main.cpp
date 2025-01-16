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

float personagemX = 0.0f, personagemZ = 0.0f;

// Largura e altura da janela
int width = 800, height = 500;
float flag = true;

// Coordenadas da posicao atual do mouse
int m_x, m_y;

// ------------------- Mapa e colisoes --------------------
int matrizMapa[LINHAS_MAPA][COLUNAS_MAPA] = {
	{M, N, M, N, M, N, M, N, M, N, M, N, M, N, M, N},
	{N, 0, 0, 0, N, 0, C, 0, 0, C, C, 0, C, C, 0, N},
	{M, 0, M, 0, 0, 0, M, 0, 0, 0, N, C, N, M, 0, M},
	{N, C, M, 0, M, C, N, C, N, 0, M, 0, N, 0, 0, N},
	{M, 0, M, 0, 0, 0, N, 0, N, C, C, 0, C, 0, C, M},
	{N, 0, M, N, M, C, M, 0, 0, 0, M, C, N, N, C, N},
	{M, 0, M, 0, 0, 0, M, 0, N, C, M, 0, 0, 0, C, M},
	{N, 0, C, 0, 0, 0, C, 0, C, 0, 0, 0, C, 0, 0, N},
	{M, 0, M, 0, N, 0, C, 0, 0, 0, N, 0, M, N, 0, M},
	{M, 0, 0, 0, N, C, M, 0, N, C, M, 0, 0, C, 0, M},
	{N, M, N, N, M, M, N, N, M, M, N, M, N, M, N, N}
};

// Conjunto para armazenar coordenadas de colisoes
std::set<std::pair<int, int> > muro;
std::set<std::pair<int, int> > caixa;

// Funcao booleanda
bool temColisao(int x, int y, const std::set< std::pair<int, int> >& colisoes) {
	// Para cada coordenada do conjunto	
    for (std::set< std::pair<int, int> >::iterator it = colisoes.begin(); it != colisoes.end(); ++it) {
        if (it->first == x && it->second == y) {
            return true;
        }
    }
    return false;
}

// captura coordenadas que geram colisao na matriz
void colisoesMatriz(){
    int inicioX = -4, inicioZ = -22; // Valor inicial de X e Z

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
                int posX = inicioX + x * 4;
                int posZ = inicioZ + z * 4;
                caixa.insert(std::make_pair(posX, posZ));     
				}
			}
        }
    }

// ------------------- Configuracoes da Bomba -------------------
// Estrutura
struct Bomba {
    float x, z;   // Posição da bomba (X e Z)
    int tempoVida;
    bool imunidade;      // Flag para imunidade de colisão
    int tempoImunidade;  // Tempo restante em imunidade (em frames ou milissegundos)
};

// controla detalhes da animacao
float escala = 1.0f;    // tamanho da bomba para controlar ela pulsando
bool isWhite = false;   // alterna entra branco e preto
bool aumentando = true; // verificar a pulsacao dela

// vetor de bombas (permite atirar multiplas bombas)
std::vector<Bomba> bombas;

// adiciona um novo registro de bomba
void spawnBomba() {
    Bomba novaBomba;
    novaBomba.x = personagemX - 28; // Posição X da bomba (relativa ao personagem)
    novaBomba.z = personagemZ;        // Posição Z da bomba
    novaBomba.tempoVida = 5000;			// 5 segundos
    novaBomba.imunidade = true;        // Começa com imunidade
    novaBomba.tempoImunidade = 1000;    // Exemplo: 1s de imunidade
    bombas.push_back(novaBomba);      // Adiciona ao vetor de bombas
}

// verifica colisao bomba arredondando coordenadas
bool verificarColisaoComBombas(float jogadorX, float jogadorZ) {
    for (std::vector<Bomba>::iterator it = bombas.begin(); it != bombas.end(); ++it) {
        if (round(it->x + 28) == round(jogadorX) && round(it->z) == round(jogadorZ) && !it->imunidade) {
            return true;  // Colisão detectada
        }
    }
    return false; // Nenhuma colisão
}

// Desenhar todas as bombas (chamada na funcao display)
void desenhaBombas() {
    for (std::vector<Bomba>::iterator it = bombas.begin(); it != bombas.end(); ++it) { // Usando iterador explícito
        glPushMatrix();
        if (isWhite) glColor3f(1.0f, 1.0f, 1.0f);  // Cor branca
    	else glColor3f(0.0f, 0.0f, 0.0f);  		   // Cor preta
        glTranslated(it->x, 3.2, it->z); // Posição da bomba (X, Y fixo, Z)
    	glScalef(escala, escala, escala); 
        glutSolidSphere(1.0, 50, 50); // Desenha a esfera
        glPopMatrix();
    }
}

std::vector<std::pair<int, int> > posicoesExplosoes;

void desenharExplosao(float x, float z) {
    glPushMatrix();  // Salva o estado atual da matriz

    glColor3f(1.0f, 0.0f, 0.0f);  // Define a cor da esfera (vermelha)
    glTranslatef(x, 3.7, z);      // Aplica a translação
    printf("Explosao em (%.1f, %.1f)\n", x, z);

    // Cria um objeto quadrático para desenhar a esfera
    GLUquadric* quadric = gluNewQuadric();
    gluSphere(quadric, 1.0, 30, 30);  // Desenha uma esfera com raio 1.0 e 30 segmentos

    gluDeleteQuadric(quadric);  // Libera os recursos usados pelo objeto quadrático

    glPopMatrix();  // Restaura o estado da matriz
}

void removerCaixote(int posX, int posZ){
	int xMatriz = posX + 2;
	int zMatriz = posZ + 15;
	printf("na matriz: (%d, %d)\n", xMatriz, zMatriz);
	caixa.erase(std::make_pair(posX, posZ));  // remover colisão
	int x = (posX - 2) / 4; // Calcula a posição na matriz
    int z = (posZ - 15) / 4;
    printf("caixote em (%d, %d)\n", x, z);
}

void rastroExplosao(int bombaX, int bombaZ) {
	printf("\nBomba explodiu em (%d, %d)\n", bombaX, bombaZ);
	
    // Vetores para definir as direções (cima, baixo, esquerda, direita)
    int direcoes[4][2] = {
        {0, 4},  // Para cima (Z+)
        {0, -4}, // Para baixo (Z-)
        {-4, 0}, // Para esquerda (X-)
        {4, 0}   // Para direita (X+)
    };

    // Percorre cada direção
    for (int i = 0; i < 4; i++) {
        int dx = direcoes[i][0];
        int dz = direcoes[i][1];

        // Explode até 2 coordenadas nessa direção
        for (int passo = 1; passo <= 2; passo++) {
            int novoX = bombaX + dx * passo;
            int novoZ = bombaZ + dz * passo;

            // Verifica se há um muro, se sim, interrompe a explosão nessa direção
            if (muro.find(std::make_pair(novoX, novoZ)) != muro.end()) {
                break;
            }

            // Verifica se há uma caixa, remove a caixa e interrompe a explosão
            if (caixa.find(std::make_pair(novoX, novoZ)) != caixa.end()) {
            	removerCaixote(novoX, novoZ);
                break;
            }

            // Marca a explosão na posição atual (aqui você pode desenhar, registrar, etc.)
            desenharExplosao(novoX, novoZ);
            glutPostRedisplay();
        }
    }
}

// Funcoes de animacao
void atualizarCor(int valor){
	isWhite = !isWhite;
	
	// atualizar tela
    glutPostRedisplay();

    // Chama a função de atualização novamente após 350ms (0.35 segundos)
    glutTimerFunc(350, atualizarCor, 0);
}

void atualizarBombas(int value) {
    // Usando iterador explícito
    for (std::vector<Bomba>::iterator it = bombas.begin(); it != bombas.end(); ) {
        if (it->tempoVida > 0) {
            it->tempoVida -= 50; // Reduz o tempo de vida em 50ms

            // Caso o tempo de vida expire, remove a bomba
            if (it->tempoVida <= 0) {
                it = bombas.erase(it); // Remove a bomba do vetor e retorna o próximo iterador válido
                rastroExplosao(round(it->x + 28), round(it->z));
                continue; // Pula a iteração para evitar acessar um iterador inválido
            }
        }

        if (it->imunidade) {
            it->tempoImunidade -= 50; // Reduz o tempo de imunidade
            if (it->tempoImunidade <= 0) {
                it->imunidade = false; // Remove a imunidade
            }
        }

        ++it; // Avança para a próxima bomba
    }

    // Reagenda a atualização após 50ms
    glutTimerFunc(50, atualizarBombas, 0);
}

void atualizarEscala(int value) {
    if (aumentando) {
        escala += 0.05f;  		// Aumenta a escala
        if (escala >= 1.5f) {   // Limite máximo
            aumentando = false;
        }
    } else {
        escala -= 0.05f;  		// Diminui a escala
        if (escala <= 1.0f) {   // Limite mínimo
            aumentando = true;
        }
    }

    // Solicita a atualização da tela
    glutPostRedisplay();

    // Chama a função novamente após 50ms
    glutTimerFunc(50, atualizarEscala, 0);
}


// ------------------- Desenha o Personagem -------------------
// Prot?tipos das fun??es
void drawSphere(float radius, float r, float g, float b);
void drawCube(float size, float r, float g, float b);
void moverPersonagem(float novoX, float novoZ);
void mousePassiveMotion(int x, int y);

float anguloRotacao = 0.0f; 
bool andando = false;
bool valorW = true;
std::vector<float> v;

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
		glTranslatef(-28+personagemX, 3.7, personagemZ);
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

// ------------------- Configuracoes do GLUT -------------------
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
		eyeY = 16;
		eyeZ = personagemZ+24;
		centerX = personagemX-21;
		centerY = 0;
		centerZ = personagemZ-3;
		break;

	case '4':
		eyeX = personagemX-2;
		eyeY = 28;
		eyeZ = personagemZ+52;
		centerX = personagemX-2,
		centerY = 0,
		centerZ = personagemZ-12;
		break;
	case 32:
		spawnBomba();
		break;
	case 'w':
	case 'W': // Mover o personagem pra frente
		if (!temColisao(personagemX, personagemZ - 1, muro) && !temColisao(personagemX, personagemZ - 4, caixa) && !verificarColisaoComBombas(personagemX, personagemZ - 3)){
			centerZ -= movimento;
			eyeZ-=movimento;
			personagemZ -= movimento;
			andando = true;
			if (k >= v.size())
				flag = false;
			if (k <= 0)
				flag = true;
			t = v[k];
			//printf("t: %f ",t);
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
		if (!temColisao(personagemX, personagemZ + 4, muro) && !temColisao(personagemX, personagemZ + 4, caixa) && !verificarColisaoComBombas(personagemX, personagemZ + 3)){
			centerZ+= movimento;
			eyeZ+=movimento;
			personagemZ += movimento;
			
			andando = true;
			if (k >= v.size())
				flag = false;
			if (k <= 0)
				flag = true;
			t = v[k];
			//printf("t: %f ",t);
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
		if (!temColisao(personagemX - 1, personagemZ, muro) && !temColisao(personagemX - 3, personagemZ, caixa) && !verificarColisaoComBombas(personagemX - 3, personagemZ)){
			eyeX -= movimento;
			centerX -= movimento;
			personagemX -= movimento;
			
			andando = true;
			if (k >= v.size())
				flag = false;
			if (k <= 0)
				flag = true;
			t = v[k];
			//printf("t: %f ",t);
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
		if (!temColisao(personagemX + 4, personagemZ, muro) && !temColisao(personagemX + 4, personagemZ, caixa) && !verificarColisaoComBombas(personagemX + 3, personagemZ)){
			eyeX += movimento;
			centerX += movimento;
			personagemX += movimento;
			
			andando = true;
			if (k >= v.size())
				flag = false;
			if (k <= 0)
				flag = true;
			t = v[k];
			//printf("t: %f ",t);
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
	
	// Funcoes de animacao
	glutTimerFunc(500, atualizarCor, 0);
	glutTimerFunc(50, atualizarEscala, 0);  
	glutTimerFunc(50, atualizarBombas, 0); // Inicia a animação das bombas
	
	init();
	glutMainLoop();
}