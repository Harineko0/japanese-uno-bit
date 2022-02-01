#include <iostream>
#include <bitset>
#include <bit>

using namespace std;

// region Constants
const int MAX_PLAYER = 2;
const int MAX_CARD_RANK = 3;
const int MAX_INT_CARD = MAX_CARD_RANK * 4 - 1;
const int MAX_BIT_CARD = (1ULL << (MAX_INT_CARD + 1)) - 1ULL;
const int MAX_CARD_TO_HAVE = 5;

const int DP_PLAYER = MAX_BIT_CARD;
const int DP_LAYOUT = 1 << 3;
const int DP_ORDER = 1 << 1;
// endregion

// region Definitions
typedef long long BitCard;
typedef int IntCard;

struct Table {
    BitCard layout;
    int order;
    BitCard player0;
    BitCard player1;
};

struct Result {
    int trials;
    int win_times;

    inline Result operator+(Result a) const {
        return {a.trials + trials, a.win_times + win_times};
    }

    bool is_empty() const {
        return trials == 0;
    }
};
// endregion

// region Variables
long long sum_trials[MAX_CARD_RANK];
long long sum_win_times[MAX_CARD_RANK];
// endregion

// region Util functions
void print_bit(BitCard num) {
    cout << bitset<52>(num) << endl;
}

string to_string(const Table table) {
    return "-------\n" + to_string(table.player0) + "\n" + to_string(table.player1) + "\n-------------";
}

string to_string(const Result result) {
    return std::to_string(result.trials) + ", " + std::to_string(result.win_times);
}

// 最上位bit
int bsr(unsigned long long v) {
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
int bsf(unsigned long long v) {
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
Result next_game(Table table, Result DP[DP_PLAYER][DP_PLAYER][DP_LAYOUT][DP_ORDER]) {
    int layoutRank = bsr(table.layout) >> 2;

    Result dp = DP[table.player0][table.player1][layoutRank][table.order];
//    print(dp.to_string());
    if (!dp.is_empty()) {
        return dp;
    }

    // check is end next_game
    bool firstHasCard = __popcount(table.player0) == 0;
    if (firstHasCard || __popcount(table.player1) == 0) {
        Result result = {1, firstHasCard ? 1 : 0};

        Result resultForDP = result;
        DP[table.player0][table.player1][layoutRank][table.order] = resultForDP;

        return result;
    }

    // check can pass
    // プレイヤーが2人だから今出す順番の人が出せなければ(最上位ビットがテーブル以下ならば)パス
    BitCard playing;
    if (table.order == 1) {
        playing = table.player1;
    } else {
        playing = table.player0;
    }

    if (__popcount(table.layout) != 0 and bsr(playing) >> 2 <= layoutRank) {
        Result result = next_game({0,
                                   next_order(table.order),
                                   table.player0,
                                   table.player1}
                                   , DP);

        Result resultForDP = result;
        DP[table.player0][table.player1][layoutRank][table.order] = resultForDP;

        return result;
    }

    BitCard c = playing;
    Result result{};
    Result tmpResult{};

    while(c)
    {
        IntCard card = bsf(c);
        if (layoutRank == -1 or card >> 2 > layoutRank) {
            tmpResult = next_game({table.layout | (1 << card),
                       next_order(table.order),
                       table.order == 0 ? playing & ~(1 << card) : table.player0,
                       table.order == 1 ? playing & ~(1 << card) : table.player1}
                       , DP);
            result.win_times += tmpResult.win_times;
            result.trials += tmpResult.trials;
        }

        c &= c - 1;
    }

    Result resultForDP = result;
    DP[table.player0][table.player1][layoutRank][table.order] = resultForDP;

    return result;
}

void start_game(Table table, Result DP[DP_PLAYER][DP_PLAYER][DP_LAYOUT][DP_ORDER]) {
    BitCard c = table.player0;
    int currentRank = -1;

    while (c) {
        IntCard ic = bsf(c);
        int rank = ic >> 2;
        if (currentRank < rank) {
            currentRank = rank;

            Result result = next_game({table.layout | (1 << ic),
                                       1,
                                       table.player0 & ~(1 << ic),
                                       table.player1}
                                       , DP);

            sum_win_times[rank] += result.win_times;
            sum_trials[rank] += result.trials;
        }

        c &= c - 1;
    }
}

void deal_card(BitCard card_to_deal, Table initTable, Result DP[DP_PLAYER][DP_PLAYER][DP_LAYOUT][DP_ORDER]) {
    BitCard c = card_to_deal;

    while (c) {
        IntCard ic = bsf(c);
        Table nextTable = initTable;

        // 順列ではなく組み合わせ。プレイヤーの最大IntCardよりもdealするカードの方が大きければ配る
        if (__popcount(initTable.player0) < MAX_CARD_TO_HAVE) {
            if (bsr(initTable.player0) < ic) {
                nextTable.player0 |= 1 << ic;
                deal_card(card_to_deal & ~(1 << ic), nextTable, DP);
            }
        } else if (__popcount(initTable.player1) < MAX_CARD_TO_HAVE) {
            if (bsr(initTable.player1) < ic) {
                nextTable.player1 |= 1 << ic;
                deal_card(card_to_deal & ~(1 << ic), nextTable, DP);
            }
        } else {
            start_game(nextTable, DP);
        }

        c &= c - 1;
    }
}
// endregion

int main() {
    Result (*DP)[DP_PLAYER][DP_LAYOUT][DP_ORDER] = (Result(*)[DP_PLAYER][DP_LAYOUT][DP_ORDER])malloc(DP_PLAYER * DP_PLAYER * DP_LAYOUT * DP_ORDER * sizeof(Result));

    if(DP == nullptr){
        printf("Error\n");
    }else{
        printf("OK\n");
    }

    for(int i = 0; i < DP_PLAYER; i++){
        for(int j = 0; j < DP_PLAYER; j++){
            for(int k = 0; k < DP_LAYOUT; k++){
                for(int l = 0; l < DP_ORDER; l++){
                    DP[i][j][k][l] = {0, 0};
                }
            }
        }
    }

    deal_card(MAX_BIT_CARD, {0, 0, 0, 0}, DP);

    for (int i = 0; i < MAX_CARD_RANK; i++) {
        printf("%d: %lld / %lld = %f\n", i, sum_win_times[i], sum_trials[i], (double)sum_win_times[i] / (double)sum_trials[i]);
    }

    return 0;
}