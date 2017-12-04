#include <allegro5\allegro.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_ttf.h>
#include <allegro5\bitmap.h>
#include <allegro5\bitmap_draw.h>
#include <allegro5\allegro_image.h>
#include <stdlib.h>
#include <string.h>
#include <allegro5\display.h>
#include <allegro5\timer.h>
#include <allegro5\keyboard.h>
#include<allegro5\keycodes.h>
#include "allegro5/allegro_audio.h"
#include "allegro5/allegro_acodec.h"

#ifndef BUTTON	
#define BUTTON
typedef struct button{
	BITMAP *imagem;
	int pos_x, pos_y;
	int borda_x, borda_y;
	bool ativo;
	int flag;
}Button;

typedef struct buttonScore{
	ALLEGRO_BITMAP *vida0;
	ALLEGRO_BITMAP *vida1;
	ALLEGRO_BITMAP *vida2;
	ALLEGRO_BITMAP *vida3;
	ALLEGRO_BITMAP *vida4;
	int pos_x, pos_y;
}ButtonScore;

typedef struct usuario{
	char nome[6];
	int recorde;
}TUsuario;

typedef struct bprato{
	ALLEGRO_BITMAP *prato0;
	ALLEGRO_BITMAP *prato1;
	ALLEGRO_BITMAP *prato2;
	ALLEGRO_BITMAP *prato3;
	int pos_x, pos_y;
	int borda_x, borda_y;
	bool ativo;
	int flag;
}BPrato
;
Button *criar_botao(ALLEGRO_BITMAP *imagem, int x, int y, int borda_x, int borda_y, int flag);
ButtonScore *criar_barra_pontos(ALLEGRO_BITMAP *vida0, ALLEGRO_BITMAP *vida1, ALLEGRO_BITMAP *vida2, ALLEGRO_BITMAP *vida3, ALLEGRO_BITMAP *vida4, int x, int y);
Button comida_aleatoria(Button comidas[]);
BPrato *criar_prato(ALLEGRO_BITMAP *prato0, ALLEGRO_BITMAP *prato1, ALLEGRO_BITMAP *prato2, ALLEGRO_BITMAP *prato3, int x, int y, int borda_x, int borda_y, int flag);


#endif