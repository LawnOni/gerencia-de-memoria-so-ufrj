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
	int *process_id;
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
int request_page(int process_id);
int create_process();
void* execute_process(int id);
void initialize_page_list(int size, int* process_id, struct page *page_list);

//////////////////////////////////////////////////////
	
int main( int argc, char *argv[ ] ){
	int process1_id = create_process();
	int thread0 = pthread_create(&thread[0], NULL, execute_process, process1_id);
	pthread_join(thread[0], NULL);
	return 0;
}

void* execute_process(int id){
	int frame = request_page(id);
	main_memory[frame] = process_list[id].page_list[0];
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
	initialize_page_list(PAGE_LIMIT, &number_of_process, &_process.page_list);
	process_list[number_of_process] = _process;
	number_of_process++;

	return _process.id;
}

int request_page(int process_id){
	//srand(time(NULL));
	int i;
	for (i = 0; i < FRAME_LIMIT; i++){
		if(main_memory[i].process_id != NULL){
			return i;
		}
	}
	//FAZER LRU
}

void print_main_memory()
{
	int i;
	for (i = 0; i < FRAME_LIMIT; i++){
		if(main_memory[i].process_id != NULL)
			printf("Frame: %d -> Processo: %d -> Page: %d.\n", i, *main_memory[i].process_id, main_memory[i].number);
		else
			printf("Frame: %d Vazio\n", i);
	}
}

void initialize_page_list(int size, int* process_id, struct page *page_list){
	int i;
	for(i = 0; i < size; i++)
	{
		page_list[i].process_id = process_id;
		page_list[i].number = i;
	}
	printf("%d \n", *page_list[0].process_id);
}