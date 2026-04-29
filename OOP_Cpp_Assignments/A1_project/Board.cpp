#include "Board.hpp"
#include "Utils.hpp"
#include <iomanip>
#include <iostream>

Board::Board(std::size_t column_count)
    : column_count_(column_count), card_slots_(column_count), active_(true) {
  for (std::array<OceanCard *, 5> &column : card_slots_) {
    column.fill(nullptr);
  }
}

Board::~Board() {
  for (std::array<OceanCard *, 5> &column : card_slots_) {
    for (OceanCard *card : column) {
      delete card;
    }
  }
  card_slots_.clear();
}

Player *Board::findPlayerAt(std::vector<Player *> &players,
                            std::size_t game_column, std::size_t game_row) {
  for (Player *player : players) {
    if (player->hasStarved() == false && player->getCoordinates().has_value() &&
        player->getCoordinates()->getX() == game_column &&
        player->getCoordinates()->getY() == game_row) {
      return player;
    }
  }
  return nullptr;
}

void Board::print(std::vector<Player *> &players, Shark *shark) {
  if (!active_)
    return;

  std::cout << '\n';

  for (std::size_t row = 5; row > 0; --row) {
    std::cout << ' ' << row << ' ';

    for (std::size_t column = 0; column < column_count_; ++column) {
      std::cout << '|';
      if (shark != nullptr && shark->isActive() &&
          shark->getCoordinates()->getX() == column + 1 &&
          shark->getCoordinates()->getY() == row) {
        std::cout << UNICODE_SHARK << " ";
      } else if (Player *p = findPlayerAt(players, column + 1, row)) {
        std::cout << 'P' << std::setw(2) << std::setfill('0') << p->getId();
        std::cout << std::setfill(' ');
      } else if (card_slots_[column][row - 1] != nullptr) {
        OceanCard *card = card_slots_[column][row - 1];
        std::cout << ' ' << card->getId()[0] << ' ';
      } else {
        std::cout << "   ";
      }
    }
    std::cout << "|\n";
  }

  std::cout << "     ";
  for (std::size_t c = 1; c < column_count_; ++c) {
    std::cout << c << "   ";
  }
  std::cout << column_count_ << "  \n";

  std::cout << "    ";
  for (std::size_t column = 0; column < column_count_; ++column) {
    std::cout << std::setw(3) << std::setfill('0')
              << players.at(column)->getRations();
    if (column + 1 < column_count_) {
      std::cout << ' ';
    }
  }
  std::cout << std::setfill(' ');
  std::cout << " rations" << '\n';
  std::cout << '\n';
}

void Board::togglePrint() { active_ = !active_; }


void Board::placeOceanCard(OceanCard *ocean_card, Coordinates coordinates) 
{
  std::size_t x = coordinates.getX();
  std::size_t y = coordinates.getY();

  if ((x >= 1 && x <= column_count_) && (y >= 1 && y <= 5)) 
  {
    std::size_t col = x - 1;
    std::size_t row = y - 1;
    OceanCard *old = card_slots_[col][row];
    if (old != nullptr && old != ocean_card) {
      delete old;
    }
    card_slots_[col][row] = ocean_card;
  }
}


bool Board::isPrintActive() const { return active_; }
