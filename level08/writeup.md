# Level 08

Lorsque nous arrivons dans le level08, nous voyons que le binaire a deux
protections enclanchees.

Le binaire, en 64bits,  comprends deux fonctions que nous devons analyser: main
et log wrapper.

Apres reversing, le binaire a un comportement assez simple: apres une
verification du nombre d'arguments passés(si le nombre d'argument passe est different de deux, un usage est affiche, mais le programme continue)
il ouvre le fichier './backups/.log' et le fichier passé en arguments. Un premier message est ecrit dans le log.
Ensuite, Un nouveau fichier est cree en fusionnant la chaine './backups' avec notre argument. Le programme copie ensuite le contenu du fichier
passé en argument dans le nouveau fichier. Enfin, il enregistre un message dans le fichier .backup.log et quitte.

La vulnerabilité viens ici du fait que le dossier dans lequel le backup est crée commence par './', un
chemin relatif. Cela nous permet donc de controler l'endroit ou le nouveau
fichier sera crée en creant un dossier backup dans un dossier dans lequel nous avons les droits d'ecriture.

Nous connaissons le chemin du fichier que nous voulons obtenir, donc l'exploit se fait simplement avec les commandes suivantes:

-- Nous deplacer dans le dossier dans lequel nous avons droit d'ecriture:

> cd /tmp

-- Creer un dossier backup contenant une reproduction du chemin complet vers
notre fichier cible:
> mkdir -p ./backups/home/users/level09/

-- Executer notre le binaire avec le chemin complet de notre fichier cible:
> /home/users/level08/level08 /home/users/level09/.pass

-- Lire notre flag
> cat ./backups/home/users/level09/.pass
