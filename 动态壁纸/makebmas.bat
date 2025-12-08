ffmpeg -i 24.mp4 -vf "fps=30,scale=240:-2" 24_%04d.png
bmas ./ 24.bmas
split 24.bmas ./ 330
