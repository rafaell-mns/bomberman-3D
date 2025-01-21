#include <map>
#include <set>
#include <ctime>
#include <math.h>
#include <cstdio>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <utility>
#include <conio.h> 
#include <stdio.h>
#include <iostream>
#include <GL/glut.h>
#include <Windows.h>
#include <mmsystem.h>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "cenario.h"
#include "stb_image.h"
#include "glut_text.h"
#define LINHAS_MAPA 11
#define COLUNAS_MAPA 16

#define QTD_TEXTURAS 12

#define N 1 	// muro normal
#define M 2 	// muro com musgo
#define C 3 	// caixote

// Vetor de texturas
GLuint texID[QTD_TEXTURAS];

void danoExplosao();
void desenhaIcone(float x, float y, float z, float tamanho, GLuint texID);

float personagemX = 0.0f, personagemZ = 0.0f;
enum cam{um = 1, dois, tres, quatro};
enum paises{brasil = 1, argentina, portugal, japao};
int ultima_cam = um;
std::vector<float> v;
int mudar_musica = 1;
bool mostrarTexto = true;
int tempoInicial = 0;
int tempoFinal = 0;
int width = 800, height = 500;
float flag = true;
float perdeuTudo = false;
int quantVidas = 3;
int maxBombas = 2;
int bombasAtuais = 0;
bool dano = false;
int quantBotsMortos = 0; // quantidade bots que o player matou 
float movimento = 0.5;





// Conjunto para armazenar coordenadas de colisoes
std::set<std::pair<int, int> > muro;
std::set<std::pair<int, int> > caixa;
std::map<std::pair<int, int>, std::pair<int, int> > caixaMatriz; 
std::set<std::pair<int, int> > caixasRemovidas;



struct Itens {
    float x, z;
    bool JaAdquirido;
};

// armazenar as coordenadas dos powerups
std::vector<Itens> bombasItem;
std::vector<Itens> botas;

// Variaveis para controlar a camera
float eyeX = -2, eyeY = 41, eyeZ = 47;   	   // Posicao inicial da camera
float centerX = -2, centerY = 0, centerZ = -2; // Ponto de foco inicial

// Armazena o tempo da ?ltima colis?o
static time_t ultimoTempoColisao = 0;

// Fun??o para gerar um n?mero aleat?rio dentro de um intervalo
int numeroAleatorio(int minimo, int maximo) {
    return minimo + rand() % ((maximo - minimo) + 1);
}

int totalScore;
int calcularScore(){
	return (caixasRemovidas.size() * 10) - ((3 - quantVidas) * 20) + (quantBotsMortos * 30);
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

bool rangeColisaoitem(int x1, int z1, int x2, int z2) {
    return (std::abs(x1 - x2) <= 3) && (std::abs(z1 - z2) <= 3);
}


// Coordenadas da posicao atual do mouse
int m_x, m_y;

// ------------------- Mapa e colisoes --------------------
int matrizMapa[LINHAS_MAPA][COLUNAS_MAPA] = {
    {M, M, N, M, N, N, M, N, M, N, M, M, N, M, N, M},
    {M, 0, C, 0, M, 0, 0, 0, N, 0, 0, 0, 0, 0, 0, M},
    {N, 0, M, 0, N, 0, M, 0, N, 0, M, N, C, M, 0, N},
    {N, 0, 0, 0, M, C, M, 0, N, 0, 0, 0, 0, 0, 0, M},
    {M, 0, M, 0, 0, 0, C, 0, M, N, M, M, N, 0, 0, M},
    {M, 0, 0, 0, N, N, M, 0, 0, C, M, 0, M, 0, 0, M},
    {M, 0, N, 0, 0, 0, M, M, 0, 0, 0, 0, M, 0, C, N},
    {M, 0, 0, 0, M, 0, C, 0, 0, M, M, 0, 0, 0, 0, N},
    {N, 0, M, 0, M, 0, M, 0, 0, 0, C, 0, N, M, 0, N},
    {M, 0, N, 0, C, 0, M, 0, C, 0, M, 0, 0, 0, 0, M},
    {N, M, M, M, N, M, M, N, M, N, M, M, N, M, N, M}
};


void drawCaixaDestruida() {
    glPushMatrix();
   int e = 2.8;
	glTranslatef(-28, 5, 6);
	 glScalef(e,e,e);
    // Base da caixa
    glPushMatrix();
    glTranslatef(0.0f, -0.5f, 0.0f);
    glScalef(1.0f, 0.5f, 1.0f);

   drawCube(1.0f, 0.5f, 0.2f, 0.1f);
    glPopMatrix();

    // Lado esquerdo
    glPushMatrix();
    glTranslatef(-0.45f, -0.3, 0.0f);
    glRotatef(-30, 0.0f, 0.0f, 1.0f);
    glScalef(0.1f, 1.0f, 1.0f);
    drawCube(1.0f, 0.5f, 0.2f, 0.1f);
    glPopMatrix();

    // Lado direito
    glPushMatrix();
    glTranslatef(0.45f, 0.0f, 0.0f);
    glRotatef(30.0f, 0.0f, 0.0f, 1.0f);
    glScalef(0.1f, 1.0f, 1.0f);
     drawCube(1.0f, 0.5f, 0.2f, 0.1f);
    glPopMatrix();

    // Painel traseiro
    glPushMatrix();
    glTranslatef(0.0f, -0.5, -0.45f);
    glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
    glScalef(1.0f, 1.0f, 0.1f);
     drawCube(1.0f, 0.5f, 0.2f, 0.1f);
    glPopMatrix();

    // Painel frontal
    glPushMatrix();
    glTranslatef(0.0f, -0.5, 0.45f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glScalef(1.0f, 1.0f, 0.1f);
    drawCube(1.0f, 0.5f, 0.2f, 0.1f);
    glPopMatrix();

    // Detalhes quebrados
    glPushMatrix();
    glTranslatef(-0.2f, 0.3f, 0.2f);
    glRotatef(25.0f, 0.0f, 1.0f, 0.0f);
    glScalef(0.05f, 0.5f, 0.2f);
    drawCube(1.0f, 0.5f, 0.25f, 0.1f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.3f, -0.2f, -0.2f);
    glRotatef(-30.0f, 1.0f, 0.0f, 0.0f);
    glScalef(0.1f, 0.3f, 0.3f);
    drawCube(1.0f, 0.5f, 0.25f, 0.1f);
    glPopMatrix();

    glPopMatrix();
}

// funcao importante de upar um audio em um novo canal de audio criado
void uparAudio(const char* caminho);
void tocarmusica(const char* caminho);

// Funcao booleana
bool temColisao(int x, int y, const std::set< std::pair<int, int> >& colisoes) {
	// Para cada coordenada do conjunto	
    for (std::set< std::pair<int, int> >::iterator it = colisoes.begin(); it != colisoes.end(); ++it) {
        if (it->first == x && it->second == y) {
        	//printf("\nTEM COLISAO\n");
            return true;
            
        }
    }
     //printf("\n NAO TEM COLISAO\n");
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
                for (int i = 0; i < 4; ++i) {
                    for (int j = 0; j < 4; ++j) {
                        int posX = inicioX + x * 4 + i;
                        int posZ = inicioZ + z * 4 + j;
                        
                        // coordenadas que geram colisao
                        caixa.insert(std::make_pair(posX, posZ));
                        
                        // correspondencia entre a coordenada e o indice da matriz
                        caixaMatriz[std::make_pair(posX, posZ)] = std::make_pair(x, z);
                    }
                }
			}
		}
	}
}

std::pair<int, int> obterIndiceCaixa(int posX, int posZ) {
    std::map<std::pair<int, int>, std::pair<int, int> >::iterator it = caixaMatriz.find(std::make_pair(posX, posZ));
    if (it != caixaMatriz.end()) {
        return it->second; // Retorna o ?ndice
    }
    return std::make_pair(-1, -1); // Retorna um valor inv?lido se n?o encontrar
}

// ------------------- Configuracoes da Bomba -------------------
// Estrutura
struct Bomba {
    float x, z;   // Posi??o da bomba (X e Z)
    int tempoVida;
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
    novaBomba.x = personagemX - 28; // Posi??o X da bomba (relativa ao personagem)
    novaBomba.z = personagemZ;        // Posi??o Z da bomba
    novaBomba.tempoVida = 5650;			// 5 segundos
    bombas.push_back(novaBomba);      // Adiciona ao vetor de bombas
}

// verifica colisao bomba arredondando coordenadas
bool verificarColisaoComBombas(float jogadorX, float jogadorZ) {
    for (std::vector<Bomba>::iterator it = bombas.begin(); it != bombas.end(); ++it) {
        if (round(it->x + 28) == round(jogadorX) && round(it->z) == round(jogadorZ)) {
            return true;  // Colis?o detectada
        }
    }
    return false; // Nenhuma colis?o
}

// Desenhar todas as bombas (chamada na funcao display)
void desenhaBombas() {
    for (std::vector<Bomba>::iterator it = bombas.begin(); it != bombas.end(); ++it) { // Usando iterador expl?cito
        glPushMatrix();
    	
        if (isWhite) glColor3f(1.0f, 1.0f, 1.0f);  // Cor branca
    	else glColor3f(0.0f, 0.0f, 0.0f);  		   // Cor preta
        glTranslated(it->x, 3.2, it->z); // Posi??o da bomba (X, Y fixo, Z)
    	glScalef(escala, escala, escala); 
        glutSolidSphere(1.0, 50, 50); // Desenha a esfera
        glPopMatrix();
    }
}

std::set<std::pair<int, int> > indicesProcessados;
void removerCaixote(int posX, int posZ) {
    for (int i = -3; i <= 3; ++i) {
        for (int j = -3; j <= 3; ++j) {
            int newX = posX + i;
            int newZ = posZ + j;

            // Localizar a posi??o na lista de caixas
            std::set<std::pair<int, int> >::iterator it = caixa.find(std::make_pair(newX, newZ));
            if (it != caixa.end()) {
                caixa.erase(it); // Remove a colis?o do bloco

                // Obt?m os ?ndices correspondentes
                std::pair<int, int> indices = obterIndiceCaixa(newX, newZ);
                int yMatriz = indices.first;
                int xMatriz = indices.second;

                // Verifique se este ?ndice j? foi processado nesta chamada
                if (indicesProcessados.find(indices) == indicesProcessados.end()) {
                    indicesProcessados.insert(indices); // Marca o ?ndice como processado

                    // Adiciona ao conjunto de caixas removidas globalmente
                    if (caixasRemovidas.find(indices) == caixasRemovidas.end()) {
                        caixasRemovidas.insert(indices);
                        totalScore = calcularScore();

                        // Sorteia se ter? power-up
                        int temPowerUp = numeroAleatorio(0, 4);
                        if (temPowerUp > 0) {
                            int qualPowerUp = numeroAleatorio(9,10); // 9 ou 10
                            matrizMapa[xMatriz][yMatriz] = qualPowerUp;
                            if (qualPowerUp == 9){
                            	Itens obj;
                            	obj.x = newX;
                            	obj.z = newZ;
                            	obj.JaAdquirido = false;
								bombasItem.push_back(obj);
							}else {
               					Itens obj;
                            	obj.x = newX;
                            	obj.z = newZ;
                            	obj.JaAdquirido = false;
								botas.push_back(obj);
							}

                            
                            
                        } else {
                            matrizMapa[xMatriz][yMatriz] = 0;
                        }
                    } else {
                        matrizMapa[xMatriz][yMatriz] = 0;
                    }

                    // Atualiza o terreno
                    desenhaTerreno(LINHAS_MAPA, COLUNAS_MAPA, 4, texID, matrizMapa);
                }
            }
        }
    }
}


bool playerPerdeuVida;

void rastroExplosao(int bombaX, int bombaZ) {
	// printf("\nBomba explodiu em (%d, %d)\n", bombaX, bombaZ);
	playerPerdeuVida = false;
	
    // Vetores para definir as direcoes (cima, baixo, esquerda, direita)
    int direcoes[4][2] = {
        {0, 4},  // Para cima (Z+)
        {0, -4}, // Para baixo (Z-)
        {-4, 0}, // Para esquerda (X-)
        {4, 0}   // Para direita (X+)
    };

    // Percorre cada direcao
    for (int i = 0; i < 4; i++) {
        int dx = direcoes[i][0];
        int dz = direcoes[i][1];

        // Explode ate 2 coordenadas nessa direcao
        for (int passo = 1; passo <= 2; passo++) {
            int novoX = bombaX + dx * passo;
            int novoZ = bombaZ + dz * passo;

			// printf("Explosao em (%d, %d)\n", novoX, novoZ);
			
			// se encontrar um muro, interrompe a explosao naquela direcao
			if (muro.find(std::make_pair(novoX, novoZ)) != muro.end()) break;
            
            removerCaixote(novoX, novoZ);
        	danoExplosao();
               
            glutPostRedisplay();
        }
    }
}

// Funcoes de animacao
void atualizarCor(int valor){
	isWhite = !isWhite;
	
	// atualizar tela
    glutPostRedisplay();

    // Chama a fun??o de atualiza??o novamente ap?s 350ms (0.35 segundos)
    glutTimerFunc(350, atualizarCor, 0);
}

void atualizarBombas(int value) {
    // Usando iterador expl?cito
    for (std::vector<Bomba>::iterator it = bombas.begin(); it != bombas.end(); ) {
        if (it->tempoVida > 0) {
            it->tempoVida -= 50; // Reduz o tempo de vida em 50ms

            // Caso o tempo de vida expire, remove a bomba
            if (it->tempoVida <= 0) {
            	rastroExplosao(round(it->x + 28), round(it->z));
                it = bombas.erase(it); // Remove a bomba do vetor e retorna o pr?ximo iterador v?lido
                bombasAtuais -= 1;
                continue; // Pula a itera??o para evitar acessar um iterador inv?lido
            }
        }

        ++it; // Avan?a para a pr?xima bomba
    }

    // Reagenda a atualiza??o ap?s 50ms
    glutTimerFunc(50, atualizarBombas, 0);
}

void atualizarEscala(int value) {
    if (aumentando) {
        escala += 0.05f;  		// Aumenta a escala
        if (escala >= 1.5f) {   // Limite m?ximo
            aumentando = false;
        }
    } else {
        escala -= 0.05f;  		// Diminui a escala
        if (escala <= 1.0f) {   // Limite m?nimo
            aumentando = true;
        }
    }

    // Solicita a atualiza??o da tela
    glutPostRedisplay();

    // Chama a fun??o novamente ap?s 50ms
    glutTimerFunc(50, atualizarEscala, 0);
}

// ------------------- Desenha o Personagem -------------------
// Prot?tipos das fun??es
void drawSphere(float radius, float r, float g, float b);
void drawCube(float size, float r, float g, float b);
void moverPersonagem(float novoX, float novoZ);
void mousePassiveMotion(int x, int y);


bool andando = false;
bool valorW = true;




// funcao para desenhar vetor usado na animacao
void preencherVetor(std::vector<float>& v) {
    for (float i = 0.27f; i >= -0.27f; i -= 0.05f) {
        v.push_back(i);
    }
}

float k = 0;



// Fun??o para desenhar o personagem Bomberman
void drawBomberman(float anguloRotacao, float x_inicial, float z_inicial,float x, float z, float r_capuz, float g_capuz, float b_capuz, float r_sec, float g_sec, float b_sec, float r_corpo, float g_corpo, float b_corpo ){
	
glPushMatrix();
		// mexer na posi??o do player
		glTranslatef(x_inicial+x, 3.7, z_inicial+z);
		//glRotated(10,anguloRotacao,anguloRotacao,anguloRotacao); > talvez fique bom quando adicionar as animacoes
		float e = 2;
		glRotated(anguloRotacao,0,1,0);
		if (perdeuTudo)
			glRotated(-90,1,0,0);
		glScaled(e, e, e);
		
		// Fun??o para desenhar o personagem Bomberman
		glPushMatrix();
		// mexer no rosto + cabeca + olhos
		glTranslatef(0, -0.2, 0);
		glScaled(1.1, 1.1, 1.1);

		//Cabe?a
		glPushMatrix();
		glTranslatef(0.0f, 1.2f, 0.0f);
		drawSphere(0.5f, r_capuz, g_capuz, b_capuz); // Cabe?a branca
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
		drawCube(0.7f, r_corpo, g_corpo, b_corpo); // Corpo azul
		glPopMatrix();
	
		// fundo do cinto
		glPushMatrix();
		glTranslatef(0.0f, 0.1, 0.35f);
		e = 0.6;
		glScaled(1.8, 0.3, 0.1);
		drawCube(0.4f, r_sec, g_sec, b_sec); // Perna esquerda vermelha
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
		drawCube(0.3f, r_capuz, g_capuz, b_capuz); // Bra?o esquerdo amarelo
		glPopMatrix();
	
		// Perna  esquerda
		glPushMatrix();
		glTranslatef(-0.2f, -0.5, 0.0f);
		e = 0.9;
		glScaled(e, e, e);
		drawCube(0.4f, r_sec, g_sec, b_sec); // Perna esquerda vermelha
		glPopMatrix();
	
		// anteperna direita
		glPushMatrix();
		glTranslatef(0.2, -0.2, 0.0f);
		e = 0.9;
		glScaled(e, 1.25, e);
		drawCube(0.3f, r_capuz, g_capuz, b_capuz); // Bra?o esquerdo amarelo
		glPopMatrix();
	
		// Perna  direita
		glPushMatrix();
		glTranslatef(0.2f, -0.5, 0.0f);
		glScaled(e, e, e);
		drawCube(0.4f, r_sec, g_sec, b_sec); // Perna esquerda vermelha
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
		drawCube(0.3f, r_capuz, g_capuz, b_capuz); // Bra?o esquerdo amarelo
		glPopMatrix();
	
	
		// Bra?o esquerdo
		glPushMatrix();
		glTranslatef(-0.5, 0.63, 0);
		//glRotated(45, -1, 0, 0);        
		drawCube(0.3f, r_sec, g_sec, b_sec); // Bra?o esquerdo amarelo
		glPopMatrix();
	
		// antebra?o direito
		glPushMatrix();
		glTranslatef(0.5f, 0.8f, 0.0f);
		e = 0.9;
		//glRotated(45, -1, 0, 0);  
		glScaled(e, e, e);
		drawCube(0.3f, r_capuz, g_capuz, b_capuz); // Bra?o esquerdo amarelo
		glPopMatrix();
	
		// Bra?o direito
		glPushMatrix();
		glTranslatef(0.5, 0.63, 0);
		//glRotated(45, -1, 0, 0);  
		drawCube(0.3f, r_sec, g_sec, b_sec); // Bra?o esquerdo amarelo
		glPopMatrix();
glPopMatrix();

		// Antena
		glPushMatrix();
		glTranslatef(0.0f, 1.8f, 0.0f);
		drawSphere(0.1f, r_sec, g_sec, b_sec); // Bolinha vermelha na antena
		glPopMatrix();
	
		glPushMatrix();
		glTranslatef(0.0f, 1.5f, 0.0f);
		glScalef(0.1f, 0.5f, 0.1f);
		drawCube(1.0f, 0.5f, 0.5f, 0.5f); // Haste da antena cinza
		glPopMatrix();
glPopMatrix();
}

void mudarPais(float pais ,float*Rcapuz,float*Gcapuz,float*Bcapuz, float*Rsec, float*Gsec,float* Bsec, float*Rcorpo, float*Gcorpo, float*Bcorpo){
	//brasil
	if (pais == brasil){
		*Rcapuz = 0; *Gcapuz = 0.5; *Bcapuz = 0;
 	    *Rsec = 0.9; *Gsec = 0.9; *Bsec = 0;
		*Rcorpo = 0; *Gcorpo = 0.6; *Bcorpo = 0;
	}else if (pais == argentina){
		*Rcapuz = 1; *Gcapuz = 1; *Bcapuz = 1;
 	    *Rsec = 0; *Gsec = 1; *Bsec = 1;
		*Rcorpo = 0; *Gcorpo = 0.8; *Bcorpo = 0.8;
	}else if (pais == portugal){
        *Rcapuz = 0; *Gcapuz = 0.4; *Bcapuz = 0;
 	    *Rsec = 1; *Gsec = 0; *Bsec = 0;
		*Rcorpo = 0.8; *Gcorpo = 0; *Bcorpo = 0;
	} else if (pais == japao){
    	*Rcapuz = 1; *Gcapuz = 1; *Bcapuz = 1;
 	    *Rsec = 1; *Gsec = 0; *Bsec = 0;
		*Rcorpo = 0.8; *Gcorpo = 0; *Bcorpo = 0;
	}
}


struct Jogador{
	float addX, addZ;
	float t; // variavel usada na translacao para fazer a animacao
	float x,z; // posicao do bot
	float anguloRotacao;
	bool flag;
	std::vector<float> v;
	 float movimento;
	float k;
	int ultimaDirecao;
	bool vivo;
void preencherVetor() {
    for (float i = 0.27f; i >= -0.27f; i -= 0.05f) {
        v.push_back(i);
    }
}
	
	// Fun??o para desenhar o personagem Bomberman
	void andarBomberman(float x2, float z2, float r_capuz, float g_capuz, float b_capuz, float r_sec, float g_sec, float b_sec, float r_corpo, float g_corpo, float b_corpo) {

		
	 glPushMatrix();
			// mexer na posi??o do player
			glTranslatef(x+ x2, 3.7, z+z2);
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
			drawSphere(0.5f, r_capuz, g_capuz, b_capuz); // Cabe?a branca
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
			drawCube(0.7f, r_corpo,g_corpo, b_corpo); // Corpo azul
			glPopMatrix();
		
			// fundo do cinto
			glPushMatrix();
			glTranslatef(0.0f, 0.1, 0.35f);
			e = 0.6;
			glScaled(1.8, 0.3, 0.1);
			drawCube(0.4f, r_sec,g_sec,b_sec); // Perna esquerda vermelha
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
			drawCube(0.3f, r_capuz, g_capuz, b_capuz); // Bra?o esquerdo amarelo
			glPopMatrix();
			
			// Perna  esquerda
			glPushMatrix();
			glTranslatef(-0.2f, -0.4, -t);
			glRotated(45,-1,0,0);
			e = 0.9;
			glScaled(e, e, e);
			drawCube(0.4f, r_sec,g_sec,b_sec); // Perna esquerda vermelha
			glPopMatrix();
		
			// anteperna direita
			glPushMatrix();
			glTranslatef(0.2, -0.2, 0.0f);
			e = 0.9;
			glScaled(e, 1.25, e);
			glRotated(45,-1,0,0);
			drawCube(0.3f,r_capuz, g_capuz, b_capuz); // Bra?o esquerdo amarelo
			glPopMatrix();
			
			// Perna  direita
			glPushMatrix();
			glTranslatef(0.2f, -0.4, t);
			glRotated(45,-1,0,0);
			glScaled(e, e, e);
			drawCube(0.4f, r_sec,g_sec,b_sec); // Perna esquerda vermelha
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
			drawCube(0.3f, r_capuz, g_capuz, b_capuz); // Bra?o esquerdo amarelo
			glPopMatrix();
		
			// Bra?o esquerdo
			glPushMatrix();
			glTranslatef(-0.5, 0.63, t);
			glRotated(45, -1, 0, 0);        
			drawCube(0.3f, r_sec,g_sec,b_sec); // Bra?o esquerdo amarelo
			glPopMatrix();
	
			// antebra?o direito
			glPushMatrix();
			glTranslatef(0.5f, 0.8f, 0.0f);
			e = 0.9;
			glRotated(45, -1, 0, 0);  
			glScaled(e, e, e);
			drawCube(0.3f, r_capuz, g_capuz, b_capuz); // Bra?o esquerdo amarelo
			glPopMatrix();
		
			// Bra?o direito
			glPushMatrix();
			glTranslatef(0.5, 0.63, -t);
			glRotated(45, -1, 0, 0);  
			drawCube(0.3f, r_sec,g_sec,b_sec); // Bra?o esquerdo amarelo
			glPopMatrix();
	glPopMatrix();
			// Antena
			glPushMatrix();
			glTranslatef(0.0f, 1.8f, 0.0f);
			drawSphere(0.1f, r_sec,g_sec,b_sec); // Bolinha vermelha na antena
			glPopMatrix();
		
			glPushMatrix();
			glTranslatef(0.0f, 1.5f, 0.0f);
			glScalef(0.1f, 0.5f, 0.1f);
			drawCube(1.0f, 0.5f, 0.5f, 0.5f); // Haste da antena cinza
			glPopMatrix();
	glPopMatrix();
	}


	
	// Fun??o para desenhar o personagem Bomberman
void drawBomberman(float anguloRotacao, float x_inicial, float z_inicial,float x, float z, float r_capuz, float g_capuz, float b_capuz, float r_sec, float g_sec, float b_sec, float r_corpo, float g_corpo, float b_corpo ){
	
glPushMatrix();
		// mexer na posi??o do player
		glTranslatef(x_inicial+x, 3.7, z_inicial+z);
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
		drawSphere(0.5f, r_capuz, g_capuz, b_capuz); // Cabe?a branca
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
		drawCube(0.7f, r_corpo, g_corpo, b_corpo); // Corpo azul
		glPopMatrix();
	
		// fundo do cinto
		glPushMatrix();
		glTranslatef(0.0f, 0.1, 0.35f);
		e = 0.6;
		glScaled(1.8, 0.3, 0.1);
		drawCube(0.4f, r_sec, g_sec, b_sec); // Perna esquerda vermelha
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
		drawCube(0.3f, r_capuz, g_capuz, b_capuz); // Bra?o esquerdo amarelo
		glPopMatrix();
	
		// Perna  esquerda
		glPushMatrix();
		glTranslatef(-0.2f, -0.5, 0.0f);
		e = 0.9;
		glScaled(e, e, e);
		drawCube(0.4f, r_sec, g_sec, b_sec); // Perna esquerda vermelha
		glPopMatrix();
	
		// anteperna direita
		glPushMatrix();
		glTranslatef(0.2, -0.2, 0.0f);
		e = 0.9;
		glScaled(e, 1.25, e);
		drawCube(0.3f, r_capuz, g_capuz, b_capuz); // Bra?o esquerdo amarelo
		glPopMatrix();
	
		// Perna  direita
		glPushMatrix();
		glTranslatef(0.2f, -0.5, 0.0f);
		glScaled(e, e, e);
		drawCube(0.4f, r_sec, g_sec, b_sec); // Perna esquerda vermelha
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
		drawCube(0.3f, r_capuz, g_capuz, b_capuz); // Bra?o esquerdo amarelo
		glPopMatrix();
	
	
		// Bra?o esquerdo
		glPushMatrix();
		glTranslatef(-0.5, 0.63, 0);
		//glRotated(45, -1, 0, 0);        
		drawCube(0.3f, r_sec, g_sec, b_sec); // Bra?o esquerdo amarelo
		glPopMatrix();
	
		// antebra?o direito
		glPushMatrix();
		glTranslatef(0.5f, 0.8f, 0.0f);
		e = 0.9;
		//glRotated(45, -1, 0, 0);  
		glScaled(e, e, e);
		drawCube(0.3f, r_capuz, g_capuz, b_capuz); // Bra?o esquerdo amarelo
		glPopMatrix();
	
		// Bra?o direito
		glPushMatrix();
		glTranslatef(0.5, 0.63, 0);
		//glRotated(45, -1, 0, 0);  
		drawCube(0.3f, r_sec, g_sec, b_sec); // Bra?o esquerdo amarelo
		glPopMatrix();
glPopMatrix();

		// Antena
		glPushMatrix();
		glTranslatef(0.0f, 1.8f, 0.0f);
		drawSphere(0.1f, r_sec, g_sec, b_sec); // Bolinha vermelha na antena
		glPopMatrix();
	
		glPushMatrix();
		glTranslatef(0.0f, 1.5f, 0.0f);
		glScalef(0.1f, 0.5f, 0.1f);
		drawCube(1.0f, 0.5f, 0.5f, 0.5f); // Haste da antena cinza
		glPopMatrix();
glPopMatrix();
}

	
};




Jogador player;

void VerificarItemAdquirido(){
	// verificando as bombas
	for (size_t i = 0; i < bombasItem.size(); ++i) {
	
		
	if (rangeColisaoitem(bombasItem[i].x , bombasItem[i].z ,  personagemX , personagemZ ) && !bombasItem[i].JaAdquirido){
		bombasItem[i].JaAdquirido = true;
 		std::pair<int, int> indices = obterIndiceCaixa(bombasItem[i].x , bombasItem[i].z);
        int yMatriz = indices.first;
        int xMatriz = indices.second;
        matrizMapa[xMatriz][yMatriz] = 0;
        maxBombas +=1;
        uparAudio("item.wav");
       
	}

}
	// verificando as botas
	for (size_t i = 0; i < botas.size(); ++i) {
		
		
	if (rangeColisaoitem(botas[i].x , botas[i].z ,  personagemX , personagemZ ) && !botas[i].JaAdquirido){
		botas[i].JaAdquirido = true;
 		std::pair<int, int> indices = obterIndiceCaixa(botas[i].x , botas[i].z);
        int yMatriz = indices.first;
        int xMatriz = indices.second;
        matrizMapa[xMatriz][yMatriz] = 0;
        movimento+=0.1;
        uparAudio("item.wav");
       
	}

}


		
}


enum direcoes{cima = 1, direita, baixo, esquerda};
float dir = 1;
Jogador bot1 = {0,0};
Jogador bot2 = {0,0};
Jogador bot3 = {0,0};

float movimentoBot = 0.08;
bool spawnBot = false;	

void spawnarBots(){
	// spawna bot s? no come?o
	if(!spawnBot){
		bot1.x = 22;
		bot1.z = 14;
		bot1.vivo = true;
		bot1.movimento = movimentoBot;
		
		bot2.x = 18;
	   	bot2.z = -10;
	   	bot2.vivo = true;
	   	bot2.movimento = movimentoBot;
	   	
		bot3.x = -20;
		bot3.z = -18;
		bot3.vivo = true;
	   	bot3.movimento = movimentoBot;
	   	
	   	spawnBot = true;
	}
	
}

bool rangeColisaoBots(int x1, int z1, int x2, int z2) {
    return (std::abs(x1 - x2) <= 2) && (std::abs(z1 - z2) <= 2);
}

bool explosaoBots(int x1, int z1, int x2, int z2) {
    return (std::abs(x1 - x2) <= 6) && (std::abs(z1 - z2) <= 6);
}

void danoExplosao() {
	time_t tempoAtual = time(NULL); 
    for (std::vector<Bomba>::iterator it = bombas.begin(); it != bombas.end(); ++it) {
        Bomba& bomba = *it;
        
        if (explosaoBots((int)bot1.x + bot1.addX, (int)bot1.z + bot1.addZ, bomba.x, bomba.z)) {
        	if(bot1.vivo) quantBotsMortos++;
            bot1.vivo = false;
        } if (explosaoBots((int)bot2.x + bot2.addX, (int)bot2.z + bot2.addZ, bomba.x, bomba.z)) {
            if(bot2.vivo) quantBotsMortos++;
			bot2.vivo = false;
        } if (explosaoBots((int)bot3.x + bot3.addX, (int)bot3.z + bot3.addZ, bomba.x, bomba.z)) {
            if(bot3.vivo) quantBotsMortos++;
			bot3.vivo = false;
        } if (explosaoBots(player.x+personagemX, player.z+personagemZ, bomba.x, bomba.z) && !playerPerdeuVida) {
            
            
    			if (difftime(tempoAtual, ultimoTempoColisao) >= 3){
					dano = false;
					quantVidas -= 1;
					ultimoTempoColisao = tempoAtual;
				}else{
					dano = true;
					
				}
				
            
			if (quantVidas == 0){
				uparAudio("game_over.wav");
				perdeuTudo = true;
				mciSendString("close audio1", NULL, 0, NULL);
			}
            playerPerdeuVida = true;
        }
    }
}

int vitoria = 0;
void resultado(){
	totalScore = calcularScore();
	
	if(!bot1.vivo && !bot2.vivo && !bot3.vivo){ // tela/evento de vitoria
		if (ultima_cam == 1){
			desenhaIcone(centerX, centerY + 16, centerZ + 12, 21, texID[7]);
		}else if (ultima_cam == 2){
			glPushMatrix();
			glTranslated(centerX-10,centerY+20,centerZ-11);
			glRotated(-90,1,0,0);
			desenhaIcone(17, -12, 0, 21, texID[7]);
			glPopMatrix();	
		}else if (ultima_cam == 3){
			glPushMatrix();
			glTranslated(centerX-10,centerY+12,centerZ+10);
			//	glRotated(-90,1,0,0);
			desenhaIcone(10, 0, 10, 7, texID[7]);
			glPopMatrix();
		}
		vitoria += 1;
		if (vitoria == 1){
			mciSendString("close audio999", NULL, 0, NULL); 
			mciSendString("close audio1", NULL, 0, NULL);
				mciSendString("close audio914", NULL, 0, NULL); 
			mciSendString("open \"audios/Vitoria.wav\" type waveaudio alias audio999", NULL, 0, NULL);  // Abre o ?udio com o alias
			mciSendString("play audio999", NULL, 0, NULL);
		}
	
		
	}
	if(perdeuTudo){
		if (ultima_cam == 1){
			desenhaIcone(centerX, centerY + 16, centerZ + 12, 21, texID[8]);
		}else if (ultima_cam == 2){
			glPushMatrix();
			glTranslated(centerX-10,centerY+20,centerZ-11);
			glRotated(-90,1,0,0);
			desenhaIcone(17, -12, 0, 21, texID[8]);
			glPopMatrix();	
		}else if (ultima_cam == 3){
			glPushMatrix();
			glTranslated(centerX-10,centerY+12,centerZ+10);
			//	glRotated(-90,1,0,0);
			desenhaIcone(10, 0, 10, 7, texID[8]);
			glPopMatrix();
		}
	}
}

void moverBot(Jogador &bot, int pais, float &r1, float &g1, float &b1, float &r2, float &g2, float &b2, float &r3, float &g3, float &b3) {
    mudarPais(pais, &r1, &g1, &b1, &r2, &g2, &b2, &r3, &g3, &b3);

    bool mudouDirecao = false;

    // Tenta continuar na ?ltima dire??o
    if (bot.ultimaDirecao == cima && !temColisao(bot.x + 28, bot.z - 1, muro) && !temColisao(bot.x + 28, bot.z - 1, caixa) && !verificarColisaoComBombas((int)bot.x + 28, (int)bot.z - 3)) {
        bot.z -= bot.movimento;
		if (bot.k >= bot.v.size()) bot.flag = false;
		if (bot.k <= 0) bot.flag = true;
		bot.t = bot.v[bot.k];
		if (bot.flag) bot.k++;	
        else bot.k--;
        bot.anguloRotacao = 180; // Cima
    } else if (bot.ultimaDirecao == direita && !temColisao(bot.x + 32, bot.z, muro) && !temColisao(bot.x + 32, bot.z, caixa) && !verificarColisaoComBombas((int)bot.x + 31, (int)bot.z)) {
        bot.x += bot.movimento;
		if (bot.k >= bot.v.size()) bot.flag = false;
		if (bot.k <= 0) bot.flag = true;
		bot.t = bot.v[bot.k];
		if (bot.flag) bot.k++;	
        else bot.k--;
        bot.anguloRotacao = 90; // Direita
    } else if (bot.ultimaDirecao == esquerda && !temColisao(bot.x + 27, bot.z, muro) && !temColisao(bot.x + 27, bot.z, caixa) && !verificarColisaoComBombas((int)bot.x + 25, (int)bot.z)) {
        bot.x -= bot.movimento;
		if (bot.k >= bot.v.size()) bot.flag = false;
		if (bot.k <= 0) bot.flag = true;
		bot.t = bot.v[bot.k];
		if (bot.flag) bot.k++;	
        else bot.k--;
        bot.anguloRotacao = -90; // Esquerda
    } else if (bot.ultimaDirecao == baixo && !temColisao(bot.x + 29, bot.z + 4, muro) && !temColisao(bot.x + 29, bot.z + 4, caixa) && !verificarColisaoComBombas((int)bot.x + 28, (int)bot.z + 3)) {
        bot.z += bot.movimento;
		if (bot.k >= bot.v.size()) bot.flag = false;
		if (bot.k <= 0) bot.flag = true;
		bot.t = bot.v[bot.k];
		if (bot.flag) bot.k++;	
        else bot.k--;
        bot.anguloRotacao = 0; // Baixo
    } else {
        // Sorteia uma nova dire??o que seja diferente da ?ltima
        mudouDirecao = true;
        int novaDirecao;
       // do {
		    novaDirecao = numeroAleatorio(1, 4); // 1 = cima, 2 = direita, 3 = esquerda, 4 = baixo
		//} while (novaDirecao == bot.ultimaDirecao || !direcaoEhPerpendicular(bot.ultimaDirecao, novaDirecao));

        bot.ultimaDirecao = novaDirecao;
    }

    // Renderiza o bot
    if (!mudouDirecao) {
        bot.andarBomberman(bot.addX, bot.addZ, r1, g1, b1, r2, g2, b2, r3, g3, b3);
    } else {
        drawBomberman(bot.anguloRotacao, bot.x, bot.z, bot.addX, bot.addZ, r1, g1, b1, r2, g2, b2, r3, g3, b3);
    }
}

void atualizarBots() {
	// cores: capuz, cor secundaria e corpo
    float r1, g1, b1, r2, g2, b2, r3, g3, b3;

    if(bot1.vivo){
    	moverBot(bot1, argentina, r1, g1, b1, r2, g2, b2, r3, g3, b3);
	} 
    if(bot2.vivo){
		moverBot(bot2, portugal, r1, g1, b1, r2, g2, b2, r3, g3, b3);
	}
    if(bot3.vivo){
		moverBot(bot3, japao, r1, g1, b1, r2, g2, b2, r3, g3, b3);
	} 
}






// ------------------- Configuracoes do GLUT -------------------
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

const char* ultima_tocada = "bomb";


void tocarmusica(const char* caminho) {
    char audio1[256]; // Vari?vel para armazenar o caminho completo
    snprintf(audio1, sizeof(audio1), "audios/%s.wav", caminho); // Constr?i o caminho completo do arquivo

    // Abre o ?udio usando o caminho
    char comandoOpen[512];
    snprintf(comandoOpen, sizeof(comandoOpen), "open \"%s\" type waveaudio alias audio1", audio1);
    mciSendString(comandoOpen, NULL, 0, NULL);

    // Toca o ?udio uma vez
    mciSendString("play audio1", NULL, 0, NULL);
}







void desenhaIcone(float x, float y, float z, float tamanho, GLuint texID) {
    // Dimens?es da face
    float d = tamanho / 2;

    // Definindo os v?rtices da face
    float v1[3] = {x - d, y + d, z};
    float v2[3] = {x - d, y - d, z};
    float v3[3] = {x + d, y - d, z};
    float v4[3] = {x + d, y + d, z};

    // Desenhando a face com a textura
    glBindTexture(GL_TEXTURE_2D, texID);

    glBegin(GL_QUADS);
	glTexCoord2f(0, 1); glVertex3fv(v1);  // Coordenada de textura (1, 0) para o v?rtice superior esquerdo
	glTexCoord2f(0, 0); glVertex3fv(v2);  // Coordenada de textura (1, 1) para o v?rtice inferior esquerdo
	glTexCoord2f(1, 0); glVertex3fv(v3);  // Coordenada de textura (0, 1) para o v?rtice inferior direito
	glTexCoord2f(1, 1); glVertex3fv(v4);  // Coordenada de textura (0, 0) para o v?rtice superior direito
    glEnd();
}


void verificarColisaoBots() {


    // Obt?m o tempo atual
    time_t tempoAtual = time(NULL); 
    
	if (difftime(tempoAtual, ultimoTempoColisao) >= 3)
		dano = false;
	else
		dano = true;

	

    // Verifica colis?o com bot3
    if (bot3.vivo && rangeColisaoBots((bot3.x+bot3.addX),(bot3.z+bot3.addZ), (-28 + personagemX), personagemZ )) {
    	
        // Verifica se j? passou 3 segundos desde a ?ltima colis?o
        if (difftime(tempoAtual, ultimoTempoColisao) >= 3) {
            //printf("PERDEU UMA VIDA\n");
           
            quantVidas--;
            totalScore = calcularScore();
            uparAudio("levar_dano.wav");
            
            if (quantVidas == 0){
				uparAudio("game_over.wav");
				perdeuTudo = true;
		
      
    			mciSendString("close audio1", NULL, 0, NULL);
			}
            	
            
            // Atualiza o tempo da ?ltima colis?o
            ultimoTempoColisao = tempoAtual;
		}
    }  else if   (bot2.vivo && rangeColisaoBots((bot2.x+bot2.addX) ,  (bot2.z+bot2.addZ) , (-28 + personagemX), personagemZ)){
		        // Verifica se j? passou 3 segundos desde a ?ltima colis?o
        if (difftime(tempoAtual, ultimoTempoColisao) >= 3) {
            //printf("PERDEU UMA VIDA\n");
            quantVidas--;
            totalScore = calcularScore();
            uparAudio("levar_dano.wav");
            
        
            if (quantVidas == 0){
				uparAudio("game_over.wav");
				perdeuTudo = true;
			
				mciSendString("close audio1", NULL, 0, NULL);
			}
            
            
            // Atualiza o tempo da ?ltima colis?o
            ultimoTempoColisao = tempoAtual;
        }
	} else  if (bot1.vivo && rangeColisaoBots((bot1.x+bot1.addX) , (bot1.z+bot1.addZ) ,(-28 + personagemX),personagemZ)){
     	 if (difftime(tempoAtual, ultimoTempoColisao) >= 3) {
            //printf("PERDEU UMA VIDA\n");
            quantVidas--;
            totalScore = calcularScore();
            uparAudio("levar_dano.wav");
     	   
            if (quantVidas == 0){
				uparAudio("game_over.wav");
				perdeuTudo = true;
			
				mciSendString("close audio1", NULL, 0, NULL);
			}
            // Atualiza o tempo da ?ltima colis?o
            ultimoTempoColisao = tempoAtual;
        }
	}
}

template <typename T>
void desenharTexto(float x, float y, const T& value, float scale, float r, float g, float b) {
    // Converter o valor para string
    std::ostringstream oss;
    oss << value;
    std::string text = oss.str();

    // Texto principal
    glColor3f(r, g, b); // Cor do texto principal
    draw_text_stroke(x, y, text, scale, 0.5);
}


float r1 = 0,g1 = 0,b1 = 0 ,r2 = 0 ,g2 = 0 ,b2 = 0 ,r3 = 0 ,g3 = 0 ,b3 = 0;
void display()
{
	
	// Limpar os buffers de cor e profundidade
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Configurar a c?mera
	glLoadIdentity();
	gluLookAt(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, 0, 1, 0);

	// Desenhar o terreno
	glPushMatrix(); // Salvar o estado da matriz atual
	desenhaTerreno(LINHAS_MAPA, COLUNAS_MAPA, 4, texID, matrizMapa);
	glPopMatrix(); // Restaurar o estado da matriz
	

	
	// Desenhar o texto das coordenadas do mouse
	glPushMatrix();
	

	glDisable(GL_TEXTURE_2D);   // Desativar texturas para o texto, nao colocar nada que envolva textura depois daqui
	

	
	 if (mostrarTexto) {
	 	if (ultima_cam == 1){
		glPushMatrix();
				glTranslated(centerX-5,centerY-2,centerZ);
        		desenharTexto(-20, 20, ultima_tocada, 0.01, 0,0,0.545);
  		glPopMatrix();
		 }else if (ultima_cam == 2){
		glPushMatrix();
				glTranslated(centerX,centerY+20,centerZ+13);
				glRotated(-90,1,0,0);
				
        		desenharTexto(-20, 20, ultima_tocada, 0.01, 0,0,0.545);
  		glPopMatrix();
		 }
	
  	
    }
	tempoFinal=glutGet(GLUT_ELAPSED_TIME);
	if (tempoFinal - tempoInicial >=5000)// 5 segundos para exibir o nome da musica na tela
		mostrarTexto = false;
	
	if (ultima_cam == 1){
		glPushMatrix();	
			glTranslated(centerX,centerY,centerZ);
			desenharTexto(-20,20,to_string(quantVidas),0.02, 1,0,0 );
			desenharTexto(-13,20,to_string(maxBombas),0.02, 1,0,0 );
			desenharTexto(2,20,to_string(totalScore),0.02, 1,0,0 );
		glPopMatrix();
	}else if (ultima_cam == 3){
		glPushMatrix();	
			glTranslated(centerX+12,centerY-9,centerZ+10);
			//glRotated(-90,1,0,0);
			desenharTexto(-20,20,to_string(quantVidas),0.02, 1,0,0 );	
			desenharTexto(-15,20,to_string(maxBombas),0.02, 1,0,0 ); // zoom
			desenharTexto(-5,20,to_string(totalScore),0.02, 1,0,0 );
		glPopMatrix();
		
	} else if (ultima_cam == 2){
	glPushMatrix();	
			glTranslated(centerX+12,centerY+20,centerZ+10);
			glRotated(-90,1,0,0);
			desenharTexto(-20,20,to_string(quantVidas),0.02, 1,0,0 );	
			desenharTexto(-20,13,to_string(maxBombas),0.02, 1,0,0 );
			desenharTexto(-19,7.3,to_string(totalScore),0.02, 1,0,0 );
		glPopMatrix();
	}

	verificarColisaoBots();


	desenhaBombas(); 
	if (!dano){
		
	
	mudarPais(brasil, &r1,&g1,&b1,&r2,&g2,&b2,&r3,&g3,&b3); // essa funcao fora do condicional pq vai executar do msm jeito
	if (andando){
		
		
		player.andarBomberman(personagemX, personagemZ,  r1,g1,b1,    r2,g2,b2,    r3,g3,b3);	
	}		
	else{
		
		drawBomberman(player.anguloRotacao, player.x, player.z, personagemX, personagemZ, r1,g1,b1,    r2,g2,b2,    r3,g3,b3 );// cores: capuz, cor secundaria e corpo
	}

		
		
	}else{
		//usa o proprio r1 de flag
        if (r1 == 0) {
        
        	if (andando)
        		player.andarBomberman(personagemX, personagemZ,  r1,g1,b1,    r2,g2,b2,    r3,g3,b3);
        	else
				drawBomberman(player.anguloRotacao, player.x, player.z, personagemX, personagemZ, r1,g1,b1,    r2,g2,b2,    r3,g3,b3 );
			 r1 = 1;
		} else{
			
       	  //drawBomberman(player.anguloRotacao, player.x, player.z, personagemX, personagemZ, r1,g1,b1,    r2,g2,b2,    r3,g3,b3 );
       	  r1 = 0;
		}
    	

       
	}
	spawnarBots();
	atualizarBots();
	
	
	glEnable(GL_TEXTURE_2D);    // Reativar texturas caso necess?rio
	glPopMatrix();
	
	
	
	if (ultima_cam == 1){
		desenhaIcone(centerX-23, centerY+21, centerZ, 4, texID[5]);
		desenhaIcone(centerX-15, centerY+21, centerZ, 4, texID[6]);
		desenhaIcone(centerX-4, centerY+20.8, centerZ, 11, texID[11]); // exibe score
	}
	else if (ultima_cam == 3){
		glPushMatrix();
			glTranslated(centerX-10,centerY+12,centerZ+10);
			//	glRotated(-90,1,0,0);
			desenhaIcone(0, 0, 0, 4, texID[5]);
			desenhaIcone(5, 0, 0, 4, texID[6]);
			desenhaIcone(13, 0, 0, 8, texID[11]);
		glPopMatrix();
	}else if (ultima_cam == 2){
		glPushMatrix();
			glTranslated(centerX-10,centerY+20,centerZ-11);
			glRotated(-90,1,0,0);
			desenhaIcone(0, 0, 0, 4, texID[5]);
			desenhaIcone(0, -7, 0, 4, texID[6]);
			desenhaIcone(4, -13, 26, 4, texID[11]);
		glPopMatrix();	
	}
	
	VerificarItemAdquirido();
	resultado();
	
	// Finalizar a renderiza??o
 	glFlush();
	glutSwapBuffers();
}

bool somTocando = false; // Vari?vel para rastrear o estado do som

const char* audio2;

int audios = 2;

void uparAudio(const char* caminho) {
    // Cria um alias ?nico para cada nova reprodu??o do som
    char audio[20];  // Array para armazenar o nome do alias

    // Cria o alias ?nico usando sprintf
    sprintf(audio, "audio%d", audios);  // Gera um nome como "audio1", "audio2", etc.

    // Fecha o alias se ele j? estiver em uso (isso garante que n?o haver? conflitos)
    char closeCommand[50];
    sprintf(closeCommand, "close %s", audio);  // Prepara o comando de fechamento com o alias ?nico
    mciSendString(closeCommand, NULL, 0, NULL);  // Fecha o alias anterior

    // Abre o ?udio "bomba.wav" com o alias ?nico
    char openCommand[100];
    sprintf(openCommand, "open \"audios/%s\" type waveaudio alias %s", caminho,audio);  // Prepara o comando de abertura com o alias ?nico
    mciSendString(openCommand, NULL, 0, NULL);  // Abre o ?udio com o alias

    // Reproduz o ?udio "bomba.wav"
    char playCommand[50];
    sprintf(playCommand, "play %s", audio);  // Prepara o comando de reprodu??o com o alias
    mciSendString(playCommand, NULL, 0, NULL);  // Reproduz o ?udio

    // Incrementa a vari?vel para o pr?ximo alias
    audios++;
}



// Controle do teclado
// Funcao para capturar entradas do teclado
void teclado(unsigned char key, int x, int y)
{	

	switch (key)
	{
	case '1':
		eyeX = personagemX-2-28;
		eyeY = 41;
		eyeZ = personagemZ+47;
		centerX = personagemX-2-28;
		centerY = 0;
		centerZ = personagemZ-2;
		ultima_cam = 1;
		break ;
	case '2':
		eyeZ = personagemZ+0;
		eyeY = 64;
		eyeX = personagemX-2-28;
		centerX = personagemX-2-28;
		centerY = 0;
		centerZ = personagemZ-2;
		ultima_cam = 2;
		break;
	case '3':
		eyeX = personagemX-21;
		eyeY = 16; // 14
		eyeZ = personagemZ+24;
		centerX = personagemX-21;
		centerY = 0;
		centerZ = personagemZ-3;
		ultima_cam = 3;
		break;

	case '4':
		eyeX = personagemX-28-1;
		eyeY = 73;
		eyeZ = personagemZ-64;
		centerX = personagemX-28-2,
		centerY = 0,
		centerZ = personagemZ-2;
		ultima_cam = 4;
		break;
	case 32:
		if(bombasAtuais < maxBombas){
			uparAudio("bomba.wav");
		    spawnBomba();
		    bombasAtuais += 1;
			break;
		}
	case 'w':
	case 'W': // Mover o personagem pra frente
		if ( !perdeuTudo &&!temColisao(personagemX,personagemZ - 1, muro) && !temColisao(personagemX, player.z +  personagemZ - 1, caixa) && !verificarColisaoComBombas(personagemX, personagemZ - 3 )){
			    // Defina os arquivos de ?udio

		if (!somTocando) {
            PlaySound(TEXT("audios/passos.WAV"), NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
            somTocando = true;
        	}

			centerZ -= movimento;
			eyeZ-=movimento;
			personagemZ -= movimento;
		
			player.anguloRotacao= 180;
			
			andando = true;
			if (k >= v.size())
				flag = false;
			if (k <= 0)
				flag = true;
			player.t = v[k];
			//printf("t: %f ",t);
			if (flag)
				k++;	
            else
				k--;
			
			//printf("(%.0f, %.0f)\n", personagemX, personagemZ);	
		}
		break;
	case 's':
	case 'S': // Mover o personagem pra tras	anguloRotacao = 0;
		if ( !perdeuTudo && !temColisao(personagemX, personagemZ + 4, muro) && !temColisao(personagemX, personagemZ + 4, caixa) && !verificarColisaoComBombas(personagemX, personagemZ + 3)){
			if (!somTocando) {
            PlaySound(TEXT("audios/passos.WAV"), NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
            somTocando = true;
        	}

			
			centerZ += movimento;
			eyeZ+=movimento;
			personagemZ += movimento;
			player.anguloRotacao = 0.0f;
			
			andando = true;
			if (k >= v.size())
				flag = false;
			if (k <= 0)
				flag = true;
			player.t = v[k];
			//printf("t: %f ",t);
			if (flag)
				k++;	
            else
				k--;
			
			//printf("(%.0f, %.0f)\n", personagemX, personagemZ);

						
		}
		break;
	case 'a':
	case 'A': // Mover o personagem pra esquerda
		if (!perdeuTudo && !temColisao(personagemX - 1, personagemZ, muro) && !temColisao(personagemX - 3, personagemZ, caixa) && !verificarColisaoComBombas(personagemX - 3, personagemZ)){
					if (!somTocando) {
            PlaySound(TEXT("audios/passos.WAV"), NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
            somTocando = true;
        	}
			eyeX -= movimento;
			centerX -= movimento;
			personagemX -= movimento;
			player.anguloRotacao = 270.0f;
			andando = true;
			if (k >= v.size())
				flag = false;
			if (k <= 0)
				flag = true;
			
			player.t = v[k];
			
			//printf("t: %f ",t);
			if (flag)
				k++;	
            else
				k--;
			//printf("(%.0f, %.0f)\n", personagemX, personagemZ);
			
			
		}
		break;
	case 'd':
	case 'D': // Mover o personagem pra direita
		if (!perdeuTudo && !temColisao(personagemX + 4, personagemZ, muro) && !temColisao(personagemX + 4, personagemZ, caixa) && !verificarColisaoComBombas(personagemX + 3, personagemZ)){
			if (!somTocando) {
	            PlaySound(TEXT("audios/passos.WAV"), NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
	            somTocando = true;
	  		}
			eyeX += movimento;
			centerX += movimento;
			personagemX += movimento;
			player.anguloRotacao  = 90.0f;
			
			andando = true;
			if (k >= v.size())
				flag = false;
			if (k <= 0)
				flag = true;
			player.t = v[k];
			//printf("t: %f ",t);
			if (flag)
				k++;	
	        else
				k--;
			
			//printf("(%.0f, %.0f)\n", personagemX, personagemZ);
		}
			break;
    /*
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
	*/
	case 'r':
    case 'R':	
		mciSendString("close audio999", NULL, 0, NULL); 
		
		totalScore = 0;
		vitoria = 0;
		
    	perdeuTudo = false;
		quantVidas = 3;
		totalScore = 0;
		quantBotsMortos = 0;
		
		bot1.vivo = true;
	 	bot2.vivo = true;
	 	bot3.vivo = true;
	 	
		mciSendString("close audio914", NULL, 0, NULL); 
		mciSendString("close audio1", NULL, 0, NULL);
		mciSendString("open \"audios/bomb.wav\" type waveaudio alias audio914", NULL, 0, NULL);  // Abre o ?udio com o alias
		mciSendString("play audio914", NULL, 0, NULL);
    	
    	break;
	case 'm':
	case 'M':
		mciSendString("close audio914", NULL, 0, NULL); 
	    // Alterna para a pr?xima m?sica
	    mudar_musica += 1;
	    if (mudar_musica == 9)  // Se ultrapassar o n?mero de m?sicas, volta para a primeira
	        mudar_musica = 1;
	
	    // Para a m?sica atual e fecha, mas sem interromper o fluxo
	    mciSendString("close audio1", NULL, 0, NULL);
	
	    // Abrir e tocar a pr?xima m?sica
	    if (mudar_musica == 1) {
	        tocarmusica("bomb");
	        ultima_tocada = "Super Bomberman - Level 1 (ost snes)";
	    } else if (mudar_musica == 2) {
	        tocarmusica("bomb2");
	        ultima_tocada = "Super Bomberman - Level 2 (ost snes)";
	    } else if (mudar_musica == 3) {
	        tocarmusica("metatron_OST");
	        ultima_tocada = "Undertale OST: 068 - Death by Glamour";
	    }else if (mudar_musica == 4){
	    	tocarmusica("HK_mantis_lord");
	        ultima_tocada = "Hollow Knight OST - Mantis Lords";
		} else if (mudar_musica == 5){
	    	tocarmusica("hxh");
	        ultima_tocada = "Hunting for your Dream [8 bit Cover] - Hunter X Hunter";
		}else if (mudar_musica == 6){
	    	tocarmusica("TopGear");
	        ultima_tocada = "Top Gear Sound Theme - Tema de TopGear";
		}else if (mudar_musica == 7){
	    	tocarmusica("grimn");
	        ultima_tocada = "Nightmare King Grimn theme - Hollow Knight";
		}else if (mudar_musica == 8) {
	        // N?o h? m?sica, mas o comando de stop n?o ? necess?rio
	        ultima_tocada = "Nenhuma musica selecionada";
	    }
	    
	
	    tempoInicial = glutGet(GLUT_ELAPSED_TIME);
	    mostrarTexto = true; // Garante que o texto ser? exibido
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
		if (somTocando) {
        	PlaySound(NULL, NULL, 0); // Para o som
        	somTocando = false; 
    	}
		
		break;
	case 's':
   
	 	

	
	case 'S': // Mover o personagem pra tras
		andando = false;
		if (somTocando) {
        	PlaySound(NULL, NULL, 0); // Para o som
        	somTocando = false; 
    	}
		break;
	case 'a':
	case 'A': // Mover o personagem pra esquerda
		andando = false;
				if (somTocando) {
        	PlaySound(NULL, NULL, 0); // Para o som
        	somTocando = false; 
    	}
	case 'd':
			

	case 'D': // Mover o personagem pra direita
		andando = false;
				if (somTocando) {
        	PlaySound(NULL, NULL, 0); // Para o som
        	somTocando = false; 
    	} 
	default:
		break;
	}
	glutPostRedisplay();
}

void init()
{ 
	glClearColor(0.4, 0.7, 1, 1); // Cor de fundo azul claro
	
	// Configurar as texturas
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glGenTextures(QTD_TEXTURAS, texID);
	
	glEnable(GL_BLEND); // Ativa o blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

	//carregando vetores para animacao do player e dos bots	
     preencherVetor(v);
	bot1.preencherVetor();
	bot2.preencherVetor();
	bot3.preencherVetor();
	
    tempoInicial = glutGet(GLUT_ELAPSED_TIME);
	tocarmusica("bomb");
	ultima_tocada = "Super Bomberman - Level 1 (ost snes)";
	
	
	// Carrega as texturas no vetor
	carregaTextura(texID[0], "grama_cima.jpg");     // Textura 0 = parte de cima da grama
	carregaTextura(texID[1], "grama_lateral.jpg");  // Textura 1 = lateral da grama
	carregaTextura(texID[2], "pedra.jpg");  		// Textura 2 = muro normal
	carregaTextura(texID[3], "pedra_musgo.png");  	// Textura 3 = muro com musgo
	carregaTextura(texID[4], "caixa.jpg");  		// Textura 4 = caixa

	carregaTextura(texID[5], "vidas.png");  		// Textura 5 = coracao que indica as quantVidas
	carregaTextura(texID[6], "bomba.png");  		// Textura 6 = maximo de bombas
	carregaTextura(texID[7], "vitoria.png");  		// Textura 7 = player ganhou
	carregaTextura(texID[8], "derrota.png");  		// Textura 8 = player perdeu
	
	carregaTextura(texID[9], "powerUpBomba.png");
	carregaTextura(texID[10], "powerUpVelocidade.png");
	carregaTextura(texID[11], "score.png");

	glEnable(GL_DEPTH_TEST);  // Ativa o teste de profundidade
	
	colisoesMatriz();
}

void mousePassiveMotion(int x, int y)
{
	m_x = x;
	m_y = height - y - 1;
	glutPostRedisplay();
}

void exibirMenu(){
    printf("  ____                  _                                       ____      _ \n");
    printf(" |  _ \\                | |                                     |___ \\    | |\n");
    printf(" | |_) | ___  _ __ ___ | |__   ___ _ __ _ __ ___   __ _ _ __     __) | __| |\n");
    printf(" |  _ < / _ \\| '_ ` _ \\| '_ \\ / _ \\ '__| '_ ` _ \\ / _` | '_ \\   |__ < / _` |\n");
    printf(" | |_) | (_) | | | | | | |_) |  __/ |  | | | | | | (_| | | | |  ___) | (_| |\n");
    printf(" |____/ \\___/|_| |_| |_|_.__/ \\___|_|  |_| |_| |_|\\__,_|_| |_| |____/ \\__,_|\n");
    printf("                                                                            \n");
    printf("                                                                            \n\n");

    printf("+----------------------+-----------------------------+\n");
    printf("|       Comando        |            Acao             |\n");
    printf("+----------------------+-----------------------------+\n");
    printf("|         WASD         | Mover o player              |\n");
    printf("|    BARRA DE ESPACO   | Dropa uma bomba             |\n");
    printf("|          R           | Reiniciar                   |\n");
    printf("|          M           | Mudar/desativar musica      |\n");
    printf("|        1/2/3         | Mudar a camera              |\n");
    printf("+----------------------+-----------------------------+\n");
    printf("|         Acao         |            Score            |\n");
    printf("+----------------------+-----------------------------+\n");
	printf("|      Matar bot       |          + 30 pts           |\n");
    printf("|    Destruir caixa    |          + 10 pts           |\n");
    printf("|     Perder vida      |          - 20 pts           |\n");
    printf("+----------------------+-----------------------------+\n");
}


int main(int argc, char** argv)
{
	player.x = -28;
	player.z = 0;
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
	
	exibirMenu();

	// Funcoes de animacao
	glutTimerFunc(500, atualizarCor, 0);
	glutTimerFunc(50, atualizarEscala, 0);  
	glutTimerFunc(50, atualizarBombas, 0); // Inicia a anima??o das bombas
	
	init();
	glutMainLoop();
}
