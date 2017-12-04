#include <stdio.h>
#include"funcoes.h"

Button *criar_botao(ALLEGRO_BITMAP *imagem, int x, int y, int borda_x, int borda_y, int flag){
	
	Button * b = (Button*)malloc(sizeof(Button));
	b->imagem = imagem;
	b->pos_x = x;
	b->pos_y = y;
	b->borda_x = borda_x;
	b->borda_y = borda_y;
	b->flag = flag;
	b->ativo = false;
	return b;
}

BPrato *criar_prato(ALLEGRO_BITMAP *prato0, ALLEGRO_BITMAP *prato1, ALLEGRO_BITMAP *prato2, ALLEGRO_BITMAP *prato3, int x, int y, int borda_x, int borda_y, int flag){
	BPrato * b = (BPrato*)malloc(sizeof(BPrato));
	b->prato0 = prato0;
	
	b->prato1 = prato1;
	
	b->prato2 = prato2;
	
	b->prato3 = prato3;
	
	b->pos_x = x;
	b->pos_y = y;
	b->borda_x = borda_x;
	b->borda_y = borda_y;
	b->flag = flag;
	b->ativo = false;
	return b;
}
ButtonScore *criar_barra_pontos(ALLEGRO_BITMAP *vida0, ALLEGRO_BITMAP *vida1, ALLEGRO_BITMAP *vida2, ALLEGRO_BITMAP *vida3, ALLEGRO_BITMAP *vida4, int x, int y){
	ButtonScore *b = (ButtonScore*)malloc(sizeof(ButtonScore));
	b->pos_x = x;
	b->pos_y = y;
	b->vida0 = vida0;
	b->vida1 = vida1;
	b->vida2 = vida2;
	b->vida3 = vida3;
	b->vida4 = vida4;
	return b;
}

Button comida_aleatoria(Button comidas[]){
	int aleatorio;
	srand(time(NULL));
	//range de 0-5 comidas
	aleatorio = rand() % 4;
	return comidas[aleatorio];
}

