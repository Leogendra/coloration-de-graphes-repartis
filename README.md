# Projet de Coloration de graphe HADDAD Gatien, POINTEAU Gabrielle
Voir le rapport pour plus de détails.

**Lancement automatique**

*Lancer le serveur* : 
``python3 serveur.py fichier_graphe port_serveur``

*Utiliser le programme noeuds.py pour lancer autant de noeuds qu'il y en a dans le graphe* : 
``python3 noeuds.py fichier_graphe ip_serveur port_serveur``


**Exemples d'utilisation**
utilisation 1 :
*lancement du serveur dans le terminal 1* : ``python3 serveur.py graphes/graphe_petersen.txt 5000``
*lancement des noeuds dans le terminal 2* : ``python3 noeuds.py graphes/graphe_petersen.txt 127.0.0.1 5000``

utilisation 2 :
*lancement du serveur dans le terminal 1* : ``python3 serveur.py graphes/graphe_500.txt 8000``
*lancement des noeuds dans le terminal 2* : ``python3 noeuds.py graphes/graphe_500.txt 127.0.0.1 8000``
