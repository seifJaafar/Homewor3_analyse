# TP Contours — Sobel, Laplace, Canny

Comparaison quantitative de trois détecteurs de contours sur les 40 images de la base RUG
(contours de référence tracés à la main), avec les mesures P, TFP et TFN.

## Contenu de `TP_contours/`

| Dossier / fichier | Contenu |
|---|---|
| `src/` | Code source C++17 / OpenCV : détecteurs, évaluation, programme principal |
| `CMakeLists.txt` | Compilation avec CMake |
| `data/images`, `data/reference` | Les 40 images et leurs contours de référence |
| `results/batch/` | Cartes de contours des 3 détecteurs et `resultats.csv` (5 quantités + P, TFP, TFN) |
| `results/comparaison/` | Montages original / référence / Sobel / Laplace / Canny |
| `results/parametres/` | Étude de l'influence des paramètres (courbes + `balayage.csv`) |
| `scripts/balayage.py` | Script de l'étude des paramètres |
| `rapport/rapport.pdf` | Le rapport |
| `video/TP_contours_video.mp4` | La vidéo de présentation |
| `video/script_video.md` | Le texte de la vidéo, minuté |

## Utilisation

```bash
cd TP_contours
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/tp_contours            # mode interactif : choix du détecteur et trackbars
./build/tp_contours --batch    # traitement du lot de 40 images
python3 scripts/balayage.py    # étude des paramètres
```

Mode interactif : trackbar `Detecteur` (0 Sobel, 1 Laplace, 2 Canny, 3 les trois),
une fenêtre de paramètres par détecteur, touches `n`/`p` (image suivante/précédente),
`v` (superposition couleur), `s` (sauvegarde), `q` (quitter).

## Résultats moyens (40 images)

| | P | TFP | TFN |
|---|---|---|---|
| Sobel (blur 5, seuil 70) | 39,4 % | 37,8 % | 22,8 % |
| Laplace (blur 5, seuil 40) | 22,5 % | 63,9 % | 13,6 % |
| Canny (blur 7, 50/150) | 31,5 % | 36,1 % | 32,5 % |
