# PROJET IMAGE et COMPRESSION

## Sujet #2 : Mosaïque d’images avec critères avancés

REYNIER Théo - JALBAUD Lucas

---
### Objectif

L'objectif est de générer une grande image mosaïque à partir d’une grande base d’images de petites tailles appelées imagettes. \
Le principe de la méthode consiste à prendre une grande image, la découper en blocs de petites tailles et de remplacer chaque bloc de la grande image par l’imagette la plus similaire issue de la grande base d’imagettes.


### Compilation

```bash
chmod +x compil.sh 
./compil.sh
```

### Téléchargement et installation de la banque d'images

- Télécharger la banque d'images à cette adresse : https://data.mendeley.com/datasets/kb3ngxfmjw/1
- Créer un dossier "banque"
- Mettre les images dans le dossier


### Générer des images en blocs moyennés

Les programmes `bloc_moyen` et `bloc_moyen_RGB` permettent de générer des images en blocs de pixels moyennés.
Exemples d'utilisation :
```bash
./exe/bloc_moyen in/imageIn.pgm out/bloc/imageOut.pgm taille_bloc
./exe/bloc_moyen_RGB in/imageIn.ppm out/bloc/imageOut.ppm taille_bloc
```


### Générer des images en mosaiques en utilisant la moyenne de chaque bloc

Le programme `mosaique_moy.cpp` permet de générer des images en mosaiques en remplacant les blocs de pixels moyennés par des imagettes de la banque d'images.
Exemple d'utilisation :
```bash
./exe/mosaique_moy in/imageIn.pgm out/mosaique/imageOut.pgm taille_bloc
```

### Récupération d'images au format png

Si vous souhaitez récupérer des images au format png plutot que pgm ou ppm :

```bash
chmod +x convertIN.sh
chmod +x convertOUT.sh
./convertIN.sh
./convertOUT.sh
```