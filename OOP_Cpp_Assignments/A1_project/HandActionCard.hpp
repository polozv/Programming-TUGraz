#ifndef HANDACTIONCARD_HPP
#define HANDACTIONCARD_HPP

#include "ActionCard.hpp"
#include <optional>
#include <string>

class HandActionCard : public ActionCard {
private:
  std::optional<std::size_t> target_hand_index_;

public:
  HandActionCard(std::string &id, CompassDirection shark_direction);
  HandActionCard(const HandActionCard&) = delete;
  ~HandActionCard() = default;

  void play() override;
  void setTargetHandIndex(std::size_t target_hand_index);
};

#endif
