clear

rm FIN
g++ -I"include" src/*.cpp -o FIN -Wall -O2

# FIN testcases/scene01_basic.txt output/scene01.bmp
# FIN testcases/scene04_axes.txt output/scene04.bmp
# FIN testcases/scene06_bunny_1k.txt output/scene06.bmp
# FIN testcases/scene09_norm.txt output/scene09.bmp
# FIN testcases/scene10_wineglass.txt output/scene10.bmp
# FIN testcases/scene11_diamond.txt output/scene11.bmp
# FIN testcases/scene12_inf.txt output/scene12.bmp
./FIN testcases/scene13_ex.txt output/scene13.bmp
