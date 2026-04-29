#include "OceanCard.hpp"
#include "HandActionCard.hpp"
#include "Utils.hpp"

#include <iostream>
#include <map>
#include <string>

OceanCard::OceanCard(std::string &id, bool shark_icon) : 
    Card(id), shark_icon_(shark_icon) {}


std::string OceanCard::getFullName() const
{
  if (getId() == "WATER") 
  {
    return "Calm Waters";
  }
  if (getId() == "DBRIS") 
  {
    return "Ocean Debris";
  }
  if (getId() == "ISLND") 
  {
    return "Island Calling";
  }
  if (getId() == "KRKEN") 
  {
    return "Kraken";
  }
  return "";
}


std::string OceanCard::getPlayMessageText() const 
{
  if (getId() == "WATER") 
  {
    return "The sea is calm, nothing happened.";
  }
  if (getId() == "DBRIS") 
  {
    return "Gained a new action card!";
  }
  if (getId() == "ISLND") 
  {
    return "Moved forward and back again due to the island's calling!";
  }
  if (getId() == "KRKEN") 
  {
    return "Lost a ration to the Kraken!";
  }
  return "";
}


void OceanCard::printInformationString() const 
{
  std::cout << "- " << getFullName() << " - "
            << (hasSharkIcon() ? "SHARK" : "NO SHARK") << "\n";
}


void OceanCard::printPlayMessage() const 
{
  std::cout << "[" << UNICODE_OCEAN << "] Drew " << getFullName() << " - "
            << getPlayMessageText() << "\n";
}


void OceanCard::play() 
{
  if (owner_ == nullptr)
    return;

  if (getId() == "WATER") {}

  else if (getId() == "DBRIS") 
  {
    std::size_t rations = owner_->getRations();
    CompassDirection shark_direction;
    std::string action_id;

    if (owner_->getHandCards().empty()) 
    {
      shark_direction = CompassDirection::NORTH;
    } 
    else 
    {
      shark_direction = owner_->getHandCards()[0]->getSharkDirection();
    }
    if (rations % 2 == 0) 
    {
      action_id = "LOSTS";
    }
    else 
    {
      action_id = "PIRAT";
    }

    HandActionCard *new_card = new HandActionCard(action_id, shark_direction);
    owner_->getHandCards().push_back(new_card);
  } 
  else if (getId() == "ISLND")
  {
    owner_->move(CompassDirection::SOUTH);
  }
  else if (getId() == "KRKEN") 
  {
    owner_->deductRations(1);
  }
}


bool OceanCard::hasSharkIcon() const { return shark_icon_; }
