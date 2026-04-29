#ifndef SHARK_HPP
#define SHARK_HPP

#include <vector>

#include "Entity.hpp"
#include "Player.hpp"


class Shark : public Entity {
    private:
    std::size_t territory_width_;
    char directionToLetter(CompassDirection direction);
    void bite(Player * player);

    public:
    Shark();
    Shark(const Shark&) = delete;
    ~Shark() = default;

    bool move(CompassDirection movement_direction) override;
    bool isActive();
    void play(Player &current_player, std::vector<Player *> &players, std::vector<CompassDirection> &shark_path);
    void setTerritoryWidth(std::size_t territory_width);
};

#endif