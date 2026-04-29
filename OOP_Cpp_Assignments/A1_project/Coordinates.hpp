#ifndef COORDINATES_HPP
#define COORDINATES_HPP

#include <cstddef> 


class Coordinates 
{
private:
  std::size_t x_;
  std::size_t y_;

public:
  Coordinates(std::size_t x, std::size_t y);
  Coordinates(const Coordinates&) = default;
  ~Coordinates() = default;

  std::size_t getX() const;
  std::size_t getY() const;
};

#endif