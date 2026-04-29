#ifndef ACTIONCARD_HPP
#define ACTIONCARD_HPP

#include "Card.hpp"
#include <string>

class Player;

enum class ActionCardType {
    MESSAGE_IN_A_BOTTLE, 
    PIRATE, 
    ROGUE_WAVE, 
    LOST_AT_SEA
};

enum class CompassDirection {
    NORTH, 
    EAST, 
    SOUTH, 
    WEST
};


class ActionCard : public Card {
    private:
    CompassDirection shark_direction_;
    Player* target_player_;

    public:
    ActionCard(std::string &id, CompassDirection shark_direction);
    ActionCard(const ActionCard&) = delete;
    ~ActionCard() = default;

    static std::string compassDirectionToString(CompassDirection direction);

    void printInformationString() const override;
    void printPlayMessage() const override;
    virtual void play() override = 0;
    CompassDirection getSharkDirection() const;
    Player* getTargetPlayer() const;
    void setTargetPlayer(Player* target_player);
    std::string getPlayMessageText() const;
};

#endif