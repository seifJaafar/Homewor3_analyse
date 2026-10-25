// Évaluation d'une carte de contours par rapport à la vérité terrain (métrique du sujet).
#pragma once

#include <opencv2/core.hpp>

#include <string>

struct Mesures {
    int detectes     = 0;  // pixels de contour détectés
    int reference    = 0;  // pixels de contour de la vérité terrain
    int corrects     = 0;  // pixels détectés à moins d'1 pixel d'un contour de référence
    int fauxPositifs = 0;  // pixels détectés loin de tout contour de référence
    int fauxNegatifs = 0;  // pixels de référence sans détection à moins d'1 pixel
    double P   = 0;        // corrects / (corrects + FP + FN)
    double TFP = 0;        // FP       / (corrects + FP + FN)
    double TFN = 0;        // FN       / (corrects + FP + FN)
};

// Lit une vérité terrain (contours noirs sur fond blanc) et rend un masque : contours = 255, fond = 0.
// Rend une image vide si le fichier ne peut pas être lu.
cv::Mat loadReference(const std::string& chemin);

// Compare une carte de contours détectés (255 = contour) au masque de référence, avec une tolérance de 1 pixel.
Mesures evaluate(const cv::Mat& detectes, const cv::Mat& reference);

// Image couleur sur l'original assombri : vert = correct, rouge = faux positif, bleu = contour manqué.
cv::Mat makeOverlay(const cv::Mat& gris, const cv::Mat& detectes, const cv::Mat& reference);
