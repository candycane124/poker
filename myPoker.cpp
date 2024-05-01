#include <vector>
#include <iostream>
#include <algorithm>
#include <random>
#include <string>
#include <map>
#include "myPoker.h"

using namespace std;


// ***CARD***

Card::Card(int rank, char suit) : rank{rank}, suit{suit} {}

pair<int, char> Card::getValue() const { 
    return pair(rank, suit); 
}

char Card::getSuit() const {
    return suit;
}

int Card::getRank() const {
    return rank;
}

string Card::toString() const {
    string result;
    if (rank == 14) {
        result = "A";
    } else if (rank == 13) {
        result = "K";
    } else if (rank == 12) {
        result = "Q";
    } else if (rank == 11) {
        result = "J";
    } else {
        result = to_string(rank);
    }
    if (suit == 's') {
        // result += "\u2664";
        result += "s";
    } else if (suit == 'c') {
        // result += "\u2667";
        result += "c";
    } else if (suit == 'd') {
        // result += "\u2666";
        result += "d";
    } else if (suit == 'h') {
        // result += "\u2665";
        result += "h";
    }
    return result;
}

bool operator<(const Card& l, const Card& r) {
    return l.getRank() < r.getRank();
}

pair<int,string> scoreHand(vector<Card> hand) { //IMPROVE
    //put card values into map
    map<int, int> values;
    for (auto& c : hand) {
        if (values.count(c.getRank())) {
            values[c.getRank()]++;
        } else {
            values[c.getRank()] = 1;
        }
    }

    //check flush
    bool flush = false;
    if (hand[0].getSuit() == hand[1].getSuit() && hand[0].getSuit() == hand[2].getSuit() && hand[0].getSuit() == hand[3].getSuit() && hand[0].getSuit() == hand[4].getSuit()) {
        flush = true;
    }

    //check # of a kind
    vector<int> pairs = {};
    pair<bool,int> triple = pair(false,0);
    pair<bool,int> quad = pair(false,0);
    for (auto& i : values) {
        if (i.second == 2) {
            pairs.push_back(i.first);
        } else if (i.second == 3) {
            triple = pair(true,i.first);
        } else if (i.second == 4) {
            quad = pair(true,i.first);
        }
    }

    //check straight
    pair<bool,int> straight = pair(false,0);
    int i = 2;
    while (i <= 10) {
        if (values.count(i) != 1) {
            i++;
        } else {
            for (int j = 1; j < 5; j++) {
                if (values.count(i+j)!=1) {
                    i += j+1;
                    break;
                }
                if (j==4) {
                    straight = pair(true,i);
                    i = 11;
                    break;
                }
            }
        }
    }

    bool betterHand = false;
    if (flush || !pairs.empty() || triple.first || quad.first || straight.first) betterHand = true;
    //find high card
    int highCard = 0;
    for (auto& i : values) {
        if (betterHand) {
            if (i.first > highCard && i.second == 1) {
                highCard = i.first;
            }
        } else {
            if (i.first > highCard) {
                highCard = i.first;
            }
        }
    }

    //return score
    string type;
    int value;
    if (flush && straight.first) {
        value = 10600 + straight.second;
        type = "straight flush";
    } else if (quad.first) {
        value = 10300 + quad.second;
        type = "four of a kind";
    } else if (!pairs.empty() && triple.first) {
        value = 10000 + pairs[0] + 15*triple.second;
        type = "full house";
    } else if (flush) {
        value = 8100;
        type = "flush";
    } else if (straight.first) {
        value = 8000 + straight.second;
        type = "straight";
    } else if (triple.first) {
        value = 7000 + 15*triple.second + highCard;
        type = "three of a kind";
    } else if (pairs.size() == 2) {
        value = 5000 + 15*pairs[0] + 15*pairs[1] + highCard;
        type = "two pairs";
    } else if (pairs.size() == 1) {
        value = 2000 + pairs[0]*15 + highCard;
        type = "pair";
    } else {
        value = highCard;
        type = "high card";
    }
    return pair(value,type); //fix scoring, doesn't rly account for many ties
}


// ***PLAYER***

Player::Player() : name{"Anonymous"}, bestHandName{"n/a"}, inRound{true} {}
Player::Player(string name) : name{name}, bestHandName{"n/a"}, inRound{true} {}
Player::Player(string name, int startingMoney) : name{name}, bestHandName{"n/a"}, money{startingMoney}, inRound{true} {}

string Player::getName() const {
    return name;
}

vector<Card> Player::getHand() const {
    return hand;
}

string Player::getHandName() const {
    return bestHandName;
}

int Player::getMoney() const {
    return money;
}

bool Player::getStatus() const {
    return inRound;
}

void Player::setHand(Card card1, Card card2) {
    hand = {card1, card2};
}

bool Player::setMoney(int change) {
    if (change+money>0) {
        money += change;
        return true;
    }
    return false;
}

void Player::setStatus(bool in) {
    inRound = in;
}

string Player::toString() const {
    string result = name;
    if (!hand.empty()) {
        result += ": " + hand[0].toString() + ", " + hand[1].toString();
    }
    return result;
}

string Player::getHandString() const {
    string result = "Your Cards: " + hand[0].toString();
    result += " " + hand[1].toString();
    return result;
}

int Player::evaluateHand(vector<Card>& ccs) {
    vector<Card> sevenHand = {hand[0],hand[1],ccs[0],ccs[1],ccs[2],ccs[3],ccs[4]};
    int bestScore = -1;
    for (int i = 0; i < 6; i++) {
        for (int j = i+1; j < 7; j++) {
            vector<Card> newHand = sevenHand;
            newHand.erase(begin(newHand) + j);
            newHand.erase(begin(newHand) + i);
            pair<int,string> newValue = scoreHand(newHand);
            if (newValue.first > bestScore) {
                bestScore = newValue.first;
                this->bestHandName = newValue.second;
            }
        }
    }
    return bestScore;
}


// ***TABLE***

vector<Card> createDeck() {
    vector<char> suits = {'s','c','d','h'}; //space, club, diamond, heart
    vector<Card> deck = {};
    for (int i = 2; i <= 14; i++) {
        for (int j = 0; j < 4; j++) {
            deck.push_back(Card{i,suits[j]});
        }
    }
    return deck;
}
const vector<Card> Table::fullDeck = createDeck();
    
Table::Table(int max) : maxPlayers{max}, occupancy{0}, pot{0} {}

Table::~Table() {
    for (int i = 0; i < getOccupancy(); i++) {
        delete currentPlayers[i];
    }
}

vector<Player*> Table::getPlayers() const {
    return currentPlayers;
}

int Table::getOccupancy() const {
    return occupancy;
}

void Table::addPlayer() {
    if (occupancy==maxPlayers) {
        cout << "Cannot add player, table full." << endl;
    } else {
        currentPlayers.push_back(new Player());
        occupancy++;
    }
}

void Table::addPlayer(string name) {
    if (occupancy==maxPlayers) {
        cout << "Cannot add player, table full." << endl;
    } else {
        currentPlayers.push_back(new Player(name));
        occupancy++;
    }
}

void Table::addPlayer(string name, int money) {
    if (occupancy==maxPlayers) {
        cout << "Cannot add player, table full." << endl;
    } else {
        currentPlayers.push_back(new Player(name, money));
        occupancy++;
    }
}

void Table::removePlayer(string name) {
    for (int p = 0; p < getOccupancy(); p++) {
        if (currentPlayers[p]->getName() == name) {
            delete currentPlayers[p];
            currentPlayers.erase(begin(currentPlayers)+p);
        }
    }
}

void Table::clearTable() {
    for (int p = 0; p < getOccupancy(); p++) {
        delete currentPlayers[p];
    }
    currentPlayers = {};
}

void Table::playRound() {
    currentDeck = fullDeck;
    random_device rd;
    mt19937 g(rd());
    std::shuffle(begin(currentDeck), end(currentDeck), g);
    communityCards.clear();
    for (auto& p : currentPlayers) {
        p->setHand(dealNext(),dealNext());
        p->setStatus(true);
    }

    //blind
    bettingRound();
    if (checkIn()) {
        dealCommunity(3);
        //flop
        bettingRound();
        if (checkIn()) {
            dealCommunity(1);
            //turn
            bettingRound();
            if (checkIn()) {
                dealCommunity(1);
                //river
                bettingRound();
            }
        }
    }
    //end round
    endRound();
}

void Table::endRound() {
    vector<Player*> winner;
    int bestScore = -1;
    for (auto p : currentPlayers) {
        if (p->getStatus()) {
            int currScore = p->evaluateHand(communityCards);
            if (currScore == bestScore) {
                winner.push_back(p);
            } else if (currScore > bestScore) {
                winner = {p};
            }
        }
    }
    if (!winner.empty()) {
        cout << "Winner(s): " << endl;
        for (auto p : winner) {
            cout << " - " << p->getName() << " with a " << p->getHandName() << endl;
        }
    }
    int gain = pot/winner.size();
    for (auto p : winner) {
        p->setMoney(gain);
    }
    pot = 0;
}

Card Table::dealNext() {
    Card nextCard = currentDeck.back();
    currentDeck.pop_back();
    return nextCard;
}

void Table::toString() {
    cout << "Players: ";
    for (auto p : currentPlayers) {
        cout << p->toString() << " ";
    }
    cout << getCommunityString();
    cout << endl;
}

string Table::getCommunityString() {
    string result = "Community Cards: ";
    for (auto& c : communityCards) {
        result += c.toString() + " ";
    }
    return result;
}

void Table::dealCommunity(int n) {
    for (int i = 0; i < n; i++) {
        communityCards.push_back(dealNext());
    }
}

void Table::bettingRound() {
    bool done;
    int option;
    int matchBet = 0;
    for (auto p : currentPlayers) {
        if (p->getStatus()) {
            cout << endl;
            done = false;
            while (!done) {
                cout << getCommunityString() << endl;
                cout << p->getHandString() << endl;
                cout << p->getName() << ", what would you like to do?\n(1) Check\n(2) Raise\n(3) Fold" << endl;
                cin >> option;
                if (option == 1) {
                    if (p->setMoney(-1*matchBet)) {
                        pot += matchBet;
                    } else {
                        pot += p->getMoney();
                        p->setMoney(-1*p->getMoney());
                    }
                    done = true;
                } else if (option == 2) {
                    int raise;
                    cin >> raise;
                    if (matchBet+raise > p->getMoney()) {
                        cout << "You do not have enough coins to raise " << raise << endl;
                    } else {
                        matchBet += raise;
                        p->setMoney(-1*matchBet);
                        pot += matchBet;
                        done = true;
                    }
                } else if (option == 3) {
                    cout << "You folded" << endl;
                    p->setStatus(false);
                    done = true;
                }
            }
        }       
    }
}

bool Table::checkIn() {
    int ins = 0;
    for (auto p : currentPlayers) {
        if (p->getStatus()) {
            ins++;
        }
    }
    if (ins > 1) {
        return true;
    } else {
        return false;
    }
}


// ***SINGLEPLAYER***

SinglePlayer::SinglePlayer(string name) : Table{2}, currentState{0} {
    addPlayer(name, 1000);
    addPlayer("House");
}
SinglePlayer::SinglePlayer(string name, int startingAmt) : Table{2}, currentState{0} {
    addPlayer(name, startingAmt);
    addPlayer("House");
}

void SinglePlayer::endRound() {
    cout << endl;
    Player* user = currentPlayers[0];
    Player* house = currentPlayers[1];
    int userHand = user->evaluateHand(communityCards);
    int houseHand = house->evaluateHand(communityCards);
    if (user->getStatus() && userHand > houseHand) {
        user->setMoney(pot);
        cout << "You won " << pot << " coins!" << endl;
    } else {
        cout << "You lost, better luck next time" << endl;
    }
    cout << getCommunityString() << endl;
    cout << "You had a " << user->getHandName() << " (" << user->getHand()[0].toString() << " " << user->getHand()[1].toString() << ")";
    cout << "; the house had a " << house->getHandName() << " (" << house->getHand()[0].toString() << " " << house->getHand()[1].toString() << ")" << endl;
    pot = 0;
    cout << "Coins: " << currentPlayers[0]->getMoney() << endl;
    cout << endl;
}

void SinglePlayer::bettingRound() {
    bool done = false;
    int option;
    if (currentState == 0) {
        currentPlayers[0]->setMoney(-5);
        pot += 5;
    }
    cout << endl;
    while (!done) {
        cout << getCommunityString() << endl;
        cout << "Pot: " << pot << endl;
        cout << currentPlayers[0]->getHandString() << endl;
        cout << "Coins: " << currentPlayers[0]->getMoney() << endl;
        cout << "\nWhat would you like to do?\n(1) Check\n(2) Bet\n(3) Fold" << endl;
        cin >> option;

        if (option == 1) {
            done = true;
        } else if (option == 2) {
            int raise;
            int money = currentPlayers[0]->getMoney();
            cout << "How much would you like to bet? (You have " << money << " coins)" << endl;
            cin >> raise;
            if (raise > money) {
                cout << "You do not have enough coins" << endl;
            } else {
                currentPlayers[0]->setMoney(-1*raise);
                int inc;
                if (currentState==0) {
                    inc = raise*3;
                } else if (currentState==1) {
                    inc = raise*2;
                } else if (currentState==2) {
                    inc = raise*1.5;
                } else if (currentState==3) {
                    inc = raise*1.2;
                }
                pot += inc;
                done = true;
                cout << "You raised the pot by " << inc << ", it is now " << pot << " coin(s)" << endl;
            }
        } else if (option == 3) {
            currentPlayers[0]->setStatus(false);
            done = true;
        }
    }
    if (currentState == 3 || !checkIn()) {
        currentState = 0;
    } else {
        currentState++;
    }
}

bool SinglePlayer::checkIn() {
    return currentPlayers[0]->getStatus();
}