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
                            EXPECT_NE(new_dungeon.map[ii][jj], ' ');
                        }
                    }
                    break;
            }
        }
    }
}

TEST(player_move_tests, player_cant_move_outside_of_place) {
    dungeon_map new_dungeon;
    new_dungeon.generate(4);

    new_dungeon.move_player(-1, -1);
    EXPECT_NE(new_dungeon.player.x, -1);
    EXPECT_NE(new_dungeon.player.y, -1);
}

TEST(player_move_tests, player_cant_move_outside_of_place_2) {
    dungeon_map new_dungeon;
    new_dungeon.generate(4);

    new_dungeon.move_player(new_dungeon.width + 2, new_dungeon.height + 4);
    EXPECT_NE(new_dungeon.player.x, new_dungeon.width + 2);
    EXPECT_NE(new_dungeon.player.y, new_dungeon.height + 4);
}

TEST(player_move_tests, player_cant_move_into_the_void) {
    dungeon_map new_dungeon;
    new_dungeon.generate(4);

    new_dungeon.move_player(0, 0);
    EXPECT_NE(new_dungeon.player.x, 0);
    EXPECT_NE(new_dungeon.player.y, 0);
}

TEST(player_move_tests, player_cant_move_into_wall_test) {
    dungeon_map new_dungeon;
    new_dungeon.generate(4);

    int xx = new_dungeon.player.x;
    int yy = new_dungeon.player.y;

    while (new_dungeon.map[xx][yy] != '#') {
        new_dungeon.move_player(xx, yy);
        EXPECT_NE(new_dungeon.map[new_dungeon.player.x][new_dungeon.player.y], ' ');
        EXPECT_NE(new_dungeon.map[new_dungeon.player.x][new_dungeon.player.y], '#');
        xx++;
    }

    new_dungeon.move_player(xx, yy);
    EXPECT_NE(new_dungeon.player.x, xx);
}

TEST(player_move_tests, there_are_an_descend_and_it_works) {
    dungeon_map new_dungeon;
    new_dungeon.generate(4);

    int xx = -1;
    int yy = -1;

    for (int i=0; i<new_dungeon.width; i++) {
        for (int j=0; j<new_dungeon.height; j++) {
            if (new_dungeon.map[i][j] == '>') {
                xx = i;
                yy = j;
                break;
            }
        }
        if (xx != -1)
            break;
    }
    
    EXPECT_NE(xx, -1);

    vector <vector <char> > old_dungeon_map(new_dungeon.map);

    new_dungeon.move_player(xx, yy);

    bool flag = false;
    for (int i=0; i<new_dungeon.width; i++) {
        for (int j=0; j<new_dungeon.height; j++) {
            if (new_dungeon.map[i][j] != old_dungeon_map[i][j]) {
                flag = true;
                break;
            }
        }
        if (flag)
            break;
    }

    EXPECT_EQ(flag, true);
}


#endif

