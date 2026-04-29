#include "Entity.hpp"

Entity::Entity(EntityType type) : type_(type), coordinates_(std::nullopt) {}

Entity::Entity(EntityType type, std::size_t x, std::size_t y)
    : type_(type), coordinates_(std::make_optional(Coordinates{x, y})) {}


std::optional<Coordinates> Entity::getCoordinates() const { return coordinates_;}


std::optional<Coordinates> &Entity::coordinatesRef() { return coordinates_; }


EntityType Entity::getType() const { return type_; }