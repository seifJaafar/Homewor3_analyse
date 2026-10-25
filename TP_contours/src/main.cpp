// TP Détection de contours : Sobel, Laplace et Canny sur la base RUG (40 images + vérités terrain).
//
//   tp_contours           mode interactif : trackbars, mesures P / TFP / TFN sur chaque vignette
//   tp_contours --batch   traite les 40 images avec les paramètres par défaut et enregistre les résultats
//
// Les dossiers data/ et results/ sont relatifs au dossier TP_contours.

#include "detecteurs.hpp"
#include "evaluation.hpp"

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <algorithm>
#include <array>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

const char* const NOMS[3] = {"Sobel", "Laplace", "Canny"};
const char* const FICHIERS[3] = {"sobel", "laplace", "canny"};  // noms de dossiers et de fichiers
const int TOUS = 3;                 // valeur de la trackbar "Detecteur" qui affiche les trois
const int LARGEUR_MAX = 1280;       // au-delà, l'affichage est réduit (les calculs restent en taille réelle)

// ================================================================ Images, détection, affichage

// Noms des images de data/images, sans extension et triés : basket, bear, bear_2, ...
std::vector<std::string> listerImages()
{
    std::vector<std::string> noms;
    for (const auto& fichier : fs::directory_iterator("data/images"))
        if (fichier.path().extension() == ".pgm")
            noms.push_back(fichier.path().stem().string());
    std::sort(noms.begin(), noms.end());
    return noms;
}

cv::Mat lireImage(const std::string& nom)
{
    return cv::imread("data/images/" + nom + ".pgm", cv::IMREAD_GRAYSCALE);
}

cv::Mat lireReference(const std::string& nom)
{
    return loadReference("data/reference/" + nom + "_gt_binary.pgm");
}

// Applique les trois détecteurs à une image, dans l'ordre Sobel, Laplace, Canny
std::array<cv::Mat, 3> detecterTout(const cv::Mat& gris, const ParamsSobel& s, const ParamsLaplace& l, const ParamsCanny& c)
{
    return {detecterSobel(gris, s), detecterLaplace(gris, l), detecterCanny(gris, c)};
}

std::string texteMesures(const Mesures& m)
{
    char texte[80];
    std::snprintf(texte, sizeof texte, "P %.1f%%   TFP %.1f%%   TFN %.1f%%", 100 * m.P, 100 * m.TFP, 100 * m.TFN);
    return texte;
}

// Image (grise ou couleur) sous un bandeau sombre : un titre et, en dessous, une ligne de mesures
cv::Mat vignette(const cv::Mat& image, const std::string& titre, const std::string& mesures = "")
{
    const int bandeau = 60;
    cv::Mat v(image.rows + bandeau, image.cols, CV_8UC3, cv::Scalar(40, 40, 40));
    cv::Mat zoneImage = v(cv::Rect(0, bandeau, image.cols, image.rows));
    if (image.channels() == 1)
        cv::cvtColor(image, zoneImage, cv::COLOR_GRAY2BGR);
    else
        image.copyTo(zoneImage);

    cv::putText(v, titre, {10, 24}, cv::FONT_HERSHEY_SIMPLEX, 0.65, cv::Scalar(255, 255, 255), 2, cv::LINE_AA);
    cv::putText(v, mesures, {10, 50}, cv::FONT_HERSHEY_SIMPLEX, 0.55, cv::Scalar(140, 230, 140), 1, cv::LINE_AA);
    return v;
}

// ================================================================ Mode interactif

// Les trackbars ne donnent que des entiers 0..max : on les convertit en valeurs valides
const int KSIZES[4] = {1, 3, 5, 7};
const int APERTURES[3] = {3, 5, 7};

int indexDe(const int* valeurs, int n, int valeur)
{
    for (int i = 0; i < n; ++i)
        if (valeurs[i] == valeur)
            return i;
    return 0;
}

// Position d'une trackbar (0 si la fenêtre a été fermée)
int position(const char* trackbar, const char* fenetre)
{
    return std::max(0, cv::getTrackbarPos(trackbar, fenetre));
}

// Style OpenCV 4 : pointeur nul, la valeur est relue avec getTrackbarPos dans la boucle
void creerTrackbar(const char* trackbar, const char* fenetre, int max, int valeurInitiale)
{
    cv::createTrackbar(trackbar, fenetre, nullptr, max);
    cv::setTrackbarPos(trackbar, fenetre, valeurInitiale);
}

// Fenêtre de paramètres d'un détecteur ; l'image de titre lui donne une largeur lisible
void creerFenetreParametres(const char* detecteur)
{
    cv::namedWindow(detecteur, cv::WINDOW_AUTOSIZE);
    cv::Mat titre(40, 420, CV_8UC3, cv::Scalar(40, 40, 40));
    cv::putText(titre, std::string("Parametres ") + detecteur, {10, 27}, cv::FONT_HERSHEY_SIMPLEX, 0.7,
                cv::Scalar(255, 255, 255), 2, cv::LINE_AA);
    cv::imshow(detecteur, titre);
}

// Trois fenêtres de trackbars, initialisées avec les paramètres par défaut.
// Le flou se règle par k, qui donne la taille impaire 2k+1 (k = 0 : pas de flou).
void creerControles()
{
    const ParamsSobel s;
    const ParamsLaplace l;
    const ParamsCanny c;

    creerFenetreParametres("Sobel");
    creerTrackbar("blur", "Sobel", 7, (s.blur - 1) / 2);
    creerTrackbar("ksize", "Sobel", 3, indexDe(KSIZES, 4, s.ksize));
    creerTrackbar("seuil", "Sobel", 255, s.seuil);

    creerFenetreParametres("Laplace");
    creerTrackbar("blur", "Laplace", 7, (l.blur - 1) / 2);
    creerTrackbar("ksize", "Laplace", 3, indexDe(KSIZES, 4, l.ksize));
    creerTrackbar("seuil", "Laplace", 255, l.seuil);

    creerFenetreParametres("Canny");
    creerTrackbar("blur", "Canny", 7, (c.blur - 1) / 2);
    creerTrackbar("low", "Canny", 1000, c.low);
    creerTrackbar("high", "Canny", 1000, c.high);
    creerTrackbar("aperture", "Canny", 2, indexDe(APERTURES, 3, c.aperture));
}

ParamsSobel lireSobel()
{
    ParamsSobel p;
    p.blur  = 2 * position("blur", "Sobel") + 1;
    p.ksize = KSIZES[std::min(position("ksize", "Sobel"), 3)];
    p.seuil = position("seuil", "Sobel");
    return p;
}

ParamsLaplace lireLaplace()
{
    ParamsLaplace p;
    p.blur  = 2 * position("blur", "Laplace") + 1;
    p.ksize = KSIZES[std::min(position("ksize", "Laplace"), 3)];
    p.seuil = position("seuil", "Laplace");
    return p;
}

ParamsCanny lireCanny()
{
    ParamsCanny p;
    p.blur     = 2 * position("blur", "Canny") + 1;
    p.low      = position("low", "Canny");
    p.high     = position("high", "Canny");
    p.aperture = APERTURES[std::min(position("aperture", "Canny"), 2)];
    return p;
}

int modeInteractif()
{
    const std::vector<std::string> images = listerImages();
    if (images.empty()) {
        std::cerr << "Aucune image .pgm dans data/images\n";
        return 1;
    }

    cv::namedWindow("Contours", cv::WINDOW_AUTOSIZE);
    creerTrackbar("Detecteur", "Contours", TOUS, TOUS);
    creerControles();
    std::cout << "Detecteur : 0 Sobel, 1 Laplace, 2 Canny, 3 les trois\n"
                 "Touches (fenetre Contours) : n/p image suivante/precedente, v noir et blanc / couleur,\n"
                 "                             s sauvegarder dans results/filtres/, q ou Echap quitter\n";

    int indice = 0;                 // image affichée
    bool couleur = false;           // false : cartes noir et blanc, true : superposition couleur
    std::vector<int> etatAffiche;   // réglages de la dernière mise à jour
    cv::Mat gris, reference;
    std::array<cv::Mat, 3> vues;    // résultats tels qu'affichés (pour la sauvegarde)

    for (;;) {
        // 1. Lire toutes les trackbars
        const int detecteur = position("Detecteur", "Contours");
        const ParamsSobel s = lireSobel();
        const ParamsLaplace l = lireLaplace();
        const ParamsCanny c = lireCanny();

        // 2. Recalculer seulement si l'image, l'affichage ou un paramètre a changé
        const std::vector<int> etat = {indice, couleur, detecteur, s.blur, s.ksize, s.seuil, l.blur, l.ksize,
                                       l.seuil, c.blur, c.low, c.high, c.aperture};
        if (etat != etatAffiche) {
            etatAffiche = etat;
            const std::string nom = images[indice];
            gris = lireImage(nom);
            reference = lireReference(nom);
            if (gris.empty() || reference.empty()) {
                std::cerr << "Impossible de lire " << nom << " ou sa reference\n";
                return 1;
            }

            const std::array<cv::Mat, 3> contours = detecterTout(gris, s, l, c);
            const std::string suffixe = " - " + nom + " (" + std::to_string(indice + 1) + "/" +
                                         std::to_string(images.size()) + ")";
            std::vector<cv::Mat> vignettes;
            for (int d = 0; d < 3; ++d) {
                vues[d] = couleur ? makeOverlay(gris, contours[d], reference) : contours[d];
                if (detecteur == d || detecteur == TOUS)
                    vignettes.push_back(vignette(vues[d], NOMS[d] + suffixe, texteMesures(evaluate(contours[d], reference))));
            }

            cv::Mat affichage;
            cv::hconcat(vignettes, affichage);
            if (affichage.cols > LARGEUR_MAX) {
                const double echelle = static_cast<double>(LARGEUR_MAX) / affichage.cols;
                cv::resize(affichage, affichage, cv::Size(), echelle, echelle, cv::INTER_AREA);
            }
            cv::imshow("Contours", affichage);
        }

        // 3. Clavier (attente de 30 ms, qui laisse aussi les fenêtres se mettre à jour)
        const int touche = cv::waitKey(30) & 0xFF;
        if (cv::getWindowProperty("Contours", cv::WND_PROP_VISIBLE) < 1)
            break;  // fenêtre fermée avec la croix
        if (touche == 'q' || touche == 27) {
            break;
        } else if (touche == 'n') {
            indice = (indice + 1) % static_cast<int>(images.size());
        } else if (touche == 'p') {
            indice = (indice + static_cast<int>(images.size()) - 1) % static_cast<int>(images.size());
        } else if (touche == 'v') {
            couleur = !couleur;
        } else if (touche == 's') {
            // Sauvegarde l'original, la référence et les trois résultats tels qu'affichés
            fs::create_directories("results/filtres");
            const std::string base = "results/filtres/" + images[indice];
            cv::imwrite(base + "_original.png", gris);
            cv::imwrite(base + "_reference.png", reference);
            for (int d = 0; d < 3; ++d)
                cv::imwrite(base + "_" + FICHIERS[d] + ".png", vues[d]);
            std::cout << "Sauvegarde : " << base << "_*.png\n";
        }
    }

    cv::destroyAllWindows();
    return 0;
}

// ================================================================ Mode batch

// Les 8 valeurs d'un détecteur dans resultats.csv : les 5 quantités puis P, TFP, TFN
std::array<double, 8> valeurs(const Mesures& m)
{
    return {double(m.detectes), double(m.reference), double(m.corrects), double(m.fauxPositifs),
            double(m.fauxNegatifs), m.P, m.TFP, m.TFN};
}

// Écrit une ligne : quantités avec `decimales` chiffres après la virgule, taux avec 4
void ecrireLigne(std::ofstream& csv, const std::string& image, const std::array<std::array<double, 8>, 3>& v, int decimales)
{
    csv << image;
    char nombre[32];
    for (const auto& detecteur : v)
        for (int i = 0; i < 8; ++i) {
            std::snprintf(nombre, sizeof nombre, ";%.*f", i < 5 ? decimales : 4, detecteur[i]);
            csv << nombre;
        }
    csv << '\n';
}

int modeBatch()
{
    const std::vector<std::string> images = listerImages();
    if (images.empty()) {
        std::cerr << "Aucune image .pgm dans data/images\n";
        return 1;
    }
    for (const char* dossier : FICHIERS)
        fs::create_directories(std::string("results/batch/") + dossier);
    fs::create_directories("results/comparaison");

    // Une ligne par image : pour chaque détecteur, les 5 quantités puis P, TFP, TFN
    std::ofstream csv("results/batch/resultats.csv");
    csv << "image";
    for (const char* d : FICHIERS)
        for (const char* colonne : {"detectes", "reference", "corrects", "faux_positifs", "faux_negatifs", "P", "TFP", "TFN"})
            csv << ';' << d << '_' << colonne;
    csv << '\n';

    const ParamsSobel s;      // paramètres par défaut
    const ParamsLaplace l;
    const ParamsCanny c;
    std::array<std::array<double, 8>, 3> sommes{};
    int traitees = 0;

    for (const std::string& nom : images) {
        const cv::Mat gris = lireImage(nom);
        const cv::Mat reference = lireReference(nom);
        if (gris.empty() || reference.empty()) {
            std::cerr << "Attention : " << nom << " ou sa reference est illisible, image ignoree\n";
            continue;
        }

        const std::array<cv::Mat, 3> contours = detecterTout(gris, s, l, c);
        std::array<std::array<double, 8>, 3> ligne;
        std::vector<cv::Mat> vignettes = {vignette(gris, nom), vignette(reference, "Reference")};
        for (int d = 0; d < 3; ++d) {
            const Mesures m = evaluate(contours[d], reference);
            cv::imwrite("results/batch/" + std::string(FICHIERS[d]) + "/" + nom + ".png", contours[d]);
            vignettes.push_back(vignette(makeOverlay(gris, contours[d], reference), NOMS[d], texteMesures(m)));
            ligne[d] = valeurs(m);
            for (int i = 0; i < 8; ++i)
                sommes[d][i] += ligne[d][i];
        }

        // Montage : original | référence | Sobel | Laplace | Canny (superpositions couleur)
        cv::Mat montage;
        cv::hconcat(vignettes, montage);
        cv::imwrite("results/comparaison/" + nom + ".png", montage);

        ecrireLigne(csv, nom, ligne, 0);
        std::printf("%-12s Sobel P %5.1f%%   Laplace P %5.1f%%   Canny P %5.1f%%\n", nom.c_str(),
                    100 * ligne[0][5], 100 * ligne[1][5], 100 * ligne[2][5]);
        ++traitees;
    }
    if (traitees == 0)
        return 1;

    // Dernière ligne : les moyennes
    for (auto& detecteur : sommes)
        for (double& v : detecteur)
            v /= traitees;
    ecrireLigne(csv, "moyenne", sommes, 1);

    std::printf("\nMoyennes sur %d images\n%-9s %8s %8s %8s\n", traitees, "", "P", "TFP", "TFN");
    for (int d = 0; d < 3; ++d)
        std::printf("%-9s %7.2f%% %7.2f%% %7.2f%%\n", NOMS[d], 100 * sommes[d][5], 100 * sommes[d][6], 100 * sommes[d][7]);
    std::printf("Resultats : results/batch/ (cartes et resultats.csv), results/comparaison/ (montages)\n");
    return 0;
}

// ================================================================ Programme principal

int main(int argc, char** argv)
{
    // Lancé depuis un autre dossier : on se place dans TP_contours (chemin fixé à la compilation)
    std::error_code erreur;
    if (!fs::exists("data/images", erreur))
        fs::current_path(DOSSIER_PROJET, erreur);

    const std::string mode = argc > 1 ? argv[1] : "";
    if (mode.empty())
        return modeInteractif();
    if (mode == "--batch")
        return modeBatch();

    std::cerr << "Usage : tp_contours            (mode interactif)\n"
                 "        tp_contours --batch    (traitement des 40 images)\n";
    return 1;
}
