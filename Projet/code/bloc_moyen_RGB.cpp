#include <stdio.h>
#include <stdlib.h>
#include "image_ppm.h"


int main(int argc, char* argv[]) {
    char cNomImgLue[250], cNomImgEcrite[250];
    int nH, nW, nTaille, tailleBloc;
    
    if (argc != 4) {
        printf("Usage: ImageIn.pgm TailleBloc \n"); 
        exit (1) ;
    }
    
    sscanf(argv[1], "%s", cNomImgLue);
    sscanf(argv[2], "%s", cNomImgEcrite);
    sscanf(argv[3], "%d", &tailleBloc);

    OCTET *ImgIn, *ImgOut;
    
    lire_nb_lignes_colonnes_image_ppm(cNomImgLue, &nH, &nW);
    nTaille = nH * nW * 3;
    
    allocation_tableau(ImgIn, OCTET, nTaille);
    allocation_tableau(ImgOut, OCTET, nTaille);
    lire_image_ppm(cNomImgLue, ImgIn, nH * nW);

    int nombre_bloc_hauteur = nH / tailleBloc;
    int nombre_bloc_largeur = nW / tailleBloc;

    for (int i = 0; i < nombre_bloc_hauteur; i++) {
        for (int j = 0; j < nombre_bloc_largeur; j++) {
            int sommeR = 0, sommeG = 0, sommeB = 0;
            int cpt = 0;
            
            for (int k = 0; k < tailleBloc; k++) {
                for (int l = 0; l < tailleBloc; l++) {
                    int x = i * tailleBloc + k;
                    int y = j * tailleBloc + l;
                    
                    int index = (x * nW + y) * 3;
                    sommeR += ImgIn[index];
                    sommeG += ImgIn[index + 1];
                    sommeB += ImgIn[index + 2];
                    cpt++;
                }
            }
            
            double moyenneR = sommeR / (double)cpt;
            double moyenneG = sommeG / (double)cpt;
            double moyenneB = sommeB / (double)cpt;
            
            for (int k = 0; k < tailleBloc; k++) {
                for (int l = 0; l < tailleBloc; l++) {
                    int x = i * tailleBloc + k;
                    int y = j * tailleBloc + l;
                    
                    int index = (x * nW + y) * 3;
                    ImgOut[index] = moyenneR;
                    ImgOut[index + 1] = moyenneG;
                    ImgOut[index + 2] = moyenneB;
                }
            }
        }
    }

    ecrire_image_ppm(cNomImgEcrite, ImgOut, nH, nW);
    
    free(ImgIn);
    free(ImgOut);
    return 1;
}
