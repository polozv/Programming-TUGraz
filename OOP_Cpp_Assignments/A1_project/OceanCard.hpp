#ifndef OCEANCARD_HPP
#define OCEANCARD_HPP

#include "Card.hpp"
#include "Player.hpp"
#include <string>

enum class OceanCardType 
{ 
  CALM_WATERS, 
  OCEAN_DEBRIS, 
  ISLAND_CALLING, 
  KRAKEN 
};

class OceanCard : public Card {
private:
  bool shark_icon_;

public:
  OceanCard(std::string &id, bool shark_icon);
  OceanCard(const OceanCard&) = delete;
  ~OceanCard() = default;

  void printInformationString() const override;
  void printPlayMessage() const override;
  void play() override;
  std::string getFullName() const;
  std::string getPlayMessageText() const;
  bool hasSharkIcon() const;
};

#endif
