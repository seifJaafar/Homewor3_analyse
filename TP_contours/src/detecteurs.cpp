#include "detecteurs.hpp"

#include <opencv2/imgproc.hpp>

static cv::Mat flouter(const cv::Mat& gris, int taille)
{
    if (taille <= 1)
        return gris;
    cv::Mat floue;
    cv::GaussianBlur(gris, floue, cv::Size(taille, taille), 0);
    return floue;
}


static cv::Mat seuillerNormalise(const cv::Mat& reponse, int seuil)
{
    cv::Mat normalisee, contours;
    cv::normalize(reponse, normalisee, 0, 255, cv::NORM_MINMAX);
    cv::compare(normalisee, seuil, contours, cv::CMP_GT);  // 255 si valeur > seuil, 0 sinon
    return contours;
}

cv::Mat detecterSobel(const cv::Mat& gris, const ParamsSobel& p)
{
    const cv::Mat image = flouter(gris, p.blur);

   
    cv::Mat gx, gy, magnitude;
    cv::Sobel(image, gx, CV_32F, 1, 0, p.ksize);
    cv::Sobel(image, gy, CV_32F, 0, 1, p.ksize);
    cv::magnitude(gx, gy, magnitude);  // sqrt(gx² + gy²)

    return seuillerNormalise(magnitude, p.seuil);
}

cv::Mat detecterLaplace(const cv::Mat& gris, const ParamsLaplace& p)
{
    
    const cv::Mat image = flouter(gris, p.blur);

    cv::Mat laplacien;
    cv::Laplacian(image, laplacien, CV_32F, p.ksize);

    
    return seuillerNormalise(cv::abs(laplacien), p.seuil);
}

cv::Mat detecterCanny(const cv::Mat& gris, const ParamsCanny& p)
{
    const cv::Mat image = flouter(gris, p.blur);

   
    cv::Mat contours;
    cv::Canny(image, contours, p.low, p.high, p.aperture);
    return contours;
}
