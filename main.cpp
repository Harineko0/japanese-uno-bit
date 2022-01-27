#include <iostream>
#include <bitset>
#include <bit>
#include <cstdint>

using namespace std;

const int MAX_PLAYER = 2;

__int64 trials = 0;
__int64 win[MAX_PLAYER] = {0, 0};
int maxCard = 8;
int winTimes[8];

typedef __int64 IntCard;

void print_bit(int num) {
    cout << bitset<52>(num) << endl;
}

void print(string str) {
    cout << str << endl;
}

int bsr(unsigned __int64 v) {
    if (v == 0) -1;
    v |= (v >> 1);
    v |= (v >> 2);
    v |= (v >> 4);
    v |= (v >> 8);
    v |= (v >> 16);
    v |= (v >> 32);
    return __popcount(v) - 1;
}

int bsf(unsigned __int64 v) {
    if (v == 0) return -1;
    v |= (v << 1);
    v |= (v << 2);
    v |= (v << 4);
    v |= (v << 8);
    v |= (v << 16);
    v |= (v << 32);
    return 64 - __popcount(v);
}

int next_order(int order) {
    order++;
    if (order >= MAX_PLAYER) {
        order = 0;
    }
    return order;
}

void next_game(int order, IntCard table, IntCard player0, IntCard player1) {
    // check is end game
//    int playerWhoHasCard = 0;
//    for (int i = 0; i < MAX_PLAYER; i++) {
//        IntCard player = players[i];
//        if (__popcount(player) > 0) {
//            playerWhoHasCard++;
//        }
//    }
    bool firstHasCard = __popcount(player0) == 0;
    if (firstHasCard || __popcount(player1) == 0) {
        trials++;
        if (firstHasCard){
            win[0]++;
        } else {
            win[1]++;
        }
//        print("end game");
        return;
    }

    // check can pass
    // プレイヤーが2人だから今出す順番の人が出せなければ(最上位ビットがテーブル以下ならば)パス
    IntCard player;
    if (order == 1) {
        player = player1;
    } else {
        player = player0;
    }

    IntCard tableCard = bsr(table) >> 2;
    bool canPass = __popcount(table) != 0 and bsr(player) >> 2 <= tableCard;

    if (canPass) {
//        print("pass");
        next_game(next_order(order), 0, player0, player1);
    }

    IntCard c = player;

    while(c)
    {
        IntCard card = bsf(c);
//        print("--------------------------------------------------");
//        print(to_string(order));
//        print_bit( order == 0 ? player : player0);
//        print_bit( order == 1 ? player : player1);
//        print_bit(card);
//        print(to_string((card >> 2)) + ", " + to_string(tableCard));

        if (tableCard == -1 or card >> 2 > tableCard) {
//            print(">>>>>>>>>>>>>");
//            print_bit( order == 0 ? player & ~(1 << card) : player0);
//            print_bit( order == 1 ? player & ~(1 << card) : player1);
//            print_bit(table | (1 << card));
//            print("--------------------------------------------------");
            next_game(next_order(order), table | (1 << card),
                      order == 0 ? player & ~(1 << card) : player0
                      , order == 1 ? player & ~(1 << card) : player1);
        }

        c &= c - 1;
    }
}

int main() {
    int order = 1;
    IntCard table = 1ULL << 12;
    IntCard player1 = 1ULL << 0 | 1ULL << 4 | 1ULL << 8;
    IntCard player2 = 1ULL << 0 | 1ULL << 4 | 1ULL << 8 | 1ULL << 12;



    next_game(order, table, player1, player2);

    print(to_string(trials) + ", " + to_string(win[0]));

    return 0;
}