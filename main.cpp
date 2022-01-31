#include <iostream>
#include <bitset>
#include <bit>
#include <cstdint>

using namespace std;

// region Constants
const int MAX_PLAYER = 2;
const int MAX_CARD_RANK = 3;
const int MAX_INT_CARD = MAX_CARD_RANK * 4 - 1;
const int MAX_CARD_TO_HAVE = 3;
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

struct Result {

};
// endregion

// region Variables
__int64 trials = 0;
__int64 win_times = 0;
__int64 sum_trials[MAX_CARD_RANK];
__int64 sum_win_times[MAX_CARD_RANK];

// endregion

// region Util functions
void print_bit(BitCard num) {
    cout << bitset<52>(num) << endl;
}

void print(const string& str) {
    cout << str << endl;
}

void print(const Table table){
    print("--------");
    print_bit(table.player0);
    print_bit(table.player1);
    print("--------");
}

// 最上位bit
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

// 最下位bit
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

    int tableRank = bsr(table.layout) >> 2;
    if (__popcount(table.layout) != 0 and bsr(playing) >> 2 <= tableRank) {
        next_game({0, next_order(table.order), table.player0, table.player1});
    }

    BitCard c = playing;

    while(c)
    {
        IntCard card = bsf(c);
        if (tableRank == -1 or card >> 2 > tableRank) {
            next_game({table.layout | (1 << card),
                       next_order(table.order),
                       table.order == 0 ? playing & ~(1 << card) : table.player0,
                       table.order == 1 ? playing & ~(1 << card) : table.player1});
        }

        c &= c - 1;
    }
}

void start_game(Table table) {
//    print(table);
    BitCard c = table.player0;
    int currentRank = -1;

    while (c) {
        IntCard ic = bsf(c);
        int rank = ic >> 2;
        if (currentRank < rank) {
            currentRank = rank;

            next_game({table.layout | (1 << ic),
                   1,
                   table.player0 & ~(1 << ic),
                   table.player1});

            sum_win_times[rank] += win_times;
            sum_trials[rank] += trials;
            win_times = 0;
            trials = 0;
        }

        c &= c - 1;
    }
}

void deal_card(BitCard card_to_deal, Table initTable) {
    BitCard c = card_to_deal;

    while (c) {

        IntCard ic = bsf(c);
        Table nextTable = initTable;

        // 順列ではなく組み合わせ。プレイヤーの最大IntCardよりもdealするカードの方が大きければ配る
        if (__popcount(initTable.player0) < MAX_CARD_TO_HAVE) {
            if (bsr(initTable.player0) < ic) {

                nextTable.player0 |= 1 << ic;
                deal_card(card_to_deal & ~(1 << ic), nextTable);

            }
        } else if (__popcount(initTable.player1) < MAX_CARD_TO_HAVE) {
            if (bsr(initTable.player1) < ic) {

                nextTable.player1 |= 1 << ic;
                deal_card(card_to_deal & ~(1 << ic), nextTable);

            }
        } else {
            start_game(nextTable);
        }

        c &= c - 1;
    }
}
// endregion

int main() {
    BitCard all = (1ULL << (MAX_INT_CARD + 1)) - 1ULL;
    deal_card(all, {0, 0, 0, 0});

    for (int i = 0; i < MAX_CARD_RANK; i++) {
        print(to_string(i) + ": " + to_string(sum_win_times[i]) + " / " + to_string(sum_trials[i]) + " = " + to_string((double)sum_win_times[i] / (double)sum_trials[i]));
    }

    return 0;
}