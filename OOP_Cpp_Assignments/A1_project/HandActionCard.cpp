#include "HandActionCard.hpp"
#include "Player.hpp"

#include <string>
#include <vector>

HandActionCard::HandActionCard(std::string &id, CompassDirection shark_direction) : 
  ActionCard(id, shark_direction), target_hand_index_(std::nullopt) {}


void HandActionCard::setTargetHandIndex(std::size_t target_hand_index) 
{
  target_hand_index_ = target_hand_index;
}


void HandActionCard::play() 
{
  Player *owner = getOwner();
  Player *target = getTargetPlayer();
  std::size_t index = target_hand_index_.value();
  std::vector<ActionCard *> &target_hand = target->getHandCards();

  if (getId() == "PIRAT") 
  {
    ActionCard *to_steal = target_hand[index];
    std::erase(target_hand, to_steal);
    to_steal->setOwner(owner);
    owner->getHandCards().push_back(to_steal);
  } 
  else if (getId() == "LOSTS") 
  {
    ActionCard *to_delete = target_hand[index];
    std::erase(target_hand, to_delete);
    delete to_delete;
  }
}