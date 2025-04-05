// test_couleur.cpp : Seuille une image couleur 

#include <stdio.h>
#include "image_ppm.h"

int main(int argc, char* argv[]){
    char cNomImgLue[250];
    int nH, nW, nTaille, nR, nG, nB;
    
    if (argc != 2){
        printf("Usage: ImageIn.ppm\n"); 
        exit (1) ;
    }
    
    sscanf (argv[1],"%s",cNomImgLue) ;

    OCTET *ImgIn;
    
    lire_nb_lignes_colonnes_image_ppm(cNomImgLue, &nH, &nW);
    nTaille = nH * nW;
    
    int nTaille3 = nTaille * 3;
    allocation_tableau(ImgIn, OCTET, nTaille3);
    lire_image_ppm(cNomImgLue, ImgIn, nH * nW);

    int rouge[256]; for(int i=0;i<256;i++) rouge[i]=0;
    int vert[256]; for(int i=0;i<256;i++) vert[i]=0;
    int bleu[256]; for(int i=0;i<256;i++) bleu[i]=0;
        
    for (int i=0; i < nTaille3; i+=3){
        nR = ImgIn[i];
        nG = ImgIn[i+1];
        nB = ImgIn[i+2];

        rouge[nR] += 1;
        vert[nG] += 1;
        bleu[nB] += 1;
    }

    for(int i=0;i<256;i++){
        printf("%d %d %d %d\n",i,rouge[i],vert[i],bleu[i]);
    }

    free(ImgIn);
    return EXIT_SUCCESS;
}
