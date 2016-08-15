 # Indiquer quel compilateur est à utiliser
 CC      ?= gcc
 
 # Spécifier les options du compilateur
 CFLAGS  ?= -g -Wall 
 LDLIBS  ?= -lexif
 
 # Reconnaître les extensions de nom de fichier *.c et *.o comme suffixes
 SUFFIXES ?= .c .o 
 .SUFFIXES: $(SUFFIXES) .
 
 # Nom de l'exécutable
 PROG  = exifrename 
 
 # Liste de fichiers objets nécessaires pour le programme final
 OBJS  = main.o
  
 all: $(PROG)
 
 # Étape de compilation et d'éditions de liens
 # ATTENTION, les lignes suivantes contenant "$(CC)" commencent par un caractère TABULATION et non pas des espaces
 $(PROG): $(OBJS)
	$(CC) $(CFLAGS) $(LDLIBS) -o $(PROG) $(OBJS)
 
 .c.o:
	$(CC) $(CFLAGS) $(LDLIBS) -c $*.c
