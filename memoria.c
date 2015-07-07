//Leonardo Neves da Silva DRE110155777
//Luan Cerqueira Martins
//T2 SO 2015.1 ProfValeria

#define MULTIPLUS 1 //multiplicador para alterar facilmente/proporcionalmente o tamanho das threads e memorias 
#define SLEEP_TIME 1000000//500000/2 //3000000

#include "memoria.h"



//ao terminar lembrar de colocar argumentos: como os tamanhos e o sleep time	
int main( int argc, char *argv[ ] ){
	reset_main_memory();
 	reset_virtual_memory();

	//Inicializa mutex
	if (pthread_mutex_init(&memory_lock, NULL) != 0) {
        printf("\n mutex init failed\n");
        return 1;
    }
    if (pthread_mutex_init(&process_list_lock, NULL) != 0) {
        printf("\n mutex init failed\n");
        return 1;
    }

    //inicia threads
	int i;
	for(i = 0; i < THREAD_LIMIT; i++){
		pthread_create(&thread[i], NULL, execute_process, create_process());
		sleep(2);
	}

	for(i = 0; i < THREAD_LIMIT; i++){
		pthread_join(thread[i], NULL);
	}

	pthread_mutex_destroy(&memory_lock);

	//printa situação final
	system("clear");
	print_memories();


	return 0;
}

void* execute_process(int id){
	int i;
	for(i = 0; i < PAGE_LIMIT; i++){
		pthread_mutex_lock(&memory_lock);
		usleep(SLEEP_TIME);
		system("clear");
		printf("--->Entrando com PID: %d e Pagina: %d\n", id, i);
		request_page(id, i);
		print_memories();
		pthread_mutex_unlock(&memory_lock);

		usleep(0); //Troca de contexto
	}

	pthread_mutex_lock(&memory_lock);
	system("clear");
	printf(ANSI_COLOR_RED"\n\t---->Parando processo com PID: %d\n"ANSI_COLOR_RESET, id);
	stop_process(id);
	print_memories();
	pthread_mutex_unlock(&memory_lock);
}

int create_process(){
	int i;
	struct Process _process;
	pthread_mutex_lock(&process_list_lock);
	_process.id = number_of_process;
	process_list[_process.id] = _process;
	number_of_process++;
	running_process[running_process_index] = _process.id;
	running_process_index++;
	for(i=0;i<PAGE_LIMIT;i++) process_list[_process.id].works.frames[i] = -1;
	pthread_mutex_unlock(&process_list_lock);
	initialize_page_list_of_process(PAGE_LIMIT, number_of_process - 1);

	return _process.id;
}

void request_page(int process_id, int page_number){
	int i,j;
	bool faz=true;
	int frame=FRAME_LIMIT-1;//por padrao, em caso de erro, remover o last frame da lista para a virtual
	int freeframes = free_frames();

	if( workingset_is_full(process_id) ){
		printf("... O working set do processo %i esta cheio\n",process_id);
		frame=insert_pag_full_workingset(process_id, page_number);
	}
	//verifica se ha frames vazios
	else if ( freeframes> 0){
		printf("... Ainda existem frames vazios\n");
		frame=insert_pag_empty_frames(process_id, page_number);
	}

	else{
		printf("... A memoria esta cheia\n");
		frame=insert_pag_full_memory(process_id, page_number);
	}

	//adiciona o frame da nova pagina ao workingsetlimit	2_metodos
	//process_list[ main_memory[frame].process_id ].works.frames[ main_memory[frame].number ]=frame;
	process_list[process_id].works.frames[page_number]=frame;

	//insere na memoria principal
	main_memory[frame] = process_list[process_id].page_list[page_number];
	add_page_to_queue(PAGE_LIMIT * process_id + main_memory[frame].number);
}

void print_memories(){
	running_processes(); 

	int i;
	number_of_non_free_frames = 0;
	number_of_free_frames = 0;

	printf("\tMEMORIA PRINCIPAL\t\t\t\t\t\t\tMEMORIA VIRTUAL\n");
	printf("______________________________________\t\t_______________________________________________________________________________________\n");

	for (i = 0; i < FRAME_LIMIT; i++){
		//main 0-15
		if(main_memory[i].process_id > -1){
			printf("Frame: %2d -> Processo: %2d -> Page: %2d.\t\t", i, main_memory[i].process_id, main_memory[i].number);
			number_of_non_free_frames++;
		}
		else{
			printf("Frame: %2d Vazio\t\t\t\t\t", i);
			number_of_free_frames++;
		}
		
		//0-15
		if(virtual_memory[i].process_id > -1)  printf("Frame: %2d -> Processo: %2d -> Page: %2d.\t\t", i, virtual_memory[i].process_id, virtual_memory[i].number);
		else  printf("Frame: %2d Vazio\t\t\t\t\t", i);

		//32-47
		if (2*FRAME_LIMIT+i>=VIRTUAL_MEMORY_SIZE)	printf("--");
		else if(virtual_memory[2*FRAME_LIMIT+i].process_id > -1)  printf("Frame: %2d -> Processo: %2d -> Page: %2d.", 2*FRAME_LIMIT+i, virtual_memory[2*FRAME_LIMIT+i].process_id, virtual_memory[2*FRAME_LIMIT+i].number);
		else printf("Frame: %2d Vazio", 2*FRAME_LIMIT+i);
		printf("\n");

	}

	for (i = 0; i < FRAME_LIMIT; i++){
		//slots cheio vs vazios
		if (i== 4) printf(ANSI_BG_GREEN"LIVRES: %2i\t\t\t\t\t"ANSI_COLOR_RESET,number_of_free_frames); 
		else if (i== 5) printf(ANSI_BG_RED"CHEIOS: %2i\t\t\t\t\t"ANSI_COLOR_RESET, number_of_non_free_frames);
		else printf("\t\t\t\t\t\t");
		
		//16-31
		if (FRAME_LIMIT+i>=VIRTUAL_MEMORY_SIZE)	printf("--\t\t\t\t\t\t");
		else if(virtual_memory[FRAME_LIMIT+i].process_id > -1)  printf("Frame: %2d -> Processo: %2d -> Page: %2d.\t\t", FRAME_LIMIT+i, virtual_memory[FRAME_LIMIT+i].process_id, virtual_memory[FRAME_LIMIT+i].number);
		else printf("Frame: %2d Vazio\t\t\t\t\t", FRAME_LIMIT+i);
		
		//48-63
		if (3*FRAME_LIMIT+i>=VIRTUAL_MEMORY_SIZE)	printf("--");
		else if(virtual_memory[3*FRAME_LIMIT+i].process_id > -1)  printf("Frame: %2d -> Processo: %2d -> Page: %2d.", 3*FRAME_LIMIT+i, virtual_memory[3*FRAME_LIMIT+i].process_id, virtual_memory[3*FRAME_LIMIT+i].number);
		else printf("Frame: %2d Vazio", 3*FRAME_LIMIT+i);

		printf("\n");

	}

	for (i = 4*FRAME_LIMIT; i < VIRTUAL_MEMORY_SIZE; i++){
		printf("\t\t\t\t\t\t\t\t\t\t\t\t");
		if(virtual_memory[i].process_id > -1)  printf("Frame: %2d -> Processo: %2d -> Page: %2d.", i, virtual_memory[i].process_id, virtual_memory[i].number);
		else  printf("Frame: %2d Vazio", i);
		printf("\n");

	}

	if ( (number_of_non_free_frames + number_of_free_frames) != FRAME_LIMIT) { printf("Erro em  qtdade frames"); exit(0);}
	print_LRUF();

	//// **verificar 
	//print_queue_details();


	printf("___________________________________Numero da Pagina______");
	for (i = 0; i < PAGE_LIMIT; i++)	printf("_%2i",i);
	printf("_\n");
	for(i=0;i<THREAD_LIMIT;i++) print_workingset(i);	
}

void initialize_page_list_of_process(int size, int process_id){
	int i;
	for(i = 0; i < size; i++)
	{
		process_list[process_id].page_list[i].process_id = process_id;
		process_list[process_id].page_list[i].number = i;
	}
}

void stop_process(int process_id){
	int i;
	stopped_process[stopped_process_index] = process_id;
	stopped_process_index++;
	for (i=0; i<THREAD_LIMIT;i++) if (running_process[i] == process_id) running_process[i] = -1;

	for (i = 0; i < FRAME_LIMIT; i++){
		if(main_memory[i].process_id == process_id){
			main_memory[i].process_id = -1;
		}
	}

	for (i = 0; i < PAGE_LIMIT; i++) process_list[process_id].works.frames[i]=-1;
}

void reset_main_memory(){
	int i;
	for (i = 0; i < FRAME_LIMIT; i++){
		main_memory[i].process_id = -1;
	}
}

void reset_virtual_memory(){
	int i;
	for (i = 0; i < VIRTUAL_MEMORY_SIZE; i++){
		virtual_memory[i].process_id = -1;
	}
}

void add_page_to_queue(int newPage){
	shift_queue(0);
	page_queue[FRAME_LIMIT - 1] = newPage;
}

void refresh_queue(int page){
	int offSet = get_queue_offset(page);
	shift_queue(offSet);
	page_queue[FRAME_LIMIT - 1] = page;
}

void shift_queue(int offSet){
	int i;
	for (i = offSet; i < FRAME_LIMIT - 1; i++) page_queue[i] = page_queue[i+1];	
}

int get_queue_offset(int page){
	int i;
	for (i = 0; i < FRAME_LIMIT; i++) 
	{   
	    if(page_queue[i] == page)
	    	return i;
	}
}

void running_processes(){
	int i;
	printf(ANSI_COLOR_YELLOW "Estamos executando os processos: \n"ANSI_COLOR_RESET);
	for (i=0; i<THREAD_LIMIT;i++)
		if (running_process[i] > -1) 
			printf(ANSI_COLOR_YELLOW "%i "ANSI_COLOR_RESET,running_process[i]);
	printf("\n");
	printf(ANSI_COLOR_GREEN "Os seguintes processos terminaram: \n" ANSI_COLOR_RESET);
	for (i=0; i<THREAD_LIMIT;i++)
		if (stopped_process[i] >-1) 
			printf(ANSI_COLOR_GREEN "%i " ANSI_COLOR_RESET,stopped_process[i]);

	printf("\n");
}

void print_queue(){
	int i;
	printf(ANSI_COLOR_CYAN"\nFila:  {process,page}\n");
	for(i=0;i<FRAME_LIMIT;i++) 	printf("[%d,%d] \v", page_queue[i]/PAGE_LIMIT, page_queue[i]%PAGE_LIMIT);
	printf("\n"ANSI_COLOR_RESET);
}

void print_queue_details(){
	int i;

		for (i = 0; i < FRAME_LIMIT; i++){
			if(i == 0) 
				printf("Fila:\tSai ----> %d: Processo: %d -> Page: %d.\n", i, page_queue[i]/PAGE_LIMIT, page_queue[i]%PAGE_LIMIT);
			else if(i == FRAME_LIMIT -1)
				printf("    |\tEntra --> %d: Processo: %d -> Page: %d.\n", i, page_queue[i]/PAGE_LIMIT, page_queue[i]%PAGE_LIMIT);
			else
				printf("    |\t--------> %d: Processo: %d -> Page: %d.\n", i, page_queue[i]/PAGE_LIMIT, page_queue[i]%PAGE_LIMIT);
		}
}

void print_LRUF(){
	int i;
	printf(ANSI_BOLD_ON"LRUF - Last Recent Used Frames: [new .. old]"ANSI_COLOR_RESET);
	printf("\nRecente ->" );
	for(i=0;i<FRAME_LIMIT-1;i++) printf("  %i", recent_frame[i]);
	printf("  "ANSI_INVERSE_ON "%i" ANSI_COLOR_RESET, recent_frame[FRAME_LIMIT-1]);
	printf(" -> Removido \n");	
}

void print_workingset(int process_id){
	int i;
	printf("Paginas do processo %i esta alocado nos seguintes frames: ",process_id);
	for (i = 0; i < PAGE_LIMIT; i++) if (process_list[process_id].works.frames[i]==-1) printf(" %2i",process_list[process_id].works.frames[i]);
									else printf(ANSI_INVERSE_ON" %2i"ANSI_COLOR_RESET,process_list[process_id].works.frames[i]);
	printf("\n");
}

bool using_all_working_set(int process_id){
	int i;
	for (i = 0; i < PAGE_LIMIT; i++) if (process_list[process_id].works.frames[i]==-1){
		return false;
	}
	return true;
}

int insert_pag_empty_frames(int process_id, int page_number){
	int i,j;
	bool faz=true;
	int frame=FRAME_LIMIT-1;//por padrao, em caso de erro, remover o last frame da lista para a virtual

	for (i = 0; i < FRAME_LIMIT; i++){
		if(main_memory[i].process_id == -1){
			frame=i; //atribui a variavel frame o valor do frame vazio

			//atualiza a lista de frames recentemente utilizados
			//evita que na lista de frames ocorra o mesmo frame duas vezes
			//por exemplo, ao atualizar o frame 2 a LRU 23210 esta errada, o certo é 2310
			for (i = 0; i<FRAME_LIMIT; i++) {
				if(recent_frame[i]==frame){
					for (j = i; j > 0; j--)	recent_frame[j]=recent_frame[j-1]; //remove copias da lista
					recent_frame[0]=frame;//insere o atual
					faz=false;	//marca que ja foi feita a atualizacao e a insercao
				}
			}
			//caso nao tenha nenhum outro igual na lista fazer a insercao na fila e o refresh
			if (faz){
				for (i = FRAME_LIMIT-1; i > 0; i--) { 
					recent_frame[i] = recent_frame[i-1];
				}
				recent_frame[0] = frame;
			 }
			break;
		}
	}
	return frame;
}

int insert_pag_full_memory(int process_id, int page_number){
	int i,j;
	//bool faz=true;
	int frame=FRAME_LIMIT-1;//por padrao, em caso de erro, remover o last frame da lista para a virtual
	int recent = recent_frame[FRAME_LIMIT-1];
	int last = FRAME_LIMIT-1; //ultimo da fila, ira para o inicio da fila, será primeiro

	//atualiza processos na virtual
	if (virtual_memory[0].process_id != -1) {
		for (i = VIRTUAL_MEMORY_SIZE-1; i > 0; i--) virtual_memory[i] = virtual_memory[i-1]; 
		
	}
	
	//movimenta o LRUF
	if(recent_frame[last] != -1) { 

		for (i = 0; i<FRAME_LIMIT; i++) {

				if(recent_frame[i]==recent_frame[last]){ //atualiza o LRU, e remove copias da fila antes de inserir
					for (j = i; j > 0; j--) { 
						recent_frame[j]=recent_frame[j-1];
					}
					recent_frame[0]=recent; //o ultimo vira o primeiro
				}
			}
		//COPIA PARA A MEMORIA VIRTUAL O FRAME Q SAIRA
		virtual_memory[0] = main_memory[ recent_frame[last] ];
	}	
	
	//atualiza o valor do frame a ser retirado da memoria principal
	frame=recent_frame[last];

	//remove a pagina do workingset
	//Frame: 		frame
	//Processo: 	main_memory[frame].process_id
	//Page: 		main_memory[frame].number;
	process_list[main_memory[frame].process_id].works.frames[main_memory[frame].number]=-1;

	return frame;
}

int insert_pag_full_workingset(int process_id, int page_number){
	int i,j;
	int remover = FRAME_LIMIT-1;//POR PADRAO COLOCA NA ULTIMA POSICAO 
	int recent = -1;
	int index = -1;
	int randompage = rand()%PAGE_LIMIT; //sorteia uma pagina,
	srand(time(NULL));


	//sorteia dentre o workingset qual pagina/frame saira da memoria
	for (i = 0; i < PAGE_LIMIT; i++) if (process_list[process_id].works.frames[i] != -1) {
		remover = process_list[process_id].works.frames[i];
		//se encontrarmos uma pagina alocada antes da sorteada continuamos procurando, e encontrarmos depois usamos aquele frame para substituicao
		if (i > randompage) break; 
	}
	printf("I=%i > RAND=%i \nremoverFrame %i\n", i,randompage,remover);


	//atualiza processos na virtual
	if (virtual_memory[0].process_id != -1) {
		for (i = VIRTUAL_MEMORY_SIZE-1; i > 0; i--) virtual_memory[i] = virtual_memory[i-1]; 
	}

	for (i = 0; i<FRAME_LIMIT; i++) {
		if ( recent_frame[i] == remover ){
			recent = recent_frame[i];
			index=i;
			break ;
		}
	}

	if (recent == -1 || index ==-1)
	{
		printf("Erro**\n%i %i", recent, index);
		exit(-1);
	}


/*
	//movimenta o LRUF
	if(recent_frame[FRAME_LIMIT-1] != -1) { 

		for (i = 0; i<FRAME_LIMIT; i++) {

				if(recent_frame[i]==recent_frame[last]){ //atualiza o LRU, e remove copias da fila antes de inserir
					for (j = i; j > 0; j--) { 
						recent_frame[j]=recent_frame[j-1];
					}
					recent_frame[0]=recent; //o ultimo vira o primeiro
				}
			}
		
	}	*/
	
	//COPIA PARA A MEMORIA VIRTUAL O FRAME Q SAIRA
	virtual_memory[0] = main_memory[ remover ];


	//atualiza o valor do frame a ser retirado da memoria principal
	//frame=remover;

	//remove a pagina do workingset
	//Frame: 		frame
	//Processo: 	main_memory[frame].process_id
	//Page: 		main_memory[frame].number;
	process_list[ main_memory[remover].process_id ].works.frames[main_memory[remover].number]=-1;

	return remover;
}

bool workingset_is_full(int process_id){
	int i,workingset =0;

	for (i = 0; i < PAGE_LIMIT; i++) if (process_list[process_id].works.frames[i] != -1) workingset++;

	if (workingset == WORKSET_LIMIT) return true;
	else if (workingset < WORKSET_LIMIT) return false;
	else if (workingset > WORKSET_LIMIT) { 
		printf(ANSI_BG_RED ":::: WORKSET_LIMIT Estourado pelo processo %i ::::\n", process_id); 
		
		// printf("Remover FRAME %i \n", insert_pag_full_workingset(process_id, NULL));
		// print_memories();
		exit(-1);
	}
}

int free_frames(){
	int i;
	number_of_non_free_frames = 0;
	number_of_free_frames = 0;

	for (i = 0; i < FRAME_LIMIT; i++){
		if(main_memory[i].process_id > -1) number_of_non_free_frames++;
		else number_of_free_frames++;
	}

	if ( (number_of_non_free_frames + number_of_free_frames) != FRAME_LIMIT) { 
		printf("f=%i nf=%i\nErro2 em  qtdade frames\n",number_of_non_free_frames , number_of_free_frames); 
		exit(0);
	}
	return number_of_free_frames;
}		