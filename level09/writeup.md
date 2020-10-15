# Level09

Lorsque nous arrivons dans le level09, plusieurs protections sont en place. Nous
debutons donc notre travail de reverse engineering.

Le main, tres simple, fait un appel simple a la fonction handle_msg.

Le fonctionnement de handle_msg est relativement simple: Apres l'assignation
d'un large espace de donne, une structure est assignee. Nous avons determine que
c'etait une structure grace au comportement de dereferencement relatif vu plus
loin.

Donc, apres que cette structure soit assigée, un array de 40 caracteres est mise
a zero, puis un int est initialise a 140 caracteres. L'array initialisee etant
remplie dans la fonction set_username, nous l'appellerons username. L'array de
140 caractere etant remplie dans set_msg en utilisant l'int initialise a 140,
nous nommerons les deux champs restant de notre structure msg et len.

Avoir initialisé ces deux elements, deux fonctions sont appelées
consecutivement, set_username d'abord, puis set_msg, ces deux fonctions ne
prenant qu'un seul paramettre, l'adresse de notre structre. Apres ces deux
appels, la fonction ecrit un message, et retourne a main.

La fonction set_username est la premiere qui soit d'interet pour nous. En effet,
apres avoir aloue sur la stack et initialisé un buffer de 128 caracteres, le
programme lit sur l'entrée standard. Nous pouvons passer un premier input, qui
sera lu dans le buffer de 128 caracteres. Le contenu de ce buffer sera ensuite
copié dans le champ username de notre structure.

Le fait interessant ici est que la copie se fait a l'aide d'une boucle qui
compare si i est plus grand que 40, et sors de la boucle si c'est le cas.

> <+157>:   cmp    DWORD PTR [rbp-0x4],0x28
> <+161>:   jg     0x555555554a81 <set_username+180>

Notre array etant de 40 bytes, la condition devrait verifier si i est plus grand
ou egal a 40, pas simplement plus grand. C'est cas
d'[off-by-one error](https://fr.wikipedia.org/wiki/Erreur_off-by-one).

Nous pourrons donc ecrire sur le byte suivant dans notre structure message, byte
appartenant a l'int len.

Le reversing de la fonction setmessage nous permet de mieux entrapercevoir quel
chemin nous allons emprunter:

Apres avoir aloue un buffer de 1024 elements qui sera mis a zero, la fonction
fait un fgets de 1024 caracteres dans le buffer aloué, puis un strncpy en se
basant sur la variable stockée dans message->len pour copier dans message->msg.
Comme nous l'avons vu plus haut, nous controlons la valeur de len grace a
l'erreur off by one. Notre structure etant assigée sur la stack dans la fonction
handle_message, si nous ecrivons plus loin dans la structure, nous pourrons
reecrire sur l'adresse qui stocke l'eip pour retourner au main et prendre le
controle de l'execution. L'overflow est possible seulement parce que le buffer
dans lequel nous lisons fait 1024, tandis que celui dans lequel nous copions ne
fait que 140.

Nous avons donc avec ces deux fonctions tout les elements pour prendre le
controle de l'execution.

> info function

Nous permet de determiner qu'il y a une fonction qui n'est pas utilise dans le
binaire, secret_backdor. Cette fonction, assez simple, a le comportement
suivant: Apres avoir lu 128 caracteres dans un buffer, ce
buffer est passé en parametre a systeme. C'est ideal pour notre exploit.

> <+4>:     add    rsp,0xffffffffffffff80

Dans un premier temps, nous determinons la valeur que nous voulons passer dans
le byte auquel nous avons acces. La valeur maximale que nous pouvons passer dans
un byte est 255, soit \xff. Nous inserons bien un \n pour signifier la fin de la
lecture a fgets. Nous obtennons bien notre segfault.

Nous determinons ensuite l'offset auquel se trouve l'eip qui nous interesse avec
la commande suivante:

> python -c 'import string; print "A" * 40 + "\xff" + "\n" + "".join([i * 4 for
> i in string.ascii_uppercase]) + "".join([i * 4 for i in
> string.ascii_lowercase])' | ./level09

Nous devons donc trouver l'offset du premier 79 en ascii, soit 'y'. Nous le trouvons grave a la commande suivante :

> python -c 'import string; print zip(range(0, 230), "".join([i * 4 for i in string.ascii_uppercase]) + "".join([i * 4 for i in string.ascii_lowercase]))' | grep y

Le y se trouve donc a l'offset 200, c'est donc la que nous placerons l'adresse
de la fonction secret backdor.

Notre commande sera donc la suivante:

[40 bytes padding + \xff + \n + 200 bytes padding + adresse secret backdoor +
\n + "/bin/sh" + \n]

En formattant correctement l'adresse de secret backdoor, cela nous donne la
commande suivante :

> (python -c 'print "A" *40 + "\xff" + "\n" + "A" * 200 + "\x8c\x48\x55\x55\x55\x55\x00\x00" + "\n" + "/bin/sh\n"'; cat) | ./level09


