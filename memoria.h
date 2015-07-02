

#include	<stdio.h>
#include 	<stdlib.h>
#include	<pthread.h>
#include 	<sched.h>
#include 	<stdbool.h>
//#include 	<time.h>
//#include	<sys/timeb.h>
//#include <sys/wait.h>

//print colorido -exemplo  
//printf(ANSI_COLOR_RED "This text is RED!" ANSI_COLOR_RESET "\n");
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define WORKSET_LIMIT 4*MULTIPLUS
#define FRAME_LIMIT 16*MULTIPLUS
#define VIRTUAL_MEMORY_SIZE 2*FRAME_LIMIT //200
#define THREAD_LIMIT 10*MULTIPLUS
#define PAGE_LIMIT 10*MULTIPLUS


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

int running_process[THREAD_LIMIT] = { [0 ... THREAD_LIMIT-1 ] = -1 }; 
int running_process_index =0;
int stopped_process[THREAD_LIMIT] = { [0 ... THREAD_LIMIT-1 ] = -1 }; 
int stopped_process_index=0;

//LRU 
int recent_frame[FRAME_LIMIT] = { [0 ... FRAME_LIMIT-1 ] = -1 }; 
int number_of_free_frames = FRAME_LIMIT;
int number_of_non_free_frames = 0;


struct Page main_memory[FRAME_LIMIT];
struct Page virtual_memory[VIRTUAL_MEMORY_SIZE];

pthread_t thread[THREAD_LIMIT];
pthread_mutex_t memory_lock;
pthread_mutex_t process_list_lock;

int page_queue[FRAME_LIMIT];

// Gerenciador de memória
void print_memorys();
void reset_main_memory();
void reset_virtual_memory();
void request_page(int process_id, int page_number);
int create_process();
void* execute_process(int id);
void initialize_page_list_of_process(int size, int process_id);
void running_processes();
void stop_process(int process_id);


//Queue functions
void add_page_to_queue(int newPage);
void refresh_queue(int page);
void shift_queue(int offSet);
void print_queue();
void print_queue_details();
int get_queue_offset(int page);
void print_LRUF();

