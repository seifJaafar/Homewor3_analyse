#include "evaluation.hpp"

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

// La tolérance de 1 pixel est obtenue en dilatant avec un carré 3x3
static cv::Mat dilater(const cv::Mat& masque)
{
    const cv::Mat carre = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::Mat dilate;
    cv::dilate(masque, dilate, carre);
    return dilate;
}

cv::Mat loadReference(const std::string& chemin)
{
    const cv::Mat verite = cv::imread(chemin, cv::IMREAD_GRAYSCALE);
    if (verite.empty())
        return verite;

    // Les contours sont noirs dans le fichier : on inverse, puis on re-binarise par sécurité
    cv::Mat masque;
    cv::bitwise_not(verite, masque);
    cv::threshold(masque, masque, 127, 255, cv::THRESH_BINARY);
    return masque;
}

Mesures evaluate(const cv::Mat& detectes, const cv::Mat& reference)
{
    const cv::Mat referenceDilatee = dilater(reference);
    const cv::Mat detectesDilates = dilater(detectes);

    Mesures m;
    m.detectes     = cv::countNonZero(detectes);
    m.reference    = cv::countNonZero(reference);
    m.corrects     = cv::countNonZero(detectes & referenceDilatee);
    m.fauxPositifs = m.detectes - m.corrects;
    m.fauxNegatifs = cv::countNonZero(reference & ~detectesDilates);

    const int total = m.corrects + m.fauxPositifs + m.fauxNegatifs;
    if (total > 0) {
        m.P   = static_cast<double>(m.corrects) / total;
        m.TFP = static_cast<double>(m.fauxPositifs) / total;
        m.TFN = static_cast<double>(m.fauxNegatifs) / total;
    }
    return m;
}

cv::Mat makeOverlay(const cv::Mat& gris, const cv::Mat& detectes, const cv::Mat& reference)
{
    const cv::Mat referenceDilatee = dilater(reference);
    const cv::Mat detectesDilates = dilater(detectes);

    // Original assombri de moitié, en couleur, pour que les contours ressortent
    cv::Mat sombre, image;
    gris.convertTo(sombre, CV_8U, 0.5);
    cv::cvtColor(sombre, image, cv::COLOR_GRAY2BGR);

    // Couleurs en BGR
    image.setTo(cv::Scalar(255, 0, 0), reference & ~detectesDilates);  // bleu : contour manqué
    image.setTo(cv::Scalar(0, 0, 255), detectes & ~referenceDilatee);  // rouge : faux positif
    image.setTo(cv::Scalar(0, 255, 0), detectes & referenceDilatee);   // vert : correct
    return image;
}
