#include <vector>
#include <iostream>
#include <algorithm>
#include <random>
#include <string>
#include <map>
#include "myPoker.h"

using namespace std;

/*
ostream& operator<<(ostream& os, const Card& card) {
    string suit;
    pair<int, char> c = card.getValue();
    if (c.second == 's') {
        suit = "\u2664";
    } else if (c.second == 'c') {
        suit = "\u2667";
    } else if (c.second == 'd') {
        suit = "\u2666";
    } else if (c.second == 'h') {
        suit = "\u2665";
    }
    os << c.first << " " << suit;
    return os;
}

ostream& operator<<(ostream& os, const Player& player) {
    os << player.getName();
    vector<Card> hand = player.getHand();
    if (!hand.empty()) {
        os << ": " << player.getHand()[0] << ", " << player.getHand()[1];
    }
    return os;
}

ostream& operator<<(ostream& os, Player* player) {
    os << player->getName();
    vector<Card> hand = player->getHand();
    if (!hand.empty()) {
        os << ": " << player->getHand()[0] << ", " << player->getHand()[1];
    }
    return os;
}
*/

void startSinglePlayerGame() {

    cout << "\n\n\nWelcome to Sauga Hold'em!" << endl;
    cout << "What's your name?" << endl;
    string name;
    cin >> name;
    cout << "\nHello, " << name << "! Let's start!" << endl;

    bool end = false;
    int startingAmt = 500;
    SinglePlayer t{name, startingAmt};
    
    int option;
    int rounds = 0;
    while (!end && t.getPlayers()[0]->getMoney()>0) {
        cout << "What would you like to do?\n(1) Start new round\n(2) See Rules\n(3) Exit" << endl;
        cin >> option;
        if (option == 1) {
            t.playRound();
            rounds++;
        } else if (option == 2) {
            cout << "\nThis game is essentially single player Texas Hold'em Poker. Hands are\nranked as normal, although the house wins in tie situations. Each round\nyou will automatically blind 5 into the pot. There will be 4 rounds of\nbetting; betting earlier will give a higher payoff: x3, x2, x1.5, & x1.2.\n" << endl;
        } else {
            end = true;
        }
    }
    cout << "\nThanks for playing, " << name << "! You played " << rounds << " round(s), and ended with net " << t.getPlayers()[0]->getMoney()-startingAmt << ". Come back soon!" << endl;
}

int main() {
    startSinglePlayerGame();
    return 0;
}