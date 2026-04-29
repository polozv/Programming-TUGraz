#include "Game.hpp"
#include <iostream>

Game::Game(std::size_t player_count) : 
  board_(player_count), current_player_(nullptr), last_finished_player_(-1),
  shark_(new Shark()), is_running_(true) 
  {
    shark_->setTerritoryWidth(player_count);
    for (std::size_t id = 1; id <= player_count; id++) 
    {
      players_.push_back(new Player(id));
    }
}


Game::~Game() 
{
  for (auto player: players_) 
  {
    delete player;
  }
  players_.clear();
  delete shark_;
  for (auto a_card : action_deck_)
  {
    delete a_card;
  }
  for (auto o_card : ocean_deck_) 
  {
    delete o_card;
  }
}


CompassDirection Game::parseCompassDirection(const std::string &string) 
{
  if (string == "NORTH")
    return CompassDirection::NORTH;
  if (string == "EAST")
    return CompassDirection::EAST;
  if (string == "SOUTH")
    return CompassDirection::SOUTH;
  if (string == "WEST")
    return CompassDirection::WEST;
  else
    return CompassDirection::NORTH; 
}


bool Game::checkMagicNumber(std::string &config_file_path) 
{
  std::ifstream file(config_file_path);
  if (!file.is_open()) {
    return false;
  }

  std::string line;
  std::getline(file, line);
  if (!file || line != "OOP") {
    return false;
  }
  return true;
}


void Game::loadConfigFile(std::string &config_file_path) 
{
  std::ifstream file(config_file_path);
  if (!file.is_open()) {
    return;
  }

  std::string line;
  std::getline(file, line);
  while (std::getline(file, line)) 
  {
    std::vector<std::string> tokens;
    Utils::tokenize(line, tokens, ';');

    if (tokens[0] == "O") {
      std::string id = tokens[1];
      bool shark_icon = (tokens[2] == "TRUE");
      ocean_deck_.push_back(new OceanCard(id, shark_icon));
    }

    if (tokens[0] == "A") {
      std::string id = tokens[1];
      CompassDirection shark_direction = parseCompassDirection(tokens[2]);

      if (id == "MSSGE" || id == "RWAVE")
        action_deck_.push_back(new PlayerActionCard(id, shark_direction));
      else if (id == "PIRAT" || id == "LOSTS")
        action_deck_.push_back(new HandActionCard(id, shark_direction));
    }
  }

  for (std::size_t index = 0; index < players_.size(); index++) 
  {
    for (std::size_t count = 0; count < 3; count++) 
    {
      ActionCard *card = action_deck_.back();
      action_deck_.pop_back();
      players_[index]->getHandCards().push_back(card);
    }
  }
}

Player *Game::getNextPlayer() 
{
  std::size_t amount_of_players = players_.size();
  if (amount_of_players == 0) {
    return nullptr;
  }
  std::size_t start_index = (last_finished_player_ + 1) % amount_of_players;
  std::size_t player_index;

  for (std::size_t count = 0; count < amount_of_players; count++) 
  {
    player_index = (start_index + count) % amount_of_players;
    if (players_[player_index]->hasStarved() == true) 
    {
      continue;
    } 
    else 
    {
      return players_[player_index];
    }
  }
  return nullptr;
}


void Game::printOceanDrew(const OceanCard *card) 
{ 
  card->printPlayMessage(); 
}


bool Game::float_command() 
{
  std::cout << "Player " << current_player_->getId() << " floats in place.\n";
  current_player_->deductRations(1);
  if (board_.isPrintActive()) 
  {
    board_.print(players_, shark_);
  }
  return true;
}


void Game::gatherWay(std::vector<Player *> &players,
                     std::vector<CompassDirection> &shark_path) 
{
  for (auto player : players)
  {
    if (player->hasStarved() || player->getHandCards().empty()) 
    {
      continue;
    } 
    else 
    {
      std::size_t index =
          command_line_.getTargetHandCardIndex(*player, *player);
      ActionCard *card = player->getHandCards()[index];
      shark_path.push_back(card->getSharkDirection());
      delete card;
      player->getHandCards().erase(player->getHandCards().begin() + index); 
    }
  }
}


void Game::endWithDraw(bool print_board) 
{
  is_running_ = false;
  if (print_board && board_.isPrintActive()) 
  {
    board_.print(players_, shark_);
  }
  std::cout << "\nBeaten by the whims of the sea, the game ends in a draw... "
               "Better luck next time.\n";
}


bool Game::swim() 
{
  std::cout << "Player " << current_player_->getId()
            << " swims closer to safety.\n";

  auto position = current_player_->getCoordinates();
  if (!position.has_value()) {
    return false;
  }

  const std::size_t old_x = position->getX();
  const std::size_t old_y = position->getY();

  if (!current_player_->move(CompassDirection::NORTH)) 
  {
    return false;
  }

  
  if (old_y == 4) 
  {
    if (board_.isPrintActive()) 
    {
      board_.print(players_, shark_);
    }
    std::cout << "\nCongratulations player " << current_player_->getId()
              << ", you are the Only One Prevailing!\n";
    is_running_ = false;
    return true;
  }

  if (ocean_deck_.empty()) {
    endWithDraw(true);
    return false;
  }

  OceanCard *card = ocean_deck_.back();
  ocean_deck_.pop_back();

  if (ocean_deck_.empty()) {
    delete card; 
    endWithDraw(true);
    return false;
  }

  card->setOwner(current_player_);
  card->play();
  printOceanDrew(card);

  if (card->getId() == "ISLND") 
  {
    board_.placeOceanCard(card, *current_player_->getCoordinates());
  } 
  else 
  {
    board_.placeOceanCard(card, Coordinates(old_x, old_y));
  }

  current_player_->deductRations(2);

  if (card->hasSharkIcon()) 
  {
    std::vector<CompassDirection> shark_path;
    if (shark_->isActive()) 
    {
      std::cout << "[" << UNICODE_SHARK
                << "] Oh no, the shark is looking for food!\n";
      gatherWay(players_, shark_path);
    }
    shark_->play(*current_player_, players_, shark_path);
  }

  if (board_.isPrintActive()) 
  {
    board_.print(players_, shark_);
  }
  return true;
}


std::string Game::compassToString(CompassDirection direction) 
{
  return ActionCard::compassDirectionToString(direction);
}


void Game::hand() 
{
  if (current_player_->getHandCards().empty()) 
  {
    std::cout << "No hand cards to display.\n";

  } 
  else 
  {
    std::cout << "Hand cards:\n";
    for (auto card : current_player_->getHandCards()) {
      std::cout << "- " << card->getId() << " - " << card->getName() << " - "
                << compassToString(card->getSharkDirection()) << std::endl;
    }
  }
}


void Game::deck(const std::string &deck_type) 
{
  if (deck_type == "action") 
  {
    std::cout << "Cards of the action deck:\n";
    for (auto card : action_deck_) 
    {
      card->printInformationString();
    }
  } 
  else 
  {
    std::cout << "Cards of the ocean deck:\n";
    for (auto card: ocean_deck_) 
    {
      card->printInformationString();
    }
  }
}

void Game::action(Command &command) 
{
  std::string action_id = command.getParameters().at(0);
  Utils::toUpperCase(action_id);

  std::size_t target_player_id = 0;
  Utils::stringToSizeT(command.getParameters().at(1), target_player_id);
  Player *target_player = players_[target_player_id - 1];

  ActionCard *played = nullptr;
  for (auto card : current_player_->getHandCards()) 
  {
    if (card->getId() == action_id) 
    {
      played = card;
      break;
    }
  }

  played->setOwner(current_player_);
  played->setTargetPlayer(target_player);

  if (action_id == "PIRAT" || action_id == "LOSTS") 
  {
    std::size_t index =
        command_line_.getTargetHandCardIndex(*current_player_, *target_player);
    auto *hand_card = dynamic_cast<HandActionCard *>(played);
    if (hand_card != nullptr) 
    {
      hand_card->setTargetHandIndex(index);
    }
  }

  played->play();
  played->printPlayMessage();

  auto &hand = current_player_->getHandCards();
  std::erase(hand, played);
  delete played;
}


bool Game::validateActionCommand(const Command &command) 
{
  std::string action_id = command.getParameters().at(0);
  Utils::toUpperCase(action_id);

  if (action_id != "MSSGE" && action_id != "PIRAT" && action_id != "RWAVE" &&
      action_id != "LOSTS") 
  {
    command_line_.printErrorMessage(ErrorType::INVALID_ACTION_CARD);
    return false;
  }

  bool found_in_hand = false;
  for (auto card : current_player_->getHandCards()) 
  {
    if (card->getId() == action_id) 
    {
      found_in_hand = true;
      break;
    }
  }
  if (!found_in_hand) 
  {
    command_line_.printErrorMessage(ErrorType::ACTION_CARD_NOT_IN_HAND);
    return false;
  }

  std::size_t target_player_id = 0;
  if (!Utils::stringToSizeT(command.getParameters().at(1), target_player_id)) 
  {
    command_line_.printErrorMessage(ErrorType::PLAYER_ID_OUT_OF_RANGE);
    return false;
  }
  if (target_player_id < 1 || target_player_id > players_.size()) 
  {
    command_line_.printErrorMessage(ErrorType::PLAYER_ID_OUT_OF_RANGE);
    return false;
  }

  Player *target_player = players_[target_player_id - 1];

  if (target_player_id == current_player_->getId()) 
  {
    command_line_.printErrorMessage(ErrorType::TARGET_CURRENT_PLAYER);
    return false;
  }

  if (target_player->hasStarved()) 
  {
    command_line_.printErrorMessage(ErrorType::TARGET_PLAYER_STARVED);
    return false;
  }

  if ((action_id == "PIRAT" || action_id == "LOSTS") &&
      target_player->getHandCards().empty())
  {
    command_line_.printErrorMessage(ErrorType::HAND_CARDS_EMPTY);
    return false;
  }

  return true;
}

bool Game::validateCommand(Command &command) 
{
  if (command.getType() == CommandType::UNKNOWN) 
  {
    command_line_.printErrorMessage(ErrorType::UNKNOWN_COMMAND);
    return false;
  }

  if (command.getType() == CommandType::WRONG_PARAMETER_COUNT) 
  {
    command_line_.printErrorMessage(ErrorType::PARAMETER_COUNT);
    return false;
  }

  switch (command.getType()) {
  case CommandType::QUIT:
    return true;
  case CommandType::BOARD:
    return true;
  case CommandType::HAND:
    return true;
  case CommandType::DECK:
    if (command.getParameters().at(0) == "action" ||
        command.getParameters().at(0) == "ocean") 
    {
      return true;
    } 
    else 
    {
      command_line_.printErrorMessage(ErrorType::INVALID_DECK);
      return false;
    }
  case CommandType::SWIM:
    if (current_player_->getRations() < 2) 
    {
      command_line_.printErrorMessage(ErrorType::INSUFFICIENT_RATIONS);
      return false;
    }
    return true;
  case CommandType::FLOAT:
    return true;
  case CommandType::ACTION: 
  {
    return validateActionCommand(command);
  }
  default:
    return false;
  }
}


bool Game::executeCommand(Command &command) 
{
  switch (command.getType()) {
  case CommandType::QUIT:
    is_running_ = false;
    return true;
  case CommandType::BOARD:
    board_.togglePrint();
    if (board_.isPrintActive()) 
    {
      board_.print(players_, shark_);
    }
    return true;
  case CommandType::FLOAT:
    return float_command();
  case CommandType::SWIM:
    return swim();
  case CommandType::HAND:
    hand();
    return true;
  case CommandType::DECK:
    deck(command.getParameters().at(0));
    return true;
  case CommandType::ACTION:
    action(command);
    return true;
  default:
    return false;
  }
}

void Game::start() 
{
  if (board_.isPrintActive()) 
  {
    board_.print(players_, shark_);
  }

  current_player_ = getNextPlayer();

  while (is_running_) 
  {
    if (current_player_ == nullptr)
     {
      endWithDraw(true);
      return;
    }

    if (action_deck_.empty()) {
      endWithDraw(true);
      return;
    }

    ActionCard *drawn = action_deck_.back();
    action_deck_.pop_back();
    current_player_->getHandCards().push_back(drawn);

    if (action_deck_.empty()) {
      endWithDraw(true);
      return;
    }

    while (is_running_) {
      command_line_.printPrompt(*current_player_);
      Command cmd = command_line_.getCommand();

      if (!validateCommand(cmd)) {
        continue;
      }

      if (!executeCommand(cmd)) {
        continue;
      }

      if ((cmd.getType() == CommandType::FLOAT ||
           cmd.getType() == CommandType::SWIM) && is_running_) 
      {
        last_finished_player_ = static_cast<int>(current_player_->getId() - 1);
        current_player_ = getNextPlayer();
        break;
      }
    }
  }
}
