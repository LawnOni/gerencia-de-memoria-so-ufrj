//Leonardo Neves da Silva DRE110155777
//Luan Cerqueira Martins
//T2 SO 2015.1 ProfValeria

#define MULTIPLUS 1 //multiplicador para alterar facilmente/proporcionalmente o tamanho das threads e memorias 
#define SLEEP_TIME 500000 

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
	print_memorys();


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
		print_memorys();
		pthread_mutex_unlock(&memory_lock);

		usleep(0); //Troca de contexto
	}



	pthread_mutex_lock(&memory_lock);
	system("clear");
	printf(ANSI_COLOR_RED"\n\t---->Parando processo com PID: %d\n"ANSI_COLOR_RESET, id);
	stop_process(id);
	print_memorys();
	pthread_mutex_unlock(&memory_lock);
}

int create_process(){
	struct Process _process;
	pthread_mutex_lock(&process_list_lock);
	_process.id = number_of_process;
	process_list[_process.id] = _process;
	number_of_process++;
	running_process[running_process_index] = _process.id;
	running_process_index++;
	pthread_mutex_unlock(&process_list_lock);
	initialize_page_list_of_process(PAGE_LIMIT, number_of_process - 1);

	return _process.id;
}

void request_page(int process_id, int page_number){
	int i,j;
	bool faz=true;
	int frame=FRAME_LIMIT-1;//por padrao, em caso de erro, remover o ultimo frame da lista para a virtual
	

	//verifica se ha frames vazios
	if (number_of_free_frames > 0){
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

		// //insere na memoria principal
		// main_memory[frame] = process_list[process_id].page_list[page_number];
		// add_page_to_queue(PAGE_LIMIT * process_id + main_memory[frame].number);
	}

	else{
		//atualiza processos na virtual
		if (virtual_memory[0].process_id != -1) {
			for (i = VIRTUAL_MEMORY_SIZE-1; i > 0; i--) virtual_memory[i] = virtual_memory[i-1]; 
			
		}

		if(recent_frame[FRAME_LIMIT-1] != -1) { 

			for (i = 0; i<FRAME_LIMIT; i++) {
					if(recent_frame[i]==recent_frame[FRAME_LIMIT-1]){ //atualiza o LRU, e remove copias da fila antes de inserir
						for (j = i; j > 0; j--) { 
							recent_frame[j]=recent_frame[j-1];
						}
						recent_frame[0]=recent_frame[FRAME_LIMIT-1]; //roda, o ultimo vira o primeiro
					}
				}
			//COPIA PARA A MEMORIA VIRTUAL O FRAME Q SAIRA
			virtual_memory[0] = main_memory[ recent_frame[FRAME_LIMIT-1] ];
		}

		//atualiza o valor do frame a ser retirado da memoria principal
		frame=recent_frame[FRAME_LIMIT-1];
	}
	
	//insere na memoria principal
	main_memory[frame] = process_list[process_id].page_list[page_number];
	add_page_to_queue(PAGE_LIMIT * process_id + main_memory[frame].number);
 }

void print_memorys(){
	running_processes(); 

	int i;
	number_of_non_free_frames = 0;
	number_of_free_frames = 0;

	printf("\nMEMORIA PRINCIPAL\t\t\t\tMEMORIA VIRTUAL\n");
	printf("_________________\t\t\t\t_______________\n");

	for (i = 0; i < FRAME_LIMIT; i++){
		if(main_memory[i].process_id > -1){
			printf("Frame: %d -> Processo: %d -> Page: %d.\t\t", i, main_memory[i].process_id, main_memory[i].number);

			number_of_non_free_frames++;
		}
		else{
			printf("Frame: %d Vazio\t\t\t\t\t", i);
			number_of_free_frames++;
		}

		if(virtual_memory[i].process_id > -1){
			printf("Frame: %d -> Processo: %d -> Page: %d.\n", i, virtual_memory[i].process_id, virtual_memory[i].number);
		}
		else{
			printf("Frame: %d Vazio\n", i);
		}

	}

	for (i = FRAME_LIMIT+1; i < VIRTUAL_MEMORY_SIZE; i++){
		
		if (i== FRAME_LIMIT+4) printf(ANSI_COLOR_RED "LIVRES: %2i\t\t\t\t\t"ANSI_COLOR_RESET,number_of_free_frames); 
		else if (i== FRAME_LIMIT+5) printf(ANSI_COLOR_RED"CHEIOS: %2i\t\t\t\t\t"ANSI_COLOR_RESET, number_of_non_free_frames);
		else 	printf("\t\t\t\t\t\t");
	
		if(virtual_memory[i].process_id > -1){
			printf("Frame: %d -> Processo: %d -> Page: %d.\n", i, virtual_memory[i].process_id, virtual_memory[i].number);
		}
		else{
			printf("Frame: %d Vazio\n", i);
		}

	}

	print_LRUF();

	if ( (number_of_non_free_frames + number_of_free_frames) != FRAME_LIMIT) { printf("Erro em  qtdade frames"); exit(0);}

	print_queue_details();	
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
	printf("Fila:\n");
		for (i = 0; i < FRAME_LIMIT; i++){
			if(i == 0) 
				printf("Sai ----> %d: Processo: %d -> Page: %d.\n", i, page_queue[i]/PAGE_LIMIT, page_queue[i]%PAGE_LIMIT);
			else if(i == FRAME_LIMIT -1)
				printf("Entra --> %d: Processo: %d -> Page: %d.\n", i, page_queue[i]/PAGE_LIMIT, page_queue[i]%PAGE_LIMIT);
			else
				printf("--------> %d: Processo: %d -> Page: %d.\n", i, page_queue[i]/PAGE_LIMIT, page_queue[i]%PAGE_LIMIT);
		}
}

void print_LRUF(){
	int i;
	printf("LRUF - Last Recent Used Frames: [new .. old]\n");
	for(i=0;i<FRAME_LIMIT;i++) printf("  %i", recent_frame[i]);
	printf("\n");	
}
