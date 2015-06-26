//Leonardo Neves da Silva DRE110155777
//Luan Cerqueira Martins
//T2 SO 2015.1 ProfValeria

#include	<stdio.h>
#include 	<stdlib.h>
//#include 	<time.h>
#include	<pthread.h>
#include <sched.h>
//#include	<sys/timeb.h>
//#include <sys/wait.h>

#define WORKSET_LIMIT 4
#define FRAME_LIMIT 16
#define VIRTUAL_MEMORY_SIZE 200
#define THREAD_LIMIT 10
#define PAGE_LIMIT 10

//print colorido 
//exemplo  printf(ANSI_COLOR_RED     "This text is RED!"     ANSI_COLOR_RESET "\n");
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

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

int number_of_process = 0;
struct Process process_list[THREAD_LIMIT];
int running_process[THREAD_LIMIT]= { [0 ... THREAD_LIMIT-1 ] = -1 }; 
int running_process_index =0;
int stopped_process[THREAD_LIMIT] = { [0 ... THREAD_LIMIT-1 ] = -1 }; 
int stopped_process_index=0;
//int number_of_free_frames = FRAME_LIMIT;
struct Page main_memory[FRAME_LIMIT];
struct Page virtual_memory[VIRTUAL_MEMORY_SIZE];

pthread_t thread[THREAD_LIMIT];
pthread_mutex_t memory_lock;
pthread_mutex_t process_list_lock;

int page_queue[FRAME_LIMIT];

// Gerenciador de memória
void reset_main_memory();
void request_page(int process_id, int page_number);
int create_process();
void* execute_process(int id);
void initialize_page_list_of_process(int size, int process_id);
void running_process_processes();

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
    if (pthread_mutex_init(&process_list_lock, NULL) != 0) {
        printf("\n mutex init failed\n");
        return 1;
    }

	int i;
	for(i = 0; i < THREAD_LIMIT; i++){
		pthread_create(&thread[i], NULL, execute_process, create_process());
		sleep(2);
	}

	for(i = 0; i < THREAD_LIMIT; i++){
		pthread_join(thread[i], NULL);
	}

	pthread_mutex_destroy(&memory_lock);

	system("clear");
	running_process_processes(); 
	print_main_memory();


	return 0;
}

void* execute_process(int id){
	int i;
	for(i = 0; i < PAGE_LIMIT; i++){
		pthread_mutex_lock(&memory_lock);
		usleep(500000);
		system("clear");
		running_process_processes();
		printf("--->Entrando com PID: %d e Pagina: %d\n", id, i);
		request_page(id, i);
		print_main_memory();
		pthread_mutex_unlock(&memory_lock);
		usleep(0); //Troca de contexto
	}



	pthread_mutex_lock(&memory_lock);
	//system("clear");
	printf("---------------------------->Parando processo com PID: %d\n", id);
	stop_process(id);
	//print_main_memory();
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
	int i;
	for (i = 0; i < FRAME_LIMIT; i++){
		if(main_memory[i].process_id == -1){
			main_memory[i] = process_list[process_id].page_list[page_number];
			add_page_to_queue(PAGE_LIMIT * process_id + main_memory[i].number);
			return;
		}
	}
	printf("------------>MEMORIA CHEIA :( \n");
	
	int pid = page_queue[0]/PAGE_LIMIT;
	int pageN = page_queue[0]%PAGE_LIMIT;
	int frame;

	for (i = 0; i < FRAME_LIMIT; i++){
		if(main_memory[i].process_id == pid && main_memory[i].number == pageN){
			frame = i;
		}
	}

	main_memory[frame] = process_list[process_id].page_list[page_number];
	add_page_to_queue(PAGE_LIMIT * process_id + main_memory[frame].number);
	//FAZER LRU
	return;
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

void running_process_processes(){
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

	printf("\n\n");
}