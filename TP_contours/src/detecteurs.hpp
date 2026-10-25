// Les trois détecteurs de contours : Sobel, Laplace et Canny.
// Chacun prend une image en niveaux de gris (CV_8U) et rend une carte binaire :
// 255 = pixel de contour, 0 = fond.
#pragma once

#include <opencv2/core.hpp>

// Les valeurs par défaut sont les meilleures trouvées sur les 40 images avec la métrique du sujet.

struct ParamsSobel {
    int blur  = 5;   // taille du flou gaussien (impaire, 1 = pas de flou)
    int ksize = 3;   // taille du noyau de Sobel : 1, 3, 5 ou 7
    int seuil = 70;  // seuil sur la magnitude du gradient ramenée entre 0 et 255
};

struct ParamsLaplace {
    int blur  = 5;   // taille du flou gaussien (impaire, 1 = pas de flou)
    int ksize = 3;   // taille du noyau du Laplacien : 1, 3, 5 ou 7
    int seuil = 40;  // seuil sur |Laplacien| ramené entre 0 et 255
};

struct ParamsCanny {
    int blur     = 7;    // taille du flou gaussien (impaire, 1 = pas de flou)
    int low      = 50;   // seuil bas de l'hystérésis
    int high     = 150;  // seuil haut de l'hystérésis
    int aperture = 3;    // taille du Sobel utilisé par Canny : 3, 5 ou 7
};

cv::Mat detecterSobel(const cv::Mat& gris, const ParamsSobel& p);
cv::Mat detecterLaplace(const cv::Mat& gris, const ParamsLaplace& p);
cv::Mat detecterCanny(const cv::Mat& gris, const ParamsCanny& p);
