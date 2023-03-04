#include "fonctions.c"
#include <math.h>


void* Coloration(void* p){
	//recuperation des arguments du thread
    struct paramsColoration* args = (struct paramsColoration*) p;

	int numeroMoi = args->numero;					                  //indice pour le thread
	int ordre = args->ordre;							              //ordre du noeud courant
	int nbVoisins = args->nbVoisins;					              //ordre du noeud courant
	struct couleurVoisin* tableauCouleursVoisins = args->couleurVoisins;	  //tableau des couleurs des voisins
    struct infos_Graphe *Voisins = args->VoisinsCourant;              //tableau de structure des informations des voisins
	
		int couleur = 1;

	int i = 0;
	while (i < nbVoisins) {						                    //parcourt du tableau des voisins
		if (tableauCouleursVoisins[i].couleur == couleur) {		            //si c'est la meme couleurs on change de couleur
			couleur++;							                    //incrementation
			i = 0;								                    //on passe i a 0 car fini pour cette couleur
		}
		else {i++;}							
	}

    printColorPlus(numeroMoi, "COLORE");printf("ordre %d, avec la couleur %d\n", ordre, couleur);

	//ENVOI A TOUS MES VOISINS MA COULEUR 

	//préparation à l'envoi du message
	struct messages message;
	message.requete = COULEUR;		//un de mes voisins s'est colorié
	message.ordreI = ordre;
	message.message = couleur;
		
	for (int i = 0; i < nbVoisins; i++) {
				//J'envoie à mes voisins <COULEUR, ordre_i, couleur_i> 
		int dSVoisin = Voisins[i].descripteur;
				int s = sendCompletTCP(dSVoisin, &message, sizeof(struct messages));

		    //GESTION DES ERREURS
			if (s == ERREUR) {
				printf("Je vais avoir une erreur sur l'envoie de ma couleur au noeud %d\n", message.ordreI);
				perror("\n[ERREUR] : Erreur lors de l'envoie du message ");
				pthread_exit(NULL);
				close(dSVoisin);
				exit(1);
			}
			else if (s == FERMETURE) {
				printf("Je vais avoir mon ami qui s'en va sans moi au noeud %d\n", message.ordreI);
				perror("\n[ERREUR] : Abandon de la socket principale dans le l'envoie");
				pthread_exit(NULL);
				close(dSVoisin);
				exit(1); 
			}

		        
	}
	    pthread_exit(NULL);
}



/////////////////////////
//   PROGRAME NOEUDS   //
/////////////////////////
int main(int argc, char *argv[]) {

    //GESTION DES INFORMATIONS DONEES EN PARAMETRES
    if (argc != 5){
        printf("\n[UTILISATION] %s ip_serveur port_serveur port_noeud numero_noeud\n\n", argv[0]);
        exit(1);
    }

    char* adresseIP = argv[1];          	//adresse ip du serveur
    char* port_serveur = argv[2];       	//port du serveur
    char* port_noeud = argv[3];         	//port du noeud
    int numero_noeud = atoi(argv[4]);    	//numero du client


    printColorNoeud("\n************************************************\n------------------- NOEUD ", numero_noeud, " -------------------\n************************************************\n\n");
	printColor(numero_noeud);printf("\033[4mInforamtions données en paramètres :\033[0m\n");
	printf("\n       Adresse du serveur : %s\n       Port : %d", adresseIP, atoi(port_serveur));
	printf("\n       Port du noeud : %d", atoi(port_noeud));
	printf("\n       Indice du noeud : %d\n\n", numero_noeud);
	

//A - CONSTRUCTION D’UN SOMMET

    //ETAPE 1 : CREATION DE LA SOCKET QUI DISCUTE AVEC SERVEUR
    int dSProcServ = creationSocket();
    
    printColor(numero_noeud);printf("Création de la socket pour serveur réussie !\n");
    printColor(numero_noeud);printf("Le descripteur du noeud est %d \n\n", dSProcServ);

  
    //ETAPE 2 : DESIGNATION DE LA SOCKET SERVEUR
    struct sockaddr_in sockServ = designationSocket(adresseIP, port_serveur);
    char adrServ[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &sockServ.sin_addr, adrServ, INET_ADDRSTRLEN);
    int portServ = htons((short) sockServ.sin_port);
    printColor(numero_noeud);printf("Designation de la socket du serveur réussi ! \n");
    printColor(numero_noeud);printf("Le serveur a donc pour adresse %s:%d\n\n", adrServ, portServ);


    //ETAPE 3 : DEMANDE DE CONNEXION AU SERVEUR
    connexion(dSProcServ, &sockServ);
    
    printColor(numero_noeud);printf("Connexion au serveur réussi !\n\n");


	
//B - CONSTRUCTION DE LA SOCKET DE RECEPTION : Creation d'une autre socket qui va jouer le rôle de serveur pour ses voisins qui demande une connexion

    //ETAPE 4 : GESTION DE LA SOCKET QUI JOUE LE RÔLE DE SERVEUR POUR LES AUTRES NOEUDS

		//a) creation de la socket
    int dSVoisinAttente = creationSocket();
    
    printColor(numero_noeud);printf("Création de la socket d'écoute réussi !\n");
    printColor(numero_noeud);printf("Le descripteur du noeud est %d \n\n", dSVoisinAttente);

    	//b) nommage de la socket
    struct sockaddr_in sockArete = nommageSocket(dSVoisinAttente, port_noeud);
    char adrArete[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &sockArete.sin_addr, adrArete, INET_ADDRSTRLEN);
    int portArete = htons((short) sockArete.sin_port);
    
    printColor(numero_noeud);printf("Informations du noeud (socket) de descripteur %d : %s:%i\n\n", dSVoisinAttente, adrArete, portArete);


    //ETAPE 5 : MISE SOUS ECOUTE DE LA SOCKET
    int nbMaxAttente = NOEUDS_MAX;
    ecouter(dSVoisinAttente, nbMaxAttente);
        //AFFICHAGE
    printColor(numero_noeud);printf("Mise en ecoute de la socket réussie !\n\n");

    //On vient de finir de mettre en ecoute une socket qui va avoir le rôle de serveur pour ses voisins qui demandent une connexion


//C - ENVOI LES INFORMATIONS AU SERVEUR

    //ETAPE 6 : ENVOIE DES INFORMATIONS AU SERVEUR
        //informations du noeud
    struct infos_Graphe informations_noeud;      		//structure qu'on va envoyer au serveur
    informations_noeud.numero = numero_noeud;        	//indice du noeud
	informations_noeud.ordre = 0;						//donner une valeur inutile pour que la structure soit ok
    informations_noeud.descripteur = dSVoisinAttente;  	//le descripteur
    informations_noeud.adrProc = sockArete;          	//adresse de la socket

	int s = sendCompletTCP(dSProcServ, &informations_noeud, sizeof(struct infos_Graphe));

		//GESTION DES ERREURS
		if (s == ERREUR) {
			printf("Je vais aovir un probleme sur lenvoie au serveur des infos au noeud %d\n", numero_noeud);
			perror("\n[ERREUR] : Erreur lors de l'envoie du message ");
			exit(1);
		}
		else if (s == FERMETURE) {
			printf("Mon ami le serveur va sen aller c'est trop triste au noeud %d\n", numero_noeud);
			perror("\n[ERREUR] : Abandon de la socket principale dans le l'envoie");
			exit(1); 
		}
    
    printColor(numero_noeud);printf("Envoi des inforamtions au serveur réussi !\n");
    printColor(numero_noeud);printf("\033[4mEnvoie des informations suivantes :\033[0m\n");
    printf("\n       Adresse du noeud : %s\n       Port : %d", adrArete, portArete);
    printf("\n       Numéro du noeud : %d\n       Descripteur de la socket du noeud : %d\n\n\n", numero_noeud, dSVoisinAttente);


    //DEFINITION des variables de multiplexage
    fd_set tabScrut;
    fd_set tabScrutTmp;
	FD_ZERO(&tabScrut);					            //initialisation à 0 des booléens de scrutation
	FD_SET(dSProcServ, &tabScrut);		            //ajout de la socket qui discute avec le serveur
	FD_SET(dSVoisinAttente, &tabScrut);	            //ajout de la socket de l'arête
	int maxDs = MAX(dSProcServ, dSVoisinAttente);	//nb de socket a scrutter

	int dSVoisinEntrant = 0;			            //descripteur
	int dSVoisinDemande = 0;			            //descripteur

	int nbVoisinTotal = 1;				            //car on est connecté à au moins un sommet (graphe connexe)
	int nbTotalNoeuds = 0;
	int nbVoisinDemande, nbVoisinAttente;		   
	int nbVoisinsConnectes = 0;			            //nb de voisins auquel on s'est connecté
	int nbVoisinsAcceptes = 0;			            //nb de voisins que l'on a accepté

	int ordre;							            //ordre de priorité du sommet
    struct infos_Graphe* info_voisins;
	
    while(nbVoisinsConnectes+nbVoisinsAcceptes < nbVoisinTotal) {		//tant qu'on a pas accepté tous les voisins)) {
        
        tabScrutTmp = tabScrut;
		//demande de scruter le tableau pour maxDs sockets
        if (select(maxDs+1, &tabScrutTmp, NULL, NULL, NULL) == -1) {	
            printColor(numero_noeud);printf("Problème lors du select\n");
        }


		for (int df = 2; df < maxDs+1; df++) {		//on parcours le tableau de scrutation
			
			if (!FD_ISSET(df, &tabScrutTmp)) {	    //on cherche le descripteur qui a produit un evenement
                continue;
            }

            else if (df == dSProcServ) {                                            //si un evenement se produit sur la socket serveur
				
				//RECEPTION du nombre de voisins
                struct nbVois nbVoisin;
                int r = recvCompletTCP2(dSProcServ, &nbVoisin, sizeof(struct nbVois), numero_noeud, "nombre de voisin");

					//GESTION DES ERREURS
					if (r == ERREUR) {
						printColorPlus(numero_noeud, "ERREUR");printf("Je vais avoir un probleme sur la reception des info au voisin %d\n", numero_noeud);	perror("[ERREUR] : Erreur lors de la reception du message de nombre de voisin \n");
						close(dSProcServ);
						exit(1);
					}
					else if (r == FERMETURE) {
						printColorPlus(numero_noeud, "FERMETURE//ERREUR");printf("Mon ami va s'en aller sur la reception des infos au voisin %d\n", numero_noeud);
						perror("\n[ERREUR] : Abandon de la socket principale dans la reception des nombre de voisins\n");
						close(dSProcServ);
						exit(1); 
					} 
					else if(r == STOP){
						printColorPlus(numero_noeud, "NON ENVOIE");printf("C'est pas le bon message passons a autre chose on s'arrete pour ce message\n");
						close(dSProcServ);
						exit(1);	//on arrete le programme
					}

				nbTotalNoeuds = nbVoisin.nbNoeuds;						   //nombre de noeuds total dans le graphe
                nbVoisinTotal = nbVoisin.nbVoisinTotal;					   //nombre de voisins total
                nbVoisinDemande = nbVoisin.nbVoisinDemande;				   //nombre de voisin a qui je dois demander une connexion
                nbVoisinAttente = nbVoisinTotal - nbVoisinDemande;		   //nombre de voisin que je dois attendre
            			
                printColor(numero_noeud);printf("Reception du nombre de voisin réussi !\n");
                printf("	Nombre de voisin total : %d\n", nbVoisinTotal);
                if (nbVoisinDemande > 0) {
            	    printf("	Nombre de voisin%s auxquels se connecter : %d\n", nbVoisinDemande>1?"s":"", nbVoisinDemande);
                }
				if (nbVoisinAttente > 0) {
            	    printf("	Nombre de voisin%s que je dois accepter : %d\n", nbVoisinAttente>1?"s":"", nbVoisinAttente);
                }

				//RECEPTION de l'ordre de priorité du sommet
                r = recvCompletTCP2(dSProcServ, &ordre, sizeof(int), numero_noeud, "ordre");

					//GESTION DES ERREURS
					if (r == ERREUR) {
						printColorPlus(numero_noeud,"ERREUR");printf("Je vais avoir un probleme sur la reception des info au voisin %d\n", numero_noeud);	perror("\n[ERREUR] : Erreur lors de la reception du message ");
						close(dSProcServ);
						exit(1);
					}
					else if (r == FERMETURE) {
						printColorPlus(numero_noeud, "FERMETURE//ERREUR");printf("Mon ami va s'en aller sur la reception des infos au voisin %d\n", numero_noeud);
						perror("\n[ERREUR] : Abandon de la socket principale dans le la reception");
						close(dSProcServ);
						exit(1); 
					}
					else if(r == STOP){
						printColorPlus(numero_noeud, "NON ENVOIE");printf("C'est pas le bon message passons a autre chose on s'arrete pour ce message \n");
						close(dSProcServ);
						exit(1);
					}
                printf("	Priorité du sommet : %d\n\n", ordre);
                
            	printf("\n************************************************"); 
            	printf("\n************************************************\n\n\n"); 


				//ETAPE 8 : RECEPTIONS DES INFORMATIONS DES VOISINS QUE JE DOIS CONTACTER
					
					//a) données de tous les voisins auxquels je dois me connecter
                    //tableau des informations de tous mes voisins
				info_voisins = (struct infos_Graphe*) malloc(nbVoisinTotal * sizeof(struct infos_Graphe)); 

				if (nbVoisinDemande > 0) {
						//b) parcours du nombre de voisin a qui je demande par le serveur
	                for (int vois = 0; vois < nbVoisinDemande; vois++) {
	                	struct infos_Graphe info_voisin_courant;     	//structure du voisin courant
						r = recvCompletTCP2(dSProcServ, &info_voisin_courant, sizeof(struct infos_Graphe), numero_noeud, "info voisin par serveur");

							//GESTION DES ERREURS
							if (r == ERREUR) {
								printColorPlus(numero_noeud, "ERREUR");printf("Je vais avoir un probleme sur la reception des info au voisin %d\n", numero_noeud);	perror("\n[ERREUR] : Erreur lors de la reception du message ");
								close(dSProcServ);
								exit(1);
							}
							else if (r == FERMETURE) {
								printColorPlus(numero_noeud, "FERMETURE//ERREUR");printf("Mon ami va s'en aller sur la reception des infos du voisin %d\n", numero_noeud);perror("\n[ERREUR] : Abandon de la socket principale dans la reception des infos des voisins");
								close(dSProcServ);
								exit(1); 
							}
							else if(r == STOP){
								printColorPlus(numero_noeud, "NON ENVOIE");printf("C'est pas le bon message passons a autre chose on s'arrete pour ce message \n");
								close(dSProcServ);
								exit(1);
							}

	                	info_voisins[vois] = info_voisin_courant;			//on ajoute ces informations dans le tableau prevu a cet effet
	                	printColorPlus(numero_noeud, "RECEPTION");printf("-> je dois me connecter au noeud %d d'ordre %d\n", info_voisin_courant.numero, info_voisin_courant.ordre);	
	                }
				
					
					//ETAPE 10 : DEMANDE DE CONNEXION AUX VOISINS
		            for (int v = 0; v < nbVoisinDemande; v++) {
						//a) Création de la socket qui discute avec le noeud voisin
		                dSVoisinDemande = creationSocket();
	    	            	    	            	    	        
	    	              	//b) designation de la socket du voisin
	    	            struct sockaddr_in sockVoisin = info_voisins[v].adrProc;	//recuperation des informations du voisin en fonction de son indice 
	    	            	    	              
	    					//c) demande de connexion
	                        //connexion du descripteur qu'on vient de créer et la socket du voisin courant
	    	            connexion(dSVoisinDemande, &sockVoisin);	
						info_voisins[v].descripteur = dSVoisinDemande;
						informations_noeud.ordre = ordre;
						
	                    	//d) affichage
			            char adrDem[INET_ADDRSTRLEN];
	    		        inet_ntop(AF_INET, &sockVoisin.sin_addr, adrDem, INET_ADDRSTRLEN);
	    		        //int portDem = htons((short) sockVoisin.sin_port); 
	                    
	    		        nbVoisinsConnectes++;					//on incrémente le nombre de voisins acceptés  

                        //Envoie de nos infos au voisin
                        s = sendCompletTCP(dSVoisinDemande, &informations_noeud, sizeof(struct infos_Graphe));
						
							//GESTION DES ERREURS
							if (s == ERREUR) {
								printf("Je vais avoir un probleme sur l'envoie des info au voisin %d\n", numero_noeud);
								perror("\n[ERREUR] : Erreur lors de l'envoie du message ");
								exit(1);
							}
							else if (s == FERMETURE) {
								printf("Mon ami va s'en aller sur l'envoie des infos au voisin %d\n", numero_noeud);
								perror("\n[ERREUR] : Abandon de la socket principale dans le l'envoie");
								exit(1); 
							}

						//Ajout de la nouvelle socket dans tabScrut
						FD_SET(dSVoisinDemande, &tabScrut);		//on ajoute la socket acceptée dans les socket à scruter
						maxDs = MAX(maxDs, dSVoisinDemande);	//on réajuste le max
						printColorPlus(numero_noeud, "ENVOIE");printf("des info au voisins\n");
				        
	                }
				}


					//c) Fermeture de la socket qui discute avec le serveur
				FD_CLR(dSProcServ, &tabScrut);
				if (maxDs == dSProcServ){		//si c'était le max on decremente sinon on a pas besoin
					maxDs--;
				}
				
            }
                
            else {	//je recois une connexion d'un voisin
            	
				//ETAPE 12 : ACCEPTATION DU NOEUD DONC RECEPTION DES CONNEXIONS

				    //données de adresse
    			struct sockaddr_in sockVoisinAccept;          //on declare la socket du Noeud
    			socklen_t lgAdr;                              //taille de l'adresse
						
	                //a) acceptation
	            dSVoisinEntrant = accept(dSVoisinAttente, (struct sockaddr*)&sockVoisinAccept, &lgAdr);     //on accepte le Noeud qui demande
				
					//GESTION ERREUR
					if (dSVoisinEntrant == ERREUR) {
						perror("\n\n[ERREUR] lors de l'accept d'un voisin : ");
						close(dSProcServ);
						exit(1); //on arrête le programme
					}
	            
                //Reception des infos entrant du voisin pour plus tard
                struct infos_Graphe info_voisin_courant;     	    //structure du voisin courant
				int r = recvCompletTCP2(dSVoisinEntrant, &info_voisin_courant, sizeof(struct infos_Graphe), numero_noeud, "info voisin apres acceptation");

					//GESTION DES ERREURS
					if (r == ERREUR) {
						printf("Je vais avoir un probleme sur la reception des info au voisin %d\n", numero_noeud);
						perror("\n[ERREUR] : Erreur lors de la reception du message ");
						exit(1);
					}
					else if (r == FERMETURE) {
						printf("Mon ami va s'en aller sur la reception des infos au voisin %d\n", numero_noeud);
						perror("\n[ERREUR] : Abandon de la socket principale dans le la reception");
						exit(1); 
					}
					else if(r == STOP){
						printColorPlus(numero_noeud, "NON ENVOIE");printf("C'est pas le bon message passons a autre chose on s'arrete pour ce message \n");
						continue;
					}

                info_voisin_courant.descripteur = dSVoisinEntrant;								//on met a jour le descripteur du voisin
				printColorPlus(numero_noeud, "RECEPTION");printf("des infos de mon voisin %d, d'ordre %d, de descripteur %d \n",  info_voisin_courant.numero, info_voisin_courant.ordre, info_voisin_courant.descripteur);
	
				//ETAPE 13 AJOUT DE LA NOUVELLE SOCKET DANS tabScrut
				FD_SET(dSVoisinEntrant, &tabScrut);		//on ajoute la socket acceptée dans les socket à scruter
				maxDs = MAX(maxDs, dSVoisinEntrant);	//on réajuste le max
				                
                    //on ajoute les infos du voisin dans le tableau des noeuds qui se connectent a nous
				info_voisins[nbVoisinDemande+nbVoisinsAcceptes] = info_voisin_courant;			
				nbVoisinsAcceptes++;

            } //fin du else
            
		} //fin du for 
        
    } //fin du while
    
    int toutVoisinsConnectes = nbVoisinsAcceptes+nbVoisinsConnectes == nbVoisinTotal;
	s = sendCompletTCP(dSProcServ, &toutVoisinsConnectes, sizeof(int)); 

		//GESTION DES ERREURS
		if (s == ERREUR) {
			printf("[NOEUD %d] Je vais avoir une erreur sur tout connecté \n", numero_noeud);
			perror("\n[ERREUR] : Erreur lors de l'envoie du message ");
			exit(1);
		}
		else if (s == FERMETURE) {
			printf("[NOEUD %d] Mon ami va s'en aller sur tout connecté \n", numero_noeud);
			perror("\n[ERREUR] : Abandon de la socket principale dans le l'envoie");
			exit(1);  
		} 
    
    printColor(numero_noeud);printf("voisins connectés : %d/%d\n", nbVoisinsAcceptes+nbVoisinsConnectes, nbVoisinTotal );
    
	int signal;
    int r = recvCompletTCP2(dSProcServ, &signal, sizeof(int), numero_noeud, "le signal du serveur");

		//GESTION DES ERREURS
		if (r == ERREUR) {
			printf("Je vais avoir un probleme sur le signal %d\n", numero_noeud);
			perror("\n[ERREUR] : Erreur lors de la reception du message ");
			exit(1);
		}
		else if (r == FERMETURE) {
			printf("Mon ami va s'en aller sur le signal %d\n", numero_noeud);
			perror("\n[ERREUR] : Abandon de la socket principale dans le la reception");
			exit(1); 
		}
    
    printColorPlus(numero_noeud, "FERMETURE");printf("J'ai reçu le signal du serveur !\n");
    close(dSProcServ);

    ////////////////    
    //  PARTIE 2  //
    ////////////////
    
	int couleurMax = 1;			//couleur max utilisée jusque là
    int dernierFini = 0;        //quel est le dernier noeud à s'être colorié
    int jeSuisColore = FALSE;
 
	//structure où l'on va stocker les couleurs de nos voisins
	struct couleurVoisin* tableauCouleursVoisins = (struct couleurVoisin*)malloc(nbVoisinTotal * sizeof(struct couleurVoisin));   	
	for (int i = 0; i<nbVoisinTotal; i++) {
		tableauCouleursVoisins[i].couleur = 0;
		tableauCouleursVoisins[i].ordre = info_voisins[i].ordre;
	}

	struct paramsColoration infos_Coloration;					//paramaetre pour le thread
	infos_Coloration.numero = numero_noeud;						//indice pour le thread
	infos_Coloration.ordre = ordre;								//ordre du noeud courant
	infos_Coloration.nbVoisins = nbVoisinTotal;					//nombre de voisins
	
	//si je suis le premier, je commence directement
    if (ordre == 1) {    
		printColorPlus(numero_noeud, "ORDRE 1");printf("Je suis le 1er, je commence ! %d\n", numero_noeud);    
		infos_Coloration.couleurVoisins = tableauCouleursVoisins;	        //tableau des couleurs
		infos_Coloration.VoisinsCourant = info_voisins;   			//structure des informations du voisins
        
		pthread_t threadColoration = 0;
				int res_create = pthread_create(&threadColoration, NULL, Coloration, &infos_Coloration); //je me colorie
			
			//GESTION ERREUR
			if (res_create == ERREUR){
				perror("[ERREUR] lors de la creation du thread de coloration : ");
				exit(1);
			}

        dernierFini = 1;
    }

    
	//Boucle pour le colorage, on sort lorsqu'on a colorié tous les noeuds
	while(dernierFini < nbTotalNoeuds) {
		//if (jeSuisColore == TRUE){
		//	printColorPlus(numero_noeud, "DEBUG");printf("dernierFini = %d, nbTotalNoeuds = %d\n", dernierFini, nbTotalNoeuds);
		//}
	
		//TABLEAU DE SCRUTATION EN MULTIPLEXAGE
        tabScrutTmp = tabScrut;
        int res = select(maxDs+1, &tabScrutTmp, NULL, NULL, NULL);
			
			//GESTION ERREUR
			if (res == ERREUR) {	
				perror("[ERREUR] Problème lors du select\n"); 
				exit(1);
			} 

		for (int df = 2; df < maxDs+1; df++) {		//on parcours le tableau de scrutation
			
			if (FD_ISSET(df, &tabScrutTmp)) {		
				struct messages msg;
				r = recvCompletTCP2(df, &msg, sizeof(struct messages), numero_noeud, "type de message");		//on recoit un message
					
					//GESTION DES ERREURS
					if (r == ERREUR) {
						printColorPlus(numero_noeud, "ERREUR");perror("\n[ERREUR] : Erreur lors de la reception du message ");
						close(df);
						exit(1);
					}
					else if (r == FERMETURE) {
						printColorPlus(numero_noeud, "FERMETURE//ERREUR");printf("Mon ami va s'en aller sur la reception des infos au voisin %d\n", numero_noeud);	perror("\n[ERREUR] : Abandon de la socket principale dans le la reception");
						close(df);
						exit(1);
					}
					else if(r == STOP){
						printColorPlus(numero_noeud, "NON ENVOIE");printf("C'est pas le bon message on passe au suivant \n");
						continue;
					}
					
				if ((msg.requete != 1) && (msg.requete != 0)){
                					}
				//else{
					//données du message
					int type_i = msg.requete;
					int ordre_i = msg.ordreI;
					int couleur_i = msg.message;
					
					couleurMax = MAX(couleurMax, couleur_i);  					//on met à jour la couleur max utilisée jusque là

					//modification de la couleur d'un voisin dans le tableau
					if ((jeSuisColore == FALSE) && (type_i == COULEUR)) {      //COULEUR signifie que le message vient d'un voisin
																		
						for (int i=0; i<nbVoisinTotal; i++) {
							if (tableauCouleursVoisins[i].ordre == ordre_i) {           //on cherche l'ordre du sommet qui vient de nous envoyer un message
																tableauCouleursVoisins[i].couleur = couleur_i;   		//je met à jour mon tableau des couleurs	
								break;											//on break pcq on vient de modifier le bon
							}
						}
					}
					
					//si je n'étais pas au courant que ce noeud était colorié car mon dernier fini n'est pas le même
					if (dernierFini < ordre_i) {

						dernierFini = ordre_i;                     			//on met à jour notre dernier noeud fini

						if (ordre_i != ordre) {                             //optimisation pour pas renvoyer son propre message
							//Message reenvoyé a tout mes voisins
							struct messages information;
							information.requete = INFO;                   		    //on défini le type du message en le modifiant en INFO
							information.ordreI = ordre_i;                   		    
							information.message = couleur_i;          		    

							//J'envoie à mes voisins <INFO, ordre_i, couleur_i>
							for (int i = 0; i < nbVoisinTotal; i++) {
								int dSVoisin = info_voisins[i].descripteur; 
															s = sendCompletTCP(dSVoisin, &information, sizeof(struct messages));	//envoie le meme message en changeant le type
									
									//GESTION DES ERREURS
									if (s == ERREUR) {
										printf("[NOEUD %d] Je vais avoir une erreur sur le brodcast de %d\n", numero_noeud, dSVoisin);
										perror("\n[ERREUR] : Erreur lors de l'envoie du message ");
										close(dSVoisin);
										exit(1);
									}
									else if (s == FERMETURE) {
										printf("[NOEUD %d] Mon ami va s'en aller sur le brodcast de %d\n", numero_noeud, dSVoisin);
										perror("\n[ERREUR] : Abandon de la socket principale dans le l'envoie");
										close(dSVoisin);
										exit(1);  
									}

							}
						}
					} //fin du if (message pas reçu)
						
						
					//si je suis le suivant (si mon ordre est bien le dernier qui a fini + 1) et que je me suis pas coloré
					if (dernierFini+1 == ordre && jeSuisColore == FALSE) {

						int voisinManquant = 0;
						for (int voisin = 0 ; voisin<nbVoisinTotal; voisin++){
							if ((tableauCouleursVoisins[voisin].ordre < ordre) && (tableauCouleursVoisins[voisin].couleur == 0)) {
								//si il nous reste un voisin qui nous à pas envoyé sa couleur
								voisinManquant = 1;
								break;
							}
						}
						// si j'ai tous mes voisins d'ordre inférieur coloriés
						if  (voisinManquant == 0) {
							
							jeSuisColore = TRUE;				//boolean pour savoir si je suis coloré ou pas
							
							infos_Coloration.couleurVoisins = tableauCouleursVoisins;			//tableau des couleurs
							infos_Coloration.VoisinsCourant = info_voisins;   			//structure des informations du voisins
							
							pthread_t threadColoration = 0;
														int res_create = pthread_create(&threadColoration, NULL, Coloration, &infos_Coloration); //je me colore

								//GESTION DES ERREURS
								if (res_create == ERREUR) {
									perror("[ERREUR] lors de la creation du thread de coloration : ");
									close(dSVoisinDemande);
    								close(dSVoisinEntrant);
									exit(1);
								}

													}
					}
				//} //fin du if de securité
            } //fin du if (évenement)
		} //fin du for (scrutage)
	} //fin du while
	if (dernierFini < nbTotalNoeuds){
		printf("ouii on a fini\n");
	}


    printColorPlus(numero_noeud, "TERMINÉ");printf("Je sais que le graphe est %d-coloriable !\n",couleurMax);
	sleep(5);
        //FERMETURE DE LA SOCKET CLIENTE QUI ECOUTE ET DES SOCKET QUI ACCEPTENT ET QUI SE CONNECTENT
    //printColorPlus(numero_noeud, "FERMETURE");printf("Je peux m'en aller !\n");

    close(dSVoisinDemande);
    close(dSVoisinEntrant);

    //liberation de la mémoire
    free(info_voisins); 

	return 0;
	
} //Fin du main
