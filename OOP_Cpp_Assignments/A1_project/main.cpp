//----------------------------------------------------------------------------------------------------------------------
/// An implementation of "Only One Prevailing", which is a turn-based survival card game for 2-5 players.
/// Each player controls a castaway trying to survive and reach the top of a board while managing their rations.
/// This file handles some return value checks before starting the core game loop.
///
/// Author(s): Tutors
//----------------------------------------------------------------------------------------------------------------------
#include "Game.hpp"
#include "Utils.hpp"

const std::size_t VALID_PARAMETER_COUNT = 3;
const std::size_t MINIMUM_PLAYER_COUNT = 2;
const std::size_t MAXIMUM_PLAYER_COUNT = 5;

const std::string INVALID_ARGUMENT_COUNT_MESSAGE = " Wrong number of arguments!";
const std::string INVALID_PLAYER_COUNT_MESSAGE = " Invalid number of players!";
const std::string INVALID_FILE_MESSAGE = " Invalid file!";
const std::string WELCOME_MESSAGE = "Welcome to Only One Prevailing!";

enum _ReturnValue_
{
  SUCCESS,
  INVALID_ARGUMENT_COUNT,
  INVALID_PLAYER_COUNT,
  INVALID_FILE
};

//----------------------------------------------------------------------------------------------------------------------
/// @brief Main function of the program. Deals with return value checks and calls all necessary functions to correctly
///        initialize the game before starting its core game loop.
/// @param argc number of commandline arguments
/// @param argv array containing strings of commandline arguments
/// @return return value of the program, non-zero if an error occurred, otherwise 0
int main(int argc, char *argv[])
{
  if (argc != VALID_PARAMETER_COUNT)
  {
    std::cout << std::format("[{}]", UNICODE_CROSS) << INVALID_ARGUMENT_COUNT_MESSAGE << std::endl;
    return INVALID_ARGUMENT_COUNT;
  }

  std::size_t player_count = 0;
  Utils::stringToSizeT(argv[1], player_count);
  if (player_count < MINIMUM_PLAYER_COUNT || player_count > MAXIMUM_PLAYER_COUNT)
  {
    std::cout << std::format("[{}]", UNICODE_CROSS) << INVALID_PLAYER_COUNT_MESSAGE << std::endl;
    return INVALID_PLAYER_COUNT;
  }

  std::string config_file_path = argv[2];
  Game game(player_count);
  if (!game.checkMagicNumber(config_file_path))
  {
    std::cout << std::format("[{}]", UNICODE_CROSS) << INVALID_FILE_MESSAGE << std::endl;
    return INVALID_FILE;
  }

  std::cout << WELCOME_MESSAGE << std::endl;
  game.loadConfigFile(config_file_path);
  game.start();

  return SUCCESS;
}
