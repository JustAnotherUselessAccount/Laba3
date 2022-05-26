#ifndef TEST_H
#define TEST_H

#include <gtest/gtest.h>
#include <vector>
#include <queue>

#include <fcntl.h>
#include <errno.h>
//#include <unistd.h>
#include "dungeon_generate.cpp"

using namespace std;

TEST(generation_tests, check_if_all_cells_are_accessible) {
    dungeon_map new_dungeon;
    new_dungeon.generate(4);

    int xx = new_dungeon.player.x;
    int yy = new_dungeon.player.y;
    vector <vector <bool> > checked(new_dungeon.width, vector <bool> (new_dungeon.height, false));
    queue <pair <int, int> > q;
    checked[xx][yy] = true;
    q.push({xx, yy});
    while (!q.empty()) {
        pair <int, int> nd = q.front();
        q.pop();
        int xxx = nd.first;
        int yyy = nd.second;
        switch (new_dungeon.map[xxx][yyy]) {
            case '.':
            case '[':
            case '>':
                if (!checked[xxx + 1][yyy]) {q.push({xxx + 1, yyy}); checked[xxx + 1][yyy] = true;}
                if (!checked[xxx - 1][yyy]) {q.push({xxx - 1, yyy}); checked[xxx - 1][yyy] = true;}
                if (!checked[xxx][yyy + 1]) {q.push({xxx, yyy + 1}); checked[xxx][yyy + 1] = true;}
                if (!checked[xxx][yyy - 1]) {q.push({xxx, yyy - 1}); checked[xxx][yyy - 1] = true;}
                break;
        }
    }

    for (int i=0; i<new_dungeon.width; i++) {
        for (int j=0; j<new_dungeon.height; j++) {
            switch (new_dungeon.map[i][j]) {
                case '.':
                case '[':
                case '>':
                    EXPECT_EQ(checked[i][j], true);
                    break;
            }
        }
    }
}

TEST(generation_tests, check_if_all_movable_cells_cant_lead_into_the_void) {
    dungeon_map new_dungeon;
    new_dungeon.generate(4);

    for (int i=0; i<new_dungeon.width; i++) {
        for (int j=0; j<new_dungeon.height; j++) {
            switch (new_dungeon.map[i][j]) {
                case '.':
                case '[':
                case '>':
                    for (int ii=i-1; ii<=i+1; ii++) {
                        for (int jj=j-1; jj<=j+1; jj++) {
                        EXPECT_NE(new_dungeon.map[ii][jj], " ");
                    }
            }
        }
    }
}

#endif

