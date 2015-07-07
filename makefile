all: 
	@echo ____________________________________________________________________Cria
	clear all
	@echo ____________________________________________________________________compila
	make gcc
	ls
	@echo ____________________________________________________________________Executa
	./memoria.out

	

clean:
	clear all 
	rm a.out memoria memoria.out

gcc:	
	@gcc memoria.c -o memoria.out -lpthread -w

guake_sublime:
	@gcc memoria.c -o memoria.out -lpthread	
	guake	 
	guake -e ./memoria.out