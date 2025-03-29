#include <vector>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <cfloat>
#include <string>
#include <array>
#include "ImageBase.h"

using namespace std;

struct ImagetteData {
    size_t index;
	ImageBase imagette;
    array<int, 256> greyLevels;
    
    ImagetteData(size_t index, const ImageBase& imagette, const array<int, 256>& greyLevels)
        : index(index), imagette(imagette), greyLevels(greyLevels) {}
};

unsigned char getMoyennePixel(ImageBase& imIn, int x, int y) {
	unsigned long sommePixels = 0;
	unsigned int count = 0;
	for(int j = -1; j < 2; j++) {
		for (int i = -1; i < 2; i++) {
			if(x+i >= 0 && y+j >= 0 && x+i < imIn.getWidth() && y+j < imIn.getHeight()) {
				sommePixels += imIn[y+j][x+i];
				count++;
			}
		}
	}
	return clamp(double(sommePixels) / double(count), 0., 255.);
}

void filtreMoyenneur(ImageBase& imIn, ImageBase& imOut) {
	for(int y = 0; y < imIn.getHeight(); y++) {
		for(int x = 0; x < imIn.getWidth(); x++) {
			imOut[y][x] = getMoyennePixel(imIn, x, y);
		}
	}
}

int getLaplacienPixel(ImageBase& imIn, int x, int y) {
    int sum = 0;
    int kernel[3][3] = {
        {0, -1, 0},
        {-1, 4, -1},
        {0, -1, 0}
    };
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            int nx = x + j;
            int ny = y + i;

            if (nx >= 0 && nx < imIn.getWidth() && ny >= 0 && ny < imIn.getHeight()) {
                sum += imIn[ny][nx] * kernel[i + 1][j + 1];
            }
        }
    }
    return clamp(sum, 0, 255);
}

void filtreLaplacien(ImageBase& imIn, ImageBase& imOut) {
	for(int y = 0; y < imIn.getHeight(); y++) {
		for(int x = 0; x < imIn.getWidth(); x++) {
			imOut[y][x] = getLaplacienPixel(imIn, x, y);
		}
	}
}

unsigned char getInfiniteNorm(ImageBase& imIn, int x, int y) {
    int Ai = (x+1 < imIn.getWidth() ? imIn[y][x+1] : imIn[y][x-1]) - imIn[y][x];
    int Aj = (y+1 < imIn.getHeight() ? imIn[y+1][x] : imIn[y-1][x]) - imIn[y][x];
    return max(abs(Ai), abs(Aj));
}

void createGradientByPhase(ImageBase& imIn, ImageBase& imOut) {
    for (int y = 0; y < imIn.getHeight(); y++) {
        for (int x = 0; x < imIn.getWidth(); x++) {
            imOut[y][x] = getInfiniteNorm(imIn, x, y);
        }
    }
}

unsigned char getGradientNorm(ImageBase& imIn, int x, int y) {
	int horizontalGradient = x-1 < 0 ? imIn[y][x+1] - imIn[y][x] : imIn[y][x-1] - imIn[y][x];
	int verticalGradient = y-1 < 0 ? imIn[y+1][x] - imIn[y][x] : imIn[y-1][x] - imIn[y][x];
	horizontalGradient = clamp(horizontalGradient, 0, 255);
	verticalGradient = clamp(verticalGradient, 0, 255);
	return sqrt(pow(horizontalGradient, 2) + pow(verticalGradient, 2));
}

void createImageGradientPassage0(ImageBase& imLaplacien, ImageBase& imPassage, ImageBase& imGradient) {
	for(int y = 0; y < imLaplacien.getHeight(); y++) {
		for(int x = 0; x < imLaplacien.getWidth(); x++) {
			imGradient[y][x] = imPassage[y][x] == 0 ? getGradientNorm(imLaplacien, x, y) : 0;
		}
	}
}

void seuillageHysteresis(ImageBase& imIn, ImageBase& imOut, int sB, int sH) {
	vector<vector<int>> bordsFaibles;
	for(int y = 0; y < imIn.getHeight(); y++) {
		for(int x = 0; x < imIn.getWidth(); x++) {
			if(imIn[y][x] > sH) {
				imOut[y][x] = 255;
			}
			else if(imIn[y][x] < sB) {
				imOut[y][x] = 0;
			}
			else {
				bordsFaibles.push_back({x, y});
			}
		}
	}
	for(int i = 0; i < bordsFaibles.size(); i++) {
		int x = bordsFaibles[i][0];
		int y = bordsFaibles[i][1];
		for(int j = -1; j < 2; j+=2) {
			if(x+j >= 0 && x+j < imIn.getHeight()) {
				if(imIn[y][x+j] > sH) imOut[y][x] = 255;
				else imOut[y][x] = 0;
			}
			if(y+j >= 0 && y+j < imIn.getHeight()) {
				if(imIn[y+j][x] > sH) imOut[y][x] = 255;
				else imOut[y][x] = 0;
			}
		}
	}
}

void compileImagettesData(vector<ImagetteData>& imagettesList, int blockSize, int nbrImagettes) {
	ImageBase imagette;
	ImageBase imResize(blockSize, blockSize, false);
	ImageBase imFlou(blockSize, blockSize, false);
	ImageBase imLaplacien(blockSize, blockSize, false);
	ImageBase imPassage0(blockSize, blockSize, false);
	ImageBase imGradient(blockSize, blockSize, false);
	ImageBase imSeuil(blockSize, blockSize, false);

	array<int, 256> greyLevels{0};
	
	for (size_t i = 1; i <= nbrImagettes; i++) {
		string fichier = "banque/" + to_string(i) + ".pgm";
		imagette.load((char*)fichier.c_str());
    
        // Redimension de l'imagette
        for(size_t j = 0; j < blockSize; j++) {
            for(size_t k = 0; k < blockSize; k++) {
                int x = (k * imagette.getWidth()) / blockSize;
                int y = (j * imagette.getHeight()) / blockSize;
				imResize[j][k] = imagette[y][x];
            }
        }
    
        // Calcul de l'image des contours
        filtreMoyenneur(imResize, imFlou);
		filtreLaplacien(imFlou, imLaplacien);
		createGradientByPhase(imLaplacien, imPassage0);
		createImageGradientPassage0(imLaplacien, imPassage0, imGradient);
		seuillageHysteresis(imGradient, imSeuil, 2, 3);

		// Calcul de l'histogramme
		for(size_t j = 0; j < blockSize; j++) {
            for(size_t k = 0; k < blockSize; k++) {
                greyLevels[imSeuil[j][k]]++;
            }
        }
    
        // Stockage des données de l'imagette
        imagettesList.push_back({i, imResize, greyLevels});
	}
}

void blockReplacement(ImageBase& imIn, ImageBase& imOut, vector<ImagetteData>& imagettesList, int blockSize, int nbrImagettes) {
	size_t blockWidthNb = imIn.getWidth() / blockSize;
	size_t blockHeightNb = imIn.getHeight() / blockSize;

	ImageBase imBlock(blockSize, blockSize, false);
	ImageBase imFlou(blockSize, blockSize, false);
	ImageBase imLaplacien(blockSize, blockSize, false);
	ImageBase imPassage0(blockSize, blockSize, false);
	ImageBase imGradient(blockSize, blockSize, false);
	ImageBase imSeuil(blockSize, blockSize, false);
	ImageBase imagette(blockSize, blockSize, false);

	array<int,256> greyLevels{0};

    for (size_t i = 0; i < blockHeightNb; i++) {
		for (size_t j = 0; j < blockWidthNb; j++) {
			cout << "i : " << i << ", j : " << j << endl;
			
			// Création du bloc
			for (size_t k = 0; k < blockSize; k++) {
				for (size_t l = 0; l < blockSize; l++) {
					int x = j * blockWidthNb + l;
					int y = i * blockHeightNb + k;
					imBlock[k][l] = imIn[y][x];
				}
			}

            // Calcul de l'image des contours
			filtreMoyenneur(imBlock, imFlou);
			filtreLaplacien(imFlou, imLaplacien);
			createGradientByPhase(imLaplacien, imPassage0);
			createImageGradientPassage0(imLaplacien, imPassage0, imGradient);
			seuillageHysteresis(imGradient, imSeuil, 2, 3);

			// Calcul de l'histogramme
			for (size_t k = 0; k < blockSize; k++) {
				for (size_t l = 0; l < blockSize; l++) {
					greyLevels[imSeuil[k][l]]++;
				}
			}

			// Comparaison des histogrammes par bloc
            double closest = DBL_MAX;
			double current;
            size_t bestImagette;
            for (size_t k = 0; k < nbrImagettes; k++) {
                current = 0.;
                for (size_t l = 0; l < 256; l++) {
                    current += sqrt(greyLevels[l] * imagettesList[k].greyLevels[l]);
                }
                current = -log(current);
                if (current < closest) {
                    closest = current;
                    bestImagette = k;
                }
            }

            // Remplacement du bloc par l'imagette
			imagette = imagettesList[bestImagette].imagette;
            for (size_t k = 0; k < blockSize; k++) {
                for (size_t l = 0; l < blockSize; l++) {
					int x = j * blockWidthNb + l;
					int y = i * blockHeightNb + k;
					imOut[y][x] = imagette[k][l];
                }
            }
		}
	}
}

int main(int argc, char* argv[]) {
    char cNomImgLue[250], cNomImgEcrite[250];
    int blockSize, nbrImagettes;

    if (argc != 5) {
        printf("Usage: ImageIn.ppm ImageOut.ppm TailleBloc nbrImagettes \n");
        exit(1);
    }

    sscanf(argv[1], "%s", cNomImgLue);
    sscanf(argv[2], "%s", cNomImgEcrite);
    sscanf(argv[3], "%d", &blockSize);
    sscanf(argv[4], "%d", &nbrImagettes);

	ImageBase imIn;
	imIn.load(cNomImgLue);

	ImageBase imOut(imIn.getWidth(), imIn.getHeight(), imIn.getColor());

	vector<ImagetteData> imagettesList;

	time_t startTime;
    time(&startTime);
    cout << "Récupération des images..." << endl;

	compileImagettesData(imagettesList, blockSize, nbrImagettes);

	time_t endTime;
    time(&endTime);
    double duration = difftime(endTime, startTime);

    cout << "Temps récupération : " << duration << " secondes." << endl;

    time(&startTime);
    cout << "Remplacement des blocs..." << endl;

	blockReplacement(imIn, imOut, imagettesList, blockSize, nbrImagettes);

    time(&endTime);
    duration = difftime(endTime, startTime);

	cout << "Temps récupération : " << duration << " secondes." << endl;

	imOut.save(cNomImgEcrite);

	return 0;
}