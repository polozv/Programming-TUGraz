#include "Command.hpp"
#include "Utils.hpp"

#include <array>
#include <string>
#include <utility>
#include <vector>


void Command::parseFromTokens(std::vector<std::string> &input_tokens,
                              CommandType &type,
                              std::vector<std::string> &parameters) 
{
  static const std::array<std::pair<const char *, CommandType>, 7>
      command_table
      {{
          {"quit", CommandType::QUIT},
          {"board", CommandType::BOARD},
          {"deck", CommandType::DECK},
          {"hand", CommandType::HAND},
          {"action", CommandType::ACTION},
          {"swim", CommandType::SWIM},
          {"float", CommandType::FLOAT},
      }};

  if (input_tokens.empty()) {
    parameters.clear();
    return;
  }

  std::string token = input_tokens.at(0);
  Utils::toLowerCase(token);

  for (const auto &cmd_pair  : command_table) 
  {
    if (token == cmd_pair.first) {
      type = cmd_pair.second;
      parameters.assign(input_tokens.begin() + 1, input_tokens.end());
      return;
    }
  }
}


Command::Command(std::vector<std::string> &input_tokens) 
{
  parseFromTokens(input_tokens, type_, parameters_);
}


Command::Command(CommandType type) : type_(type) {}



