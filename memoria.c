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

struct page
{
	int process_id;
    int number;
    int value;
};

struct process
{
    int id;
    struct page page_list[PAGE_LIMIT];
};

int number_of_process = 0;
struct process process_list[THREAD_LIMIT];

//int number_of_free_frames = FRAME_LIMIT;
struct page main_memory[FRAME_LIMIT];
struct page virtual_memory[VIRTUAL_MEMORY_SIZE];

pthread_t thread[THREAD_LIMIT];

int teste = 0;

// Gerenciador de memória
void reset_main_memory();
int request_page(int process_id);
int create_process();
void* execute_process(int id);
void initialize_page_list_of_process(int size, int process_id);

//////////////////////////////////////////////////////
	
int main( int argc, char *argv[ ] ){
	reset_main_memory();

	int i;
	for(i = 0; i < THREAD_LIMIT; i++){
		pthread_create(&thread[i], NULL, execute_process, create_process());
	}

	for(i = 0; i < THREAD_LIMIT; i++){
		pthread_join(thread[i], NULL);
	}
	return 0;
}

void* execute_process(int id){
	int frame = request_page(id);
	main_memory[frame] = process_list[id].page_list[0];
	print_main_memory();

	frame = request_page(id);
	main_memory[frame] = process_list[id].page_list[3];
	print_main_memory();
	// int i;
	// for(i = 0; i < THREAD_LIMIT; i++)
	// {
	// 	request_page(id);
	// }
}

int create_process(){
	struct process _process;
	_process.id = number_of_process;
	process_list[_process.id] = _process;
	initialize_page_list_of_process(PAGE_LIMIT, number_of_process);
	number_of_process++;

	return _process.id;
}

int request_page(int process_id){
	//srand(time(NULL));
	int i;
	for (i = 0; i < FRAME_LIMIT; i++){
		if(main_memory[i].process_id == -1){
			return i;
		}
	}
	printf("CABOU MEMORIA :(");
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