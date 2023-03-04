#LANCEUR DU SERVEUR PLUS NETOYER LE TERMINAL ET LES PORTS

import os, sys

#GESTION DES PARAMETRES
if (len(sys.argv) != 3):
  print(f"[UTILISATION] :\npython3 {sys.argv[0]} nom_fichier port_serveur")
  exit(1)

	
# NETOYAGE DES PROCESSUS
try:	#essaie
  os.system("python3 scripts/cleaner.py")								      #utilisation de cleaner pour fermer les ports
  print("Netoyage des processus effectué")						#afficahge
except:	#si arrive pas
  print("Processus non nétoyés (processus blocant)")


	
# COMPILATION
try: #essaie
  os.system("mkdir bin")		#créer le dossier bin
  os.system("mkdir obj")		#créer le dossier obj
except:
  print("Dossiers prêts")
#make pour clear et compiler les fichiers
os.system("clear")
os.system("make")



# INFORMATIONS DE CONNEXION
fichier_graphe = sys.argv[1]		  #du fichier pour le graphe
port_serveur = int(sys.argv[2])		#recuperation du port


# Lancement du serveur
cmd = ["./bin/serveur", fichier_graphe, str(port_serveur)]		#lancer la commande dans un terminal pour lancer le serveur
os.system(" ".join(cmd))