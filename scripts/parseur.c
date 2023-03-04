#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structures.c"

#define TAILLE_MAX 1000			//taile de la ligne au maximum


/////////////////////////////
// FONCTION nbAreteNbNoeud //
/////////////////////////////
/// FONCTION QUI RECUPERE LA STRUCTURE DES NOMBRES D'ARRETES ET DE SOMMETS
/// nom_fichier : nom du fichier qu'on veut ouvrir
/// return : structure des informations des nombres de sommets et de d'aretes
struct info_nb nbAreteNbNoeud(char * nom_fichier){

  	FILE * fichier;					//declaration du fichier
  
  	// OUVERTURE EN LECTURE
  	fichier = fopen(nom_fichier, "r");																		//ouverture en lecture	

		//GESTION DES ERREURS
  		if (fichier == NULL) {										
		    printf("\n[ERREUR] Impossible d'ouvrir le fichier '%s'\n", nom_fichier);
		    exit(2);		//si on ne lit pas le fichier on doit arreter parce u'on ne peux rien faire
		  }

  	//DONNEES
  	char ligne[TAILLE_MAX];			//ligne de recuperation de la ligne
  	int nb_sommets = 0;				//nombre de sommets qu'on va recuperer
  	int nb_aretes = 0;				//nombre d'aretes qu'on va recuperer
  	struct info_nb info_nb;			//structure qui va nous servir à stocker les nombre d'aretes et de sommets

  	//LECTURE
	int i = 0;			//on definie un indice qui va etre 
	while (fgets(ligne, TAILLE_MAX, fichier) != NULL) {
    	// Si la ligne commence par "c" c'est un commentaire on l'ignore
	    if (ligne[0] == 'c') {
	      continue;
	    }
	    // Si la ligne commence par "p" c'est la ligne qui contient le nombre de sommets et de aretes
	    if (ligne[0] == 'p') {
	     	char *tmp = strtok(ligne, " ");			//on stocke la ligne dans un tableau de char
	      	while (tmp != NULL) {					//tant que la ligne n'est pas vide
	        	if (i == 2) {						//on regarde si on est au bon endroit
	          		nb_sommets = atoi(tmp);			//pour lire le nombre de sommets
	        	}
	        	if (i == 3) {
	          		nb_aretes = atoi(tmp);				//sinon le nombre d'aretes
	        	}
	        	tmp = strtok(NULL, " ");				//au passe a la case suivante apres l'espace et on prend met a NULL la case actuel
	        	i++;									//on incremente i pour passer a la case suivante en terme d'indice
	      	}											//le tmp est vide alors on va passer a la ligne suivante
	      	continue;									//et on continue
	    }
	}

  fclose(fichier);        //Fermeture du fichier

  //données a mettre dans le tableau
  info_nb.nb_sommets = nb_sommets;
  info_nb.nb_aretes = nb_aretes;

  return info_nb;
  
}



/////////////////////
// FONCTION Aretes //
/////////////////////
/// FONCTION QUI RECUPERE LES ARETES
/// nom_fichier : nom du fichier qu'on veut ouvrir
/// aretes : tableau de structure d'aretes qui repertorie les noeuds qui sont liés (noeud1 et noeud2)
void Aretes (char * nom_fichier, struct aretes * aretes) {

  	FILE * fichier;
	  
  	// OUVERTURE EN LECTURE
  	fichier = fopen(nom_fichier, "r");

		//GESTION DES ERREURS
		if (fichier == NULL) {
			printf("\n[ERREUR] Impossible d'ouvrir le fichier '%s'\n", nom_fichier);
			exit(2);
	  	}

  	//DONNEES
  	char ligne[TAILLE_MAX];
  
	  
	int i=0;
  	while (fgets(ligne, TAILLE_MAX, fichier) != NULL) {
    	// Si la ligne commence par "c" c'est un commentaire on l'ignore, si "p" on continue car on la traité avant
	    if (ligne[0] == 'c' || ligne[0] == 'p') {
	      	continue;
	    }
	
	    // Si la ligne commence par "e" on récupère les aretes
	    if (ligne[0] == 'e') {
	      	char *tmp = strtok(ligne, " ");     	//recuperation de la ligne
		    int j = 0;                          	//indice pour savoir au quel mot on est
			while (tmp != NULL) {              		//tant que la ligne n'est pas fini
				if (j == 1) {                    	//si je suis au deuxieme mot 
				  	aretes[i].noeud1 = atoi(tmp);  	//donne les info du noeuds 1
				}
				if (j == 2) {                    	//si je suis au deuxieme mot
				  	aretes[i].noeud2 = atoi(tmp);  	//je recupere le noeud2
				}
				tmp = strtok(NULL, " ");          	//rend null le mot courant
				j++;                            	//on incremente pour passer au mot suivant
			}
			i++;                              	//indice pour le tableau des aretes
			continue;
    	}
	}	//fin de lecture

}