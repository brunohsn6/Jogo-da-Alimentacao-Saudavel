#include <stdio.h>
#include "funcoes.h"
#include"score.h"


int WIDTH = 640;
int HEIGHT = 460;
ALLEGRO_DISPLAY *view = NULL;
ALLEGRO_FONT *font = NULL;
ALLEGRO_BITMAP *bgImagem = NULL;
ALLEGRO_EVENT_QUEUE *fila_eventos = NULL, *fila_contador = NULL, *fila_eventosT = NULL;

bool running = false;

void jogar();
int main(){
	int em_jogar = 0, em_score = 0, c_jogar = 0, c_score = 0, sair = 0;

	if (!al_init()){
		fprintf(stderr, "Erro ao inicializar!\n");
		return -1;
	}
	view = al_create_display(WIDTH, HEIGHT);
	al_set_window_title(view, "Comer Saudável");

	if (!al_install_mouse()){
		fprintf(stderr, "erro ao inicializar mouse!\n");
		al_destroy_display(view);
		return -1;
	}

	if (!al_set_system_mouse_cursor(view, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT)){
		fprintf(stderr, "Falha ao atribuir ponteiro do mouse.\n");
		al_destroy_display(view);
		return -1;
	}
	//addons do allegros!!
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_image_addon();
	font = al_load_font("Arcanum_Order.ttf", 50, 0);
	bgImagem = al_load_bitmap("backgroundHome.png");

	if (!view){
		printf("Erro ao criar display!\n");
		return -1;
	}

	fila_eventos = al_create_event_queue();
	al_register_event_source(fila_eventos, al_get_mouse_event_source());
	al_register_event_source(fila_eventos, al_get_display_event_source(view));
	
	while (!sair){
		al_clear_to_color(al_map_rgb(255, 255, 255));
		al_draw_bitmap(bgImagem, 0, 0, 0);
		//JOGAR E SCORE SERÃO BOTÕES QUE REDIRECIONARÃO PARA GAME E SCORE RESPECTIVAMENTE!!
		al_draw_text(font, al_map_rgb(0, 0, 0), (WIDTH / 2), (HEIGHT / 2) + 10, ALLEGRO_ALIGN_CENTER, "JOGAR");
		al_draw_text(font, al_map_rgb(0, 0, 0), (WIDTH / 2), (HEIGHT / 2) + 80, ALLEGRO_ALIGN_CENTER, "SCORE");
		al_draw_text(font, al_map_rgb(0, 0, 0), (WIDTH / 2), (HEIGHT / 2) + 150, ALLEGRO_ALIGN_CENTER, "SAIR");
		al_flip_display();
		
		while (!al_is_event_queue_empty(fila_eventos)){
			ALLEGRO_EVENT evento;
			ALLEGRO_TIMEOUT timeout;
			al_wait_for_event(fila_eventos, &evento);
			al_init_timeout(&timeout, 0.05);
			int tem_eventos = al_wait_for_event_until(fila_eventos, &evento, &timeout);

			if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
				printf("X = %d\n Y = %d\n", evento.mouse.x, evento.mouse.y);
				if (evento.mouse.x >= 260 - 10 &&
					evento.mouse.x <= 390 + 10 &&
					evento.mouse.y <= 280 - 10 &&
					evento.mouse.y >= 240 + 10){
					//printf("clique em jogar");
					al_draw_text(font, al_map_rgb(255, 255, 255), (WIDTH / 2), (HEIGHT / 2) + 10, ALLEGRO_ALIGN_CENTER, "JOGAR");
					al_flip_display();
					al_rest(0.3);
					jogar();
				}
				else if (evento.mouse.x >= 247 - 10 &&
					evento.mouse.x <= 372 + 10 &&
					evento.mouse.y <= 345 - 10 &&
					evento.mouse.y >= 300 + 10){
					//printf("clique em score");
					al_draw_text(font, al_map_rgb(255, 255, 255), (WIDTH / 2), (HEIGHT / 2) + 80, ALLEGRO_ALIGN_CENTER, "SCORE");
					al_flip_display();
					al_rest(0.3);
					score();

				}
				else if (evento.mouse.x >= 283 - 10 &&
					evento.mouse.x <= 363 + 10 &&
					evento.mouse.y <= 421 - 10 &&
					evento.mouse.y >= 378 + 10){
					printf("clique em sair");
					al_draw_text(font, al_map_rgb(255, 255, 255), (WIDTH / 2), (HEIGHT / 2) + 150, ALLEGRO_ALIGN_CENTER, "SAIR");
					al_flip_display();
					al_rest(0.3);
					sair = 1;

				}
				else if (tem_eventos && evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
					sair = 1;
					break;
					printf("clique no X");
				}

			}
		}	
		
	}

	al_destroy_display(view);
	al_destroy_font(font);
	al_destroy_bitmap(bgImagem);
	return 0;
}

void jogar(){
	ALLEGRO_BITMAP *bgImagem = NULL, *prato = NULL;
	ALLEGRO_TIMER *timer = NULL, *contador = 0;
	int tecla = 0;
	int min = 0, seg = 0;
	bool fim = false;

	//ADD-ONS
	al_init_image_addon();

	//INICIALIZAÇÕES
	timer = al_create_timer(0.1);
	if (!timer){
		fprintf(stderr, "Falha ao criar timer.\n");
		al_destroy_event_queue(fila_eventos);
		al_destroy_display(view);
		return;
	}
	contador = al_create_timer(1.0);
	if (!contador){
		fprintf(stderr, "Falha ao criar timer.\n");
		al_destroy_font(font);
		al_destroy_timer(timer);
		al_destroy_event_queue(fila_eventos);
		al_destroy_display(view);
		return;
	}

	fila_contador = al_create_event_queue();
	if (!fila_contador){
		fprintf(stderr, "Falha ao criar fila do contador.\n");
		al_destroy_timer(contador);
		al_destroy_font(font);
		al_destroy_timer(timer);
		al_destroy_event_queue(fila_eventos);
		al_destroy_display(view);
		return;
	}
	if (!al_install_keyboard()){
		fprintf(stderr, "Falha ao inicializar o teclado.\n");
		return;
	}
	//CARREGANDO OS BITMAPS

	bgImagem = al_load_bitmap("cozinha.jpg");
	prato = al_load_bitmap("prato.bmp");


	printf("*********\n**JOGAR**\n*********\n");
	printf("jogando....");

	al_clear_to_color(al_map_rgb(255, 255, 255));
	al_draw_bitmap(bgImagem, 0, 0, 0);
	al_draw_textf(font, al_map_rgb(100, 250, 0), 500, 20, 0, "%.2d:%.2d", min, seg);
	al_flip_display();

	fila_eventosT = al_create_event_queue();
	al_register_event_source(fila_eventos, al_get_timer_event_source(timer));
	al_register_event_source(fila_contador, al_get_timer_event_source(contador));
	al_register_event_source(fila_eventosT, al_get_keyboard_event_source());
	al_start_timer(timer);
	al_start_timer(contador);
	while (!fim){

		//CHECA SE HOUVE DIGITAÇÃO DE TECLAS!!!

		while (!al_is_event_queue_empty(fila_eventosT)){
			ALLEGRO_EVENT evento;
			al_wait_for_event(fila_eventosT, &evento);
			
			if (evento.type == ALLEGRO_EVENT_KEY_UP){
				switch (evento.keyboard.keycode){
				case ALLEGRO_KEY_LEFT:
					//mover boneco para a esquerda
					printf("esquerda\n");
					tecla = 1;
					break;
				case ALLEGRO_KEY_RIGHT:
					//mover boneco para a direita
					printf("direita\n");
					tecla = 2;
					break;
				default:
					//faz nada
					printf("tecla invalida\n");
					tecla = 0;
					break;
				}
			}
			else if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
				fim = true;
			}

		}
		//IMPLEMENTAÇÃO DO TIMER
		if (!al_is_event_queue_empty(fila_contador)){
			ALLEGRO_EVENT evento;
			al_wait_for_event(fila_contador, &evento);

			if (evento.type == ALLEGRO_EVENT_TIMER){
				seg++;
				if (seg == 60){
					min++;
					seg = 0;
				}
			}
			al_clear_to_color(al_map_rgb(255, 255, 255));
			al_draw_bitmap(bgImagem, 0, 0, 0);
			al_draw_bitmap_region(prato, 10, 10, 30, 30, 0, 0, 0);
			al_draw_textf(font, al_map_rgb(100, 250, 0), 500, 20, 0, "%.2d:%.2d", min, seg);
			al_flip_display();
		}
		//SE ALGUMA TECLA FOR BATIDA, ENTRA AQUI!!!!
		if (tecla != 0){
			if (tecla == 1){
				//executar código de movimentação à esquerda
			}
			else if (tecla == 2){
				//executar código de movimentação à direita
			}
			al_clear_to_color(al_map_rgb(255, 255, 255));
			al_draw_bitmap(bgImagem, 0, 0, 0);
			al_draw_textf(font, al_map_rgb(100, 250, 0), 500, 20, 0, "%.2d:%.2d", min, seg);
			al_flip_display();
		}

		//checar colisão dos objetos!!!!

	}
	al_destroy_event_queue(fila_eventos);
	al_destroy_event_queue(fila_eventosT);
	al_destroy_bitmap(bgImagem);
	al_destroy_timer(timer);
}
	
