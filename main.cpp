#include <iostream>
#include <bitset>
#include <bit>
#include <cstdint>
#include <limits>

using namespace std;

const int MAX_PLAYER = 2;

int trials;
int win[MAX_PLAYER];

typedef int IntCard;

void print_bit(int num) {
    cout << bitset<52>(num) << endl;
}

void print(string str) {
    cout << str << endl;
}

int MSB64bit(unsigned __int64 v) {
    if (v == 0) -1;
    v |= (v >> 1);
    v |= (v >> 2);
    v |= (v >> 4);
    v |= (v >> 8);
    v |= (v >> 16);
    v |= (v >> 32);
    return __popcount(v) - 1;
}

int next_order(int order) {
    order++;
    if (order >= MAX_PLAYER) {
        order = 0;
    }
    return order;
}

void next_game(int order, IntCard table, IntCard players[]) {
    // check is end game
    int playerWhoHasCard = 0;
    for (int i = 0; i < MAX_PLAYER; i++) {
        IntCard player = players[i];
        if (__popcount(player) > 0) {
            playerWhoHasCard++;
        }
    }
    if (playerWhoHasCard < 2) {
        print("end");
        return;
    }

    // check can pass
    // プレイヤーが2人だから今出す順番の人が出せなければ(最上位ビットがテーブル以下ならば)パス
    IntCard p = players[order];
    bool canPass = __popcount(table) != 0 and MSB64bit(p) >> 2 <= MSB64bit(table) >> 2;
    if (canPass) {
        next_game(next_order(order), 0, players);
    }

    IntCard c = players[order];

    while(c)
    {
        IntCard ic = c & -c;
//        print_bit(ic);

        c &= c - 1;
    }
}

int main() {
    cout << "Hello, World!" << endl;

    int order = 0;
    IntCard table = 0;
    IntCard players[MAX_PLAYER];
    players[0] = 1ULL << 0 | 1ULL << 4 | 1ULL << 8;
    players[1] = 1ULL << 0 | 1ULL << 4 | 1ULL << 8;

    print_bit(players[0]);

    next_game(order, table, players);

    return 0;
}