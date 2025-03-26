#include <stdio.h>
#include <string>
#include <cmath>
#include <float.h>
#include <ctime>
#include "image_ppm.h"

struct Image{
    int numero;
    double moyenne;
    
    Image(int n=0,double m=0.0):
        numero(n),moyenne(m){}
};

void specification(OCTET * Img,OCTET* Bloc,OCTET* Spec,int blocH,int blocW){
    double* ddp = new double[256]();
    double* ddpBloc = new double[256]();
    double* F = new double[256]();
    double* FBloc = new double[256]();
    double* T = new double[256]();

    for(int i = 0;i < blocH*blocW;i++){
        ddp[Img[i]] += 1.0;
        ddpBloc[Bloc[i]] += 1.0;
    }

    for(int i = 0;i < 256;i++){
        ddp[i] /= (double)(blocH*blocW);
        ddpBloc[i] /= (double)(blocH*blocW);
    }
    
    F[0] = ddp[0];
    FBloc[0] = ddpBloc[0];

    for(int i = 1;i < 256;i++){
        F[i] = F[i-1] + ddp[i];
        FBloc[i] = FBloc[i-1] + ddpBloc[i];
    }

    for(int i = 0;i < 256;i++){
        float diffMin = 1.0f;
        int jBest = 0;
        for (int j = 0; j < 256; j++) {
            float diff = fabs(F[i] - FBloc[j]);
            if(diff < diffMin){
                diffMin = diff;
                jBest = j;
            }
        }
        T[i] = jBest;
    }

    for(int i = 0;i < blocH*blocW;i++){
        Spec[i] = T[Img[i]];
    }

    delete[] ddp;
    delete[] ddpBloc;
    delete[] F;
    delete[] FBloc;
    delete[] T;
}

int main(int argc, char* argv[])
{
    char cNomImgLue[250], cNomImgEcrite[250];
    int nH, nW, nTaille, tailleBloc, nbrImagettes;
    
    if (argc != 5) {
        printf("Usage: ImageIn.pgm ImageOut.pgm TailleBloc nbrImagettes \n"); 
        exit (1) ;
    }
    
    sscanf (argv[1],"%s",cNomImgLue) ;
    sscanf (argv[2],"%s",cNomImgEcrite) ;
    sscanf (argv[3],"%d",&tailleBloc) ;
    sscanf (argv[4],"%d",&nbrImagettes) ;

    OCTET *ImgIn, *ImgOut;
    
    lire_nb_lignes_colonnes_image_pgm(cNomImgLue, &nH, &nW);
    nTaille = nH * nW;
    
    allocation_tableau(ImgIn, OCTET, nTaille);
    lire_image_pgm(cNomImgLue, ImgIn, nH * nW);
    allocation_tableau(ImgOut, OCTET, nTaille);

    time_t start_time_recup;
    time(&start_time_recup);
    printf("Récupération des images...\n");
    double moyennes[nbrImagettes+1];

    //Moyenne de chaque image dans la banque
    for(int i=1;i<=nbrImagettes;i++){
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

    time_t end_time_recup;
    time(&end_time_recup);
    double duration_recup = difftime(end_time_recup, start_time_recup);

    printf("Temps récupération : %.3f secondes.\n",duration_recup);

    time_t start_time_modif;
    time(&start_time_modif);
    printf("Remplacement des blocs...\n");

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
            for(int k=1;k<=nbrImagettes;k++){
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

            OCTET *Bloc;
            allocation_tableau(Bloc,OCTET,tailleBloc*tailleBloc);
            for(int k = 0; k < tailleBloc; k++){
                for(int l = 0; l < tailleBloc; l++){
                    int x = i*tailleBloc+k;
                    int y = j*tailleBloc+l;
            
                    Bloc[k*tailleBloc+l] = ImgIn[x * nW + y];
                }
            }

            OCTET *Spec;
            allocation_tableau(Spec,OCTET,tailleBloc*tailleBloc);
            specification(Temp,Bloc,Spec,tailleBloc,tailleBloc);

            // Remplacement du bloc
            for(int k=0; k < tailleBloc; k++){
                for(int l=0; l < tailleBloc; l++){
                    int x = i*tailleBloc+k;
                    int y = j*tailleBloc+l;
                    ImgOut[x * nW + y] = Spec[k*tailleBloc+l];
                }
            }
        }
    }

    time_t end_time_modif;
    time(&end_time_modif);
    double duration_modif = difftime(end_time_modif, start_time_modif);

    printf("Temps remplacement : %.3f secondes.\n",duration_modif);

    ecrire_image_pgm(cNomImgEcrite, ImgOut, nH, nW);

    double eqm = 0.0;
    for(int i = 0;i < nTaille;i++){
        double diff = ImgIn[i] - ImgOut[i];
        eqm += diff * diff;
    }
    eqm /= nTaille;
    if(eqm == 0){
        printf("Images similaires : eqm = 0");
        return 0;
    };

    double psnr = 10* std::log10((255*255)/eqm);
    printf("PSNR : %.2f dB\n",psnr);
    
    free(ImgIn);
    free(ImgOut);
    return 1;
}
