#include <stdio.h>
#include <string>
#include <cmath>
#include <float.h>
#include "image_ppm.h"

struct Image{
    int numero;
    double moyenne;
    
    Image(int n=0,double m=0.0):
        numero(n),moyenne(m){}
};


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

    double moyennes[10000];

    //Moyenne de chaque image dans la banque
    for(int i=1;i<=10000;i++){
        int nH_i,nW_i,nTaille_i;

        // Recupere et lit chaque imagette de la banque d'images
        OCTET *Img;
        std::string fichier = "banque/"+std::to_string(i)+".pgm";
        lire_nb_lignes_colonnes_image_pgm((char*)fichier.c_str(), &nH_i, &nW_i);
        
        nTaille_i = nW_i*nH_i;
        allocation_tableau(Img,OCTET,nTaille_i);
        lire_image_pgm((char*)fichier.c_str(),Img,nTaille_i);

        int somme = 0;
        for(int j=0;j<nTaille_i;j++){
            somme += Img[j];
        }
        moyennes[i] = (double)(somme /= nTaille_i);
        free(Img);
    }

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
            double compare = FLT_MAX;
            int best = -1;

            // Cherche la meilleure imagette
            for(int k=1;k<=10000;k++){
                double comp = std::fabs(moyennes[k]-moyenne);
                if(comp<compare){
                    compare = comp;
                    best = k;
                }
            }

            // Recupere et lit chaque imagette de la banque d'images
            OCTET *Imagette;
            int nH_imagette,nW_imagette,nTaille_imagette;
            std::string fichier = "banque/"+std::to_string(best)+".pgm";
            lire_nb_lignes_colonnes_image_pgm((char*)fichier.c_str(), &nH_imagette, &nW_imagette);
            
            nTaille_imagette = nW_imagette*nH_imagette;
            allocation_tableau(Imagette,OCTET,nTaille_imagette);
            lire_image_pgm((char*)fichier.c_str(),Imagette,nTaille_imagette);

            OCTET *Temp;
            allocation_tableau(Temp,OCTET,tailleBloc*tailleBloc);

            // Resize des images pour correspondre au bloc
            for(int k = 0; k < tailleBloc; k++){
                for(int l = 0; l < tailleBloc; l++){
                    int x_source = (k*nH_imagette)/tailleBloc;
                    int y_source = (l*nW_imagette)/tailleBloc;
            
                    Temp[k*tailleBloc+l] = Imagette[x_source*nW_imagette+y_source];
                }
            }

            // Remplacement du bloc par l'imagette
            for(int k=0; k < tailleBloc; k++){
                for(int l=0; l < tailleBloc; l++){
                    int x = i*tailleBloc+k;
                    int y = j*tailleBloc+l;
                    ImgOut[x * nW + y] = Temp[k*tailleBloc+l];
                }
            }
        }
    }

    ecrire_image_pgm(cNomImgEcrite, ImgOut, nH, nW);
    
    free(ImgIn);
    free(ImgOut);
    return 1;
}
