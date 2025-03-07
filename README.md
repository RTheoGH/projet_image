# PROJET IMAGE et COMPRESSION

## Sujet #2 : Mosaïque d’images avec critères avancés

REYNIER Théo - JALBAUD Lucas

---
Compilation

```bash
chmod +x compil.sh 
./compil.sh
```
---

Etape pour la banque d'images :

- Télécharger la banque d'images à cette adresse : https://data.mendeley.com/datasets/kb3ngxfmjw/1
- Créer un dossier "banque"
- Mettre les images dans le dossier

---

L'objectif est de générer une grande image mosaïque à partir d’une grande base d’images de petites tailles appelées imagettes. \
Le principe de la méthode consiste à prendre une grande image, la découper en blocs de petites tailles et de remplacer chaque bloc de la grande image par l’imagette la plus similaire issue de la grande base d’imagettes.

---

Les programmes `bloc_moyen` et `bloc_moyen_RGB` permettent de générer des images en blocs de pixels moyennés.
Exemples :
```bash
./exe/bloc_moyen in/imageIn.pgm out/imageOut.pgm taille_bloc
./exe/bloc_moyen_RGB in/imageIn.ppm out/imageOut.ppm taille_bloc
```