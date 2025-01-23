
mrpbuilder -t unpack -i run.mrp -o build

mrpbuilder -t pack -i assets/ build/cfunction.ext build/ic_launcher.bma build/start.mr main.c(name=mpc.c) bitmap.h button.h CheckBox.c CheckBox.h coding.h ImageButton.h ProgressBar.h SeekBar.h TextView.h View.h ^
-o xcolor.mrp -displayname "X调色板" -appid 30035 -vendor "风的影子" -filename tsbxl.mrp -version 1003 -desc "调色板XL,风的影子作品"
xcolor.mrp