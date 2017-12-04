#include <stdio.h>
#include "funcoes.h"
#include"score.h"

FILE *criarArquivo(char nome[]) {
	FILE *arq;
	arq = fopen(nome, "r+b");
	if (arq == NULL) {
		arq = fopen(nome, "w+b");
	}
	return arq;
}
//-----------------------------VARIÁVEIS DO MAIN---------------------------------------
int WIDTH = 640;
int HEIGHT = 460;
ALLEGRO_DISPLAY *view = NULL;
ALLEGRO_FONT *font = NULL;
ALLEGRO_BITMAP *bgImagem = NULL, *tela_instrucoes = NULL;
ALLEGRO_EVENT_QUEUE *fila_eventos = NULL, *fila_contador = NULL, *fila_eventosT = NULL;
ALLEGRO_AUDIO_STREAM *musica = NULL;
bool running = false;
const int FRAMES_POR_SEGUNDO = 60;
double tempoInicial = 0;
TUsuario usuario;
char str[6];
FILE *arq;
int pontos;
//_____________________________________________________________________________________
//-------------------------------PROTÓTIPOS--------------------------------------------

void mover_prato(Button **b, int pos);
void mover_comidas(Button *comidas[], int qtd);
void redesenhar(Button **b_prato, Button *comidas[], int qtd, ALLEGRO_BITMAP *bgImagem, int min, int seg, int contadores[], int pontos);
int colisao(Button *prato, Button *comidas[], int qtd, int *qtd_frutas, int *qtd_proteinas, int *qtd_restritivos, int *qtd_hortalicas, int *qtd_carbo, int *qtd_derivados);
void jogar();
void score();
void inicializaComidas(Button *comidas[], ALLEGRO_BITMAP *cenoura, ALLEGRO_BITMAP *maca, ALLEGRO_BITMAP *berinjela, ALLEGRO_BITMAP *pera, ALLEGRO_BITMAP *carne);
ALLEGRO_BITMAP* checa_vez(int *vez, ALLEGRO_BITMAP *cenoura, ALLEGRO_BITMAP *maca, ALLEGRO_BITMAP *berinjela, ALLEGRO_BITMAP *pera, ALLEGRO_BITMAP *carne);
void libera_comida(Button *comidas[], Button *aux[], int *qtd);
void destruir_bitmaps(Button comidas[], int tamanho);
int checa_flag(ALLEGRO_BITMAP *aux, ALLEGRO_BITMAP *cenoura, ALLEGRO_BITMAP *maca, ALLEGRO_BITMAP *berinjela, ALLEGRO_BITMAP *pera, ALLEGRO_BITMAP *carne);
void controla_contador(int flag, int contadores[]);
int regra_de_fim(int min, int contadores[]);
int score_atual(int qtd_frutas, int qtd_proteinas, int qtd_restritivos, int qtd_hortalicas, int qtd_carbo, int qtd_derivados);
void fim_de_jogo(int pontos);
void destroi_imagem(ALLEGRO_BITMAP **imagem);
void instrucoes();
void salva_recorde();

//_____________________________________________________________________________________
//------------------------------------------MAIN---------------------------------------
int main(){
	int em_jogar = 0, em_score = 0, c_jogar = 0, c_score = 0, sair = 0, status, tam;
	arq = criarArquivo("Recorde.txt");
	
//----------------------INICIALIZAÇÕES INICIAIS----------------------------------------
	fseek(arq, -sizeof(TUsuario), 2);
	tam = ftell(arq) + sizeof(TUsuario) / sizeof(TUsuario);
	if (tam == 3) {
		status = fread(&usuario, sizeof(TUsuario), 1, arq);
		if (status != 1) {
			printf("Erro de leitura\n");
		}
	}
	else {
		usuario.recorde = 0;
		strcpy(usuario.nome, "");
	}
	
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

	if (!al_install_audio()) {
		printf("Falha ao inicializar \E1udio.\n");
		return 0;
	}
	if (!al_init_acodec_addon()) {
		printf("Falha ao inicializar codecs de \E1udio.\n");
		return 0;
	}
	if (!al_reserve_samples(1)){
		fprintf(stderr, "falha ao alocar canais de audio\n");
		return false;
	}
	musica = al_load_audio_stream("pou.wav", 4, 1024);
	if (!musica) {
		printf("Falha ao carregar \"pou.wav\".\n");
		return 0;
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
	font = al_load_font("Forte.ttf", 40, 0);
	bgImagem = al_load_bitmap("fundo.jpg");
	tela_instrucoes = al_load_bitmap("fundo_instrucoes.jpg");

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
	al_attach_audio_stream_to_mixer(musica, al_get_default_mixer());
	al_set_audio_stream_playmode(musica, ALLEGRO_PLAYMODE_LOOP);
	while (!sair){
		//Atualizando o record
		if (usuario.recorde < pontos) {
			fim_de_jogo(pontos);
			pontos = 0;
		}
		al_clear_to_color(al_map_rgb(255, 255, 255));
		al_draw_bitmap(bgImagem, 0, 0, 0);
		//JOGAR E SCORE SERÃO BOTÕES QUE REDIRECIONARÃO PARA GAME E SCORE RESPECTIVAMENTE!!
		al_draw_text(font, al_map_rgb(173, 0, 0), (WIDTH / 2), (HEIGHT / 2) + 20, ALLEGRO_ALIGN_CENTER, "JOGAR");
		al_draw_text(font, al_map_rgb(173, 0, 0), (WIDTH / 2), (HEIGHT / 2) + 90, ALLEGRO_ALIGN_CENTER, "SCORE");
		al_draw_text(font, al_map_rgb(173, 0, 0), (WIDTH / 2), (HEIGHT / 2) + 160, ALLEGRO_ALIGN_CENTER, "SAIR");
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
					evento.mouse.y <= 290 - 10 &&
					evento.mouse.y >= 260 + 10){
					//printf("clique em jogar");
					
					al_draw_text(font, al_map_rgb(255, 255, 255), (WIDTH / 2), (HEIGHT / 2) + 20, ALLEGRO_ALIGN_CENTER, "JOGAR");
					al_flip_display();
					al_rest(0.3);
					instrucoes();
					//jogar();
				}
				else if (evento.mouse.x >= 247 - 10 &&
					evento.mouse.x <= 372 + 10 &&
					evento.mouse.y <= 355 - 10 &&
					evento.mouse.y >= 310 + 10){
					//printf("clique em score");
					al_draw_text(font, al_map_rgb(255, 255, 255), (WIDTH / 2), (HEIGHT / 2) + 90, ALLEGRO_ALIGN_CENTER, "SCORE");
					al_flip_display();
					al_rest(0.3);
					score();

				}
				else if (evento.mouse.x >= 283 - 10 &&
					evento.mouse.x <= 363 + 10 &&
					evento.mouse.y <= 431 - 10 &&
					evento.mouse.y >= 388 + 10){
					printf("clique em sair");
					al_draw_text(font, al_map_rgb(255, 255, 255), (WIDTH / 2), (HEIGHT / 2) + 160, ALLEGRO_ALIGN_CENTER, "SAIR");
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
	al_destroy_audio_stream(musica);
	fclose(arq);
	return 0;
}
//________________________________________________________________________________

void mover_prato(BPrato **b, int pos){
	
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
			comidas[i]->pos_y = comidas[i]->pos_y + 15;
		}
	}
	
}

void redesenhar(BPrato **prato, Button **b_prato, Button *comidas[], int qtd, ALLEGRO_BITMAP *bgImagem, int min, int seg, int pontos){
	al_clear_to_color(al_map_rgb(255, 255, 255));
	al_draw_bitmap(bgImagem, 0, 0, 0);
	if (pontos < 1){
		al_draw_bitmap((*prato)->prato0, (*prato)->pos_x, (*prato)->pos_y, 0);
	}
	else if (pontos <= 3){
		al_draw_bitmap((*prato)->prato1, (*prato)->pos_x, (*prato)->pos_y, 0);
	}
	else if (pontos <= 6){
		al_draw_bitmap((*prato)->prato2, (*prato)->pos_x, (*prato)->pos_y, 0);
	}
	else if (pontos <= 9){
		al_draw_bitmap((*prato)->prato3, (*prato)->pos_x, (*prato)->pos_y, 0);
	}
	for (int i = 0; i < qtd; i++){
		al_draw_bitmap(comidas[i]->imagem, comidas[i]->pos_x, comidas[i]->pos_y, 0);
	}
	//al_draw_bitmap((*b_carrot)->imagem, (*b_carrot)->pos_x, (*b_carrot)->pos_y, 0);
	al_draw_textf(font, al_map_rgb(237, 166, 26), 500, 20, 0, "%.2d:%.2d", min, seg);
	al_flip_display();
}

void redesenhar_pontuacao(ButtonScore **pts_frutas, ButtonScore *pts_proteinas, ButtonScore *pts_restritivos, ButtonScore *pts_hortalicas, ButtonScore *pts_carbo, ButtonScore *pts_derivados, int qtd_frutas, int qtd_proteinas, int qtd_restritivos, int qtd_hortalicas, int qtd_carbo, int qtd_derivados){
	ALLEGRO_FONT *fonte2 = al_load_font("forte.ttf", 10, 0);
	if (qtd_frutas == 0){
		al_draw_bitmap((*pts_frutas)->vida0, (*pts_frutas)->pos_x, (*pts_frutas)->pos_y, 0);
	}
	else if (qtd_frutas == 1){
		al_draw_bitmap((*pts_frutas)->vida1, (*pts_frutas)->pos_x, (*pts_frutas)->pos_y, 0);
	}
	else if (qtd_frutas == 2){
		al_draw_bitmap((*pts_frutas)->vida2, (*pts_frutas)->pos_x, (*pts_frutas)->pos_y, 0);
	}
	else if (qtd_frutas == 3){
		al_draw_bitmap((*pts_frutas)->vida3, (*pts_frutas)->pos_x, (*pts_frutas)->pos_y, 0);
	}
	else if (qtd_frutas >= 4){
		al_draw_bitmap((*pts_frutas)->vida4, (*pts_frutas)->pos_x, (*pts_frutas)->pos_y, 0);
	}
	al_draw_text(fonte2, al_map_rgb(0, 100, 0), 30, 90, ALLEGRO_ALIGN_CENTER, "FRUTAS");
	if (qtd_proteinas == 0){
		al_draw_bitmap(pts_proteinas->vida0, pts_proteinas->pos_x, pts_proteinas->pos_y, 0);
	}
	else if (qtd_proteinas == 1){
		al_draw_bitmap(pts_proteinas->vida1, pts_proteinas->pos_x, pts_proteinas->pos_y, 0);
	}
	else if (qtd_proteinas == 2){
		al_draw_bitmap(pts_proteinas->vida2, pts_proteinas->pos_x, pts_proteinas->pos_y, 0);
	}
	else if (qtd_proteinas == 3){
		al_draw_bitmap(pts_proteinas->vida3, pts_proteinas->pos_x, pts_proteinas->pos_y, 0);
	}
	else if (qtd_proteinas >= 4){
		al_draw_bitmap(pts_proteinas->vida4, pts_proteinas->pos_x, pts_proteinas->pos_y, 0);
	}
	al_draw_text(fonte2, al_map_rgb(0, 100, 0), 90, 90, ALLEGRO_ALIGN_CENTER, "PROTEINAS");
	if (qtd_restritivos == 0){
		al_draw_bitmap(pts_restritivos->vida0, pts_restritivos->pos_x, pts_restritivos->pos_y, 0);
	}
	else if (qtd_restritivos == 1){
		al_draw_bitmap(pts_restritivos->vida2, pts_restritivos->pos_x, pts_restritivos->pos_y, 0);
	}
	else if (qtd_restritivos == 2){
		al_draw_bitmap(pts_restritivos->vida3, pts_restritivos->pos_x, pts_restritivos->pos_y, 0);
	}
	else if (qtd_restritivos > 2){
		al_draw_bitmap(pts_restritivos->vida4, pts_restritivos->pos_x, pts_restritivos->pos_y, 0);
	}
	al_draw_text(fonte2, al_map_rgb(0, 100, 0), 160, 90, ALLEGRO_ALIGN_CENTER, "RESTRITIVOS");
	if (qtd_hortalicas == 0){
		al_draw_bitmap(pts_hortalicas->vida0, pts_hortalicas->pos_x, pts_hortalicas->pos_y, 0);
	}
	else if (qtd_hortalicas == 1){
		al_draw_bitmap(pts_hortalicas->vida1, pts_hortalicas->pos_x, pts_hortalicas->pos_y, 0);
	}
	else if (qtd_hortalicas == 2){
		al_draw_bitmap(pts_hortalicas->vida2, pts_hortalicas->pos_x, pts_hortalicas->pos_y, 0);
	}
	else if (qtd_hortalicas == 3){
		al_draw_bitmap(pts_hortalicas->vida3, pts_hortalicas->pos_x, pts_hortalicas->pos_y, 0);
	}
	else if (qtd_hortalicas >= 4){
		al_draw_bitmap(pts_hortalicas->vida4, pts_hortalicas->pos_x, pts_hortalicas->pos_y, 0);
	}
	al_draw_text(fonte2, al_map_rgb(0, 100, 0), 230, 90, ALLEGRO_ALIGN_CENTER, "HORTALICAS");
	if (qtd_carbo < 2){
		al_draw_bitmap(pts_carbo->vida0, pts_carbo->pos_x, pts_carbo->pos_y, 0);
	}
	else if (qtd_carbo < 4){
		al_draw_bitmap(pts_carbo->vida1, pts_carbo->pos_x, pts_carbo->pos_y, 0);
	}
	else if (qtd_carbo < 6){
		al_draw_bitmap(pts_carbo->vida2, pts_carbo->pos_x, pts_carbo->pos_y, 0);
	}
	else if (qtd_carbo < 8){
		al_draw_bitmap(pts_carbo->vida3, pts_carbo->pos_x, pts_carbo->pos_y, 0);
	}
	else if (qtd_carbo >= 9){
		al_draw_bitmap(pts_carbo->vida4, pts_carbo->pos_x, pts_carbo->pos_y, 0);
	}
	al_draw_text(fonte2, al_map_rgb(0, 100, 0), 310, 90, ALLEGRO_ALIGN_CENTER, "CARBOIDRATOS");
	if (qtd_derivados == 0){
		al_draw_bitmap(pts_derivados->vida0, pts_derivados->pos_x, pts_derivados->pos_y, 0);
	}
	else if (qtd_derivados == 1){
		al_draw_bitmap(pts_derivados->vida1, pts_derivados->pos_x, pts_derivados->pos_y, 0);
	}
	else if (qtd_derivados == 2){
		al_draw_bitmap(pts_derivados->vida2, pts_derivados->pos_x, pts_derivados->pos_y, 0);
	}
	else if (qtd_derivados == 3){
		al_draw_bitmap(pts_derivados->vida3, pts_derivados->pos_x, pts_derivados->pos_y, 0);
	}
	else if (qtd_derivados >= 4){
		al_draw_bitmap(pts_derivados->vida4, pts_derivados->pos_x, pts_derivados->pos_y, 0);
	}
	al_draw_text(fonte2, al_map_rgb(0, 100, 0), 390, 90, ALLEGRO_ALIGN_CENTER, "DERIVADOS");
	al_flip_display();
}

void destroi_imagem(ALLEGRO_BITMAP *imagem){
	al_destroy_bitmap(imagem);
}
void destruir_bitmaps(Button comidas[], int tamanho){
	for (int i = 0; i < tamanho; i++){
		destroi_imagem(&comidas[i].imagem);
		free(&comidas[i]);
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
int checa_tipo(ALLEGRO_BITMAP *imagens[], ALLEGRO_BITMAP *imagem){
	//essa função aqui retorna os tipos que repreentam esses alimentos na categoria aimentar
	for (int i = 0; i < 20; i++){
		if (i < 5){
			if (imagens[i] == imagem){
				return 1; //frutas
			}
			
		}
		else if (i < 8){
			if (imagens[i] == imagem){
				return 2; //proteinas
			}
			
		}
		else if (i < 11){
			if (imagens[i] == imagem){
				return 3; //restritivos
			}
			
		}
		else if(i< 13){
			if (imagens[i] == imagem){
				return 4; //hortaliças
			}
			
		}
		else if (i < 18){
			if (imagens[i] == imagem){
				return 5; //carboidratos
			}
			
		}
		else{
			if (imagens[i] == imagem){
				return 6; //derivados
			}
		}
	}
}

//substituir esse procedimento aqui pelo de baixo, esse aqui será o novo inicalizador de bitmaps
void inicializa_Comidas(Button *comidas[], ALLEGRO_BITMAP *imagens[]){
	int coluna1 = 0, coluna2 = 1, coluna3 = 2, coluna4 = 3;
	int cont = 0, flag;
	Button *comida;
	for (int i = 0; i < 60; i++){
		if (cont == 20){ //esse contador controla as posições do vetor de imagens que contém 20 imagens inicalizadas 
			cont = 0;	//e que vão ser distribuidas num vetor de 60.
		}
		if (i == coluna1){
			flag = checa_tipo(imagens, imagens[cont]);
			comida = criar_botao(imagens[cont], 64, 100, 15, 15, flag);
			al_convert_mask_to_alpha(comida->imagem, al_map_rgb(255, 255, 255));
			comidas[i] = comida;
			coluna1 = coluna1 + 4;
			cont++;
		}
		else if (i == coluna2){
			flag = checa_tipo(imagens, imagens[cont]);
			comida = criar_botao(imagens[cont], 192, 100, 15, 15, flag);
			al_convert_mask_to_alpha(comida->imagem, al_map_rgb(255, 255, 255));
			comidas[i] = comida;
			coluna2 = coluna2 + 4;
			cont++;
		}
		else if (i == coluna3){
			flag = checa_tipo(imagens, imagens[cont]);
			comida = criar_botao(imagens[cont], 320, 100, 15, 15, flag);
			al_convert_mask_to_alpha(comida->imagem, al_map_rgb(255, 255, 255));
			comidas[i] = comida;
			coluna3 = coluna3 + 4;
			cont++;
		}
		else if (i == coluna4){
			flag = checa_tipo(imagens, imagens[cont]);
			comida = criar_botao(imagens[cont], 448, 100, 15, 15, flag);
			al_convert_mask_to_alpha(comida->imagem, al_map_rgb(255, 255, 255));
			comidas[i] = comida;
			coluna4 = coluna4 + 4;
			cont++;
		}
		else{//não é para ser acessado aqui!!
			flag = checa_tipo(imagens, imagens[cont]);
			comida = criar_botao(imagens[cont], 576, 100, 15, 15, flag);
			al_convert_mask_to_alpha(comida->imagem, al_map_rgb(255, 255, 255));
			comidas[i] = comida;
			cont++;
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
int colisao(BPrato *prato, Button *comidas[], int qtd, int *qtd_frutas, int *qtd_proteinas, int *qtd_restritivos, int *qtd_hortalicas, int *qtd_carbo, int *qtd_derivados){
	if (qtd != 0){
		for (int i = 0; i< qtd; i++){
			if (comidas[i]->ativo == true){
				if ((prato->pos_x + prato->borda_x) >(comidas[i]->pos_x - comidas[i]->borda_x) &&
					(prato->pos_x - prato->borda_x) < (comidas[i]->pos_x + comidas[i]->borda_x) &&
					(prato->pos_y - prato->borda_y) < (comidas[i]->pos_y + comidas[i]->borda_y) &&
					(prato->pos_y + prato->borda_y) > (comidas[i]->pos_y + comidas[i]->borda_y)){
					comidas[i]->ativo = false;
					if (comidas[i]->flag == 1){
						(*qtd_frutas)++;
					}
					else if (comidas[i]->flag == 2){
						(*qtd_proteinas)++;
					}
					else if (comidas[i]->flag == 3){
						(*qtd_restritivos)++;
					}
					else if (comidas[i]->flag == 4){
						(*qtd_hortalicas)++;
					}
					else if (comidas[i]->flag == 5){
						(*qtd_carbo)++;
					}
					else{
						(*qtd_derivados)++;
					}
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
int regra_de_fim(int min, int pontos){
	if (min == 2){
		return 1; //2 min de jogo, fim do jogo!!
	}
	else if (pontos ==  22){
		return 1; //O usuario pegou todos os alimentos!!
	}
	else{
		return 0; //o jogo ainda não pode acabar!!
	}
}

int score_atual(int qtd_frutas, int qtd_proteinas, int qtd_restritivos, int qtd_hortalicas, int qtd_carbo, int qtd_derivados){
	int pontos = 0;
	int frutas = qtd_frutas, proteinas = qtd_proteinas, restritivos = qtd_restritivos, hortalicas = qtd_hortalicas, carbo = qtd_carbo, derivados = qtd_derivados;
	if (qtd_frutas > 3){
		pontos = pontos - (qtd_frutas-3);
	}
	if (qtd_proteinas > 3){
		pontos = pontos - (qtd_proteinas - 3);
	}
	if (qtd_restritivos > 2){
		pontos = pontos - (qtd_restritivos - 2);
	}
	if (qtd_hortalicas > 3){
		pontos = pontos - (qtd_hortalicas - 3);
	}
	if (qtd_carbo > 8){
		pontos = pontos - (qtd_carbo - 8);
	}
	if (qtd_derivados > 3){
		pontos = pontos - (qtd_derivados - 3);
	}
	pontos = pontos + frutas + proteinas + restritivos + hortalicas + carbo + derivados;
	return pontos;
}
void exibir_texto_centralizado(){
	if (strlen(str) > 0)
	{
		al_draw_text(font, al_map_rgb(255, 255, 255), WIDTH / 2, 400,ALLEGRO_ALIGN_CENTRE, str);
	}
}
void fim_de_jogo(int pontos){
	int sair = 0;
	strcpy(str, "");
	al_clear_to_color(al_map_rgb(255, 178, 28));
	al_draw_text(font, al_map_rgb(255, 255, 255), WIDTH / 2, HEIGHT - 300, ALLEGRO_ALIGN_CENTER, "Novo recorde batido.");
	al_draw_text(font, al_map_rgb(255, 255, 255), WIDTH / 2, HEIGHT - 250, ALLEGRO_ALIGN_CENTER, "Digite seu nome: ");
	al_flip_display();
	al_register_event_source(fila_eventos, al_get_keyboard_event_source());
	al_register_event_source(fila_eventos, al_get_display_event_source(view));
	while (!sair) {
		al_clear_to_color(al_map_rgb(255, 178, 28));
		al_draw_text(font, al_map_rgb(255, 255, 255), WIDTH / 2, HEIGHT - 300, ALLEGRO_ALIGN_CENTER, "Novo recorde batido.");
		if (!al_event_queue_is_empty(fila_eventos)) {
			ALLEGRO_EVENT evento;
			al_wait_for_event(fila_eventos, &evento);
			if (evento.type == ALLEGRO_EVENT_KEY_CHAR) {
				if (strlen(str) <= 4) {
					char temp[] = { evento.keyboard.unichar, '\0' };
					if (evento.keyboard.unichar >= '0' && evento.keyboard.unichar <= '9') {
						strcat(str, temp);
					}
					else if (evento.keyboard.unichar >= 'A' && evento.keyboard.unichar <= 'Z') {
						strcat(str, temp);
					}
					else if (evento.keyboard.unichar >= 'a' && evento.keyboard.unichar <= 'z') {
						strcat(str, temp);
					}
				}
				else {
					sair = 1;
				}
				if (evento.keyboard.keycode == ALLEGRO_KEY_BACKSPACE && strlen(str) != 0) {
					str[strlen(str) - 1] = '\0';
				}
			}
			if (evento.type == ALLEGRO_EVENT_KEY_DOWN && evento.keyboard.keycode == ALLEGRO_KEY_ENTER) {
				sair = 1;
			}
			al_draw_textf(font, al_map_rgb(255, 255, 255), WIDTH / 2, HEIGHT - 250, ALLEGRO_ALIGN_CENTER, "Digite seu nome:%s", str);
			al_flip_display();
		}
	}
	usuario.recorde = pontos;
	strcpy(usuario.nome, str);
	salva_recorde();
}

void salva_recorde() {
	int status;
	TUsuario aux[3];
	fseek(arq, 0, 0);
	int i = 0;
	while (!feof(arq)) {
		status = fread(&aux[i], sizeof(TUsuario), 1, arq);
		if (status != 1) {
			if (!feof(arq)) {
				printf("Erro na leitura do arquivo. \n");
				break;
			}
		}
		else {
			i++;
		}
	}
	if (i == 0) {//Nao tem nd no arquivo
		fseek(arq, 0, 0);
		status = fwrite(&usuario, sizeof(TUsuario), 1, arq);
		if (status != 1) {
			printf("Erro na gravacao do arquivo. \n");
		}
		usuario.recorde = 0;
	}
	else if (i == 1) {//Tem um usuario
		if (aux[0].recorde > usuario.recorde) {
			fseek(arq, 0, 2);
			status = fwrite(&usuario, sizeof(TUsuario), 1, arq);
			if (status != 1) {
				printf("Erro na gravacao do arquivo. \n");
			}
		}
		else {
			fseek(arq, 0, 0);
			status = fwrite(&usuario, sizeof(TUsuario), 1, arq);
			if (status != 1) {
				printf("Erro na gravacao do arquivo. \n");
			}
			status = fwrite(&aux[0], sizeof(TUsuario), 1, arq);
			if (status != 1) {
				printf("Erro na gravacao do arquivo. \n");
			}
		}
		usuario.recorde = 0;
	}
	else if (i == 2) {//Tem dois usuarios
		for (int j = 0; j < i; ++j) {
			if (aux[j].recorde < usuario.recorde) {
				fseek(arq, j * sizeof(TUsuario), 0);
				if (j == 0) {
					fwrite(&usuario, sizeof(TUsuario), 1, arq);
					fwrite(&aux[0], sizeof(TUsuario), 1, arq);
					fwrite(&aux[1], sizeof(TUsuario), 1, arq);
				}
				else if (j == 1) {
					fwrite(&usuario, sizeof(TUsuario), 1, arq);
					fwrite(&aux[1], sizeof(TUsuario), 1, arq);
				}
				usuario.recorde = aux[1].recorde;
				break;
			}
			else if (aux[j].recorde >= usuario.recorde && j == 1) {
				fseek(arq, 0, 2);
				fwrite(&usuario, sizeof(TUsuario), 1, arq);
			}
		}
	}
	else {//Tem tres usuarios
		for (int j = 0; j < i; ++j) {
			if (aux[j].recorde < usuario.recorde) {
				fseek(arq, j * sizeof(TUsuario), 0);
				if (j == 0) {
					fwrite(&usuario, sizeof(TUsuario), 1, arq);
					fwrite(&aux[0], sizeof(TUsuario), 1, arq);
					fwrite(&aux[1], sizeof(TUsuario), 1, arq);
				}
				else if (j == 1) {
					fwrite(&usuario, sizeof(TUsuario), 1, arq);
					fwrite(&aux[1], sizeof(TUsuario), 1, arq);
				}
				else{
					fwrite(&usuario, sizeof(TUsuario), 1, arq);
				}
				break;
			}
		}
	}
}


void inicializa_imagens(ALLEGRO_BITMAP *alimentos[]){
	alimentos[0] = al_load_bitmap("banana.png");
	alimentos[1] = al_load_bitmap("melancia.png");
	alimentos[2] = al_load_bitmap("maca.png");
	alimentos[3] = al_load_bitmap("tomate.png");
	alimentos[4] = al_load_bitmap("uva.png");
	alimentos[5] = al_load_bitmap("ovo.png");
	alimentos[6] = al_load_bitmap("carne.png");
	alimentos[7] = al_load_bitmap("frango.png");
	alimentos[8] = al_load_bitmap("chocolate.png");
	alimentos[9] = al_load_bitmap("batatafrita.png");
	alimentos[10] = al_load_bitmap("hamburguer.png");
	alimentos[11] = al_load_bitmap("cenoura.png");
	alimentos[12] = al_load_bitmap("beterraba.png");
	alimentos[13] = al_load_bitmap("feijao.png");
	alimentos[14] = al_load_bitmap("paes.png");
	alimentos[15] = al_load_bitmap("mandioca.png");
	alimentos[16] = al_load_bitmap("macarrao.png");
	alimentos[17] = al_load_bitmap("milho.png"); 
	alimentos[18] = al_load_bitmap("leite.png");
	alimentos[19] = al_load_bitmap("queijo.png");
}
void instrucoes(){
	int sair = 0;
	while (!sair){
		al_clear_to_color(al_map_rgb(0, 0, 0));
		al_draw_bitmap(tela_instrucoes, 0, 0, 0);
		al_draw_text(font, al_map_rgb(173, 0, 0), (WIDTH / 2) - 20, (HEIGHT / 2) + 150, ALLEGRO_ALIGN_CENTER, "Jogar");
		al_flip_display();

		while (!al_is_event_queue_empty(fila_eventos)){
			ALLEGRO_EVENT evento;
			ALLEGRO_TIMEOUT timeout;
			al_wait_for_event(fila_eventos, &evento);
			al_init_timeout(&timeout, 0.05);
			int tem_eventos = al_wait_for_event_until(fila_eventos, &evento, &timeout);

			if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
				printf("X = %d\n Y = %d\n", evento.mouse.x, evento.mouse.y);
				if (evento.mouse.x >= 247 - 10 &&
					evento.mouse.x <= 372 + 10 &&
					evento.mouse.y <= 428 - 10 &&
					evento.mouse.y >= 350 + 10){
					//printf("clique em jogar");

					al_draw_text(font, al_map_rgb(255, 255, 255), (WIDTH / 2) - 20, (HEIGHT / 2) + 150, ALLEGRO_ALIGN_CENTER, "Jogar");
					al_flip_display();
					al_rest(0.3);
					jogar();
				}
			}
			else if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
				sair = 1;
			}
		}
	}
}

void iniciarTimer()
{
	tempoInicial = al_get_time();
}

double obterTempoTimer()
{
	return al_get_time() - tempoInicial;
}

void jogar(){
	//---------------------DECLARAÇÃO DE VARIÁVEIS--------------------------------------------------
	ALLEGRO_BITMAP *bgImagem = NULL; //PLANO DE FUNDO
	ALLEGRO_BITMAP *prato = NULL, *alimentos[20]={NULL}; //OBJETOS
	ALLEGRO_BITMAP *prato0 = NULL, *prato1 = NULL, *prato2 = NULL, *prato3 = NULL;
	ALLEGRO_BITMAP *vida0 = NULL, *vida1 = NULL, *vida2 = NULL, *vida3 = NULL, *vida4 = NULL;
	ALLEGRO_TIMER *timer = NULL, *contador = 0;
	Button *b_prato;
	BPrato *pratos;
	Button *comidas[60], *comidas_aux[60];
	ButtonScore *pts_frutas, *pts_proteinas, *pts_restritivos, *pts_hortalicas, *pts_carbo, *pts_derivados;
	int tecla = 0, *qtd = 0;
	int min = 0, seg = 0, tempo_de_print = 1;
	int fim = 0;
	int qtd_frutas = 0, qtd_proteinas = 0, qtd_restritivos = 0, qtd_hortalicas = 0, qtd_carbo = 0, qtd_derivados = 0;
	bool continuar = true;
	int frame = 0;
	bool limitado = true;
	
	//ADD-ONS
	al_init_image_addon();

	//------------------------------------INICIALIZAÇÕES---------------------------
	timer = al_create_timer(1.0/60);
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

	bgImagem = al_load_bitmap("capaJogo.jpg");
	prato = al_load_bitmap("prato.png");
	/*carrot = al_load_bitmap("cenoura.png");
	apple = al_load_bitmap("maca.png");
	meat = al_load_bitmap("carne.png");
	pear = al_load_bitmap("pera.png");
	eggplant = al_load_bitmap("berinjela.png");*/
	vida0 = al_load_bitmap("vida0.png");
	vida1 = al_load_bitmap("vida1.png");
	vida2 = al_load_bitmap("vida2.png");
	vida3 = al_load_bitmap("vida3.png");
	vida4 = al_load_bitmap("vida4.png");
	pts_frutas = criar_barra_pontos(vida0, vida1, vida2, vida3, vida4, 15, 14);
	pts_proteinas = criar_barra_pontos(vida0, vida1, vida2, vida3, vida4, 85, 14);
	pts_restritivos = criar_barra_pontos(vida0, vida1, vida2, vida3, vida4, 155, 14);
	pts_hortalicas = criar_barra_pontos(vida0, vida1, vida2, vida3, vida4, 225, 14);
	pts_carbo = criar_barra_pontos(vida0, vida1, vida2, vida3, vida4, 295, 14);
	pts_derivados = criar_barra_pontos(vida0, vida1, vida2, vida3, vida4, 365, 14);
	prato0 = al_load_bitmap("prato0.png");
	
	prato1 = al_load_bitmap("prato1.png");
	
	prato2 = al_load_bitmap("prato2.png");
	
	prato3 = al_load_bitmap("prato3.png");
	
	pratos = criar_prato(prato0, prato1, prato2, prato3, 260, 400, 25, 25, 0);
	al_convert_mask_to_alpha(pratos->prato0, al_map_rgb(255, 255, 255));
	al_convert_mask_to_alpha(pratos->prato1, al_map_rgb(255, 255, 255));
	al_convert_mask_to_alpha(pratos->prato2, al_map_rgb(255, 255, 255));
	al_convert_mask_to_alpha(pratos->prato3, al_map_rgb(255, 255, 255));
 	inicializa_imagens(alimentos);
	b_prato = criar_botao(prato, 260, 400, 25, 25, 0); //O 0 é a flag do prato!!!
	al_convert_mask_to_alpha(b_prato->imagem, al_map_rgb(255, 255, 255));
	b_prato->ativo = true; //ativa o botão para poder ser printado
	inicializa_Comidas(&comidas, alimentos);
	//inicializaComidas(&comidas, carrot, apple, eggplant, pear, meat);
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
		iniciarTimer();
		//CHECA SE HOUVE DIGITAÇÃO DE TECLAS!!!
		
		while (!al_is_event_queue_empty(fila_eventosT)){
			ALLEGRO_EVENT evento;
			al_wait_for_event(fila_eventosT, &evento);
		
			if (evento.type == ALLEGRO_EVENT_KEY_UP){
				switch (evento.keyboard.keycode){
				case ALLEGRO_KEY_LEFT:
					//mover boneco para a esquerda
					printf("esquerda\n");
					mover_prato(&pratos, 0);
					printf("%d\n", b_prato->pos_x);
					redesenhar(&pratos, &b_prato, comidas_aux, qtd, bgImagem, min, seg, pontos);
					redesenhar_pontuacao(&pts_frutas, pts_proteinas, pts_restritivos, pts_hortalicas, pts_carbo, pts_derivados, qtd_frutas, qtd_proteinas, qtd_restritivos, qtd_hortalicas, qtd_carbo, qtd_derivados);

					tecla = 1;
					break;
				case ALLEGRO_KEY_RIGHT:
					//mover boneco para a direita
					printf("direita\n");
					mover_prato(&pratos, 1);
					printf("%d\n", b_prato->pos_x);
					redesenhar(&pratos, &b_prato, comidas_aux, qtd, bgImagem, min, seg, pontos);
					redesenhar_pontuacao(&pts_frutas, pts_proteinas, pts_restritivos, pts_hortalicas, pts_carbo, pts_derivados, qtd_frutas, qtd_proteinas, qtd_restritivos, qtd_hortalicas, qtd_carbo, qtd_derivados);

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
		pontos = score_atual(qtd_frutas, qtd_proteinas, qtd_restritivos, qtd_hortalicas, qtd_carbo, qtd_derivados);
		if (regra_de_fim(min, pontos) == 1){
			//al_destroy_event_queue(fila_eventos);
			//al_destroy_event_queue(fila_eventosT);
			//al_destroy_bitmap(bgImagem);
			//al_destroy_timer(timer);
			break;
		}
		if (continuar){
			continuar = false;
			mover_comidas(&comidas_aux, qtd);
			redesenhar(&pratos, &b_prato, &comidas_aux, qtd, bgImagem, min, seg, pontos);
			redesenhar_pontuacao(&pts_frutas, pts_proteinas, pts_restritivos, pts_hortalicas, pts_carbo, pts_derivados, qtd_frutas, qtd_proteinas, qtd_restritivos, qtd_hortalicas, qtd_carbo, qtd_derivados);

		}
		//IMPLEMENTAÇÃO DO TIMER
		if (!al_is_event_queue_empty(fila_contador)){
			ALLEGRO_EVENT evento;
			al_wait_for_event(fila_contador, &evento);

			if (evento.type == ALLEGRO_EVENT_TIMER){
				seg++;
				if (tempo_de_print == 2){
					libera_comida(&comidas, &comidas_aux, &qtd);
					tempo_de_print = 0;
					limitado = !limitado;
				}
				else{
					tempo_de_print++;
				}
				if (seg == 60){
					min++;
					seg = 0;
				}
				continuar = true;
			}
			
			
			/*mover_comidas(&comidas_aux, qtd);
			redesenhar(&b_prato, &comidas_aux, qtd, bgImagem, min, seg, contadores);*/
			//mover_comidas(&comidas_aux, qtd);
			if (colisao(pratos, &comidas_aux, qtd, &qtd_frutas, &qtd_proteinas, &qtd_restritivos, &qtd_hortalicas, &qtd_carbo, &qtd_derivados) == 1){
				printf("prato e comida colidiram!\n");

			}
			frame++;

			if (limitado && (obterTempoTimer() < 1.0 / FRAMES_POR_SEGUNDO))
			{
				al_rest((1.0 / FRAMES_POR_SEGUNDO) - obterTempoTimer());
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
	int status;
	TUsuario aux;
	int y = WIDTH / 3 + 50;
	al_clear_to_color(al_map_rgb(255, 178, 28));
	al_draw_text(font, al_map_rgb(0, 0, 0), WIDTH / 2, 100, ALLEGRO_ALIGN_CENTER, "Recordes");
	fseek(arq, 0, 0);
	while (1) {
		status = fread(&aux, sizeof(TUsuario), 1, arq);
		if (status != 1) {
			if (feof(arq)) {
				printf("Fim de arquivo\n");
				break;
			}
			else {
				printf("Erro na leitura\n");
				return;
			}
		}
		else {
			al_draw_textf(font, al_map_rgb(0, 0, 0), WIDTH / 2, y, ALLEGRO_ALIGN_CENTER,
				"Nome:%s ---- Record:%d", aux.nome, aux.recorde);
			al_flip_display();
			y += 50;
		}
	}
	al_rest(3.0);
}
