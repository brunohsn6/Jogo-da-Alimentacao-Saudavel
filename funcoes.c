#include <stdio.h>
#include"funcoes.h"

Button *criar_botao(BITMAP *imagem, int x, int y, int borda_x, int borda_y){
	
	Button * b = (Button*)malloc(sizeof(Button));
	b->imagem = imagem;
	b->pos_x = x;
	b->pos_y = y;
	b->borda_x = borda_x;
	b->borda_y = borda_y;
	b->ativo = false;
	return b;
}

Button comida_aleatoria(Button comidas[]){
	int aleatorio;
	srand(time(NULL));
	//range de 0-5 comidas
	aleatorio = rand() % 4;
	return comidas[aleatorio];
}

