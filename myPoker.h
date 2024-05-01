#pragma once
#include <vector>
#include <string>

class Card {
    public:
        Card(int rank, char suit);
        std::pair<int, char> getValue() const;
        char getSuit() const;
        int getRank() const;
        std::string toString() const;

        friend bool operator<(const Card& l, const Card& r);
    private:
        int rank;
        char suit;
};

bool operator<(const Card& l, const Card& r);

std::pair<int,std::string> scoreHand(std::vector<Card> hand);

class Player {
    public:
        Player();
        Player(std::string name);
        Player(std::string name, int startingMoney);
        std::string getName() const;
        std::vector<Card> getHand() const;
        std::string getHandName() const;
        int getMoney() const;
        bool getStatus() const;
        void setHand(Card card1, Card card2);
        bool setMoney(int change);
        void setStatus(bool in);
        std::string toString() const;
        std::string getHandString() const;
        int evaluateHand(std::vector<Card>& ccs);
    private:
        std::string name;
        std::vector<Card> hand;
        std::string bestHandName;
        int money;
        bool inRound;
};

class Table {
    public:
        Table(int max);
        virtual ~Table();
        std::vector<Player*> getPlayers() const;
        int getOccupancy() const;
        
        void addPlayer();
        void addPlayer(std::string name);
        void addPlayer(std::string name, int money);
        void removePlayer(std::string name);
        void clearTable();
        void playRound();
        virtual void endRound();
        Card dealNext();
        void toString();
        std::string getCommunityString();
        
        const int maxPlayers;
        const static std::vector<Card> fullDeck;
    protected:
        int occupancy;
        std::vector<Player*> currentPlayers;
        std::vector<Card> communityCards;
        std::vector<Card> currentDeck = fullDeck;
        int pot;

        void dealCommunity(int n);
        virtual void bettingRound();
        virtual bool checkIn();
};

class SinglePlayer : public Table {
    public:
        SinglePlayer(std::string name);
        SinglePlayer(std::string name, int startingAmt);
        virtual void endRound() override;
        // void probPrintSomething();
    private:
        int currentState;
        virtual void bettingRound() override;
        virtual bool checkIn() override;

};