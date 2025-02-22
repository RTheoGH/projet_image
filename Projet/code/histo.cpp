// test_couleur.cpp : Seuille une image en niveau de gris

#include <stdio.h>
#include "image_ppm.h"

int main(int argc, char* argv[])
{
    char cNomImgLue[250], cNomImgEcrite[250];
    int nH, nW, nTaille;
    
    if (argc != 2) 
        {
        printf("Usage: ImageIn.pgm \n"); 
        exit (1) ;
        }
    
    sscanf (argv[1],"%s",cNomImgLue) ;

    OCTET *ImgIn;
    
    lire_nb_lignes_colonnes_image_pgm(cNomImgLue, &nH, &nW);
    nTaille = nH * nW;
    
    allocation_tableau(ImgIn, OCTET, nTaille);
    lire_image_pgm(cNomImgLue, ImgIn, nH * nW);

    int histo[256];
    for(int i=0;i<256;i++) histo[i] = 0;

    for (int i=0; i < nH; i++)
    for (int j=0; j < nW; j++)
        {
            histo[ImgIn[i*nW+j]] += 1;
        }

    for(int i=0;i<256;i++){
        printf("%d %d\n",i,histo[i]);
    }

    free(ImgIn);
    return 1;
}
