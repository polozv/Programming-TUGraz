#include "Card.hpp"

#include <string>

Card::Card(std::string &id) : 
id_(id), name_(IDtoName(id)), owner_(nullptr) {}


std::string const &Card::getId() const { return id_; }


void Card::setOwner(Player *player) { owner_ = player; }


std::string const &Card::getName() const { return name_; }


Player *Card::getOwner() const { return owner_; }


std::string Card::IDtoName(std::string id) 
{
  if (id == "MSSGE")
    return "Message in a Bottle";
  if (id == "PIRAT")
    return "Pirate";
  if (id == "RWAVE")
    return "Rogue Wave";
  if (id == "LOSTS")
    return "Lost at Sea";

  if (id == "WATER")
    return "Calm Waters";
  if (id == "DBRIS")
    return "Ocean Debris";
  if (id == "ISLND")
    return "Island Calling";
  if (id == "KRKEN")
    return "Kraken";
  else
    return "";
}