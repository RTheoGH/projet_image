#include <array>
#include <stdio.h>
#include <string>
#include <cmath>
#include <float.h>
#include <ctime>
#include <vector>
#include "image_ppm.h"

struct Image{
    int numero;
    double moyenne;
    std::vector<int> histo;
    
    Image(int n=0,double m=0.0,std::vector<int> h = std::vector<int>(256,0)):
        numero(n),moyenne(m),histo(h){}
};


int main(int argc, char* argv[])
{
    char cNomImgLue[250], cNomImgEcrite[250];
    int nH, nW, nTaille, tailleBloc, nbrImagettes;
    
    if (argc != 5) {
        printf("Usage: ImageIn.pgm ImageOut.pgm TailleBloc nbrImagettes \n"); 
        exit (1) ;
    }
    
    sscanf(argv[1],"%s",cNomImgLue);
    sscanf(argv[2],"%s",cNomImgEcrite);
    sscanf(argv[3],"%d",&tailleBloc);
    sscanf(argv[4],"%d",&nbrImagettes);

    OCTET *ImgIn,*ImgOut;
    
    lire_nb_lignes_colonnes_image_pgm(cNomImgLue,&nH,&nW);
    nTaille = nH * nW;
    
    allocation_tableau(ImgIn,OCTET,nTaille);
    lire_image_pgm(cNomImgLue,ImgIn,nH*nW);
    allocation_tableau(ImgOut,OCTET,nTaille);

    std::vector<Image> listeImages;
    std::vector<std::vector<OCTET>> listeImagesData;

    time_t start_time_recup;
    time(&start_time_recup);
    printf("Récupération des images...\n");

    //Histogramme de chaque image dans la banque
    for(int i = 1;i <= nbrImagettes;i++){
        int nH_i, nW_i, nTaille_i;
        OCTET *Img, *ImgR;
        std::array<int, 256> histoI{0};
    
        std::string fichier = "banque/" + std::to_string(i) + ".pgm";
        lire_nb_lignes_colonnes_image_pgm((char*)fichier.c_str(),&nH_i,&nW_i);
    
        nTaille_i = nW_i * nH_i;
        allocation_tableau(Img,OCTET,nTaille_i);
        lire_image_pgm((char*)fichier.c_str(),Img,nTaille_i);
        allocation_tableau(ImgR,OCTET,nTaille_i);
    
        // Redimensionnement de l'image
        for(int k = 0;k < tailleBloc;k++){
            for(int l = 0;l < tailleBloc;l++){
                int x_source = (k * nH_i) / tailleBloc;
                int y_source = (l * nW_i) / tailleBloc;
                ImgR[k*tailleBloc+l] = Img[x_source*nW_i+y_source];
            }
        }
    
        // Calcul de l'histogramme
        int tB2 = tailleBloc * tailleBloc;
        for(int j = 0;j < tB2;j++){
            histoI[ImgR[j]]++;
        }
    
        // Stockage des données de l'imagette
        listeImages.push_back({i,0.0,std::vector<int>(histoI.begin(),histoI.end())});
        listeImagesData.push_back(std::vector<OCTET>(ImgR,ImgR+tB2));
    
        free(Img);
        free(ImgR);
    }

    time_t end_time_recup;
    time(&end_time_recup);
    double duration_recup = difftime(end_time_recup,start_time_recup);

    printf("Temps récupération : %.3f secondes.\n",duration_recup);

    time_t start_time_modif;
    time(&start_time_modif);
    printf("Remplacement des blocs...\n");

    int nombre_bloc_hauteur = nH / tailleBloc;
    int nombre_bloc_largeur = nW / tailleBloc;

    for(int i = 0;i < nombre_bloc_hauteur; i++){
        for(int j = 0;j < nombre_bloc_largeur; j++){
            std::array<int,256> histoB{0};
    
            for(int k = 0;k < tailleBloc;k++){
                for(int l = 0;l < tailleBloc;l++) {
                    int x = i * tailleBloc + k;
                    int y = j * tailleBloc + l;
                    histoB[ImgIn[x*nW+y]]++;
                }
            }
    
            double compare = FLT_MAX;
            int best = -1;
    
            for(int k = 0;k < nbrImagettes;k++) {
                double comp = 0.0;
                for (int l = 0;l < 256;l++){
                    comp += sqrt(histoB[l]*listeImages[k].histo[l]);
                }
                comp = -log(comp);
                if(comp < compare){
                    compare = comp;
                    best = k;
                }
            }

            // Remplacement du bloc par l'imagette
            std::vector<OCTET> Imagette = listeImagesData[best];
            for(int k = 0;k < tailleBloc;k++){
                for(int l = 0;l < tailleBloc;l++) {
                    int x = i * tailleBloc + k;
                    int y = j * tailleBloc + l;
                    ImgOut[x*nW+y] = Imagette[k*tailleBloc+l];
                }
            }
        }
    }

    time_t end_time_modif;
    time(&end_time_modif);
    double duration_modif = difftime(end_time_modif,start_time_modif);

    printf("Temps remplacement : %.3f secondes.\n",duration_modif);

    ecrire_image_pgm(cNomImgEcrite,ImgOut,nH,nW);

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
    return EXIT_SUCCESS;
}