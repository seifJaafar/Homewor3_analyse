# Script de la vidéo — TP Contours (durée 4 min 49 s)

La vidéo `TP_contours_video.mp4` est muette ; ce texte est à lire par-dessus.
Chaque bloc indique le moment de la vidéo et ce qui est affiché à l'écran.
Débit visé : environ 2,5 mots par seconde.

---

### 0:00 – 0:10 · Diapositive « TP Contours »
Bonjour. Dans cette vidéo, je présente notre TP de détection de contours : nous comparons trois détecteurs, Sobel, Laplace et Canny, sur une base de quarante images.

### 0:10 – 0:32 · Diapositive « Données et mesures »
Nous utilisons la base de l'université de Groningue : quarante images naturelles, chacune avec des contours tracés à la main. Pour chaque image, on compte les pixels corrects, les faux positifs et les faux négatifs, avec une tolérance d'un pixel grâce à un voisinage trois sur trois. On en déduit la performance P, le taux de faux positifs et le taux de faux négatifs.

### 0:32 – 0:54 · Diapositive « Les trois détecteurs »
Les trois méthodes commencent par un flou gaussien. Sobel calcule le gradient en x et en y, puis sa norme, que l'on seuille. Laplace utilise la dérivée seconde, dont on seuille la valeur absolue. Canny ajoute la suppression des non-maxima et un seuillage par hystérésis avec deux seuils : il produit des contours d'un seul pixel.

### 0:54 – 1:12 · Diapositive « Organisation du programme »
Le code est en C++ avec OpenCV. Un fichier contient les détecteurs, un autre l'évaluation. Le programme a deux modes : un mode interactif avec des trackbars, et un mode batch qui traite le lot complet des quarante images.

### 1:12 – 1:19 · Mode interactif, vue d'ensemble
Voici le mode interactif. En haut, les trois détecteurs sur la même image, avec les mesures P, TFP et TFN calculées en direct. En bas, la fenêtre des paramètres.

### 1:19 – 1:27 · Touche « v », superposition couleur
Avec la touche v, on superpose le résultat à l'image : en vert les pixels corrects, en rouge les faux positifs, en bleu les contours manqués.

### 1:27 – 1:48 · Seuil de Sobel
Je modifie le seuil de Sobel. En le baissant, l'herbe apparaît en rouge : les faux positifs explosent. En le montant, les contours de l'objet disparaissent en bleu : ce sont des faux négatifs. Le seuil règle donc le compromis entre les deux. Je reviens à soixante-dix, la meilleure valeur trouvée.

### 1:48 – 1:56 · Flou de Sobel
Si je supprime le flou, le bruit augmente et la performance chute. Je remets le flou de cinq.

### 1:56 – 2:13 · Seuil haut de Canny
Pour Canny, j'augmente le seuil haut de l'hystérésis. Il reste moins de faux positifs, mais on perd des contours faibles. Je reviens à cent cinquante.

### 2:13 – 2:28 · Trackbar « Detecteur »
La trackbar Détecteur permet d'afficher un seul détecteur : ici Canny seul, puis Sobel seul, puis les trois ensemble pour les comparer.

### 2:28 – 2:49 · Parcours des images
Les touches n et p font défiler le lot d'images. Sur l'image hyena, le fond est assez uniforme : Sobel atteint soixante-six pour cent et Canny cinquante-trois. La touche s enregistre les résultats affichés.

### 2:49 – 3:16 · Terminal, mode batch
Le mode batch traite les quarante images avec les paramètres par défaut. Il affiche la performance de chaque détecteur pour chaque image, puis les moyennes. Il enregistre aussi les cartes de contours, les montages de comparaison et un fichier CSV avec les cinq quantités et les trois mesures.

### 3:16 – 3:34 · Diapositive « Résultats moyens »
En moyenne, Sobel obtient trente-neuf pour cent, Canny trente et un, et Laplace vingt-deux. Sobel gagne sur trente-huit images sur quarante. Laplace a beaucoup de faux positifs ; Canny en a peu, mais manque davantage de contours.

### 3:34 – 3:56 · Diapositive « Influence des paramètres »
Ces courbes montrent la performance en fonction du seuil, pour quatre tailles de flou. Chaque courbe a un maximum net : trop bas, on garde la texture ; trop haut, on perd les contours. Le flou améliore nettement le résultat, surtout pour Canny, dont le meilleur seuil dépend fortement du flou.

### 3:56 – 4:18 · Diapositive « Bonne détection et bonne localisation »
En haut, l'image hyena : les contours sont bien détectés, et Canny les localise le plus finement. En bas, l'ours devant le gravier : tous les détecteurs échouent, car la texture du fond produit des milliers de faux positifs, et cette texture n'est pas annotée dans la référence.

### 4:18 – 4:36 · Diapositive « Limites de la méthode de comparaison »
La tolérance trois sur trois est indispensable : sans elle, les scores sont divisés par trois. Mais la mesure a un défaut : un contour épais compte plusieurs pixels corrects pour un seul pixel de référence, ce qui avantage Sobel face à Canny.

### 4:36 – 4:48 · Diapositive « Conclusion »
En conclusion, Sobel a le meilleur score, Canny la meilleure localisation, et Laplace est le plus sensible au bruit. Merci de votre attention.
