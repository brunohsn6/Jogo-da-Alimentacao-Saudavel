#include <allegro5\allegro.h>
#include <allegro5\allegro_font.h>
#include <allegro5\allegro_ttf.h>
#include <allegro5\bitmap.h>
#include <allegro5\bitmap_draw.h>
#include <allegro5\allegro_image.h>
#include <stdlib.h>
#include <allegro5\display.h>
#include <allegro5\timer.h>
#include <allegro5\keyboard.h>
#include<allegro5\keycodes.h>

#ifndef BUTTON	
#define BUTTON
typedef struct button{
	BITMAP *imagem;
	int pos_x, pos_y;

}Button;

Button *criar_botao(BITMAP *imagem, int x, int y);
void button_update(Button *b);


#endif