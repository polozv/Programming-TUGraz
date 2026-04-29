#ifndef GAME_HPP
#define GAME_HPP

#include "ActionCard.hpp"
#include "Board.hpp"
#include "Command.hpp"
#include "CommandLine.hpp"
#include "HandActionCard.hpp"
#include "OceanCard.hpp"
#include "Player.hpp"
#include "PlayerActionCard.hpp"
#include "Shark.hpp"
#include "Utils.hpp"
#include <cstddef>
#include <fstream>
#include <string>
#include <vector>

class Game {
private:
  CommandLine command_line_;
  Board board_;
  std::vector<Player *> players_;
  Player *current_player_;
  int last_finished_player_;
  Shark *shark_;
  std::vector<ActionCard *> action_deck_;
  std::vector<OceanCard *> ocean_deck_;
  bool is_running_;
  std::string compassToString(CompassDirection d);
  void endWithDraw(bool print_board);

public:
  Game(std::size_t player_count);
  Game(const Game&) = delete;
  ~Game();

  bool checkMagicNumber(std::string &config_file_path);
  void loadConfigFile(std::string &config_file_path);
  Player *getNextPlayer();
  bool validateCommand(Command &command);
  bool executeCommand(Command &command);
  void start();
  bool float_command();
  bool swim();
  void printOceanDrew(const OceanCard *card);
  void gatherWay(std::vector<Player *> &players, std::vector<CompassDirection> &shark_path);
  void hand();
  void deck(const std::string &deck_type);
  void action(Command &command);
  CompassDirection parseCompassDirection(const std::string &string);
  bool validateActionCommand(const Command &command);
};

#endif