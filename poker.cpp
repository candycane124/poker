#include <vector>
#include <iostream>
#include <algorithm>
#include <random>
#include <string>
#include <map>

using namespace std;

class Card {
    public:
        Card(int rank, char suit);
        std::pair<int, char> getValue() const { return pair(rank, suit); }
        char getSuit() const { return suit; }
        int getRank() const { return rank; }
        std::string toString() const;

        friend bool operator<(const Card& l, const Card& r);
    private:
        int rank;
        char suit;
};

Card::Card(int rank, char suit) : rank{rank}, suit{suit} {}

bool operator<(const Card& l, const Card& r) {
    return l.getRank() < r.getRank();
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
        result += "\u2664";
    } else if (suit == 'c') {
        result += "\u2667";
    } else if (suit == 'd') {
        result += "\u2666";
    } else if (suit == 'h') {
        result += "\u2665";
    }
    return result;
}


pair<int,string> scoreHand(vector<Card> hand) {
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


class Player {
    public:
        Player();
        Player(std::string name);
        Player(std::string name, int startingMoney);
        void setHand(Card card1, Card card2);
        std::string getName() const { return name; }
        std::vector<Card> getHand() const { return hand; }
        std::string getHandName() const { return bestHandName; }
        int getMoney() const { return money; }
        void setMoney(int amount) { money += amount; }
        std::string toString() const;
        int evaluateHand(vector<Card>& ccs);
    private:
        std::string name;
        std::vector<Card> hand;
        std::string bestHandName;
        int money;
};

Player::Player() : name{"Anonymous"}, bestHandName{"n/a"} {}
Player::Player(string name) : name{name}, bestHandName{"n/a"} {}
Player::Player(string name, int startingMoney) : name{name}, bestHandName{"n/a"}, money{startingMoney} {}

void Player::setHand(Card card1, Card card2) {
    hand = {card1, card2};
}

string Player::toString() const {
    string result = name;
    if (!hand.empty()) {
        result += ": " + hand[0].toString() + ", " + hand[1].toString();
    }
    return result;
}

int Player::evaluateHand(vector<Card>& ccs) {
    vector<Card> sevenHand = {hand[0],hand[1],ccs[0],ccs[1],ccs[2],ccs[3],ccs[4]};
    // cout << "evaluating " << name << endl;
    int bestHand = -1;
    for (int i = 0; i < 6; i++) {
        for (int j = i+1; j < 7; j++) {
            //make new 5 card hand without cards at index i & j
            vector<Card> newHand = sevenHand;
            newHand.erase(begin(newHand) + j);
            newHand.erase(begin(newHand) + i);
            pair<int,string> newValue = scoreHand(newHand);
            // cout << newValue.second << ", ";
            if (newValue.first > bestHand) {
                // cout << "setting new best hand" << endl;
                bestHand = newValue.first;
                this->bestHandName = newValue.second;
            }
        }
    }
    return bestHand;
}

class Table {
    public:
        Table(int max);
        ~Table();

        void addPlayer();
        void addPlayer(std::string name);
        void addPlayer(std::string name, int money);
        void removePlayer(std::string name);
        virtual void newRound();
        Card dealNext();
        // void declareWinner();
        virtual void printTable();
        std::vector<Player*> getPlayers() const { return currentPlayers; }
        int getOccupancy() const { return currentPlayers.size(); }
        int getState() const { return currentState; }

        const int maxPlayers;
        static std::vector<Card> fullDeck;
    protected:
        int currentState; // -1: game unstarted, 0: no cards dealt, 1: flop, 2: turn, 3: river
        
        std::vector<Player*> currentPlayers;
        std::vector<Card> communityCards;
        std::vector<Card> currentDeck = fullDeck;
};
vector<Card> Table::fullDeck = {};


Table::Table(int max) : currentState{-1}, maxPlayers{max} {}
Table::~Table() {
    for (int i = 0; i < getOccupancy(); i++) {
        delete currentPlayers[i];
    }
}

void Table::addPlayer() {
    if (getOccupancy()==maxPlayers) {
        cout << "table is full" << endl;
    } else {
        currentPlayers.push_back(new Player());
    }
}

void Table::addPlayer(string name) {
    if (getOccupancy()==maxPlayers) {
        cout << "table is full" << endl;
    } else {
        currentPlayers.push_back(new Player(name));
    }
}

void Table::addPlayer(string name, int money) {
    if (getOccupancy()==maxPlayers) {
        cout << "table is full" << endl;
    } else {
        currentPlayers.push_back(new Player(name, money));
    }
}

void Table::removePlayer(string name) {
    for (int p = 0; p < getOccupancy(); p++) {
        if (currentPlayers[p]->getName() == name) {
            currentPlayers.erase(begin(currentPlayers)+p);
        }
    }
}

void Table::newRound() {
    currentState = -1;
    currentDeck = fullDeck;
    communityCards.clear();
    random_device rd;
    mt19937 g(rd());
    std::shuffle(begin(currentDeck), end(currentDeck), g);

    for (auto& p : currentPlayers) {
        p->setHand(dealNext(),dealNext());
    }

    currentState = 0;
}

Card Table::dealNext() {
    Card nextCard = currentDeck.back();
    if (currentState == -1) {
        currentDeck.pop_back();
    } else if (currentState == 0) {
        for (int i = 0; i < 3; i++) {
            currentDeck.pop_back();
            nextCard = currentDeck.back();
            currentDeck.pop_back();
            communityCards.push_back(nextCard);
        }
        currentState = 1;
    } else if (currentState == 1 || currentState == 2) {
        currentDeck.pop_back();
        nextCard = currentDeck.back();
        currentDeck.pop_back();
        communityCards.push_back(nextCard);
        currentState++;
    }
    return nextCard;
}

// void Table::declareWinner() {
//     if (currentPlayers.empty()) {
//         cout << "there is no one at the table" << endl;
//     } else {
//         vector<Player*> winner = {currentPlayers[0]};
//         int bestScore = currentPlayers[0]->evaluateHand(communityCards);
//         for (int i = 1; i < getOccupancy(); i++) {
//             int currScore = currentPlayers[i]->evaluateHand(communityCards);
//             if (currScore == bestScore) {
//                 winner.push_back(currentPlayers[i]);
//             } else if (currScore > bestScore) {
//                 winner = {currentPlayers[i]};
//             }
//         }
//         cout << "and the winner is... " << endl;
//         for (auto p : winner) {
//             cout << " - " << p->getName() << " with a " << p->getHandName() << endl;
//         }
//     }
// }

void Table::printTable() {
    cout << "Players (" << getOccupancy() << "/" << maxPlayers << "): " << endl;
    for (auto p : currentPlayers) {
        cout << "[" << p->toString() << "] ";
    }
    cout << endl;
    if (communityCards.empty()) {
        cout << "\nNo cards dealt yet";
    } else {
        for (auto& c : communityCards) {
            cout << c.toString() << " ";
        }
    }
    cout << endl;
}

class SinglePlayer : public Table {
    public:
        SinglePlayer(std::string name);
        bool bet(int amount);
        void findWinner();
        virtual void printTable();
        virtual void newRound();
    private:
        // int betIncrements;
        int moneyPool;
};

SinglePlayer::SinglePlayer(string name) : Table{2}, moneyPool{0} {
    addPlayer("House");
    addPlayer(name, 100);
}

bool SinglePlayer::bet(int amount) {
    Player* user = currentPlayers[1];
    if (user->getMoney() >= amount) {
        if (currentState==0) {
            moneyPool += 3*amount;
        } else if (currentState==1) {
            moneyPool += 2*amount;
        } else if (currentState==2) {
            moneyPool += 1.5*amount;
        } else if (currentState==3) {
            moneyPool += 1.2*amount;
        }
        user->setMoney(-1*amount);
        return true;
    }
    return false;
    // int currentState; // -1: game unstarted, 0: no cards dealt, 1: flop, 2: turn, 3: river
}

void SinglePlayer::findWinner() {
    cout << endl;
    printTable();
    int houseScore = currentPlayers[0]->evaluateHand(communityCards);
    int playerScore = currentPlayers[1]->evaluateHand(communityCards);
    cout << endl;
    if (playerScore > houseScore) {
        currentPlayers[1]->setMoney(moneyPool);
        cout << "You won the pot! Good job, you had a " << currentPlayers[1]->getHandName() << "." << endl;
    } else {
        cout << "You lost. Better luck next time." << endl;
    }
    cout << "The house had " << currentPlayers[0]->getHand()[0].toString() << " " << currentPlayers[0]->getHand()[1].toString() << " (" << currentPlayers[0]->getHandName() << ")." << endl;
    // newRound();
}

void SinglePlayer::printTable() {
    Player* user = currentPlayers[1];
    cout << "Your Cards: " << user->getHand()[0].toString() << " " << user->getHand()[1].toString() << endl;
    if (communityCards.empty()) {
        cout << "No community cards dealt yet";
    } else {
        cout << "Community Cards: ";
        for (auto& c : communityCards) {
            cout << c.toString() << " ";
        }
    }
    cout << "\nCurrent Pot: " << moneyPool << endl;
    cout << "(You have " << user->getMoney() << ")" << endl;
}

void SinglePlayer::newRound() {
    currentState = -1;
    currentDeck = fullDeck;
    communityCards.clear();
    random_device rd;
    mt19937 g(rd());
    std::shuffle(begin(currentDeck), end(currentDeck), g);

    for (auto& p : currentPlayers) {
        p->setHand(dealNext(),dealNext());
    }
    
    currentState = 0;
    currentPlayers[1]->setMoney(-5);
    moneyPool = 5;
}


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



void check(SinglePlayer& t, bool& roundEnd) {
    if (t.getState()==3) {
        t.findWinner();
        roundEnd = true;
    } else {
        t.dealNext();
    }
}

void startSinglePlayerGame() {
    vector<char> suits = {'s','c','d','h'}; //space, club, diamond, heart
    for (int i = 2; i <= 14; i++) {
        for (int j = 0; j < 4; j++) {
            Table::fullDeck.push_back(Card{i,suits[j]});
        }
    }

    cout << "\n\n\nWelcome to Sauga Hold'em!" << endl;
    cout << "What's your name?" << endl;
    string name;
    cin >> name;
    cout << "\nHello, " << name << "! Let's start!" << endl;

    bool end = false;
    SinglePlayer t{name};
    
    int option;
    int rounds = 0;
    while (!end && t.getPlayers()[1]->getMoney()>0) {
        cout << "What would you like to do?\n(1) Start new round\n(2) See Rules\n(3) Exit" << endl;
        cin >> option;
        if (option == 1) {
            t.newRound();
            bool roundEnd = false;
            while (!roundEnd) {
                cout << endl;
                t.printTable();
                cout << "\nWhat would you like to do?\n(1) Check\n(2) Raise\n(3) Fold\n";
                cin >> option;

                if (option == 1) { //check
                    check(t, roundEnd);
                } else if (option == 2) { //bet
                    int amount;
                    cout << "How much would you like to bet? (You currently have " << t.getPlayers()[1]->getMoney() <<")" << endl;
                    cin >> amount;
                    if (!t.bet(amount)) {
                        cout << "You do not have enough money to bet " << amount << endl;
                    } else {
                        cout << "You bet " << amount << "." << endl;
                        check(t, roundEnd);
                    }
                } else if (option == 3) { //fold
                    cout << "You folded." << endl;
                    roundEnd = true;
                } else {
                    cout << "That is not one of the options." << endl;
                }
            }
            cout << endl;
            rounds++;
        } else if (option == 2) {
            cout << "\nThis game is essentially single player Texas Hold'em Poker. Hands are\nranked as normal, although the house wins in tie situations. Each round\nyou will automatically blind 5 into the pot. There will be 4 rounds of\nbetting; betting earlier will give a higher payoff: x3, x2, x1.5, & x1.2.\n" << endl;
        } else {
            end = true;
        }
    }
    cout << "\nThanks for playing, " << name << "! You played " << rounds << " round(s), and ended with net " << t.getPlayers()[1]->getMoney()-100 << ". Come back soon!" << endl;
}

void testing() {
    vector<char> suits = {'s','c','d','h'}; //space, club, diamond, heart
    for (int i = 2; i <= 14; i++) {
        for (int j = 0; j < 4; j++) {
            Table::fullDeck.push_back(Card{i,suits[j]});
        }
    }
    SinglePlayer t{"tester"};
    for (int i = 0; i < 10; i++) {
        t.newRound();
        t.dealNext();
        t.dealNext();
        t.dealNext();
        t.findWinner();
    }
}


int main() {
    // testing();
    startSinglePlayerGame();
    return 0;
}