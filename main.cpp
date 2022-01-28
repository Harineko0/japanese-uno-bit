#include <iostream>
#include <bitset>
#include <bit>
#include <cstdint>

using namespace std;

// region Constants
const int MAX_PLAYER = 2;
const int MAX_CARD_RANK = 3;
const int MAX_INT_CARD = MAX_CARD_RANK * 4 - 1;
const int MAX_CARD_TO_HAVE = 2;
// endregion

// region Definitions
typedef __int64 BitCard;
typedef int IntCard;

struct Table {
    BitCard layout;
    int order;
    BitCard player0;
    BitCard player1;
};
// endregion

// region Variables
__int64 trials = 0;
__int64 win_times;
__int64 sum_win_times[MAX_CARD_RANK];

Table initTable = {0, 0, 0, 0};
// endregion

// region Util functions
void print_bit(BitCard num) {
    cout << bitset<52>(num) << endl;
}

void print(const string& str) {
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
// endregion

// region Core functions
void next_game(Table table) {
    // check is end next_game
//    int player_who_has_card = 0;
//    for (int i = 0; i < MAX_PLAYER; i++) {
//        BitCard playing = table.players[i];
//        if (__popcount(playing) > 0) {
//            player_who_has_card++;
//        }
//    }

    bool firstHasCard = __popcount(table.player0) == 0;
    if (firstHasCard || __popcount(table.player1) == 0) {
        trials++;
        if (firstHasCard){
            win_times++;
        }
        return;
    }

    // check can pass
    // プレイヤーが2人だから今出す順番の人が出せなければ(最上位ビットがテーブル以下ならば)パス
    BitCard playing;
    if (table.order == 1) {
        playing = table.player1;
    } else {
        playing = table.player0;
    }

    BitCard tableCard = bsr(table.layout) >> 2;
    // Pass
    if (__popcount(table.layout) != 0 and bsr(playing) >> 2 <= tableCard) {
        next_game({0, next_order(table.order), table.player0, table.player1});
    }

    BitCard c = playing;

    while(c)
    {
        IntCard card = bsf(c);
//        print("--------------------------------------------------");
//        print(to_string(order));
//        print_bit( order == 0 ? playing : player0);
//        print_bit( order == 1 ? playing : player1);
//        print_bit(card);
//        print(to_string((card >> 2)) + ", " + to_string(tableCard));

        if (tableCard == -1 or card >> 2 > tableCard) {
//            print(">>>>>>>>>>>>>");
//            print_bit( order == 0 ? playing & ~(1 << card) : player0);
//            print_bit( order == 1 ? playing & ~(1 << card) : player1);
//            print_bit(table | (1 << card));
//            print("--------------------------------------------------");
            next_game({table.layout | (1 << card),
                       next_order(table.order),
                       table.order == 0 ? playing & ~(1 << card) : table.player0,
                       table.order == 1 ? playing & ~(1 << card) : table.player1});
        }

        c &= c - 1;
    }
}

void start_game(Table table) {
    BitCard c = table.player0;

    while (c) {
        IntCard ic = bsf(c);
        next_game({table.layout | (1 << ic),
                   next_order(table.order),
                   table.order == 0 ? table.player0 & ~(1 << ic) : table.player0,
                   table.order == 1 ? table.player0 & ~(1 << ic) : table.player1});

        sum_win_times[ic] += win_times;

        c &= c - 1;
    }
}

void player_loop(BitCard card_to_deal) {
    BitCard c = card_to_deal;

    while (c) {
        IntCard ic = bsf(c);
        if (__popcount(initTable.player0) < MAX_CARD_TO_HAVE) {
            initTable.player0 |= 1 << ic;
            player_loop(card_to_deal & ~ (1 << ic));
        } else if (__popcount(initTable.player1) < MAX_CARD_TO_HAVE) {
            initTable.player1 |= 1 << ic;
            player_loop(card_to_deal & ~ (1 << ic));
        } else {
//            print("----------");
//            print_bit(initTable.player0);
//            print_bit(initTable.player1);
            start_game(initTable);
            initTable = {0, 0, 0, 0};
        }

        c &= c - 1;
    }
}

// endregion

int main() {
//    int order = 1;
//    BitCard layout = 0;
//    BitCard player0 = 1ULL << 0 | 1ULL << 4 | 1ULL << 8;
//    BitCard player1 = 1ULL << 0 | 1ULL << 4 | 1ULL << 8 | 1ULL << 12;
//
//    next_game({layout, order, player0, player1});

    BitCard all = (1ULL << (MAX_INT_CARD + 1)) - 1ULL;
    player_loop(all);

    for (int i = 0; i < MAX_CARD_RANK; i++) {
        print(to_string(i) + ": " + to_string(sum_win_times[i]));
    }

//    print(to_string(trials) + ", " + to_string(win_times[0]));

    return 0;
}