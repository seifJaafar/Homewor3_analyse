# Génère les diapositives 1920x1080 de la vidéo (PNG) dans /video/slides/
import os

import matplotlib
matplotlib.use("Agg")
import matplotlib.image as mpimg
import matplotlib.pyplot as plt

R = "/work/TP_contours/results"
OUT = "/video/slides"
os.makedirs(OUT, exist_ok=True)
BG, FG, ACC, MUTED = "#16181d", "#f2f2f2", "#7cc4ff", "#a9b0bb"


def slide(nom, titre, corps=None, image=None, img_box=(0.05, 0.08, 0.9, 0.62), sous_titre=None, taille=30):
    fig = plt.figure(figsize=(19.2, 10.8), dpi=100, facecolor=BG)
    fig.text(0.05, 0.9, titre, color=FG, fontsize=52, weight="bold", va="center")
    fig.add_artist(plt.Line2D([0.05, 0.95], [0.84, 0.84], color=ACC, lw=3))
    if sous_titre:
        fig.text(0.05, 0.79, sous_titre, color=MUTED, fontsize=26, va="center")
    if corps:
        y = 0.74 if not sous_titre else 0.70
        for ligne in corps:
            if ligne == "":
                y -= 0.03
                continue
            couleur, t, ind = FG, ligne, 0.06
            if ligne.startswith("##"):
                couleur, t = ACC, ligne[2:].strip()
            elif ligne.startswith("  "):
                couleur, t, ind = MUTED, ligne.strip(), 0.09
            elif ligne.startswith("- "):
                t = "•  " + ligne[2:]
            fig.text(ind, y, t, color=couleur, fontsize=taille, va="center", family="DejaVu Sans")
            y -= 0.062
    if image:
        ax = fig.add_axes(img_box)
        ax.imshow(mpimg.imread(image))
        ax.axis("off")
    fig.savefig(f"{OUT}/{nom}.png", facecolor=BG)
    plt.close(fig)


# 1. Titre
fig = plt.figure(figsize=(19.2, 10.8), dpi=100, facecolor=BG)
fig.text(0.5, 0.6, "TP Contours", color=FG, fontsize=96, weight="bold", ha="center")
fig.text(0.5, 0.47, "Comparaison de trois détecteurs : Sobel, Laplace et Canny", color=ACC, fontsize=40, ha="center")
fig.text(0.5, 0.33, "Analyse d'image  ·  C++17 / OpenCV 4  ·  Base de contours RUG (40 images)", color=MUTED, fontsize=28, ha="center")
fig.savefig(f"{OUT}/01_titre.png", facecolor=BG)
plt.close(fig)

slide("02_mesures", "Données et mesures", [
    "##Base RUG : 40 images naturelles + contours de référence tracés à la main",
    "",
    "- contours_corrects : pixels détectés situés sur un contour de référence",
    "  (tolérance : voisinage 3×3, soit 1 pixel de décalage autorisé)",
    "- faux_positifs = détectés − corrects",
    "- faux_négatifs = référence non retrouvée (même tolérance 3×3)",
    "",
    "##Avec T = corrects + faux_positifs + faux_négatifs :",
    "- Performance  P = corrects / T",
    "- Taux de faux positifs  TFP = faux_positifs / T",
    "- Taux de faux négatifs  TFN = faux_négatifs / T          (P + TFP + TFN = 1)",
])

slide("03_detecteurs", "Les trois détecteurs (OpenCV)", [
    "##Pré-traitement commun : cv::GaussianBlur (taille « blur »)",
    "",
    "##Sobel — gradient (dérivée 1re)",
    "  cv::Sobel en x et y (ksize) → cv::magnitude → cv::normalize 0..255 → seuil",
    "##Laplace — dérivée 2nde",
    "  cv::Laplacian (ksize) → |Δ| → cv::normalize 0..255 → seuil",
    "##Canny — gradient + affinage",
    "  cv::Canny(low, high, aperture) : non-maxima + seuillage par hystérésis",
    "",
    "- Sobel et Laplace donnent des bandes épaisses ; Canny des contours de 1 pixel",
])

slide("04_programme", "Organisation du programme", [
    "- detecteurs.cpp : detecterSobel / detecterLaplace / detecterCanny",
    "- evaluation.cpp : loadReference, evaluate (P, TFP, TFN), makeOverlay",
    "- main.cpp : deux modes",
    "",
    "##tp_contours            → mode interactif",
    "  trackbar « Detecteur » (0 Sobel, 1 Laplace, 2 Canny, 3 les trois)",
    "  une fenêtre de trackbars par détecteur, mesures recalculées en direct",
    "  touches : n / p image suivante / précédente, v couleur, s sauvegarde",
    "##tp_contours --batch    → lot de 40 images",
    "  cartes de contours, montages, resultats.csv (5 quantités + 3 mesures)",
])

# 5. Tableau des moyennes
fig = plt.figure(figsize=(19.2, 10.8), dpi=100, facecolor=BG)
fig.text(0.05, 0.9, "Résultats moyens sur les 40 images", color=FG, fontsize=52, weight="bold", va="center")
fig.add_artist(plt.Line2D([0.05, 0.95], [0.84, 0.84], color=ACC, lw=3))
lignes = [("", "P", "TFP", "TFN", "Paramètres"),
          ("Sobel", "39,4 %", "37,8 %", "22,8 %", "blur 5, ksize 3, seuil 70"),
          ("Laplace", "22,5 %", "63,9 %", "13,6 %", "blur 5, ksize 3, seuil 40"),
          ("Canny", "31,5 %", "36,1 %", "32,5 %", "blur 7, low 50, high 150")]
xs = [0.07, 0.27, 0.42, 0.57, 0.71]
for i, l in enumerate(lignes):
    y = 0.70 - i * 0.1
    for j, c in enumerate(l):
        fig.text(xs[j], y, c, color=ACC if i == 0 or j == 0 else FG, fontsize=36 if j < 4 else 26,
                 weight="bold" if i == 0 or j == 0 else "normal", va="center")
fig.text(0.07, 0.24, "•  Sobel obtient le meilleur P sur 38 images sur 40 (Canny sur 2)", color=FG, fontsize=30)
fig.text(0.07, 0.17, "•  Laplace : très sensible au bruit et à la texture → TFP élevé", color=FG, fontsize=30)
fig.text(0.07, 0.10, "•  Canny : peu de faux positifs, mais plus de contours manqués", color=FG, fontsize=30)
fig.savefig(f"{OUT}/05_resultats.png", facecolor=BG)
plt.close(fig)

slide("06_parametres", "Influence des paramètres", None, f"{R}/parametres/performance_vs_seuil.png",
      img_box=(0.04, 0.2, 0.92, 0.6), sous_titre="P moyenne en fonction du seuil, pour 4 tailles de flou")
fig = plt.gcf()

slide("07_localisation", "Bonne détection et bonne localisation", None, f"{R}/comparaison/hyena.png",
      img_box=(0.02, 0.42, 0.96, 0.34),
      sous_titre="vert = correct   rouge = faux positif   bleu = contour manqué")
# Deuxième montage sur la même diapositive
fig = plt.figure(figsize=(19.2, 10.8), dpi=100, facecolor=BG)
plt.close(fig)
img = plt.imread(f"{OUT}/07_localisation.png")
fig = plt.figure(figsize=(19.2, 10.8), dpi=100, facecolor=BG)
ax = fig.add_axes([0, 0, 1, 1]); ax.imshow(img); ax.axis("off")
ax2 = fig.add_axes([0.02, 0.04, 0.96, 0.34]); ax2.imshow(mpimg.imread(f"{R}/comparaison/bear_7.png")); ax2.axis("off")
fig.savefig(f"{OUT}/07_localisation.png", facecolor=BG)
plt.close(fig)

slide("08_metrique", "Limites de la méthode de comparaison", [
    "##Effet de la tolérance (paramètres par défaut, P moyenne)",
    "  voisinage 3×3 :   Sobel 39,4 %    Laplace 22,5 %    Canny 31,5 %",
    "  position exacte : Sobel 14,9 %    Laplace   5,9 %    Canny 12,9 %",
    "",
    "- Atout : simple, objectif, comparable entre images et détecteurs",
    "- Faiblesse : un contour épais (Sobel) compte plusieurs « corrects »",
    "  pour un seul pixel de référence → avantage aux bandes épaisses",
    "- Faiblesse : la référence ne contient que les contours d'objets ;",
    "  la texture (herbe, eau, gravier) est comptée comme faux positif",
])

slide("09_conclusion", "Conclusion", [
    "- Meilleur score P : Sobel (39,4 %), puis Canny (31,5 %), puis Laplace (22,5 %)",
    "- Meilleure localisation : Canny (contours fins de 1 pixel, hystérésis)",
    "- Le flou gaussien est indispensable, surtout pour Laplace et Canny",
    "- Le seuil règle le compromis faux positifs / faux négatifs",
    "- Un réglage par image ne gagne que 2 à 4 points : le réglage global est robuste",
    "",
    "##Limite principale : la texture de fond, non annotée dans la référence",
])
print("ok")
