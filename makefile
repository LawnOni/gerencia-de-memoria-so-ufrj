all: 
	@echo ____________________________________________________________________Cria
	clear all
	@echo ____________________________________________________________________compila
	make gcc
	ls
	@echo ____________________________________________________________________Executa
	./memoria

	

clean:
	clear all 
	rm a.out memoria

gcc:	
	@gcc memoria.c -o memoria -lpthread

guake_sublime:
	@gcc memoria.c -o memoria -lpthread	
	guake	 
	guake -e ./memoria

