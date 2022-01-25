#include <iostream>
#include <bitset>

using namespace std;

const int MAX_PLAYER = 2;

int trials;
int win[MAX_PLAYER];

typedef int IntCard;

void print(int num) {
    cout << bitset<52>(num) << endl;
}

void next_game(int order, IntCard table, IntCard players[]) {
    IntCard c = players[order];

    while(c)
    {
        IntCard ic = c & -c;
        print(ic);

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

    print(players[0]);

    next_game(order, table, players);

    return 0;
}