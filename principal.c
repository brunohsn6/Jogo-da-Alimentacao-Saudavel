#include <stdio.h>
#include "funcoes.h"
#include"score.h"

//-----------------------------VARIÁVEIS DO MAIN---------------------------------------
int WIDTH = 640;
int HEIGHT = 460;
ALLEGRO_DISPLAY *view = NULL;
ALLEGRO_FONT *font = NULL;
ALLEGRO_BITMAP *bgImagem = NULL;
ALLEGRO_EVENT_QUEUE *fila_eventos = NULL, *fila_contador = NULL, *fila_eventosT = NULL;
bool running = false;
//_____________________________________________________________________________________
//-------------------------------PROTÓTIPOS--------------------------------------------

void mover_prato(Button **b, int pos);
void mover_comidas(Button *comidas[], int qtd);
void redesenhar(Button **b_prato, Button *comidas[], int qtd, ALLEGRO_BITMAP *bgImagem, int min, int seg, int contadores[]);
int colisao(Button *prato, Button *comidas[], int qtd, int contadores[]);
void jogar();
void score();
void inicializaComidas(Button *comidas[], ALLEGRO_BITMAP *cenoura, ALLEGRO_BITMAP *maca, ALLEGRO_BITMAP *berinjela, ALLEGRO_BITMAP *pera, ALLEGRO_BITMAP *carne);
ALLEGRO_BITMAP* checa_vez(int *vez, ALLEGRO_BITMAP *cenoura, ALLEGRO_BITMAP *maca, ALLEGRO_BITMAP *berinjela, ALLEGRO_BITMAP *pera, ALLEGRO_BITMAP *carne);
void libera_comida(Button *comidas[], Button *aux[], int *qtd);
void destruir_bitmaps(Button *comidas[], int tamanho);
int checa_flag(ALLEGRO_BITMAP *aux, ALLEGRO_BITMAP *cenoura, ALLEGRO_BITMAP *maca, ALLEGRO_BITMAP *berinjela, ALLEGRO_BITMAP *pera, ALLEGRO_BITMAP *carne);
void controla_contador(int flag, int contadores[]);
int regra_de_fim(int min, int contadores[]);
int score_atual(int contadores[]);
void fim_de_jogo(int score_atual);

//_____________________________________________________________________________________
//------------------------------------------MAIN---------------------------------------
int main(){
	int em_jogar = 0, em_score = 0, c_jogar = 0, c_score = 0, sair = 0;
//----------------------INICIALIZAÇÕES INICIAIS----------------------------------------
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
	//__________________________________________________________________________________
	//--------------------FILA DE EVENTOS ESSENCIAIS------------------------------------
	fila_eventos = al_create_event_queue();
	al_register_event_source(fila_eventos, al_get_mouse_event_source());
	al_register_event_source(fila_eventos, al_get_display_event_source(view));
	//__________________________________________________________________________________

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
			}
			else if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
				sair = 1;
				//break;
				printf("clique no X");
			}
		}	
		
	}
	al_destroy_event_queue(fila_eventos);
	al_destroy_display(view);
	al_destroy_font(font);
	al_destroy_bitmap(bgImagem);
	return 0;
}
//________________________________________________________________________________

void mover_prato(Button **b, int pos){
	
	if (pos == 0){//pos == 0 == esquerda
		if ((((*b)->pos_x) - 10) == -10){
			//faz nada, pois o prato se encontra na borda
		}
		else{
			(*b)->pos_x = (*b)->pos_x - 10;
		}
	}
	else{//pos == 1 == direita
		if((((*b)->pos_x) + 10) == 570){
			//faz nada, pois o prato se encontra na borda
		}
		else{
			(*b)->pos_x = (*b)->pos_x + 10;
		}
	}
}

void mover_comidas(Button *comidas[], int qtd){
	if (qtd != 0){
		for (int i = 0; i < qtd; i++){
			comidas[i]->pos_y = comidas[i]->pos_y + 10;
		}
	}
	
}

void redesenhar(Button **b_prato, Button *comidas[], int qtd, ALLEGRO_BITMAP *bgImagem, int min, int seg, int contadores[]){
	al_clear_to_color(al_map_rgb(255, 255, 255));
	al_draw_bitmap(bgImagem, 0, 0, 0);
	al_draw_bitmap((*b_prato)->imagem, (*b_prato)->pos_x, (*b_prato)->pos_y, 0);
	for (int i = 0; i < qtd; i++){
		al_draw_bitmap(comidas[i]->imagem, comidas[i]->pos_x, comidas[i]->pos_y, 0);
	}
	//al_draw_bitmap((*b_carrot)->imagem, (*b_carrot)->pos_x, (*b_carrot)->pos_y, 0);
	al_draw_textf(font, al_map_rgb(100, 250, 0), 500, 20, 0, "%.2d:%.2d", min, seg);
	al_draw_textf(font, al_map_rgb(100, 250, 0), 590, 95, 0, "%dX", contadores[0]); //cenoura
	al_draw_textf(font, al_map_rgb(100, 250, 0), 590, 156, 0, "%dX", contadores[1]); //maça
	al_draw_textf(font, al_map_rgb(100, 250, 0), 590, 228, 0, "%dX", contadores[2]); //carne
	al_draw_textf(font, al_map_rgb(100, 250, 0), 590, 286, 0, "%dX", contadores[3]); //pera
	al_draw_textf(font, al_map_rgb(100, 250, 0), 590, 368, 0, "%dX", contadores[4]); //berinjela
	al_flip_display();
}

ALLEGRO_BITMAP* checa_vez(int *vez, ALLEGRO_BITMAP *cenoura, ALLEGRO_BITMAP *maca, ALLEGRO_BITMAP *berinjela, ALLEGRO_BITMAP *pera, ALLEGRO_BITMAP *carne){
	if (*vez == 0){
		(*vez)++;
		return cenoura; //15|15
	}
	else if (*vez == 1){ 
		(*vez)++;
		return maca; //16|16
	}
	else if (*vez == 2){
		(*vez)++;
		return berinjela; //14|14
	}
	else if (*vez == 3){
		(*vez)++;
		return pera; //15|15
	}
	else{
		(*vez) = 0;
		return carne; //23|23
	}
}

void destruir_bitmaps(Button *comidas[], int tamanho){
	for (int i = 0; i < tamanho; i++){
		al_destroy_bitmap(comidas[i]->imagem);
	}
}
int checa_flag(ALLEGRO_BITMAP *aux, ALLEGRO_BITMAP *cenoura, ALLEGRO_BITMAP *maca, ALLEGRO_BITMAP *carne, ALLEGRO_BITMAP *pera, ALLEGRO_BITMAP *berinjela){
	if (aux == cenoura){
		return 1;
	}
	else if (aux == maca){
		return 2;
	}
	else if (aux == carne){
		return 3;
	}
	else if (aux == pera){
		return 4;
	}
	else{
		return 5;
	}
}

void inicializaComidas(Button *comidas[], ALLEGRO_BITMAP *cenoura, ALLEGRO_BITMAP *maca, ALLEGRO_BITMAP *berinjela, ALLEGRO_BITMAP *pera, ALLEGRO_BITMAP *carne){
	int coluna1 = 0, coluna2 = 1, coluna3 = 2, coluna4 = 3, coluna5 = 4, vez = 0, flag;
	ALLEGRO_BITMAP *aux;
	Button *comida;
	for (int i = 0; i<50; i++){
		if (i == coluna1){
			aux = checa_vez(&vez, cenoura, maca, berinjela, pera, carne);
			flag = checa_flag(aux, cenoura, maca, carne, pera, berinjela);
			comida = criar_botao(aux, 64, 80, 15, 15, flag);
			al_convert_mask_to_alpha(comida->imagem, al_map_rgb(255, 255, 255));
			comidas[i] = comida;
			coluna1 = coluna1 + 4;
		}
		else if (i == coluna2){
			aux = checa_vez(&vez, cenoura, maca, berinjela, pera, carne);
			flag = checa_flag(aux, cenoura, maca, carne, pera, berinjela);
			comida = criar_botao(aux, 192, 80, 15, 15, flag);
			al_convert_mask_to_alpha(comida->imagem, al_map_rgb(255, 255, 255));
			comidas[i] = comida;
			coluna2 = coluna2 + 4;
		}
		else if (i == coluna3){
			aux = checa_vez(&vez, cenoura, maca, berinjela, pera, carne);
			flag = checa_flag(aux, cenoura, maca, carne, pera, berinjela);
			comida = criar_botao(aux, 320, 80, 15, 15, flag);
			al_convert_mask_to_alpha(comida->imagem, al_map_rgb(255, 255, 255));
			comidas[i] = comida;
			coluna3 = coluna3 + 4;
		}
		else if (i == coluna4){
			aux = checa_vez(&vez, cenoura, maca, berinjela, pera, carne);
			flag = checa_flag(aux, cenoura, maca, carne, pera, berinjela);
			comida = criar_botao(aux, 448, 80, 15, 15, flag);
			al_convert_mask_to_alpha(comida->imagem, al_map_rgb(255, 255, 255));
			comidas[i] = comida;
			coluna4 = coluna4 + 4;
		}
		else{//i == coluna5
			aux = checa_vez(&vez, cenoura, maca, berinjela, pera, carne);
			flag = checa_flag(aux, cenoura, maca, carne, pera, berinjela);
			comida = criar_botao(aux, 576, 80, 15, 15, flag);
			al_convert_mask_to_alpha(comida->imagem, al_map_rgb(255, 255, 255));
			comidas[i] = comida;
			coluna5 = coluna5 + 4;
		}
	}
}

void controla_contador(int flag, int contadores[]){
	if (flag == 1){
		(contadores[0])--;
	}
	else if (flag == 2){
		(contadores[1])--;
	}
	else if (flag == 3){
		(contadores[2])--;
	}
	else if (flag == 4){
		(contadores[3])--;
	}
	else{
		(contadores[4])--;
	}
}
int colisao(Button *prato, Button *comidas[], int qtd, int contadores[]){
	if (qtd != 0){
		for (int i = 0; i< qtd; i++){
			if (comidas[i]->ativo == true){
				if ((prato->pos_x + prato->borda_x) >(comidas[i]->pos_x - comidas[i]->borda_x) &&
					(prato->pos_x - prato->borda_x) < (comidas[i]->pos_x + comidas[i]->borda_x) &&
					(prato->pos_y - prato->borda_y) < (comidas[i]->pos_y + comidas[i]->borda_y) &&
					(prato->pos_y + prato->borda_y) > (comidas[i]->pos_y + comidas[i]->borda_y)){
					comidas[i]->ativo = false;
					controla_contador(comidas[i]->flag, contadores);
					return 1;
				}

			}

		}
		
	}
	return 0;
	
}

void libera_comida(Button *comidas[], Button *aux[], int *qtd){
	int aleatorio;
	srand(time(NULL));
	while (1){
		aleatorio = rand() % 50;
		if ((comidas[aleatorio])->ativo == false){
			(comidas[aleatorio])->ativo = true;
			aux[*qtd] = comidas[aleatorio];
			(*qtd)++;
			break;
		}
	}
}
int regra_de_fim(int min, int contadores[]){
	if (min == 1){
		return 1; //1 min de jogo, fim do jogo!!
	}
	else if (contadores[0] <= 0 && contadores[1] <= 0 && contadores[2] <= 0 && contadores[3] <= 0 && contadores[4] <= 0){
		return 1; //O usuario pegou todos os alimentos!!
	}
	else{
		return 0; //o jogo ainda não pode acabar!!
	}
}

int score_atual(int contadores[]){
	return 1;
}

void fim_de_jogo(int pontos){
	printf("fim de jogo!!!!!!");
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_textf(font, al_map_rgb(255, 255, 255), WIDTH / 2, HEIGHT / 2, ALLEGRO_ALIGN_CENTER, "SEU SCORE: %d", pontos);
	al_flip_display();
	//aqui vai onde o usuario recebe insere o nome dele para salvar em arquivo o recorde!!
	al_rest(2.0);
	al_destroy_bitmap(bgImagem);
	al_destroy_font(font);
	al_destroy_display(view);
	main();

}
void jogar(){
	//---------------------DECLARAÇÃO DE VARIÁVEIS--------------------------------------------------
	ALLEGRO_BITMAP *bgImagem = NULL; //PLANO DE FUNDO
	ALLEGRO_BITMAP *prato = NULL, *carrot = NULL, *apple = NULL, *pear = NULL, *meat = NULL, *eggplant = NULL; //OBJETOS
	ALLEGRO_TIMER *timer = NULL, *contador = 0;
	Button *b_prato;
	Button *comidas[50], *comidas_aux[50];
	int tecla = 0, *qtd = 0;
	int min = 0, seg = 0, tempo_de_print = 1;
	int fim = 0;
	int *contadores[] = {2, 5, 4, 2, 3};//cenoura, maça, pera, carne, berinjela.
	int pontos = 0;

	//ADD-ONS
	al_init_image_addon();

	//------------------------------------INICIALIZAÇÕES---------------------------
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
	//_________________________________________________________________________
	//---------------------CARREGANDO OS BITMAPS-------------------------------

	bgImagem = al_load_bitmap("cozinha.jpg");
	prato = al_load_bitmap("prato.png");
	carrot = al_load_bitmap("cenoura.png");
	apple = al_load_bitmap("maca.png");
	meat = al_load_bitmap("carne.png");
	pear = al_load_bitmap("pera.png");
	eggplant = al_load_bitmap("berinjela.png");

	b_prato = criar_botao(prato, 260, 400, 25, 25, 0); //O 0 é a flag do prato!!!
	al_convert_mask_to_alpha(b_prato->imagem, al_map_rgb(255, 255, 255));
	b_prato->ativo = true; //ativa o botão para poder ser printado
	inicializaComidas(&comidas, carrot, apple, eggplant, pear, meat);
	/*b_carrot = criar_botao(carrot,WIDTH/2, 80, 15, 15);
	b_apple = criar_botao(apple, 290, 80, 16, 16);
	b_meat = criar_botao(meat, 270, 80, 23, 23);
	b_pear = criar_botao(pear, 240, 80, 15, 15);
	b_eggplant = criar_botao(eggplant, 290, 80, 14, 14);
	
	al_convert_mask_to_alpha(b_carrot->imagem, al_map_rgb(255, 255, 255));
	al_convert_mask_to_alpha(b_apple->imagem, al_map_rgb(255, 255, 255));
	al_convert_mask_to_alpha(b_meat->imagem, al_map_rgb(255, 255, 255));
	al_convert_mask_to_alpha(b_pear->imagem, al_map_rgb(255, 255, 255));
	al_convert_mask_to_alpha(b_eggplant->imagem, al_map_rgb(255, 255, 255));*/
	//_________________________________________________________________________
	printf("*********\n**JOGAR**\n*********\n");
	printf("jogando....");

	//al_clear_to_color(al_map_rgb(255, 255, 255));
	//al_draw_bitmap(bgImagem, 0, 0, 0);
	//al_draw_textf(font, al_map_rgb(100, 250, 0), 500, 20, 0, "%.2d:%.2d", min, seg);
	//al_flip_display();
	//----------------------------INICIALIZANDO FILAS DE EVENTOS------------------------------
	fila_eventosT = al_create_event_queue();
	al_register_event_source(fila_eventos, al_get_timer_event_source(timer));
	al_register_event_source(fila_contador, al_get_timer_event_source(contador));
	al_register_event_source(fila_eventosT, al_get_keyboard_event_source());
	al_register_event_source(fila_eventosT, al_get_display_event_source(view));
	al_start_timer(timer);
	al_start_timer(contador);
	//libera_comida(&comidas, &comidas_aux, &qtd);
	//________________________________________________________________________________________

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
					mover_prato(&b_prato, 0);
					printf("%d\n", b_prato->pos_x);
					redesenhar(&b_prato, comidas_aux, qtd, bgImagem, min, seg, contadores);
					tecla = 1;
					break;
				case ALLEGRO_KEY_RIGHT:
					//mover boneco para a direita
					printf("direita\n");
					mover_prato(&b_prato, 1);
					printf("%d\n", b_prato->pos_x);
					redesenhar(&b_prato, comidas_aux, qtd, bgImagem, min, seg, contadores);
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
				fim = 1;
				break;
			}
			

		}
		pontos = score_atual(contadores);
		if (regra_de_fim(min, contadores) == 1){
			//al_destroy_event_queue(fila_eventos);
			//al_destroy_event_queue(fila_eventosT);
			//al_destroy_bitmap(bgImagem);
			//al_destroy_timer(timer);
			break;
		}
		//IMPLEMENTAÇÃO DO TIMER
		if (!al_is_event_queue_empty(fila_contador)){
			ALLEGRO_EVENT evento;
			al_wait_for_event(fila_contador, &evento);

			if (evento.type == ALLEGRO_EVENT_TIMER){
				seg++;
				if (tempo_de_print == 1){
					libera_comida(&comidas, &comidas_aux, &qtd);
					tempo_de_print = 0;
				}
				else{
					tempo_de_print++;
				}
				if (seg == 60){
					min++;
					seg = 0;
				}
			}
			
			mover_comidas(&comidas_aux, qtd);
			redesenhar(&b_prato, &comidas_aux, qtd, bgImagem, min, seg, contadores);
			mover_comidas(&comidas_aux, qtd);
			redesenhar(&b_prato, &comidas_aux, qtd, bgImagem, min, seg, contadores);
			//mover_comidas(&comidas_aux, qtd);
			if (colisao(b_prato, &comidas_aux, qtd, contadores) == 1){
				printf("prato e comida colidiram!\n");
			}
		}


	}
	fim_de_jogo(pontos);
	//------------------DESTRUIR OS RECURSOS INICIALIZADOS------------------
	//al_destroy_event_queue(fila_eventos);
	al_destroy_event_queue(fila_eventosT);
	al_destroy_bitmap(bgImagem);
	//destruir_bitmaps(comidas, 50); // vetor completo das comidas
	//destruir_bitmaps(comidas_aux, qtd); // vetor com os bitmaps das comidas que foram capturadas
	//al_destroy_bitmap(&(b_prato)->imagem);
	al_destroy_timer(timer);
	//______________________________________________________________________
}
	
void score(){
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_text(font, al_map_rgb(255, 255, 255), 310, 40, ALLEGRO_ALIGN_CENTER, "SCORES");
	al_draw_text(font, al_map_rgb(255, 255, 255), 580, 420, ALLEGRO_ALIGN_RIGHT, "VOLTAR");

	al_flip_display();
	printf("*********\n**SCORE**\n*********");
	al_rest(3.0);
}