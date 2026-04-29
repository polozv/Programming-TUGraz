#include "Coordinates.hpp"

Coordinates::Coordinates(std::size_t x, std::size_t y) : x_(x), y_(y) {}


std::size_t Coordinates::getX() const { return x_; }
std::size_t Coordinates::getY() const { return y_; }