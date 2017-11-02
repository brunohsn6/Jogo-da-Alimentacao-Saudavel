#include<stdio.h>
#include<allegro5\allegro.h>

int main(){
	ALLEGRO_DISPLAY *view = NULL;
	if (!al_init()){
		printf("Erro ao inicializar!\n");
		return -1;
	}
	view = al_create_display(640, 480);
	if (!view){
		printf("Erro ao criar display!\n");
		return -1;
	}
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_flip_display();
	al_rest(10);
	al_destroy_display(view);
	return 0;
}