import os, sys

#GESTION DES PARAMETRES
if (len(sys.argv) != 4) :
  print(f"[UTILISATION] :\npython3 {sys.argv[0]} nom_fichier ip_serveur port_serveur")
  exit(1)


# LECTURE DES DONNEES DU GRAPHE
path = sys.argv[1]								# recuperation du nom du fichier
fichier = open(path, "r")						# ouverture en lecture
contenu = fichier.readline()					# lire le contenu
while (contenu[0] == 'c'):        				# on saute les commentaires
    contenu = fichier.readline()				#on lit une nouvelle ligne

p, edge, noeuds, aretes = contenu.split() 	# on récupère la première ligne avec les informations du graphe
fichier.close()								#on ferme le fichier

print(f"{noeuds} noeuds, {aretes} arêtes")	#on affiche le nombre de noeuds et le nombre d'aretes


# INFORMATION SUR LE GRAPHE
noeuds = int(noeuds)				#cast des nbNoeuds
aretes = int(aretes)				#cast des nbAretes


# INFORMATIONS DES CONNEXIONS
ip_serveur = sys.argv[2]										#recuperation de l'adresse du serveur
port_serveur = int(sys.argv[3])									#du port du serveur
print("ADRESSE serveur :",ip_serveur+":"+str(port_serveur))		#affichage de l'ip et du port


# LANCEMENT DES NOEUDS
for i in range(1, noeuds):		#pour le nombre de client qui a
    cmd = ["./bin/noeuds", ip_serveur, str(port_serveur), str(port_serveur+i), str(i), "&"]		#lancer la commande pour les n-1 noeuds
    os.system(" ".join(cmd))

cmd = ["./bin/noeuds", ip_serveur, str(port_serveur), str(port_serveur+noeuds), str(noeuds)]	#et le dernier pour ne aps gener l'affichage avec le &
os.system(" ".join(cmd))