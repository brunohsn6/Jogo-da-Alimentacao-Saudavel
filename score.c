#include "score.h"

void score(){
	al_clear_to_color(al_map_rgb(255, 255, 255));
	al_flip_display();
	printf("*********\n**SCORE**\n*********");
	al_rest(10);
}