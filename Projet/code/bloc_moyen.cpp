#include <stdio.h>
#include "image_ppm.h"


int main(int argc, char* argv[])
{
    char cNomImgLue[250], cNomImgEcrite[250];
    int nH, nW, nTaille, tailleBloc;
    
    if (argc != 4) {
        printf("Usage: ImageIn.pgm TailleBloc \n"); 
        exit (1) ;
    }
    
    sscanf (argv[1],"%s",cNomImgLue) ;
    sscanf (argv[2],"%s",cNomImgEcrite) ;
    sscanf (argv[3],"%d",&tailleBloc) ;

    OCTET *ImgIn, *ImgOut;
    
    lire_nb_lignes_colonnes_image_pgm(cNomImgLue, &nH, &nW);
    nTaille = nH * nW;
    
    allocation_tableau(ImgIn, OCTET, nTaille);
    lire_image_pgm(cNomImgLue, ImgIn, nH * nW);
    allocation_tableau(ImgOut, OCTET, nTaille);

    int nombre_bloc_hauteur = nH / tailleBloc;
    int nombre_bloc_largeur = nW / tailleBloc;

    for (int i=0; i < nombre_bloc_hauteur; i++){
        for (int j=0; j < nombre_bloc_largeur; j++){
            int somme = 0;
            int cpt = 0;

            for(int k=0; k < tailleBloc; k++){
                for(int l=0; l < tailleBloc; l++){
                    int x = i*tailleBloc+k;
                    int y = j*tailleBloc+l;

                    somme += ImgIn[x*nW+y];
                    cpt++;
                }
            }
            
            double moyenne = somme / (double)cpt;
            // printf("Bloc : %d,%d | Moyenne : %f\n",i,j,moyenne);

            for(int k=0; k < tailleBloc; k++){
                for(int l=0; l < tailleBloc; l++){
                    int x = i*tailleBloc+k;
                    int y = j*tailleBloc+l;
                    ImgOut[x * nW + y] = moyenne;
                }
            }
        }
    }

    ecrire_image_pgm(cNomImgEcrite, ImgOut, nH, nW);
    
    free(ImgIn);
    free(ImgOut);
    return 1;
}
