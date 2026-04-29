#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <vector>

#include "ActionCard.hpp"
#include "Entity.hpp"

class Player : public Entity {
private:
  std::size_t id_;
  std::vector<ActionCard *> hand_cards_;
  std::size_t rations_;

public:
  Player(std::size_t id);
  Player(const Player &other) = delete;
  ~Player() override;

  bool move(CompassDirection movement_direction) override;
  bool hasStarved();
  std::vector<ActionCard *> &getHandCards();
  std::size_t getId() const;
  std::size_t getRations() const;
  void deductRations(std::size_t amount);
  void addRations(std::size_t rations);
};

#endif