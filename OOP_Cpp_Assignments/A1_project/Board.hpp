#ifndef BOARD_HPP
#define BOARD_HPP

#include <array>
#include <vector>

#include "Coordinates.hpp"
#include "OceanCard.hpp"
#include "Player.hpp"
#include "Shark.hpp"

class Board {
private:
  std::size_t column_count_;
  std::vector<std::array<OceanCard *, 5>> card_slots_;
  bool active_;
  Player *findPlayerAt(std::vector<Player *> &players, std::size_t game_column,
                       std::size_t game_row);

public:
  Board(std::size_t column_count);
  Board(const Board&) = delete;
  ~Board();

  void print(std::vector<Player *> &players, Shark *shark);
  void togglePrint();
  void placeOceanCard(OceanCard *ocean_card, Coordinates coordinates);
  bool isPrintActive() const;
};

#endif