#include <stdio.h>
#include"funcoes.h"

Button *criar_botao(BITMAP *imagem, int x, int y){
	
	Button * b = (Button*)malloc(sizeof(Button));
	b->imagem = imagem;
	b->pos_x = x;
	b->pos_y = y;
	return b;
}

void button_update(Button *b);
