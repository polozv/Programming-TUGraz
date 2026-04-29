//----------------------------------------------------------------------------------------------------------------------
/// The CommandLine class contains functionality for converting input into a Command object as well as printing error
/// messages/prompts and a function for the additional target hand card index input loop.
///
/// Author(s): Tutors
//----------------------------------------------------------------------------------------------------------------------
#ifndef COMMANDLINE_HPP
#define COMMANDLINE_HPP

#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "Command.hpp"
#include "Utils.hpp"
#include "Player.hpp"

enum class ErrorType
{
  UNKNOWN_COMMAND,
  PARAMETER_COUNT,
  INVALID_DECK,
  INSUFFICIENT_RATIONS,
  INVALID_ACTION_CARD,
  ACTION_CARD_NOT_IN_HAND,
  PLAYER_ID_OUT_OF_RANGE,
  TARGET_CURRENT_PLAYER,
  TARGET_PLAYER_STARVED,
  HAND_CARDS_EMPTY,
  CARD_INDEX_OUT_OF_RANGE
};

class CommandLine
{
  private:
    static const std::string INPUT_MESSAGE;
    static const std::string INPUT_PROMPT;
    static const std::string TARGET_HAND_CARD_INPUT_MESSAGE;

    static const std::map<CommandType, std::set<std::size_t>> PARAMETER_COUNTS;
    static const std::map<ErrorType, std::string> ERROR_MESSAGE_MAPPING;

  public:
 	

    //------------------------------------------------------------------------------------------------------------------
    /// @brief Constructor is set to default.
    CommandLine() = default;

    //------------------------------------------------------------------------------------------------------------------
    /// @brief Copy constructor is deleted explicitly.
    CommandLine(const CommandLine &) = delete;

    //------------------------------------------------------------------------------------------------------------------
    /// @brief Destructor is set to default.
    virtual ~CommandLine() = default;

    //------------------------------------------------------------------------------------------------------------------
    /// @brief This function prints the command prompt for the current player.
    /// @param player the current player
    void printPrompt(const Player &player) const;

    //------------------------------------------------------------------------------------------------------------------
    /// @brief This function prints the error message for the given error type.
    /// @param error_type the type of error that occurred
    void printErrorMessage(const ErrorType &error_type) const;

    //------------------------------------------------------------------------------------------------------------------
    /// @brief This function reads an input from the current player and stores it in a Command object.
    /// @return the parsed Command object
    Command getCommand() const;

    //------------------------------------------------------------------------------------------------------------------
    /// @brief This function reads an input from the current player for a hand card index of another player and
    ///        checks for related errors.
    /// @param current_player the current player
    /// @param target_player the player whose hand cards are targeted
    /// @return a valid hand card index
    std::size_t getTargetHandCardIndex(Player &current_player, Player &target_player) const;
};

#endif // COMMANDLINE_HPP
