//Leonardo Neves da Silva DRE110155777
//Luan Cerqueira Martins
//T2 SO 2015.1 ProfValeria

#include	<stdio.h>
#include 	<stdlib.h>
#include 	<time.h>
#include	<sys/timeb.h>


#define WORKSET_LIMIT 4;

// struct process[50] process_list;

// struct page[64] main_memory;
// struct page[200] virtual_memory;

struct page
{
	int process_id;
    int number;
    struct page *next;
};

struct process
{
    int id;
};

// Gerenciador de memória
void request_page();

//////////////////////////////////////////////////////

void create_process();

//////////////////////////////////////////////////////
	
int main( int argc, char *argv[ ] ){

	struct page pagina;
	pagina.process_id = 2;
	pagina.number = 1;
	pagina.next->number = 3;

	printf("%d", pagina.next->number);

	return 0;
}