## Level06

On observe un binaire plutot bien protégé:

![image](https://user-images.githubusercontent.com/29956389/95348413-4e930b00-08be-11eb-916a-002e8146e74a.png)

Le binaire nous demande une paire login/serial:

![image](https://user-images.githubusercontent.com/29956389/95348563-74201480-08be-11eb-8b2b-9c40a0357832.png)

Dans le code on observe les appels: ` fgets(buffer_login, 0x20, stdin);` et `scanf("%u", serial);`, ainsi que:

```
if (auth(buffer_login, serial) == 0)
{
	puts("Authenticated!");
	system("/bin/sh");
}
```

Dans la fonction `auth()` notre login est utilisé pour générer une clé serial avec une fonction de chiffrage, et si cette clé correspond a celle que nous avons entrée, la fonction renvoie 0.

##### Problème: La fonction est protégé avec un appel a `ptrace()` pour eviter le débugage.

##### Solution: On catch le syscall ptrace, et établis une macro qui sera effectué avant: `set ($eax) = 0` ce qui remplacera l'appel systeme `26 ptrace` par le syscall `0 aucun`.

```
$ catch syscall ptrace
$ commands 1
> set ($eax) = 0
> continue
> end
```

On contourne la vérification anti-débogage et on peut afficher le contenue des variable poussé sur la stack lors de la vérification:

![image](https://user-images.githubusercontent.com/29956389/95352071-25747980-08c2-11eb-8e27-62d50929ef9c.png)

![image](https://user-images.githubusercontent.com/29956389/95351280-599b6a80-08c1-11eb-9659-ed3e0e088e26.png)

![image](https://user-images.githubusercontent.com/29956389/95351359-7041c180-08c1-11eb-895a-f452bb77a83c.png)

### Attention

Il faut bien entrer le meme login a chaque test, sinon la serial sera différente. Attention a bien entrer une serial valide pour le premier test: numérique!
