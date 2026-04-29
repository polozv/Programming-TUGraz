#ifndef PLAYERACTIONCARD_HPP
#define PLAYERACTIONCARD_HPP

#include "ActionCard.hpp"
#include <string>

class PlayerActionCard : public ActionCard {
public:
  PlayerActionCard(std::string &id, CompassDirection shark_direction);
  PlayerActionCard(const PlayerActionCard&) = delete;
  ~PlayerActionCard() = default;

  void play() override;
};

#endif