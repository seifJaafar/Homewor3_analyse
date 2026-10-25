# Étude de l'influence des paramètres (pour le rapport).
# Reproduit exactement les détecteurs et la métrique du programme C++ (src/detecteurs.cpp, src/evaluation.cpp)
# et fait varier les paramètres sur les 40 images. Écrit les courbes et un CSV dans results/parametres/.
#
#   python3 scripts/balayage.py      (lancé depuis TP_contours)

import csv
import os

import cv2
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np

IMAGES = "data/images"
REFS = "data/reference"
SORTIE = "results/parametres"
CARRE = cv2.getStructuringElement(cv2.MORPH_RECT, (3, 3))


def flouter(g, k):
    return g if k <= 1 else cv2.GaussianBlur(g, (k, k), 0)


def seuiller(rep, seuil):
    n = cv2.normalize(rep, None, 0, 255, cv2.NORM_MINMAX)
    return np.where(n > seuil, 255, 0).astype(np.uint8)


def sobel_mag(g, blur, ksize=3):
    i = flouter(g, blur)
    gx = cv2.Sobel(i, cv2.CV_32F, 1, 0, ksize=ksize)
    gy = cv2.Sobel(i, cv2.CV_32F, 0, 1, ksize=ksize)
    return cv2.magnitude(gx, gy)


def laplace_abs(g, blur, ksize=3):
    return np.abs(cv2.Laplacian(flouter(g, blur), cv2.CV_32F, ksize=ksize))


def canny(g, blur, low, high, ap=3):
    return cv2.Canny(flouter(g, blur), low, high, apertureSize=ap)


def evaluer(det, ref, tolerance=True):
    refd = cv2.dilate(ref, CARRE) if tolerance else ref
    detd = cv2.dilate(det, CARRE) if tolerance else det
    d = det > 0
    corrects = int(np.count_nonzero(d & (refd > 0)))
    fp = int(np.count_nonzero(d)) - corrects
    fn = int(np.count_nonzero((ref > 0) & (detd == 0)))
    t = corrects + fp + fn
    return (corrects / t, fp / t, fn / t) if t else (0, 0, 0)


def charger():
    donnees = []
    for f in sorted(os.listdir(IMAGES)):
        if not f.endswith(".pgm"):
            continue
        nom = f[:-4]
        g = cv2.imread(f"{IMAGES}/{f}", cv2.IMREAD_GRAYSCALE)
        r = cv2.imread(f"{REFS}/{nom}_gt_binary.pgm", cv2.IMREAD_GRAYSCALE)
        r = cv2.threshold(cv2.bitwise_not(r), 127, 255, cv2.THRESH_BINARY)[1]
        donnees.append((nom, g, r))
    return donnees


def moyenne(mesures):
    return tuple(float(np.mean([m[i] for m in mesures])) for i in range(3))


def main():
    os.makedirs(SORTIE, exist_ok=True)
    data = charger()
    lignes = []  # detecteur, blur, parametre, valeur, P, TFP, TFN
    seuils = list(range(10, 201, 10))
    blurs = [1, 3, 5, 7]

    # Sobel et Laplace : seuil x flou (la réponse est calculée une fois par flou)
    courbes = {}
    for nomdet, reponse in (("sobel", sobel_mag), ("laplace", laplace_abs)):
        for b in blurs:
            reps = [(reponse(g, b), r) for _, g, r in data]
            ps = []
            for s in seuils:
                m = moyenne([evaluer(seuiller(x, s), r) for x, r in reps])
                lignes.append((nomdet, b, "seuil", s, *m))
                ps.append(m)
            courbes[(nomdet, b)] = ps

    # Canny : seuil haut x flou, avec low = high / 3 (rapport conseillé par Canny)
    highs = list(range(25, 401, 25))
    for b in blurs:
        imgs = [(flouter(g, b), r) for _, g, r in data]
        ps = []
        for h in highs:
            m = moyenne([evaluer(cv2.Canny(i, h // 3, h), r) for i, r in imgs])
            lignes.append(("canny", b, "high(low=high/3)", h, *m))
            ps.append(m)
        courbes[("canny", b)] = ps

    # Canny : grille low x high au flou 7
    lows = list(range(10, 151, 20))
    grille = np.zeros((len(lows), len(highs)))
    imgs = [(flouter(g, 7), r) for _, g, r in data]
    for i, lo in enumerate(lows):
        for j, h in enumerate(highs):
            if lo >= h:
                grille[i, j] = np.nan
                continue
            grille[i, j] = moyenne([evaluer(cv2.Canny(im, lo, h), r) for im, r in imgs])[0]

    with open(f"{SORTIE}/balayage.csv", "w", newline="") as f:
        w = csv.writer(f, delimiter=";")
        w.writerow(["detecteur", "blur", "parametre", "valeur", "P", "TFP", "TFN"])
        for l in lignes:
            w.writerow([*l[:4], *(f"{v:.4f}" for v in l[4:])])

    # Figure 1 : P en fonction du seuil, pour chaque flou
    fig, axes = plt.subplots(1, 3, figsize=(14, 4), sharey=True)
    for ax, (nomdet, xs, titre) in zip(axes, (("sobel", seuils, "Sobel : seuil"),
                                              ("laplace", seuils, "Laplace : seuil"),
                                              ("canny", highs, "Canny : seuil haut (bas = haut/3)"))):
        for b in blurs:
            ax.plot(xs, [100 * m[0] for m in courbes[(nomdet, b)]], marker=".", label=f"flou {b}x{b}" if b > 1 else "sans flou")
        ax.set_title(titre)
        ax.set_xlabel("valeur du paramètre")
        ax.grid(alpha=0.3)
    axes[0].set_ylabel("Performance P moyenne (%)")
    axes[0].legend()
    fig.tight_layout()
    fig.savefig(f"{SORTIE}/performance_vs_seuil.png", dpi=130)

    # Figure 2 : compromis TFP / TFN au meilleur flou
    fig, axes = plt.subplots(1, 3, figsize=(14, 4), sharey=True)
    for ax, (nomdet, xs, b, titre) in zip(axes, (("sobel", seuils, 5, "Sobel (flou 5)"),
                                                 ("laplace", seuils, 5, "Laplace (flou 5)"),
                                                 ("canny", highs, 7, "Canny (flou 7)"))):
        c = courbes[(nomdet, b)]
        ax.plot(xs, [100 * m[0] for m in c], label="P", color="black")
        ax.plot(xs, [100 * m[1] for m in c], label="TFP", color="tab:red")
        ax.plot(xs, [100 * m[2] for m in c], label="TFN", color="tab:blue")
        ax.set_title(titre)
        ax.set_xlabel("seuil")
        ax.grid(alpha=0.3)
    axes[0].set_ylabel("%")
    axes[0].legend()
    fig.tight_layout()
    fig.savefig(f"{SORTIE}/compromis_tfp_tfn.png", dpi=130)

    # Figure 3 : carte low x high de Canny
    fig, ax = plt.subplots(figsize=(8, 4))
    im = ax.imshow(100 * grille, origin="lower", aspect="auto", cmap="viridis",
                   extent=[highs[0] - 12.5, highs[-1] + 12.5, lows[0] - 10, lows[-1] + 10])
    fig.colorbar(im, label="P moyenne (%)")
    ax.set_xlabel("seuil haut")
    ax.set_ylabel("seuil bas")
    ax.set_title("Canny (flou 7) : performance selon les deux seuils")
    fig.tight_layout()
    fig.savefig(f"{SORTIE}/canny_low_high.png", dpi=130)

    # Résumé : meilleur réglage global, réglage "oracle" par image, effet de la tolérance 3x3
    print("Meilleurs réglages globaux (P moyenne) :")
    for nomdet in ("sobel", "laplace", "canny"):
        best = max((l for l in lignes if l[0] == nomdet), key=lambda l: l[4])
        print(f"  {nomdet:8s} blur {best[1]} {best[2]} {best[3]} -> P {100*best[4]:.2f}%  TFP {100*best[5]:.2f}%  TFN {100*best[6]:.2f}%")
    i, j = np.unravel_index(np.nanargmax(grille), grille.shape)
    print(f"  canny grille flou 7 : low {lows[i]} high {highs[j]} -> P {100*grille[i, j]:.2f}%")

    print("\nRéglage optimal par image (oracle, flou par défaut) vs réglage global :")
    for nomdet, b, f, defaut in (("sobel", 5, lambda g, s: seuiller(sobel_mag(g, 5), s), 70),
                                 ("laplace", 5, lambda g, s: seuiller(laplace_abs(g, 5), s), 40),
                                 ("canny", 7, lambda g, h: canny(g, 7, h // 3, h), 150)):
        xs = seuils if nomdet != "canny" else highs
        oracle = []
        for _, g, r in data:
            oracle.append(max(evaluer(f(g, x), r)[0] for x in xs))
        print(f"  {nomdet:8s} oracle P {100*np.mean(oracle):.2f}%")

    print("\nEffet de la tolérance (paramètres par défaut) : P avec voisinage 3x3 / P stricte")
    for nomdet, f in (("sobel", lambda g: seuiller(sobel_mag(g, 5), 70)),
                      ("laplace", lambda g: seuiller(laplace_abs(g, 5), 40)),
                      ("canny", lambda g: canny(g, 7, 50, 150))):
        avec = moyenne([evaluer(f(g), r, True) for _, g, r in data])
        sans = moyenne([evaluer(f(g), r, False) for _, g, r in data])
        print(f"  {nomdet:8s} 3x3 : P {100*avec[0]:.2f}% TFP {100*avec[1]:.2f}% TFN {100*avec[2]:.2f}%"
              f"   strict : P {100*sans[0]:.2f}% TFP {100*sans[1]:.2f}% TFN {100*sans[2]:.2f}%")


if __name__ == "__main__":
    main()
