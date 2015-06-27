all: 
	@echo ____________________________________________________________________Cria
	clear all
	@echo ____________________________________________________________________compila
	@gcc memoria.c -o memoria -lpthread 
	ls
	@echo ____________________________________________________________________Executa
	./memoria

	

clean: 
	rm a.out memoria

