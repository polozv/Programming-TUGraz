#ifndef CARD_HPP
#define CARD_HPP

#include <string>

class Player;

class Card {
private:
  std::string id_;
  std::string name_;
  std::string IDtoName(std::string id);

protected:
  Player *owner_;

public:
  Card(std::string &id);
  Card(const Card&) = delete;
  virtual ~Card() = default;

  virtual void printInformationString() const = 0;
  virtual void printPlayMessage() const = 0;
  virtual void play() = 0;
  std::string const &getId() const;
  std::string const &getName() const;
  void setOwner(Player *player);
  Player *getOwner() const;
};

#endif