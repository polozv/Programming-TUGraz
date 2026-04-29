#include "Shark.hpp"
#include "Utils.hpp"
#include <iostream>
#include <string>

Shark::Shark() : 
  Entity(EntityType::SHARK), territory_width_(0) {}


bool Shark::isActive() 
{ 
  return getCoordinates().has_value(); 
}


void Shark::setTerritoryWidth(std::size_t territory_width) 
{
  territory_width_ = territory_width;
}


bool Shark::move(CompassDirection movement_direction) 
{
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
  case CompassDirection::EAST:
    if (x >= territory_width_)
    {
      return false;
    }
    position = Coordinates(x + 1, y);
    return true;
  case CompassDirection::WEST:
    if (x <= 1)
    {
      return false;
    }
    position = Coordinates(x - 1, y);
    return true;
  default:
    return false;
  }
}


char Shark::directionToLetter(CompassDirection direction) 
{
  switch (direction) {
  case CompassDirection::NORTH:
    return 'N';
  case CompassDirection::EAST:
    return 'E';
  case CompassDirection::SOUTH:
    return 'S';
  case CompassDirection::WEST:
    return 'W';
  default:
    return 'N';
  }
}


void Shark::bite(Player *player) 
{
  std::cout << "[" << UNICODE_SHARK
            << "] Yum, the shark was given a ration to eat!\n";

  player->deductRations(1);
}


void Shark::play(Player &current_player, std::vector<Player *> &players,
                 std::vector<CompassDirection> &shark_path) 
{
  auto coord = current_player.getCoordinates();
  if (!coord.has_value()) 
  {
    return;
  }
  std::size_t x = coord->getX();
  std::size_t y = coord->getY();

  if (!isActive()) 
  {
    if (y == 1) {
      coordinatesRef() = std::make_optional(Coordinates(x, y));
    } 
    else 
    {
      coordinatesRef() = std::make_optional(Coordinates(x, y - 1));
    }

    std::cout << "[" << UNICODE_SHARK
              << "] The shark smells food and approaches the players...\n";
    return;
  }

  std::string path;
  for (auto dir : shark_path) {
    path += directionToLetter(dir);
    path += ' ';
  }

  std::cout << "[" << UNICODE_SHARK
            << "] The shark will move along the path [ " << path
            << "] swiftly!\n";

  std::size_t start_x = coordinatesRef()->getX();
  std::size_t start_y = coordinatesRef()->getY();

  for (auto dir : shark_path) 
  {
    move(dir);

    std::size_t x_shark = coordinatesRef()->getX();
    std::size_t y_shark = coordinatesRef()->getY();

    if (x_shark == start_x && y_shark == start_y) 
    {
      continue;
    }

    for (auto player : players) 
    {
      if (!player->getCoordinates().has_value() || player->hasStarved()) 
      {
        continue;
      }
      auto player_coord = player->getCoordinates();
      if (x_shark == player_coord->getX() && y_shark == player_coord->getY()) 
      {
        bite(player);
      }
    }
  }
}