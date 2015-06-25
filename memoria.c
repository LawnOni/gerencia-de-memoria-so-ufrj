//Leonardo Neves da Silva DRE110155777
//Luan Cerqueira Martins
//T2 SO 2015.1 ProfValeria

#include	<stdio.h>
#include 	<stdlib.h>
//#include 	<time.h>
#include	<pthread.h>
//#include	<sys/timeb.h>
//#include <sys/wait.h>

#define WORKSET_LIMIT 4
#define FRAME_LIMIT 16
#define VIRTUAL_MEMORY_SIZE 200
#define THREAD_LIMIT 10
#define PAGE_LIMIT 5

struct Page
{
	int process_id;
    int number;
    int value;
};

struct Process
{
    int id;
    struct Page page_list[PAGE_LIMIT];
};

int page_queue[FRAME_LIMIT];

int number_of_process = 0;
struct Process process_list[THREAD_LIMIT];

//int number_of_free_frames = FRAME_LIMIT;
struct Page main_memory[FRAME_LIMIT];
struct Page virtual_memory[VIRTUAL_MEMORY_SIZE];

pthread_t thread[THREAD_LIMIT];
pthread_mutex_t memory_lock;

int teste = 0;

// Gerenciador de memória
void reset_main_memory();
void request_page(int process_id, int page_number);
int create_process();
void* execute_process(int id);
void initialize_page_list_of_process(int size, int process_id);

//Queue functions
void add_page_to_queue(int newPage);
void refresh_queue(int page);
void shift_queue(int offSet);
int get_queue_offset(int page);

//////////////////////////////////////////////////////
	
int main( int argc, char *argv[ ] ){
	reset_main_memory();

	//Inicializa mutex
	if (pthread_mutex_init(&memory_lock, NULL) != 0) {
        printf("\n mutex init failed\n");
        return 1;
    }

	int i;
	for(i = 0; i < THREAD_LIMIT; i++){
		pthread_create(&thread[i], NULL, execute_process, create_process());
	}

	for(i = 0; i < THREAD_LIMIT; i++){
		pthread_join(thread[i], NULL);
	}

	pthread_mutex_destroy(&memory_lock);
	return 0;
}

void* execute_process(int id){
	pthread_mutex_lock(&memory_lock);
	request_page(id, 0);
	print_main_memory();
	pthread_mutex_unlock(&memory_lock);


	pthread_mutex_lock(&memory_lock);
	request_page(id, 3);
	print_main_memory();
	pthread_mutex_unlock(&memory_lock);
	// int i;
	// for(i = 0; i < THREAD_LIMIT; i++)
	// {
	// 	request_page(id);
	// }
}

int create_process(){
	struct Process _process;
	_process.id = number_of_process;
	process_list[_process.id] = _process;
	initialize_page_list_of_process(PAGE_LIMIT, number_of_process);
	number_of_process++;

	return _process.id;
}

void request_page(int process_id, int page_number){
	//srand(time(NULL));
	int i;
	for (i = 0; i < FRAME_LIMIT; i++){
		if(main_memory[i].process_id == -1){
			main_memory[i] = process_list[process_id].page_list[page_number];
			add_page_to_queue(PAGE_LIMIT * process_id + main_memory[i].number);
			return;
		}
	}
	printf("------------>CABOU MEMORIA :( \n");
	return;
	//FAZER LRU
}

void print_main_memory()
{
	int i;
	for (i = 0; i < FRAME_LIMIT; i++){
		if(main_memory[i].process_id != -1)
			printf("Frame: %d -> Processo: %d -> Page: %d.\n", i, main_memory[i].process_id, main_memory[i].number);
		else
			printf("Frame: %d Vazio\n", i);
	}
	// printf("Fila:\n");
	// for (i = 0; i < FRAME_LIMIT; i++){
	// 	printf("---> %d: Processo: %d -> Page: %d.\n", i, page_queue[i]/PAGE_LIMIT, page_queue[i]%PAGE_LIMIT);
	// }
}

void initialize_page_list_of_process(int size, int process_id){
	int i;
	for(i = 0; i < size; i++)
	{
		process_list[process_id].page_list[i].process_id = process_id;
		process_list[process_id].page_list[i].number = i;
	}
}

void reset_main_memory(){
	int i;
	for (i = 0; i < FRAME_LIMIT; i++){
		main_memory[i].process_id = -1;
	}
}

void add_page_to_queue(int newPage){
	shift_queue(0);
	page_queue[FRAME_LIMIT] = newPage;
}

void refresh_queue(int page){
	int offSet = get_queue_offset(page);
	shift_queue(offSet);
	page_queue[FRAME_LIMIT] = page;
}

void shift_queue(int offSet){
	int i;
	for (i = offSet; i < FRAME_LIMIT - 1; i++) 
	{   
	    page_queue[i] = page_queue[i+1];
	}
}

int get_queue_offset(int page){
	int i;
	for (i = 0; i < FRAME_LIMIT; i++) 
	{   
	    if(page_queue[i] == page)
	    	return i;
	}
}