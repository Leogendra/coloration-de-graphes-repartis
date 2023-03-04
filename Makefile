########################################
#~ définitions
########################################

BIN=bin/noeuds  bin/serveur

SRCS1=scripts/noeuds.c
SRCS2=scripts/serveur.c

default: obj/scripts $(BIN)

########################################
#~ regles pour l'executable
########################################

obj/scripts:
	mkdir -p obj/scripts

obj/%.o: %.c
	gcc -Wall -Iinclude -c $< -o $@

bin/noeuds: $(SRCS1:%.c=obj/%.o)
	gcc -o $@ $+ -lpthread -lm

bin/serveur: $(SRCS2:%.c=obj/%.o)
	gcc -o $@ $+ -lpthread -lm

clean:
	rm -f $(BIN) obj/*.o *~