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

struct page
{
	int *process_id;
    int number;
    int value;
};

struct process
{
    int id;
    struct page page_list[50];
};

int number_of_process = 0;
struct process process_list[THREAD_LIMIT];

struct page main_memory[FRAME_LIMIT];
struct page virtual_memory[VIRTUAL_MEMORY_SIZE];




// Gerenciador de memória
void request_page();
int create_process();
void initialize_page_list(int size, int* process_id, struct page *page_list);

//////////////////////////////////////////////////////
	
int main( int argc, char *argv[ ] ){
	int process1_id = create_process();

	printf("%d \n", process_list[process1_id].page_list[2].number);
	return 0;
}

int create_process()
{
	struct process _process;
	_process.id = number_of_process;
	initialize_page_list(50, &number_of_process, _process.page_list);
	process_list[number_of_process] = _process;
	number_of_process++;

	return _process.id;

}

void initialize_page_list(int size, int* process_id, struct page *page_list)
{
	int i;
	for(i = 0; i < size; i++)
	{
		page_list[i].process_id = process_id;
		page_list[i].number = i;
	}
}