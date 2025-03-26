#include <stdio.h>
#include <string>
#include <cmath>
#include <float.h>
#include <ctime>
#include "image_ppm.h"

struct Image{
    int numero;
    double moyenneR, moyenneG, moyenneB;
    
    Image(int n=0, double r=0.0, double g=0.0, double b=0.0) : numero(n), moyenneR(r), moyenneG(g), moyenneB(b) {}
};

void specification_couleur(OCTET * Img,OCTET* Bloc,OCTET* Spec,int blocH,int blocW){
    double *ddp[3], *ddpBloc[3], *F[3], *FBloc[3], *T[3];

    for(int c = 0;c < 3;c++){
        ddp[c] = new double[256]();
        ddpBloc[c] = new double[256]();
        F[c] = new double[256]();
        FBloc[c] = new double[256]();
        T[c] = new double[256]();
    }

    for(int i = 0;i < blocH * blocW;i++){
        for(int c = 0;c < 3;c++){
            int idx = i*3 + c;
            ddp[c][Img[idx]] += 1.0;
            ddpBloc[c][Bloc[idx]] += 1.0;
        }
    }

    for(int c = 0;c < 3;c++){
        for(int i = 0;i < 256;i++){
            ddp[c][i] /= (double)(blocH * blocW);
            ddpBloc[c][i] /= (double)(blocH * blocW);
        }
    }

    for(int c = 0;c < 3;c++){
        F[c][0] = ddp[c][0];
        FBloc[c][0] = ddpBloc[c][0];
        for(int i = 1;i < 256;i++){
            F[c][i] = F[c][i - 1] + ddp[c][i];
            FBloc[c][i] = FBloc[c][i - 1] + ddpBloc[c][i];
        }
    }

    for(int c = 0;c < 3;c++){
        for(int i = 0;i < 256;i++){
            float diffMin = 1.0f;
            int jBest = 0;
            for(int j = 0;j < 256;j++){
                float diff = fabs(F[c][i] - FBloc[c][j]);
                if(diff < diffMin){
                    diffMin = diff;
                    jBest = j;
                }
            }
            T[c][i] = jBest;
        }
    }

    for(int i = 0;i < blocH * blocW;i++) {
        for(int c = 0;c < 3;c++){
            int idx = i*3 + c;
            Spec[idx] = T[c][Img[idx]];
        }
    }

    for(int c = 0;c < 3;c++){
        delete[] ddp[c];
        delete[] ddpBloc[c];
        delete[] F[c];
        delete[] FBloc[c];
        delete[] T[c];
    }
}

int main(int argc, char* argv[]) {
    char cNomImgLue[250], cNomImgEcrite[250];
    int nH, nW, nTaille, tailleBloc, nbrImagettes;
    
    if (argc != 5) {
        printf("Usage: ImageIn.ppm ImageOut.ppm TailleBloc nbrImagettes \n"); 
        exit(1);
    }
    
    sscanf(argv[1], "%s", cNomImgLue);
    sscanf(argv[2], "%s", cNomImgEcrite);
    sscanf(argv[3], "%d", &tailleBloc);
    sscanf(argv[4], "%d", &nbrImagettes);

    OCTET *ImgIn, *ImgOut;
    
    lire_nb_lignes_colonnes_image_ppm(cNomImgLue, &nH, &nW);
    nTaille = nH * nW * 3;
    
    allocation_tableau(ImgIn, OCTET, nTaille);
    lire_image_ppm(cNomImgLue, ImgIn, nH * nW);
    allocation_tableau(ImgOut, OCTET, nTaille);

    time_t start_time_recup;
    time(&start_time_recup);
    printf("Récupération des images...\n");
    
    Image imagettes[nbrImagettes];

    for (int i = 1; i <= nbrImagettes; i++) {
        int nH_i,nW_i,nTaille_i;
        std::string fichier = "banque_couleur/"+std::to_string(i)+".ppm";
        lire_nb_lignes_colonnes_image_ppm((char*)fichier.c_str(),&nH_i,&nW_i);
        
        nTaille_i = nW_i*nH_i*3;
        OCTET *Img;
        allocation_tableau(Img, OCTET,nTaille_i);
        lire_image_ppm((char*)fichier.c_str(),Img,nW_i*nH_i);

        double sommeR = 0, sommeG = 0, sommeB = 0;
        for(int j = 0;j < nTaille_i;j += 3){
            sommeR += Img[j];
            sommeG += Img[j+1];
            sommeB += Img[j+2];
        }
        imagettes[i-1] = Image(i,sommeR/(nTaille_i/3),sommeG/(nTaille_i/3), sommeB/(nTaille_i/3));
        free(Img);
    }

    time_t end_time_recup;
    time(&end_time_recup);
    printf("Temps récupération : %.3f secondes.\n",difftime(end_time_recup,start_time_recup));

    time_t start_time_modif;
    time(&start_time_modif);
    printf("Remplacement des blocs...\n");

    int nombre_bloc_hauteur = nH / tailleBloc;
    int nombre_bloc_largeur = nW / tailleBloc;

    for(int i = 0; i < nombre_bloc_hauteur; i++) {
        for(int j = 0; j < nombre_bloc_largeur; j++) {
            double sommeR = 0, sommeG = 0, sommeB = 0;
            int cpt = 0;
    
            for(int k = 0; k < tailleBloc; k++) {
                for(int l = 0; l < tailleBloc; l++) {
                    int index = ((i * tailleBloc + k) * nW + (j * tailleBloc + l)) * 3;
                    sommeR += ImgIn[index];
                    sommeG += ImgIn[index + 1];
                    sommeB += ImgIn[index + 2];
                    cpt++;
                }
            }
    
            double moyenneR = sommeR / cpt;
            double moyenneG = sommeG / cpt;
            double moyenneB = sommeB / cpt;
            double compare = FLT_MAX;
            int best = -1;
    
            for(int k = 0; k < nbrImagettes; k++) {
                double dist = std::sqrt(
                    std::pow(imagettes[k].moyenneR - moyenneR, 2) +
                    std::pow(imagettes[k].moyenneG - moyenneG, 2) +
                    std::pow(imagettes[k].moyenneB - moyenneB, 2)
                );
                if(dist < compare) {
                    compare = dist;
                    best = imagettes[k].numero;
                }
            }
    
            // Remplacement du bloc par l'imagette correspondante
            OCTET *Imagette;
            int nH_imagette, nW_imagette, nTaille_imagette;
            std::string fichier = "banque_couleur/" + std::to_string(best) + ".ppm";
            lire_nb_lignes_colonnes_image_ppm((char*)fichier.c_str(), &nH_imagette, &nW_imagette);
    
            nTaille_imagette = nW_imagette * nH_imagette * 3;
            allocation_tableau(Imagette, OCTET, nTaille_imagette);
            lire_image_ppm((char*)fichier.c_str(), Imagette, nW_imagette * nH_imagette);
    
            OCTET *Temp;
            allocation_tableau(Temp,OCTET,tailleBloc*tailleBloc*3);

            // Resize des images pour correspondre au bloc
            for(int k = 0; k < tailleBloc; k++){
                for(int l = 0; l < tailleBloc; l++){
                    int x = (k*nH_imagette)/tailleBloc;
                    int y = (l*nW_imagette)/tailleBloc;

                    int index = (x * nW_imagette + y) * 3;
                    int srcIndex = (k * tailleBloc + l) * 3;
            
                    Temp[srcIndex] = Imagette[index];
                    Temp[srcIndex+1] = Imagette[index+1];
                    Temp[srcIndex+2] = Imagette[index+2];
                }
            }

            OCTET *Bloc;
            allocation_tableau(Bloc, OCTET, tailleBloc * tailleBloc * 3);
            for(int k = 0; k < tailleBloc; k++) {
                for(int l = 0; l < tailleBloc; l++) {
                    int x = i*tailleBloc+k;
                    int y = j*tailleBloc+l;

                    int index = (x * nW + y) * 3;
                    int srcIndex = (k * tailleBloc + l) * 3;
    
                    Bloc[srcIndex] = ImgIn[index];
                    Bloc[srcIndex + 1] = ImgIn[index + 1];
                    Bloc[srcIndex + 2] = ImgIn[index + 2];
                }
            }
    
            OCTET *Spec;
            allocation_tableau(Spec, OCTET, tailleBloc * tailleBloc * 3);
            specification_couleur(Temp, Bloc, Spec, tailleBloc, tailleBloc);
    
            for(int k = 0; k < tailleBloc; k++) {
                for(int l = 0; l < tailleBloc; l++) {
                    int x = i*tailleBloc+k;
                    int y = j*tailleBloc+l;

                    int index = (x * nW + y) * 3;
                    int srcIndex = (k * tailleBloc + l) * 3;

                    ImgOut[index] = Spec[srcIndex];
                    ImgOut[index + 1] = Spec[srcIndex + 1];
                    ImgOut[index + 2] = Spec[srcIndex + 2];
                }
            }
            free(Imagette);
            free(Bloc);
            free(Spec);
        }
    }
    
    time_t end_time_modif;
    time(&end_time_modif);
    double duration_modif = difftime(end_time_modif,start_time_modif);

    printf("Temps remplacement : %.3f secondes.\n",duration_modif);

    ecrire_image_ppm(cNomImgEcrite,ImgOut,nH,nW);

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
