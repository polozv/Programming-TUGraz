//----------------------------------------------------------------------------------------------------------------------
/// The CommandLine class contains functionality for converting input into a Command object as well as printing error
/// messages/prompts and a function for the additional target hand card index input loop.
///
/// Author(s): Tutors
//----------------------------------------------------------------------------------------------------------------------
#include "CommandLine.hpp"

const std::string CommandLine::INPUT_MESSAGE = "P";
const std::string CommandLine::INPUT_PROMPT = " > ";
const std::string CommandLine::TARGET_HAND_CARD_INPUT_MESSAGE = "Select target hand card:";

const std::map<CommandType, std::set<std::size_t>> CommandLine::PARAMETER_COUNTS{
  {CommandType::QUIT, {0}},
  {CommandType::BOARD, {0}},
  {CommandType::DECK, {1}},
  {CommandType::HAND, {0}},
  {CommandType::ACTION, {2}},
  {CommandType::SWIM, {0}},
  {CommandType::FLOAT, {0}}
};

const std::map<ErrorType, std::string> CommandLine::ERROR_MESSAGE_MAPPING
{
  {ErrorType::UNKNOWN_COMMAND,          " Entered command not found!\n"},
  {ErrorType::PARAMETER_COUNT,          " Wrong number of parameters for this command!\n"},
  {ErrorType::INVALID_DECK,             " Invalid card deck type!\n"},
  {ErrorType::INSUFFICIENT_RATIONS,     " Insufficient amount of rations!\n"},
  {ErrorType::INVALID_ACTION_CARD,      " Action card does not exist!\n"},
  {ErrorType::ACTION_CARD_NOT_IN_HAND,  " Action card not found in hand cards!\n"},
  {ErrorType::PLAYER_ID_OUT_OF_RANGE,   " Player ID is out of range!\n"},
  {ErrorType::TARGET_CURRENT_PLAYER,    " You cannot target yourself!\n"},
  {ErrorType::TARGET_PLAYER_STARVED,    " Targeted player has starved!\n"},
  {ErrorType::HAND_CARDS_EMPTY,         " Targeted player has no hand cards to target!\n"},
  {ErrorType::CARD_INDEX_OUT_OF_RANGE,  " Card index is out of range for the targeted player!\n"}
};

void CommandLine::printPrompt(const Player &player) const
{
  std::cout << INPUT_MESSAGE << player.getId() << INPUT_PROMPT;
  std::cout.flush();
}

void CommandLine::printErrorMessage(const ErrorType &error_type) const
{
  std::cout << std::format("[{}]", UNICODE_CROSS) << ERROR_MESSAGE_MAPPING.at(error_type);
  std::cout.flush();
}

Command CommandLine::getCommand() const
{
  std::string input;
  std::getline(std::cin, input);
  Utils::trim(input);
  if (input.empty())
  {
    return Command(CommandType::UNKNOWN);
  }
  Utils::toLowerCase(input);
  std::vector<std::string> input_tokens;
  Utils::tokenize(input, input_tokens, ' ');
  Command command(input_tokens);
  CommandType type = command.getType();
  std::size_t parameter_count = command.getParameters().size();
  if (PARAMETER_COUNTS.contains(type) && !PARAMETER_COUNTS.at(type).contains(parameter_count))
  {
    command.setType(CommandType::WRONG_PARAMETER_COUNT);
  }
  return command;
}

std::size_t CommandLine::getTargetHandCardIndex(Player &current_player, Player &target_player) const
{
  std::string input;
  std::size_t hand_card_number = 0;
  do
  {
    std::cout << TARGET_HAND_CARD_INPUT_MESSAGE << std::endl;
    printPrompt(current_player);

    std::getline(std::cin, input);

    if (
      !Utils::stringToSizeT(input, hand_card_number)
      || hand_card_number <= 0
      || hand_card_number >= target_player.getHandCards().size() + 1
    )
    {
      printErrorMessage(ErrorType::CARD_INDEX_OUT_OF_RANGE);
      continue;
    }

    break;
  } while (true);
  return hand_card_number - 1;
}
