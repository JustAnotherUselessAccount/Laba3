#include <iostream>
#include "dungeon_generate.cpp"
#include <random>
#include <ctime>
// #include "conio.h"
// #include <conio.h>

using namespace std;

int main() {
    srand(time(0));
    int k = 4;
    dungeon_map new_dungeon;
    new_dungeon.generate(k);
    new_dungeon.draw();

    while (true) {
        char k;
        k = cin.get();//getch();
        if (k != ' ') {
            int xx = new_dungeon.player.x;
            int yy = new_dungeon.player.y;
            switch (k) {
                case 'w':
                    yy--;
                    break;
                case 'a':
                    xx--;
                    break;
                case 's':
                    yy++;
                    break;
                case 'd':
                    xx++;
                    break;
                case 'z':
                    exit(0);
            }
            new_dungeon.move_player(xx, yy);
            new_dungeon.draw();
        }
    }
}
