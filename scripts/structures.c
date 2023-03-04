#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

 
////////////////////////////////////////
// STRUCTURES POUR ENVOI INFORMATIONS //
//////////////////////////////////////// 

            /******************************/
            /*********** COMMUN ***********/
            /******************************/


/// Structure qui permet de dire pour chaque voisin combien il a de voisin en totalité et cb de voisin il va devoir envoyer une demande de connection
struct nbVois{
    int nbVoisinTotal;
    int nbVoisinDemande;		//nb de voisin a qui tu doit demander une connexion
    int nbNoeuds;
};


/// Structure des inforamtions avec la requete les inforamtions que l'on a besoin selon la requete et l'adresse du processus 
struct infos_Graphe {
    int numero;                             //numero du noeud courant sur le graphe
	int ordre;    							//ordre du noeud courant
	int descripteur;                        //descripteur du noeud
    struct sockaddr_in adrProc;             //adresse du processus dont on parle
};

            /*******************************/
            /*********** SERVEUR ***********/
            /*******************************/

//CREATION D'UNE STRUCTURE DES ARETES POUR LE SERVEUR
struct aretes{
  	int noeud1;
  	int noeud2;
};

//CREATION STRUCTURE POUR LES NOMBRE IMPORTANTS
struct info_nb{
  	int nb_sommets;
  	int nb_aretes;
};

//STRUCTURE POUR STOCKER LES DEGRES DES SOMMETS
struct degres {
  int noeud;
  int degre;
};


            /*****************************/
            /*********** NOEUD ***********/
            /*****************************/

//THREAD structure pour regrouper les paramétres d'un thread
struct paramsNoeud {
  	int idThread;                         	//un identifiant de thread, de 1 à N (N le nombre total de theads secondaires)
	int numero_proc;						//numero dans le graphe du processus courant
	int indice_vois;						//indice du processus voisin parmis les voisins du noeud courant
	int Acc;                              	//boolean pour savoir si je suis une acceptation ou non 
  	struct infos_Graphe *VoisinCourant;   	//structure des informations du voisins
};
 
//THREAD structure pour regrouper les paramétres du thread pour la coloration
struct paramsColoration {
	int numero;								//numero dans le noeud du processus courant
	int ordre;								//ordre du processus courant
	int nbVoisins;							//nombre de voisins du processus courant
	struct couleurVoisin* couleurVoisins;	//tableau des couleurs
  	struct infos_Graphe* VoisinsCourant;   	//structure des informations du voisins
};



//Structure pour l'envoie et la reception d'un message
struct messages {
	int requete;						//type du message : {"COULEUR"}
	int ordreI;							//numero du processus I
	int message;						//informations du message ici la couleur du processus I
}; 


//Structure pour regrouper la couleur et l'ordre d'un noeud
struct couleurVoisin {
    int couleur;
    int ordre;
};
