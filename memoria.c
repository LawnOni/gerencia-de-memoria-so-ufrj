//Leonardo Neves da Silva DRE110155777
//Luan Cerqueira Martins
//T2 SO 2015.1 ProfValeria

#include	<stdio.h>
#include 	<stdlib.h>
#include 	<time.h>
#include	<sys/timeb.h>


#define WORKSET_LIMIT 4;



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


// struct process[50] process_list;

// struct page[64] main_memory;
// struct page[200] virtual_memory;




// Gerenciador de memória
void request_page();




//////////////////////////////////////////////////////

void create_process();

//////////////////////////////////////////////////////
	
int main( int argc, char *argv[ ] ){

	struct page pagina1;
	pagina1.process_id = 1;
	pagina1.number = 1;

	struct page pagina2;
	pagina2.process_id =2;
	pagina2.number=2; 

	pagina1.next= &pagina2;

	printf("\n%d", pagina1.number);
	printf("\n%d", pagina1.next -> number);
	return 0;
}
