# Configurazione
CC:= gcc
CFLAGS:=-O2  #flag di compilazione che ottimizza a livello 2

# Sorgenti
SRCS:= main.c processore2.c ram.c
EXES:=$(SRCS:.c=)	#toglie l'estensione (es main.c ===> main)

# Target
all: elaborato

elaborato: $(EXES)
	@echo Compiled $@


clean:
	@echo Cleaning.
	@rm -f $(EXES) #serve solo se faccio "make clean" e mi toglie gli eseguibili, -f (force) serve perchè se non ci sono non mi da errore


%: %.c
	@echo $@
	@$(CC) $(CFLAGS) -o $@ $<

#esegue il target "all" e "clean" lo stesso anche senza controlli di tempo
.PHONY: all clean	
