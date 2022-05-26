#pragma once

#include <vector>
#include <queue>
#include <cmath>
#include <iostream>
#include <random>

using namespace std;

struct Player {
    int x, y;
};

struct sector {
    int x1;
    int y1;
    int x2;
    int y2;
    int iter;
};

struct dsu {
    vector <int> pars;
    dsu(int size) {
        pars.resize(size);
        for (int i=0; i<size; i++) pars[i] = i;
    }

    int get(int node) {return pars[node] = (pars[node] == node ? node : get(pars[node]));}
    void unite(int a, int b) {pars[get(a)] = get(b);}
};

//     1
//     |
//  8<- ->2
//     |
//     4

struct door {
    int xx;
    int yy;
    int dir;
};

struct room {
    int x1;
    int y1;
    int x2;
    int y2;
    vector <door> doors;
};

struct dungeon_map {
    int width = 80;
    int height = 40;

    int min_room_size = 5;

    vector <vector <char> > map;

    Player player;

private:
    int non_connected_rooms;
    void make_path(door dr, int i, bool flag);

public:
    void generate(int iters = 4) {
        map.clear();
        map.resize(width, vector <char> (height, ' '));

        for (int i=0; i<width; i++) {
            map[i][0] = '_';
            map[i][height-1] = '_';
        }
        for (int i=0; i<height; i++) {
            map[0][i] = '|';
            map[width-1][i] = '|';
        }
        for (int i=1; i<width-1; i++) {
            map[i][1] = '-';
            map[i][height-2] = '-';
        }
        for (int i=1; i<height-1; i++) {
            map[1][i] = '-';
            map[width-2][i] = '-';
        }

        queue <sector> q, q2;
        q.push({2, 2, width-3, height-3, 0});

        // Разбиение карты на сектора
        for (int i=0; i<iters; i++) {
            sector nd = q.front();
            while (!q.empty() && nd.iter == i) {
                q.pop();
                int w = nd.x2 - nd.x1 + 1;
                int h = nd.y2 - nd.y1 + 1;

                if (w >= h) {
                    if (w > min_room_size*2 + 1) {
                        int line = nd.x1 + max(min_room_size, w/4) + (rand() % (w - max(min_room_size, w/4) * 2));
                        for (int j=nd.y1; j<=nd.y2; j++) {
                            map[line][j]='-';
                        }
                        q.push({nd.x1, nd.y1, line - 1, nd.y2, i+1});
                        q.push({line + 1, nd.y1, nd.x2, nd.y2, i+1});
                    } else {
                        nd.iter = i+1;
                        q.push(nd);
                    }
                } else {
                    if (h > min_room_size*2 + 1) {
                        int line = nd.y1 + max(min_room_size, h/4) + (rand() % (h - max(min_room_size, h/4) * 2));
                        for (int j=nd.x1; j<=nd.x2; j++) {
                            map[j][line]='-';
                        }
                        q.push({nd.x1, nd.y1, nd.x2, line - 1, i+1});
                        q.push({nd.x1, line + 1, nd.x2, nd.y2, i+1});
                    } else {
                        nd.iter = i+1;
                        q.push(nd);
                    }
                }

                nd = q.front();
            }
        }

        vector <room> rooms;

        // Создание комнат
        while (!q.empty()) {
            sector nd = q.front();
            q.pop();

            // cout << nd.x1 << " " << nd.y1 << " " << nd.x2 << " " << nd.y2 << endl;

            room rm;
            int sc_w = nd.x2 - nd.x1 + 1;
            int sc_h = nd.y2 - nd.y1 + 1;
            int w = min_room_size + rand() % (sc_w - min_room_size + 1);
            int h = min_room_size + rand() % (sc_h - min_room_size + 1);
            rm.x1 = nd.x1 + rand() % (sc_w - w + 1);
            rm.y1 = nd.y1 + rand() % (sc_h - h + 1);
            rm.x2 = rm.x1 + w - 1;
            rm.y2 = rm.y1 + h - 1;

            int d = 1 + rand() % 15;
            if (d >= 8) {
                d -= 8;
                rm.doors.push_back({rm.x1, rm.y1 + 1 + rand() % (h - 2), 8});
            }
            if (d >= 4) {
                d -= 4;
                rm.doors.push_back({rm.x1 + 1 + rand() % (w - 2), rm.y2, 4});
            }
            if (d >= 2) {
                d -= 2;
                rm.doors.push_back({rm.x2, rm.y1 + 1 + rand() % (h - 2), 2});
            }
            if (d >= 1) {
                d -= 1;
                rm.doors.push_back({rm.x1 + 1 + rand() % (w - 2), rm.y1, 1});
            }

            for (int i=rm.y1; i<=rm.y2; i++) {
                for (int j=rm.x1; j<=rm.x2; j++) {
                    map[j][i] = '.';
                }
            }

            for (int j=rm.x1; j<=rm.x2; j++) {
                map[j][rm.y1] = '#';
                map[j][rm.y2] = '#';
            }
            for (int j=rm.y1; j<=rm.y2; j++) {
                map[rm.x1][j] = '#';
                map[rm.x2][j] = '#';
            }

            for (int i=0; i<rm.doors.size(); i++) {
                map[rm.doors[i].xx][rm.doors[i].yy] = '[';
            }

            rooms.push_back(rm);
        }
        
        player.x = (rooms[0].x2 + rooms[0].x1) / 2;
        player.y = (rooms[0].y2 + rooms[0].y1) / 2;
        map[(rooms.back().x2 + rooms.back().x1) / 2][(rooms.back().y2 + rooms.back().y1) / 2] = '>';

        // Генерация дверей
        vector <vector <int> > owner(width, vector <int>(height, -1)), checked(width, vector <int>(height, 0));
        for (int i=0; i<rooms.size(); i++) {
            room rm = rooms[i];
            for (int j=0; j<rm.doors.size(); j++) {
                switch (rm.doors[j].dir) {
                    int xx, yy;
                    case (8):
                        xx = rm.doors[j].xx - 1;
                        while (map[xx][rm.doors[j].yy] != '-') {
                            map[xx][rm.doors[j].yy] = '-';
                            owner[xx][rm.doors[j].yy] = i;
                            xx--;
                        }
                        owner[xx][rm.doors[j].yy] = i;
                        break;
                    case (4):
                        yy = rm.doors[j].yy + 1;
                        while (map[rm.doors[j].xx][yy] != '-') {
                            map[rm.doors[j].xx][yy] = '-';
                            owner[rm.doors[j].xx][yy] = i;
                            yy++;
                        }
                        owner[rm.doors[j].xx][yy] = i;
                        break;
                    case (2):
                        xx = rm.doors[j].xx + 1;
                        while (map[xx][rm.doors[j].yy] != '-') {
                            map[xx][rm.doors[j].yy] = '-';
                            owner[xx][rm.doors[j].yy] = i;
                            xx++;
                        }
                        owner[xx][rm.doors[j].yy] = i;
                        break;
                    case (1):
                        yy = rm.doors[j].yy - 1;
                        while (map[rm.doors[j].xx][yy] != '-') {
                            map[rm.doors[j].xx][yy] = '-';
                            owner[rm.doors[j].xx][yy] = i;
                            yy--;
                        }
                        owner[rm.doors[j].xx][yy] = i;
                        break;
                }
            }
        }

        dsu rm_dsu(rooms.size());
        non_connected_rooms = rooms.size() - 1;

        for (int i=0; i<rooms.size(); i++) {
            room rm = rooms[i];
            for (int j=0; j<rm.doors.size(); j++) {
                door dr = rm.doors[j];
                make_path(dr, i, owner, rm_dsu, 0);
            }
        }

        // Соединение комнат
        while (non_connected_rooms) {
            int i = rand() % rooms.size();
            room rm = rooms[i];
            door dr = rm.doors[rand() % rm.doors.size()];
            make_path(dr, i, owner, rm_dsu, 1);
        }

        // Преобработка
        for (int i=0; i<height; i++) {
            for (int j=0; j<width; j++) {
                switch (map[j][i]) {
                    case '|':
                    case '-':
                    case '_':
                        map[j][i] = ' ';
                        break;
                    case '*':
                        map[j][i] = '.';
                        for (int ii=i-1; ii<=i+1; ii++) {
                            for (int jj=j-1; jj<=j+1; jj++) {
                                switch (map[jj][ii]) {
                                    case '|':
                                    case '-':
                                    case '_':
                                    case ' ':
                                        map[jj][ii] = '#';
                                        break;
                                }
                            }
                        }
                        break;
                }
            }
        }
    }

    void draw() {
        string line = "";
        for (int i=0; i<height; i++) {
            for (int j=0; j<width; j++) {
                if (j == player.x && i == player.y) {
                    line += '@';
                } else
                    line += map[j][i];
            }
            line += "\n";
        }
        cout << line;
    }

    void make_path(door dr, int i, vector <vector <int> > &owner, dsu &rm_dsu,  bool flag = 0) {
        vector <door> q;
        int q_pointer = 0;
        q.push_back({dr.xx, dr.yy, 0});

        vector <vector <int> > checked(width, vector <int>(height, 0));

        while (q.size() > q_pointer) {
            door point = q[q_pointer];
            if (owner[point.xx][point.yy] != -1 && ((owner[point.xx][point.yy] != i && flag == 0) || rm_dsu.get(owner[point.xx][point.yy]) != rm_dsu.get(i))) {
                if (rm_dsu.get(owner[point.xx][point.yy]) != rm_dsu.get(i)) 
                    non_connected_rooms--;
                rm_dsu.unite(i, owner[point.xx][point.yy]);

                owner[point.xx][point.yy]=i;
                map[point.xx][point.yy] = '*';
                while (point.dir != q[point.dir].dir) {
                    point = q[point.dir];
                    owner[point.xx][point.yy]=i;
                    map[point.xx][point.yy] = '*';
                };

                break;
            }

            if (!checked[point.xx + 1][point.yy] && (map[point.xx + 1][point.yy] == '-' || map[point.xx + 1][point.yy] == '*')) {q.push_back({point.xx + 1, point.yy, q_pointer}); checked[point.xx + 1][point.yy] = 1;}
            if (!checked[point.xx - 1][point.yy] && (map[point.xx - 1][point.yy] == '-' || map[point.xx - 1][point.yy] == '*')) {q.push_back({point.xx - 1, point.yy, q_pointer}); checked[point.xx - 1][point.yy] = 1;}
            if (!checked[point.xx][point.yy + 1] && (map[point.xx][point.yy + 1] == '-' || map[point.xx][point.yy + 1] == '*')) {q.push_back({point.xx, point.yy + 1, q_pointer}); checked[point.xx][point.yy + 1] = 1;}
            if (!checked[point.xx][point.yy - 1] && (map[point.xx][point.yy - 1] == '-' || map[point.xx][point.yy - 1] == '*')) {q.push_back({point.xx, point.yy - 1, q_pointer}); checked[point.xx][point.yy - 1] = 1;}
            
            q_pointer++;
        }
    }

    void move_player(int xx, int yy) {
        switch (map[xx][yy]) {
            case '.':
            case '[':
            case '>':
                player.x = xx;
                player.y = yy;
                if (map[xx][yy] == '>') {
                    generate();
                }
                break;
        }
    }

public:
    dungeon_map() {
        map.resize(width, vector <char> (height, ' '));
    }
};