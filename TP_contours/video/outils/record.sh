#!/bin/bash
# Enregistre les deux séquences "live" de la vidéo sur l'écran virtuel :99
#   live_interactif.mp4 : mode interactif (trackbars)
#   live_batch.mp4      : mode batch dans un terminal
set -e
export DISPLAY=:99
cd /work/TP_contours
w(){ xdotool search --name "^$1\$" | tail -1; }
k(){ xdotool key --delay "${3:-60}" --repeat "$2" "$1"; }
grab(){ ffmpeg -loglevel error -y -f x11grab -framerate 30 -video_size 1920x1080 -i :99 \
          -c:v libx264 -preset veryfast -crf 20 -pix_fmt yuv420p "$1" & echo $! > /tmp/ff.pid; }
stopgrab(){ kill -INT "$(cat /tmp/ff.pid)"; sleep 2; }

pkill -f tp_contours || true; pkill xterm || true; sleep 1
./build/tp_contours > /tmp/prog.log 2>&1 &
sleep 4
for n in Sobel Laplace Canny; do xdotool windowsize "$(w $n)" 1500 260; done
xdotool windowmove "$(w Contours)" 210 25
for n in Laplace Sobel Canny; do xdotool windowmove "$(w $n)" 210 610; done
xdotool windowactivate --sync "$(w Contours)"
xdotool mousemove 960 950
sleep 1

# ---------------------------------------------------------------- séquence interactive
grab /video/live_interactif.mp4
sleep 7                                          # A : vue d'ensemble, noir et blanc
xdotool key v; sleep 8                           # B : superposition couleur
xdotool windowactivate --sync "$(w Sobel)"; sleep 2
xdotool mousemove 718 769 click 1; sleep 1       # C : seuil Sobel
k Left 40; sleep 3; k Right 80; sleep 3; k Left 40; sleep 2
xdotool mousemove 709 719 click 1; sleep 1       # D : flou Sobel
k Left 2 600; sleep 3; k Right 2 600; sleep 2
xdotool windowactivate --sync "$(w Canny)"; sleep 2
xdotool mousemove 565 769 click 1; sleep 1       # E : seuil haut Canny
k Right 150 30; sleep 3; k Left 150 30; sleep 2
xdotool windowactivate --sync "$(w Contours)"; sleep 1
xdotool mousemove 1702 542 click 1; sleep 1      # F : choix du détecteur
k Left 1; sleep 4; k Left 2 600; sleep 4; k Right 3 400; sleep 2
xdotool mousemove 960 950 click 1                # G : parcours du lot d'images
xdotool windowactivate --sync "$(w Contours)"
for i in 1 2 3; do xdotool key n; sleep 2; done
k n 29 150; sleep 6
xdotool key s; sleep 3
stopgrab

# ---------------------------------------------------------------- séquence batch
pkill -f tp_contours || true; sleep 1
cd /work
xterm -geometry 110x40+260+40 -fa 'DejaVu Sans Mono' -fs 13 -bg '#1e1e1e' -fg '#e6e6e6' &
sleep 2
xdotool mousemove 960 1070
grab /video/live_batch.mp4
sleep 2
xdotool type --delay 70 "cd TP_contours && ./build/tp_contours --batch"; sleep 1
xdotool key Return; sleep 12
xdotool type --delay 70 "ls results/batch results/comparaison | head"; sleep 1
xdotool key Return; sleep 8
stopgrab
pkill xterm || true
echo ok
