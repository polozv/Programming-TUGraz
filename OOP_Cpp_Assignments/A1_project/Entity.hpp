#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "ActionCard.hpp"
#include "Coordinates.hpp"
#include <optional>
#include <string>

enum class EntityType 
{ 
    PLAYER, 
    SHARK 
};

class Entity {
private:
  EntityType type_;
  std::optional<Coordinates> coordinates_;

protected:
  std::optional<Coordinates> &coordinatesRef();

public:
  Entity(EntityType type);
  Entity(EntityType type, std::size_t x, std::size_t y);
  Entity(const Entity&) = delete;
  virtual ~Entity() = default;

  virtual bool move(CompassDirection movement_direction) = 0;
  std::optional<Coordinates> getCoordinates() const;
  EntityType getType() const;
};

#endif