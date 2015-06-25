//Leonardo Neves da Silva DRE110155777
//Luan Cerqueira Martins
//T2 SO 2015.1 ProfValeria

#include	<stdio.h>
#include 	<stdlib.h>
#include 	<time.h>
#include	<sys/timeb.h>

#define WORKSET_LIMIT 4
#define FRAME_LIMIT 64
#define VIRTUAL_MEMORY_SIZE 200
#define THREAD_LIMIT 20
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

int number_of_free_frames = FRAME_LIMIT;
struct page main_memory[FRAME_LIMIT];
struct page virtual_memory[VIRTUAL_MEMORY_SIZE];




// Gerenciador de memória
void request_page(int process_id);
int create_process();
void initialize_page_list(int size, int* process_id, struct page *page_list);

//////////////////////////////////////////////////////
	
int main( int argc, char *argv[ ] ){
	int process1_id = create_process();

	printf("%d \n", process_list[process1_id].page_list[3].number);
	return 0;
}

int create_process(){
	struct process _process;
	_process.id = number_of_process;
	initialize_page_list(PAGE_LIMIT, &number_of_process, _process.page_list);
	process_list[number_of_process] = _process;
	number_of_process++;

	return _process.id;
}

void request_page(int process_id){
	srand(time(NULL));
	int page_number = rand() * FRAME_LIMIT;
	while(main_memory[page_number].process_id != NULL){
		page_number = rand() * FRAME_LIMIT;
		number_of_free_frames--;
	}


}

void initialize_page_list(int size, int* process_id, struct page *page_list){
	int i;
	for(i = 0; i < size; i++)
	{
		page_list[i].process_id = process_id;
		page_list[i].number = i;
	}
}