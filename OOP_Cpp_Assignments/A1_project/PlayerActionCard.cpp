#include "PlayerActionCard.hpp"
#include "Player.hpp"

#include <string>

PlayerActionCard::PlayerActionCard(std::string &id, CompassDirection shark_direction) : 
  ActionCard(id, shark_direction) {}


void PlayerActionCard::play() 
{
  Player *owner = getOwner();
  Player *target = getTargetPlayer();

  if (getId() == "RWAVE") 
  {
    target->move(CompassDirection::SOUTH);
  } 
  else if (getId() == "MSSGE") 
  {
    std::size_t current_target_rations = target->getRations();
    if (current_target_rations >= 2) 
    {
      owner->addRations(2);
      target->deductRations(2);
    } 
    else if (current_target_rations == 1) 
    {
      owner->addRations(1);
      target->deductRations(1);
    }
  }
}
