//----------------------------------------------------------------------------------------------------------------------
/// This program simulates the strategy game Teeko
/// Author: 12347025
//----------------------------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "framework.h"

#define SIZE 5
#define AMOUNT_OF_REMAINING_PIECES 4
#define MAX_SIZE_OF_ARRAY 10
#define SECOND_ELEMENT 2
#define INPUT_RETRY 2
#define INPUT_OK 1
#define INPUT_EXIT 0
#define INPUT_INVALID -1
#define ACTION_SUCCESS 5
#define NO_WINNER 0
#define PLAYER_1_WINS 1
#define PLAYER_2_WINS 2

typedef struct _Players_
{
  int moves_;
  int id_;
  int placed_pieces_;
} Players;

typedef struct _Coordinates 
{
  int column_;
  int row_;
} Coordinates;

typedef struct _Board_
{
  int (*board_)[SIZE];
  bool color_status_;
  Players *player_1_;
  Players *player_2_;
} Board;

typedef enum
{
  PLACEMENT,
  SHIFTING
} GamePhase;

typedef struct _MoveInput_
{
  char *input_;
  Coordinates *coordinate_from_;
  Coordinates *coordinate_to_;
  GamePhase phase_;
} MoveInput; 

void printWelcomeMessage(void);
int chooseColor(int argc, char *argv[], Board *board_game);
void clearTheBuffer(void);
void printGameState(Players *current_player);
void printRemainingPieces(Players player_1, Players player_2, GamePhase phase);
int messageForWinner(Players *current_player, Board board_game);
int getTheLine(char *input, Players *current_player);
bool isValidCoordinate(char *input);
bool checkDiapason(char *input);
int checkMistakesForOneCoordinate(char *input);
int checkMistakesForTwoCoordinates(char *input, Coordinates *coordinate_from, Coordinates *coordinate_to);
bool checkIfFieldIsOccupied(char *input, Board *board_game);
bool checkMistakesforShifting(Coordinates coordinate_from, Coordinates coordinate_to, 
                              Board *board_game, Players *current_player);
int checkMove(MoveInput *move_input, Board *board_game, Players *current_player);
int makeMovePlacement(Coordinates *coordinates, Players **current_player, Board *board_game, MoveInput *move_input);
int makeMoveShifting(Players **current_player, Board *board_game, MoveInput *move_input);
void checkCurrentPlayer(Players *player_1, Players *player_2, Players **current_player);
bool checkVerticalWin(int id, Board *board_game);
bool checkHorizontalWin(int id, Board *board_game);
bool checkDiagonalRightWin(int id, Board *board_game);
bool checkDiagonalLeftWin(int id, Board *board_game);
bool checkSquareWin(int id, Board *board_game);
int checkTheWinner(Players *current_player, Board *board_game);


//----------------------------------------------------------------------------------------------------------------------
/// @brief Main function of the program. Initializes the game, handles settings, runs the game loop until the game ends 
/// @param argc number of command-line arguments
/// @param argv array of argument strings
/// @return 0 if program terminates succesfully 

int main(int argc, char *argv[])
{
  int board[SIZE][SIZE] = {0};
  Players player_1 = {1, 1, 0};
  Players player_2 = {1, 2, 0};
  Players *current_player = &player_1;
  Board board_game = {board, false, &player_1, &player_2};

  Coordinates coordinates;
  Coordinates coordinate_from;
  Coordinates coordinate_to;
  char input[MAX_SIZE_OF_ARRAY];
  MoveInput move_input = {input, &coordinate_from, &coordinate_to, PLACEMENT};

  int color = chooseColor(argc, argv, &board_game);
  if (color == INPUT_INVALID)
  {
    return INPUT_INVALID;
  }

  printWelcomeMessage();

  do
  {
    GamePhase phase;
    printGameState(current_player);
    printBoard(SIZE, board, board_game.color_status_, player_1.id_, player_2.id_);
    phase = (player_1.placed_pieces_ >= AMOUNT_OF_REMAINING_PIECES 
             && player_2.placed_pieces_ >= AMOUNT_OF_REMAINING_PIECES) ? SHIFTING : PLACEMENT;
    move_input.phase_ = phase;
    printRemainingPieces(player_1, player_2, phase);

    int result;

    switch (phase)
    {
      case PLACEMENT:
        result = makeMovePlacement(&coordinates, &current_player, &board_game, &move_input);
        break;

      case SHIFTING:
        result = makeMoveShifting(&current_player, &board_game, &move_input);
        break;

      default:
        result = 0;
        break;
    }
    
    if (result == INPUT_EXIT) 
      return INPUT_EXIT;

    if (result == PLAYER_1_WINS  || result == PLAYER_2_WINS) 
      return result;
      
  } while (1);
  
  return 0;
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief This function prints welcome message
void printWelcomeMessage(void)
{
  printf("===| Welcome to       |===\n"
         "===| [I]ntricate      |===\n"
         "===| [S]quare         |===\n"
         "===| [P]lacement      |===\n"
         "\n");
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief This function sets color mode based on command-line arguments 
/// @param argc number of arguments
/// @param argv array of argument strings 
/// @param board_game pointer to a Board struct where the color settings are stored 
/// @return INPUT_OK is the settings were apllied, INPUT_INVALID for invalid arguments
int chooseColor(int argc, char *argv[], Board *board_game)
{
  if (argc == 1)
  {
    board_game->color_status_ = false;
    return INPUT_OK;
  }
  else if (argc == 2)
  {
    char *arg = argv[1];
    int i = 0;

    while (arg[i])
    {
      arg[i] = (char)tolower(arg[i]);
      i++;
    }

    if (strcmp(arg, "enabled") == 0)
    {
      board_game->color_status_ = true;
      return INPUT_OK;
    }
    else if (strcmp(arg, "disabled") == 0)
    {
      board_game->color_status_ = false;
      return INPUT_OK;
    }
    else
    {
      printf("\n[ERROR] (Optional) color output argument can either be \"disabled\" or \"enabled\".\n\n");
      board_game->color_status_ = false;
      return  INPUT_INVALID;
    }
  }
  else
  {
    printf("\n[ERROR] Usage: ./a2 [optional: color_enabled]\n\n");
    board_game->color_status_ = false;
    return  INPUT_INVALID;
  }
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief This function clears the input buffer until a newline or EOF is reached 
void clearTheBuffer(void)
{
  int character;
  while ((character = getchar()) != '\n' && character != EOF){}
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief This function prints the current move number and the active player
/// @param current_player the pointer to the player who is currently making a move
void printGameState(Players *current_player) 
{
  printf("\n===| Move %d: Player %d |===\n\n", current_player->moves_, current_player->id_);
}


//---------------------------------------------------------------------------------------------------------------------
/// @brief This function prints how many pieces each player has left (during placement phase)
/// @param player_1 pointer to the 1st player's struct
/// @param player_2 pointer to the 2nd player's struct
/// @param phase current game phase (prints only during placement)
void printRemainingPieces(Players player_1, Players player_2, GamePhase phase)
{
  if (phase == PLACEMENT)
  {
    printf("---| Remaining Pieces |---\n\nPlayer 1: %d\nPlayer 2: %d\n\n",
           AMOUNT_OF_REMAINING_PIECES - player_1.placed_pieces_, AMOUNT_OF_REMAINING_PIECES - player_2.placed_pieces_);
  }
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief This function prints the message if we have a winner
/// @param current_player pointer to the player who won the game
/// @param board_game pointer to a Board struct from which the final layout is printed
/// @return the id of the winning player
int messageForWinner(Players *current_player, Board board_game)
{
  printf("\n===| Final Game State |===\n\n");
  printBoard(SIZE, board_game.board_, board_game.color_status_, board_game.player_1_->id_, board_game.player_2_->id_);
  printf("===| Winner: Player %d |===\n", current_player->id_);
  return current_player->id_;
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief This function reads the input from the user, processes and normalizes it (spaces + uppercase)
/// @param input pointer to a char array where the processed input will be stored
/// @param current_player pointer to a current player, used for printing player's ID in the promt 
/// @return INPUT_OK if input was read, INPUT_RETRY if the input was too long and must be re-entered
///         INPUT_EXIT if the user want to leave the game
int getTheLine(char *input, Players *current_player)
{
  printf("P%d > ", current_player->id_);

  int current_character;
  int length = 0;
  int first_char_seen = 0;

  while ((current_character = getchar()) != EOF && current_character != '\n')
  {
    if (!first_char_seen)
    {
      if (isspace(current_character))
      {
        continue;
      }
      first_char_seen  = 1;
    }

    if (length >=  MAX_SIZE_OF_ARRAY - 1)
    {
      clearTheBuffer();
      input[0] = '\0';
      return INPUT_RETRY;
    }

    if (isspace(current_character))
    {
      if (length > 0 && input[length - 1] != ' ')
      {
        input[length++] = ' ';
      }
    }
    else
    {
      input[length++] = toupper(current_character);
    }
  }

  if (current_character == EOF && length == 0)
  {
    input[0] = '\0';
    return 0;
  }

  if (length > 0 && input[length - 1] == ' ')
  {
    length--;
  }

  input[length] = '\0';

  if (strcmp(input, "QUIT") == 0)
  {
    return INPUT_EXIT;
  }

  return INPUT_OK;
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief This function validates the coodinates given by user, 
///        checking if the first is a letter and second is a number
/// @param input an array to check
/// @return true if the coordinates were correct and false otherwise 
bool isValidCoordinate(char *input)
{
  if (!isalpha(input[0]) || !isdigit(input[1]))
  {
    printf("\n[ERROR] Unknown command.\n\n");
    return false;
  }

  return true;
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief This function validates the coodinates given by user, checking if coordinate is within the board
/// @param input an array to check
/// @return true if the coordinate is within the board and false otherwise
bool checkDiapason(char *input)
{
  char column = input[0];
  char row = input[1];

  if (column < 'A' || column > 'E') 
  {
    printf("\n[ERROR] Coordinates not within board.\n\n");
    return false;
  }
  else if (row < '1' || row > '5') 
  {
    printf("\n[ERROR] Coordinates not within board.\n\n");
    return false;
  }

  return true;
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief This function checks is a single coordinate is valid 
/// @param input an array to check
/// @return INPUT_OK if the coordinate is valid, INPUT_RETRY if it is invalid 
int checkMistakesForOneCoordinate(char *input)
{
  if (!isValidCoordinate(input))
  {
    return INPUT_RETRY;
  }

  else if (!checkDiapason(input))
  {
    return INPUT_RETRY;
  }

  return INPUT_OK;
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief This function checks is the user's input contain two valid coordinates 
/// @param input an array to check
/// @param coordinate_from parsed starting coordinates for the move
/// @param coordinate_to parsed target coordinates for the move
/// @return INPUT_OK if both coornates are valid and move can be made, INPUT_RETRY if input is incorrect 
int checkMistakesForTwoCoordinates(char *input, Coordinates *coordinate_from, Coordinates *coordinate_to)
{
  if (!isspace(input[SECOND_ELEMENT])) 
  {
    printf("\n[ERROR] Unknown command.\n\n");
    return INPUT_RETRY;
  }

  char *first_input = strtok(input, " "); 
  char *second_input = strtok(NULL, " "); 

  if (!isValidCoordinate(first_input) || !isValidCoordinate(second_input)) 
  {
    return INPUT_RETRY;
  }

  else if ((!checkDiapason(first_input)) || !checkDiapason(second_input)) 
  { 
    return INPUT_RETRY;
  }

  else
  {
    coordinate_from->row_ = first_input[1] - '1';
    coordinate_from->column_ = first_input[0] - 'A';

    coordinate_to->row_ = second_input[1] - '1';
    coordinate_to->column_ = second_input[0] - 'A';
    return INPUT_OK;
  }
}


///---------------------------------------------------------------------------------------------------------------------
/// @brief This function checks if the field specified by input is already occupied 
/// @param input user's input buffer (coordinate)
/// @param board_game the game board used to check whether the field is free
/// @return true if the field is free, false if it is occupied 
bool checkIfFieldIsOccupied(char *input, Board *board_game)
{
  if (board_game->board_[(int)input[1]][(int)input[0]] != 0)
  {
    printf("\n[ERROR] Field at coordinates occupied.\n\n");
    return false;
  }

  return true;
}


///---------------------------------------------------------------------------------------------------------------------
/// @brief This function validates a shifting move by checking piece ownership, target availability, and adjacency
/// @param coordinate_from starting coordinate of the move
/// @param coordinate_to target coordinate of the move
/// @param board_game pointer to a Board struct from which current piece positions are read 
/// @param current_player pointer to the player making the move
/// @return true if shift is valid, false otherwise
bool checkMistakesforShifting(Coordinates coordinate_from, Coordinates coordinate_to, 
    Board *board_game, Players *current_player)
{
  int from_row = coordinate_from.row_;
  int from_col = coordinate_from.column_;
  int to_row = coordinate_to.row_;
  int to_col = coordinate_to.column_;

  if (board_game->board_[from_row][from_col] != current_player->id_)
  {
    printf("\n[ERROR] No shiftable piece at coordinates.\n\n");
    return false;
  }
  if (board_game->board_[to_row][to_col] != 0)
  {
    printf("\n[ERROR] Field at coordinates occupied.\n\n");
    return false;
  }
  int row_difference = from_row - to_row;
  int column_difference = from_col - to_col;
  if (abs(row_difference) > 1 || abs(column_difference) > 1 || (row_difference == 0 && column_difference == 0))
  {
    printf("\n[ERROR] Field at coordinates not adjacent to chosen piece.\n\n");
    return false;
  }

  return true;
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief This function checks is the user's move input is valid for the current game phase
/// @param move_input pointer to MoveInput struct containing input, coordinates, and phase
/// @param board_game the game board (to validate moves and check occupancy)
/// @param current_player the player who is making the move 
/// @return INPUT_OK if move can be made, INPUT_RETRY if move cannot be made 
///         INPUT_EXIT if the user wants to leave the game
int checkMove(MoveInput *move_input, Board *board_game, Players *current_player)
{
  int result = getTheLine(move_input->input_, current_player);
  if (result == 0)
  {
    return INPUT_EXIT;
  }

  if (strlen(move_input->input_) <= 1)
  {
    printf("\n[ERROR] Unknown command.\n\n");
    return INPUT_RETRY;
  }

  if (strlen(move_input->input_) == 2)
  {
    int result = checkMistakesForOneCoordinate(move_input->input_);
    if (move_input->phase_ == PLACEMENT && result == 1)
    {
      move_input->input_[0] -= 'A';
      move_input->input_[1] -= '1';

      if (!checkIfFieldIsOccupied(move_input->input_, board_game))
      {
        return INPUT_RETRY;
      }
      return INPUT_OK;
    }

    else if (move_input->phase_ == SHIFTING && result == INPUT_OK)
    {
      printf("\n[ERROR] You must specify a piece to shift and a target field.\n\n");
      return INPUT_RETRY;
    }
  }

  else if (strlen(move_input->input_) > 2)
  {
    int result = checkMistakesForTwoCoordinates(move_input->input_, move_input->coordinate_from_, 
                                                move_input->coordinate_to_);

    if (move_input->phase_ == PLACEMENT && result == INPUT_OK)
    {
      printf("\n[ERROR] You must first place all your remaining pieces.\n\n");
      return INPUT_RETRY;
    }

    else if ((move_input->phase_ == SHIFTING) && result == INPUT_OK)
    {
      if (!checkMistakesforShifting(*move_input->coordinate_from_, *move_input->coordinate_to_,
                                    board_game, current_player))
      {
        return INPUT_RETRY;
      }
      else
      {
        return INPUT_OK;
      }
    }
  }

  return INPUT_RETRY;
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief This function performs a shifting move: reads input, validates input and moves a piece 
/// @param current_player the player who is making the move 
/// @param board_game the game board (to validate the shift and update pieces positions)
/// @param move_input pointer to MoveInput struct containing input, coordinates, and phase
/// @return ACTION_SUCCESS if move is done, INPUT_EXIT to quit the game
int makeMoveShifting(Players **current_player, Board *board_game, MoveInput *move_input)
{
  int valid;

  do
  {
    valid = checkMove(move_input, board_game, *current_player); 
    if (valid == INPUT_OK)
    {
      Coordinates *from = move_input->coordinate_from_;
      Coordinates *to = move_input->coordinate_to_;
      board_game->board_[to->row_][to->column_] = board_game->board_[from->row_][from->column_];
      board_game->board_[from->row_][from->column_] = 0;
    }

  } while (valid == INPUT_RETRY);

  if (valid == INPUT_EXIT)
  {
    return INPUT_EXIT;
  }
  int winner = checkTheWinner(*current_player, board_game);
  if (winner ==  PLAYER_1_WINS || winner ==  PLAYER_2_WINS)
  {
    return winner;
  }
  (*current_player)->moves_++;
  checkCurrentPlayer(board_game->player_1_, board_game->player_2_, current_player);

  return ACTION_SUCCESS;
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief This function performs a placement move: reads input, validates input and places a piece 
/// @param coordinates coordinate where the piece will be placed
/// @param current_player the player who is making the move 
/// @param board_game the game board (to check if the target field is free to place the piece)
/// @param move_input pointer to MoveInput struct containing input, coordinates, and phase
/// @return ACTION_SUCCESS if placement was successful, INPUT_EXIT to quit the game
int makeMovePlacement(Coordinates *coordinates, Players **current_player, Board *board_game, MoveInput *move_input)
{
  int valid;
  do
  {
    valid = checkMove(move_input, board_game, *current_player); 
    if (valid == INPUT_OK)
    {
      coordinates->column_ = move_input->input_[0];
      coordinates->row_ = move_input->input_[1];
    }

  } while (valid == INPUT_RETRY);

  if (valid == INPUT_EXIT)
  {
    return INPUT_EXIT;
  }

  board_game->board_[coordinates->row_][coordinates->column_] = (*current_player)->id_;
  (*current_player)->moves_++;
  (*current_player)->placed_pieces_++;

  int winner = checkTheWinner(*current_player, board_game);

  if (winner == PLAYER_1_WINS || winner == PLAYER_2_WINS)
  {
    return winner; 
  }
  checkCurrentPlayer(board_game->player_1_, board_game->player_2_, current_player);

  return ACTION_SUCCESS;
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief This function switches the current player 
/// @param player_1 pointer to a Players struct representing the first player
/// @param player_2 pointer to a Players struct representing the second player
/// @param current_player pointer to the pointer to the active player (will be updated)
void checkCurrentPlayer(Players *player_1, Players *player_2, Players **current_player)
{
  *current_player = (*current_player == player_1) ? player_2 : player_1;
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief This function checks for a vertical win condition (4 in a column)
/// @param id the player id to check for
/// @param board_game pointer to a Board struct from which piece positions are read
/// @return true if a vertical win is found, false otherwise
bool checkVerticalWin(int id, Board *board_game)
{
  for (int column = 0; column < SIZE; column++)
  {
    for (int row = 0; row <= SIZE - 4; row++)
    {
      if (board_game->board_[row][column] == id &&
          board_game->board_[row + 1][column] == id &&
          board_game->board_[row + 2][column] == id &&
          board_game->board_[row + 3][column] == id)
      {
        return true;
      }
    }
  }
  return false;
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief This function checks for a horizontal win condition (4 in a row)
/// @param id the player id to check for
/// @param board_game pointer to a Board struct from which piece positions are read
/// @return true if a horizontal win is found, false otherwise
bool checkHorizontalWin(int id, Board *board_game)
{
  for (int row = 0; row < SIZE; row++)
  {
    for (int column = 0; column <= SIZE - 4; column++)
    {
      if (board_game->board_[row][column] == id &&
          board_game->board_[row][column + 1] == id &&
          board_game->board_[row][column + 2] == id &&
          board_game->board_[row][column + 3] == id)
      {
        return true;
      }
    }
  }
  return false;
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief This function checks for a diagonal win condition (top-left to bottom-right)
/// @param id the player id to check for
/// @param board_game pointer to a Board struct from which piece positions are read
/// @return true if a diagonal right win is found, false otherwise
bool checkDiagonalRightWin(int id, Board *board_game)
{
  for (int row = 0; row <= SIZE - 4; row++)
  {
    for (int column = 0; column <= SIZE - 4; column++)
    {
      if (board_game->board_[row][column] == id &&
          board_game->board_[row + 1][column + 1] == id &&
          board_game->board_[row + 2][column + 2] == id &&
          board_game->board_[row + 3][column + 3] == id)
      {
        return true;
      }
    }
  }
  return false;
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief This function checks for a diagonal win condition (top-right to bottom-left)
/// @param id the player id to check for
/// @param board_game pointer to a Board struct from which piece positions are read
/// @return true if a diagonal left win is found, false otherwise
bool checkDiagonalLeftWin(int id, Board *board_game)
{
  for (int row = 0; row <= SIZE - 4; row++)
  {
    for (int column = 3; column < SIZE; column++)
    {
      if (board_game->board_[row][column] == id &&
          board_game->board_[row + 1][column - 1] == id &&
          board_game->board_[row + 2][column - 2] == id &&
          board_game->board_[row + 3][column - 3] == id)
      {
        return true;
      }
    }
  }
  return false;
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief This function checks for a square win condition (2x2 square)
/// @param id the player id to check for
/// @param board_game pointer to a Board struct from which piece positions are read
/// @return true if a square win is found, false otherwise

bool checkSquareWin(int id, Board *board_game)
{
  for (int row = 0; row < SIZE - 1; row++)
  {
    for (int column = 0; column < SIZE - 1; column++)
    {
      if (board_game->board_[row][column] == id &&
          board_game->board_[row][column + 1] == id &&
          board_game->board_[row + 1][column] == id &&
          board_game->board_[row + 1][column + 1] == id)
      {
        return true;
      }
    }
  }
  return false;
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief This function checks if the current player has won the game
/// @param current_player pointer to the player whose victory is being checked
/// @param board_game pointer to a Board struct from which piece positions are read 
/// @return the winner's id if the winner is found, otherwise NO_WINNER

int checkTheWinner(Players *current_player, Board *board_game)
{
  int id = current_player->id_;

  if (checkVerticalWin(id, board_game) ||
      checkHorizontalWin(id, board_game) ||
      checkDiagonalRightWin(id, board_game) ||
      checkDiagonalLeftWin(id, board_game) ||
      checkSquareWin(id, board_game))
  {
    return messageForWinner(current_player, *board_game);
  }

  return NO_WINNER;
}
