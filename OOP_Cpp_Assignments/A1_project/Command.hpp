#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>
#include <vector>

enum class CommandType {
  QUIT,
  BOARD,
  DECK,
  HAND,
  ACTION,
  SWIM,
  FLOAT,
  WRONG_PARAMETER_COUNT,
  UNKNOWN
};

class Command {
private:
  CommandType type_ = CommandType::UNKNOWN;
  std::vector<std::string> parameters_;

  static void parseFromTokens(std::vector<std::string> &input_tokens,
                              CommandType &type,
                              std::vector<std::string> &parameters);

public:
  Command(std::vector<std::string> &input_tokens);
  Command(CommandType type);
  Command(const Command&) = default;
  ~Command() = default;

  CommandType getType() const { return type_; }
  const std::vector<std::string> &getParameters() const { return parameters_; }
  void setType(CommandType type) { type_ = type; }
};

#endif
