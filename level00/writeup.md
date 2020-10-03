## Level0

![image](https://user-images.githubusercontent.com/29956389/94953208-6d248b00-04e7-11eb-8beb-69cb2200ff06.png)

On nous demande un mot de passe. Observons le binaire dans GDB:

![image](https://user-images.githubusercontent.com/29956389/94953335-a957eb80-04e7-11eb-8ff3-2f24674c9f7f.png)

On observe les premier appels a `puts()` et `printf()` qui afficher le banner.

![image](https://user-images.githubusercontent.com/29956389/94953406-ce4c5e80-04e7-11eb-9e6b-28f8836ad30c.png)

Un appel a scanf qui stocke notre mot de passe.

![image](https://user-images.githubusercontent.com/29956389/94953536-ff2c9380-04e7-11eb-8959-3921597134b9.png)

Une comparaison de notre mot de passe a 0x149c (5276).

![image](https://user-images.githubusercontent.com/29956389/94953569-0eabdc80-04e8-11eb-9fb4-0cafb3240122.png)

Les deux issue, suite a la condition, qui execute `system(/bin/sh)`, ou quitte le programme.

![image](https://user-images.githubusercontent.com/29956389/94953752-592d5900-04e8-11eb-9e38-8b17953214fb.png)

Il suffit donc de passer 5276 en mot de passe.

![image](https://user-images.githubusercontent.com/29956389/94953879-8da11500-04e8-11eb-9b4f-f1a783c5cbc0.png)
