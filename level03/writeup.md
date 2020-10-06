## Level03

En arrivant dans le niveau 3, nous voyons plusieurs protections actives.

![Capture d’écran 2020-10-05 à 23 35 23](https://user-images.githubusercontent.com/25014717/95136681-dcf47900-0766-11eb-85a9-e422567bfaed.png)

Le binraire nous demande un mot de passe.

![Capture d’écran 2020-10-05 à 23 36 44](https://user-images.githubusercontent.com/25014717/95136679-dc5be280-0766-11eb-9e93-f850b0d7efd5.png)

Nous avons dans le binaire plusieurs fonctions interessantes: main, test et decrypt.

![Capture d’écran 2020-10-05 à 23 37 27](https://user-images.githubusercontent.com/25014717/95136678-dc5be280-0766-11eb-8242-fa8f92b9c9f6.png)

Le dessassemblage de main nous permet de voir qu'un generateur de nombre aleatoire est set sur le temps actuel. puis qu'un int est lu sur l'entree standard. Cet int est ensuite passe en parametre a la fonction test. La fonction test prends deux paramettre, le nombre que nous entrons et un deuxieme parametre donc. Ce paramettre est le nombre 0x1337d00d, passe en dur dans le code.

![Capture d’écran 2020-10-05 à 23 40 01](https://user-images.githubusercontent.com/25014717/95136677-dbc34c00-0766-11eb-95cb-7b6d79b918fa.png)

La fonction test commence soustraire le nombre que nous avons passe en paramettre a 0x1337d00d. Il y a ensuite une verification pour determiner si la difference est inferrieure a 0x15. L'instruction "ja" nous permet de savoir que c'est une difference entre deux entiers positifs. Si la difference entre le nombre passe et le nombre defini n'est pas inferieure a 0x15, la fonction decrypt est appele, mais avec un nombre aleatoire, ce que nous voulons eviter.


![Capture d’écran 2020-10-05 à 23 40 43](https://user-images.githubusercontent.com/25014717/95136675-da921f00-0766-11eb-9525-ff6dd63b7e12.png)

Le nombre passe en paramettre sera donc entre 322424844 et 322424823.

Si la difference est inferieur a 22, une multiplication par deux puis l'ajout d'un padding fixe ferra sauter l'eip a une instruction precise, ce qui elimine egalement certains nombres.


La fonction decrypt a une chaine de caractere fixe alloue en debut de fonction. Un strlen est effectue sur cette chaine pour definir sa longueur, puis l'operation qui decrypte est effectuee: pour chaque caractere de cette chaine, un XOR est effectue avec la difference entre le nombre fixe 0x1337d00d et notre nombre passe en paramettre. Une fois que tout les characteres de la chaine sont passe par cette opperation, la string resultante est comparee a la string "Congratulations!". Si elles sont identiques, nous avons acces a un shell.

![Capture d’écran 2020-10-05 à 23 51 10](https://user-images.githubusercontent.com/25014717/95136671-d8c85b80-0766-11eb-9cf9-e3f1c887e391.png)

Fort de tout ces elements, nous avons fait un programme nous permettant de savoir quel nombre passer au programme pour nous donner acces au shell. Ce nombre etant 322424827.
