ulimit -d 1000000 -m 1000000 -v 1000000 -s 65532
g++-10 -O2 -std=gnu++20 NPC/NPC.cpp 
./a.out < input.txt
