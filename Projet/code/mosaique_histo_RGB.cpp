#include <array>
#include <stdio.h>
#include <string>
#include <cmath>
#include <float.h>
#include <ctime>
#include <vector>
#include "image_ppm.h"

struct Image {
    int numero;
    double moyenne;
    std::vector<int> histoR, histoG, histoB;

    Image(int n = 0, double m = 0.0,
          std::vector<int> hR = std::vector<int>(256, 0),
          std::vector<int> hG = std::vector<int>(256, 0),
          std::vector<int> hB = std::vector<int>(256, 0))
        : numero(n), moyenne(m), histoR(hR), histoG(hG), histoB(hB) {}
};

void redimensionner_image(OCTET* ImgIn, OCTET* ImgOut, int nH_i, int nW_i, int tailleBloc) {
    for (int k = 0; k < tailleBloc; k++) {
        for (int l = 0; l < tailleBloc; l++) {
            int x_source = (k * nH_i) / tailleBloc;
            int y_source = (l * nW_i) / tailleBloc;
            if (x_source < nH_i && y_source < nW_i) {
                for (int c = 0; c < 3; c++) {
                    ImgOut[(k * tailleBloc + l) * 3 + c] = ImgIn[(x_source * nW_i + y_source) * 3 + c];
                }
            }
        }
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

    std::vector<Image> listeImages;

    time_t start_time_recup;
    time(&start_time_recup);
    printf("Récupération des images...\n");

    for (int i = 1; i <= nbrImagettes; i++) {
        int nH_i, nW_i, nTaille_i;
        OCTET *Img;
        std::vector<int> histoR(256, 0), histoG(256, 0), histoB(256, 0);

        std::string fichier = "banque_couleur/" + std::to_string(i) + ".ppm";
        lire_nb_lignes_colonnes_image_ppm((char*)fichier.c_str(), &nH_i, &nW_i);

        nTaille_i = nW_i * nH_i * 3;
        allocation_tableau(Img, OCTET, nTaille_i);
        lire_image_ppm((char*)fichier.c_str(), Img, nW_i * nH_i);

        for (int j = 0; j < nTaille_i; j += 3) {
            histoR[Img[j]]++;
            histoG[Img[j + 1]]++;
            histoB[Img[j + 2]]++;
        }

        listeImages.push_back({i, 0.0, histoR, histoG, histoB});

        free(Img);
    }

    time_t end_time_recup;
    time(&end_time_recup);
    double duration_recup = difftime(end_time_recup, start_time_recup);

    printf("Temps récupération : %.3f secondes.\n", duration_recup);

    time_t start_time_modif;
    time(&start_time_modif);
    printf("Remplacement des blocs...\n");

    for (int i = 0; i < nH / tailleBloc; i++) {
        for (int j = 0; j < nW / tailleBloc; j++) {
            std::vector<int> histoR(256, 0), histoG(256, 0), histoB(256, 0);

            for (int k = 0; k < tailleBloc; k++) {
                for (int l = 0; l < tailleBloc; l++) {
                    int x = i * tailleBloc + k;
                    int y = j * tailleBloc + l;
                    histoR[ImgIn[(x * nW + y) * 3]]++;
                    histoG[ImgIn[(x * nW + y) * 3 + 1]]++;
                    histoB[ImgIn[(x * nW + y) * 3 + 2]]++;
                }
            }

            double compare = FLT_MAX;
            int best = -1;

            for (int k = 0; k < nbrImagettes; k++) {
                std::vector<double> comp(3, 0.0);

                for (int l = 0; l < 256; l++) {
                    comp[0] += sqrt(histoR[l] * listeImages[k].histoR[l]);
                    comp[1] += sqrt(histoG[l] * listeImages[k].histoG[l]);
                    comp[2] += sqrt(histoB[l] * listeImages[k].histoB[l]);
                }

                double somme = 0.0;

                for (int c = 0; c < 3; c++) {
                    if (comp[c] > 0) {
                        comp[c] = -log(comp[c]);
                    }
                    somme += comp[c];
                }

                somme /= 3.0;

                if (somme < compare) {
                    compare = somme;
                    best = k;
                }
            }

            OCTET* Imagette_resize;
            OCTET* Imagette;
            int nH_i2, nW_i2, nTaille_i2;

            std::string fichier = "banque_couleur/" + std::to_string(best + 1) + ".ppm";
            lire_nb_lignes_colonnes_image_ppm((char*)fichier.c_str(), &nH_i2, &nW_i2);
            nTaille_i2 = nH_i2 * nW_i2 * 3;
            allocation_tableau(Imagette, OCTET, nTaille_i2);
            lire_image_ppm((char*)fichier.c_str(), Imagette, nW_i2 * nH_i2);

            allocation_tableau(Imagette_resize, OCTET, tailleBloc * tailleBloc * 3);
            redimensionner_image(Imagette, Imagette_resize, nH_i2, nW_i2, tailleBloc);

            for (int k = 0; k < tailleBloc; k++) {
                for (int l = 0; l < tailleBloc; l++) {
                    int x = i * tailleBloc + k;
                    int y = j * tailleBloc + l;
                    for (int c = 0; c < 3; c++) {
                        ImgOut[(x * nW + y) * 3 + c] = Imagette_resize[(k * tailleBloc + l) * 3 + c];
                    }
                }
            }

            free(Imagette_resize);
            free(Imagette);
        }
    }

    time_t end_time_modif;
    time(&end_time_modif);
    double duration_modif = difftime(end_time_modif, start_time_modif);

    printf("Temps remplacement : %.3f secondes.\n", duration_modif);

    ecrire_image_ppm(cNomImgEcrite, ImgOut, nH, nW);

    double eqm = 0.0;
    for (int i = 0; i < nTaille; i++) {
        double diff = ImgIn[i] - ImgOut[i];
        eqm += diff * diff;
    }
    eqm /= nTaille;
    if (eqm == 0) {
        printf("Images similaires : eqm = 0");
        return 0;
    }

    double psnr = 10 * std::log10((255 * 255) / eqm);
    printf("PSNR : %.2f dB\n", psnr);

    free(ImgIn);
    free(ImgOut);
    return EXIT_SUCCESS;
}
