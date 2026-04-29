#include "ActionCard.hpp"
#include "Utils.hpp"

#include <string>
#include <iostream>


ActionCard::ActionCard(std::string &id, CompassDirection shark_direction) :
Card(id), shark_direction_(shark_direction), target_player_(nullptr) {}


std::string ActionCard::compassDirectionToString(CompassDirection direction)
{
    switch (direction) {
        case CompassDirection::NORTH: return "NORTH";
        case CompassDirection::EAST:  return "EAST";
        case CompassDirection::SOUTH: return "SOUTH";
        case CompassDirection::WEST:  return "WEST";
    }
    return "NORTH";
}


void ActionCard::printInformationString() const
{
    std::cout << "- " << getName() << " - "
              << ActionCard::compassDirectionToString(getSharkDirection())
              << "\n";
}


std::string ActionCard::getPlayMessageText() const
{
    if(getId() == "MSSGE")
    return "Stole up to 2 rations!";

    else if(getId() == "PIRAT")
    return "Stole another player's hand card!";

    else if(getId() == "RWAVE")
    return "Moved back one space!";

    else if(getId() == "LOSTS")
    return "Discarded another player's hand card!";

    else
    return"";
}


void ActionCard::printPlayMessage() const
{
    std::cout <<"[" << UNICODE_ACTION_FIRE << "] Played " << getName() << " - " << getPlayMessageText() << "\n";
}

CompassDirection ActionCard::getSharkDirection() const {return shark_direction_; }
Player* ActionCard::getTargetPlayer() const {return target_player_; }
void ActionCard::setTargetPlayer(Player* target_player) {target_player_ = target_player; }