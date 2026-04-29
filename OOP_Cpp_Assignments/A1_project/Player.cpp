#include "Player.hpp"
#include <iostream>

Player::Player(std::size_t id) : 
  Entity(EntityType::PLAYER, id, 1), id_(id), rations_(12) {}


Player::~Player() 
{
  for (auto card : hand_cards_)
  {
    delete card;
  }
  hand_cards_.clear();
}


bool Player::move(CompassDirection movement_direction) 
{
  if (getType() != EntityType::PLAYER) 
  {
    return false; 
  }
  auto &position = coordinatesRef();
  if (!position.has_value()) 
  {
    return false;
  }

  std::size_t x = position->getX();
  std::size_t y = position->getY();

  switch (movement_direction) {
  case CompassDirection::NORTH:
    if (y >= 5)
    {
      return false;
    }
    position = Coordinates(x, y + 1);
    return true;
  case CompassDirection::SOUTH:
    if (y <= 1)
    {
      return false;
    }
    position = Coordinates(x, y - 1);
    return true;
  default:
    return false;
  }
}


bool Player::hasStarved() { return rations_ == 0; }


std::vector<ActionCard *> &Player::getHandCards() { return hand_cards_; }


std::size_t Player::getId() const { return id_; }


std::size_t Player::getRations() const { return rations_; }


void Player::deductRations(std::size_t amount) 
{
  if (amount == 0 || rations_ == 0) 
  {
    return;
  }
  const std::size_t before = rations_;
  if (amount >= rations_) 
  {
    rations_ = 0;
  } 
  else 
  {
    rations_ -= amount;
  }
  if (before > 0 && rations_ == 0) 
  {
    std::cout << "Oh no, Player " << id_ << " has starved!\n";
  }
}


void Player::addRations(std::size_t rations) 
{ 
  rations_ += rations; 
}
