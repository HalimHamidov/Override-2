# Level09

Lorsque nous arrivons dans le level09, plusieurs protections sont en place.

![Capture d’écran 2020-10-15 à 08 26 02](https://user-images.githubusercontent.com/25014717/96085053-80a8fc00-0ec0-11eb-867d-fe0fe49674e6.png)

Nous debutons donc notre travail de reverse engineering.

Le main, tres simple, fait un appel simple a la fonction handle_msg.

![Capture d’écran 2020-10-15 à 08 27 09](https://user-images.githubusercontent.com/25014717/96085105-97e7e980-0ec0-11eb-9f41-c770b27f5242.png)

Le fonctionnement de handle_msg est relativement simple: Apres l'assignation
d'un large espace de donne, une structure est assignee. Nous avons determine que
c'etait une structure grace au comportement de dereferencement relatif vu plus
loin.

![Capture d’écran 2020-10-15 à 08 27 31](https://user-images.githubusercontent.com/25014717/96085255-d67da400-0ec0-11eb-8e5a-4d2c8e03e276.png)


Donc, apres que cette structure soit assigée, un array de 40 caracteres est mise
a zero, puis un int est initialise a 140 caracteres. L'array initialisee etant
remplie dans la fonction set_username, nous l'appellerons username. L'array de
140 caractere etant remplie dans set_msg en utilisant l'int initialise a 140,
nous nommerons les deux champs restant de notre structure msg et len.

Avoir initialisé ces deux elements, deux fonctions sont appelées
consecutivement, set_username d'abord, puis set_msg, ces deux fonctions ne
prenant qu'un seul paramettre, l'adresse de notre structre. Apres ces deux
appels, la fonction ecrit un message, et retourne a main.


![Capture d’écran 2020-10-15 à 08 27 54](https://user-images.githubusercontent.com/25014717/96085336-f614cc80-0ec0-11eb-9f26-5d2ec8ad347b.png)

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
ou egal a 40, pas simplement plus grand. C'est un cas
d'[off-by-one error](https://fr.wikipedia.org/wiki/Erreur_off-by-one).

Nous pourrons donc ecrire sur le byte suivant dans notre structure message, byte
appartenant a l'int len.

Le reversing de la fonction set_msg nous permet de mieux entrapercevoir quel
chemin nous allons emprunter:

![Capture d’écran 2020-10-15 à 08 28 11](https://user-images.githubusercontent.com/25014717/96085341-f90fbd00-0ec0-11eb-9c6e-75dc79514c98.png)

Apres avoir aloue un buffer de 1024 elements qui sera mis a zero, la fonction
fait un fgets de 1024 caracteres dans le buffer, puis un strncpy en se
basant sur la variable stockée dans message->len pour copier dans message->msg.

Comme nous l'avons vu plus haut, nous controlons la valeur de len grace a
l'erreur off by one. Notre structure etant assigée sur la stack dans la fonction
handle_message, si nous ecrivons plus loin dans la structure, nous pourrons
reecrire sur l'adresse qui stocke l'eip pour retourner au main et prendre le
controle de l'execution. L'overflow est possible seulement parce que le buffer
dans lequel nous lisons fait 1024, tandis que celui dans lequel nous copions ne
fait que 140.

Nous avons donc avec ces deux fonctions tout les elements pour prendre le
controle de l'execution. Reste a savoir ce que nous allons faire avec ce controle. 
Mais grace a une verification des fonctions presentes dans le binaire avec info functions, cette question, elle est vite repondue:

![Capture d’écran 2020-10-15 à 08 28 40](https://user-images.githubusercontent.com/25014717/96085493-307e6980-0ec1-11eb-849b-6ba8b4f30b1e.png)

Nous permet de determiner qu'il y a une fonction qui n'est pas utilise dans le
binaire, secret_backdor. Cette fonction, assez simple, a le comportement
suivant: Apres avoir lu 128 caracteres dans un buffer, ce
buffer est passé en parametre a systeme. C'est ideal pour notre exploit.

![Capture d’écran 2020-10-15 à 08 28 28](https://user-images.githubusercontent.com/25014717/96085533-3f651c00-0ec1-11eb-9deb-8e8456613067.png)

Dans un premier temps, nous determinons la valeur que nous voulons passer dans
le byte auquel nous avons acces. La valeur maximale que nous pouvons passer dans
un byte est 255, soit \xff. Nous inserons bien un \n pour signifier la fin de la
lecture a fgets. Nous obtennons bien notre segfault.

![Capture d’écran 2020-10-15 à 06 40 14](https://user-images.githubusercontent.com/25014717/96085789-a1258600-0ec1-11eb-8b2c-f059a324b0b9.png)

Nous determinons ensuite l'offset auquel se trouve l'eip qui nous interesse avec
la commande suivante:

> python -c 'import string; print "A" * 40 + "\xff" + "\n" + "".join([i * 4 for i in string.ascii_uppercase]) + "".join([i * 4 for i in string.ascii_lowercase])' | ./level09

![Capture d’écran 2020-10-15 à 07 14 17](https://user-images.githubusercontent.com/25014717/96085832-b00c3880-0ec1-11eb-95e5-15abbcbf211a.png)

Nous devons donc trouver l'offset du premier 79 en ascii, soit 'y'. Nous le trouvons grave a la commande suivante :

> python -c 'import string; print zip(range(0, 230), "".join([i * 4 for i in string.ascii_uppercase]) + "".join([i * 4 for i in string.ascii_lowercase]))' | grep y


![Capture d’écran 2020-10-15 à 08 39 20](https://user-images.githubusercontent.com/25014717/96086026-efd32000-0ec1-11eb-8f73-40e284fc69f1.png)

Le y se trouve donc a l'offset 200, c'est donc la que nous placerons l'adresse de la fonction secret backdor.

Notre commande sera donc la suivante:

[40 bytes padding + \xff + \n + 200 bytes padding + adresse secret backdoor + \n + "/bin/sh" + \n]

En formattant correctement l'adresse de secret backdoor, cela nous donne la commande suivante :

> (python -c 'print "A" *40 + "\xff" + "\n" + "A" * 200 + "\x8c\x48\x55\x55\x55\x55\x00\x00" + "\n" + "/bin/sh\n"'; cat) | ./level09

![Capture d’écran 2020-10-15 à 07 48 52](https://user-images.githubusercontent.com/25014717/96086106-0aa59480-0ec2-11eb-8478-4c7a95d6020c.png)
