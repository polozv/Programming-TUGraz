//----------------------------------------------------------------------------------------------------------------------
/// Stringtango Text Editor - text editor that allows users to manipulate text strings
/// using different commands
///
/// Author: 12347025
//----------------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_SIZE_STEP 5
#define NULL_TERMINATOR_SIZE 1

#define RETURN_SUCCESS 0
#define RETURN_ERROR 1

#define ERROR_MEMORY_ALLOCATION "[ERROR] Memory allocation failed!\n"
#define ERROR_STRING_EMPTY "[ERROR] String can't be empty!\n"
#define ERROR_SUBSTRING_NOT_FOUND "[ERROR] Substring not found!\n"
#define ERROR_NO_WORDS_TO_SORT "[ERROR] No words to sort!\n"
#define ERROR_ALL_WORDS_UNIQUE "[ERROR] All words are already unique\n"
#define ERROR_COMMAND_UNKNOWN "[ERROR] Command unknown!\n"

typedef enum _Command_
{
	COMMAND_APPEND = 'a',
	COMMAND_REPLACE = 'r',
	COMMAND_SORT = 's',
	COMMAND_UNIQUE = 'u'
} Command;

typedef enum _CommandResult_
{
	RESULT_OKAY,
	RESULT_QUIT,
	RESULT_QUIT_AND_PRINT
} CommandResult;

void printWelcomeMessage(void);
char *find(char *buffer, char c);
char *getString(CommandResult *command_result);
char *takeInput(CommandResult *command_result);
void *resizeString(void *str, size_t current_size, size_t new_size);
void printString(char *input);
char *stringReplacement(char *input, CommandResult *command_result);
char *processQuitCommand(char *input, CommandResult *command_result);
int countMatches(char *str, char *substr);
char *replaceMatches(char *input, char *search_string, char *replacement_string, int matches_count);
char *splitAndSort(char *input);
char **splitIntoWords(char *input, int *count, char **input_copy);
void bubbleSort(char **words, int words_number);
void rebuildString(char **words, char *input, int words_number);
char *takeCommand(CommandResult *command_result);
char *processCommand(char *command, char *input, CommandResult *command_result);
char *appendText(char *input, CommandResult *command_result);
int compareWords(char *word_a, char *word_b);
char *deleteDuplicates(char *input);
char *runCommandLoop(char *input, CommandResult *command_result);

//----------------------------------------------------------------------------------------------------------------------
/// @brief Main function that initializes the text editor and runs the command loop.
/// @return 0 if execution was successful, 1 if an error occurred
int main(void)
{
	printWelcomeMessage();
	CommandResult command_result = RESULT_OKAY;
	char *input = takeInput(&command_result);

	if (input == NULL && command_result == RESULT_OKAY)
	{
		return RETURN_ERROR;
	}

	if (command_result == RESULT_QUIT)
	{
		return RETURN_SUCCESS;
	}

	input = runCommandLoop(input, &command_result);

	if (input == NULL && command_result == RESULT_OKAY)
	{
		return RETURN_ERROR;
	}

	if (command_result == RESULT_QUIT_AND_PRINT && input != NULL)
	{
		printString(input);
	}
	free(input);
	return RETURN_SUCCESS;
}

//----------------------------------------------------------------------------------------------------------------------
/// @brief Prints the welcome message
void printWelcomeMessage(void)
{
	printf("\n"
				 "Welcome to the\n"
				 "Stringtango Text Editor!\n"
				 "\n");
}

//----------------------------------------------------------------------------------------------------------------------
/// @brief Runs the main command loop, processing user commands until user types quit
/// @param input The current text string
/// @param command_result Pointer to the command result (okay, quit, quit and print)
/// @return The modified input string, or NULL on error/quit
char *runCommandLoop(char *input, CommandResult *command_result)
{
	while (*command_result == RESULT_OKAY && input != NULL)
	{
		char *command = takeCommand(command_result);
		if (command == NULL || *command_result != RESULT_OKAY)
		{
			free(command);
			break;
		}

		char *new_input = processCommand(command, input, command_result);
		free(command);

		if (new_input == NULL)
		{
			free(input);
			return NULL;
		}

		if (*command_result != RESULT_OKAY)
		{
			input = new_input;
			break;
		}

		input = new_input;
	}
	return input;
}

//----------------------------------------------------------------------------------------------------------------------
/// @brief Prompts the user for initial text input
/// @param command_result Pointer to the command result status
/// @return The entered text string, or NULL on error/quit
char *takeInput(CommandResult *command_result)
{
	printf("Enter a text:\n"
				 " > ");
	char *input = getString(command_result);
	return input;
}

//----------------------------------------------------------------------------------------------------------------------
/// @brief Finds the first occurrence of a character in a buffer.
/// @param buffer The buffer to search in
/// @param c The character to find
/// @return Pointer to the first occurrence of the character, or NULL if not found
/// this function is taken from  the lecture (KU 08)
char *find(char *buffer, char c)
{
	for (char *iterator = buffer; *iterator != '\0'; ++iterator)
	{
		if (*iterator == c)
		{
			return iterator;
		}
	}
	return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
/// @brief Reads a string from stdin with dynamic buffer allocation until user types newline or quit command
/// @param command_result Pointer to the command result status
/// @return The read string, or NULL on error or quit command
/// part of code is taken from  the lecture (KU 08)
//start
char *getString(CommandResult *command_result)
{
	int buffer_size = BUFFER_SIZE_STEP;

	char *initial_buffer = resizeString(NULL, 0, buffer_size * sizeof(char));
	if (initial_buffer == NULL)
	{
		return NULL;
	}

	fgets(initial_buffer, buffer_size, stdin);

	while (find(initial_buffer, '\n') == NULL)
	{
		size_t current_length = strlen(initial_buffer);
		size_t old_size = buffer_size * sizeof(char);
		buffer_size += BUFFER_SIZE_STEP;
		size_t new_size = buffer_size * sizeof(char);

		char *new_buffer = resizeString(initial_buffer, old_size, new_size);

		if (new_buffer == NULL)
		{
			free(initial_buffer);
			return NULL;
		}
		initial_buffer = new_buffer;
		fgets(initial_buffer + current_length, BUFFER_SIZE_STEP + 1, stdin);
	}
	//end
	char *newLine = find(initial_buffer, '\n');
	if (newLine != NULL)
	{
		*newLine = '\0';
	}

	size_t final_length = strlen(initial_buffer);
	size_t final_size = final_length + NULL_TERMINATOR_SIZE;
	size_t current_buffer_size = buffer_size * sizeof(char);

	if (final_size < current_buffer_size)
	{
		char *resized_buffer = resizeString(initial_buffer, current_buffer_size, final_size);
		if (resized_buffer == NULL)
		{
			free(initial_buffer);
			return NULL;
		}
		initial_buffer = resized_buffer;
	}

	return processQuitCommand(initial_buffer, command_result);
}

//----------------------------------------------------------------------------------------------------------------------
/// @brief Resizes a memory block (malloc/realloc wrapper with error handling)
/// @param str Pointer to the existing memory block (NULL for malloc)
/// @param current_size Current size of the memory block
/// @param new_size New size of the memory block
/// @return Pointer to the resized memory block, or NULL on error
void *resizeString(void *str, size_t current_size, size_t new_size)
{
	if (str == NULL)
	{
		if (new_size == 0)
		{
			return NULL;
		}
		void *result = malloc(new_size);
		if (result == NULL)
		{
			printf(ERROR_MEMORY_ALLOCATION);
		}
		return result;
	}

	if (new_size == 0)
	{
		free(str);
		return NULL;
	}

	if (new_size == current_size)
	{
		return str;
	}

	void *new_memory = realloc(str, new_size);
	if (new_memory == NULL)
	{
		printf(ERROR_MEMORY_ALLOCATION);
		free(str);
		return NULL;
	}
	return new_memory;
}

//----------------------------------------------------------------------------------------------------------------------
/// @brief Prints the current text string
/// @param input The string to print
void printString(char *input)
{
	printf("\nCurrent text:\n%s\n", input);
}

//----------------------------------------------------------------------------------------------------------------------
/// @brief Appends text to the current input string
/// @param input The current input string
/// @param command_result Pointer to the command result (okay, quit, quit and print)
/// @return The modified input string with appended text, or original input on error/quit command
char *appendText(char *input, CommandResult *command_result)
{
	printf("\nPlease enter the string that should be appended:\n > ");
	char *append_string = getString(command_result);
	if (append_string == NULL)
	{
		return NULL;
	}

	if (strlen(append_string) == 0)
	{
		printf(ERROR_STRING_EMPTY);
		free(append_string);
		return input;
	}

	size_t old_size = strlen(input) + NULL_TERMINATOR_SIZE;
	size_t new_size = strlen(input) + strlen(append_string) + NULL_TERMINATOR_SIZE;

	char *new_input = resizeString(input, old_size, new_size);
	if (new_input == NULL)
	{
		free(append_string);
		return NULL;
	}

	strcat(new_input, append_string);
	free(append_string);
	printString(new_input);
	return new_input;
}

//----------------------------------------------------------------------------------------------------------------------
/// @brief Counts the number of occurrences of a substring in a string
/// @param input_original The string to search in
/// @param substring The substring to search for
/// @return The number of occurrences of the substring
int countMatches(char *input_original, char *substring)
{
	int count = 0;
	size_t length = strlen(substring);
	char *input_copy = input_original;

	while ((input_copy = strstr(input_copy, substring)) != NULL)
	{
		count++;
		input_copy += length;
	}
	return count;
}

//----------------------------------------------------------------------------------------------------------------------
/// @brief Replaces all occurrences of a search string with a replacement string
/// @param input The input string
/// @param search_string The string to search for
/// @param replacement_string The string to replace with
/// @param matches_count The number of matches (to calculate new size for memory allocation)
/// @return A new string with all replacements made, or NULL on error/quit command
char *replaceMatches(char *input, char *search_string, char *replacement_string, int matches_count)
{
	size_t search_length = strlen(search_string);
	if (search_length == 0)
	{
		return NULL;
	}
	size_t replacement_length = strlen(replacement_string);
	size_t original_length = strlen(input);
	size_t final_size = original_length + matches_count * (replacement_length - search_length) + NULL_TERMINATOR_SIZE;

	char *final_string = resizeString(NULL, 0, final_size);
	if (final_string == NULL)
	{
		return NULL;
	}

	char *where_to = final_string;
	char *from_where = input;

	for (char *found_where;
			 (found_where = strstr(from_where, search_string)) != NULL;
			 from_where = found_where + search_length)
	{
		size_t length = found_where - from_where;
		memcpy(where_to, from_where, length);
		where_to += length;
		memcpy(where_to, replacement_string, replacement_length);
		where_to += replacement_length;
	}
	strcpy(where_to, from_where);

	return final_string;
}

//----------------------------------------------------------------------------------------------------------------------
/// @brief Handles the search and replace command, prompting for search and replacement strings
/// @param input The current input string
/// @param command_result Pointer to the command result (okay, quit, quit and print)
/// @return The modified input string with replacements, or original input on error/quit command
char *stringReplacement(char *input, CommandResult *command_result)
{
	if (strlen(input) == 0)
	{
		printf(ERROR_STRING_EMPTY);
		return input;
	}

	printf("\nPlease enter the substring to search for:\n > ");
	char *search_string = getString(command_result);
	if (search_string == NULL)
	{
		return NULL;
	}

	if (strlen(search_string) == 0)
	{
		printf(ERROR_STRING_EMPTY);
		free(search_string);
		return input;
	}

	int matches_count = countMatches(input, search_string);
	if (matches_count == 0)
	{
		printf(ERROR_SUBSTRING_NOT_FOUND);
		free(search_string);
		return input;
	}

	printf("\nPlease enter the new substring:\n > ");
	char *replacement_string = getString(command_result);
	if (replacement_string == NULL)
	{
		free(search_string);
		return NULL;
	}

	char *result_text = replaceMatches(input, search_string, replacement_string, matches_count);
	if (result_text == NULL)
	{
		free(search_string);
		free(replacement_string);
		return NULL;
	}

	free(input);

	printString(result_text);
	free(search_string);
	free(replacement_string);

	return result_text;
}

//----------------------------------------------------------------------------------------------------------------------
/// @brief Splits the input string into words, sorts them alphabetically, and rebuilds the string
/// @param input The input string to process
/// @return The sorted string, or original input on error
char *splitAndSort(char *input)
{
	if (strlen(input) == 0)
	{
		printf(ERROR_NO_WORDS_TO_SORT);
		return input;
	}
	int count = 0;
	char *input_copy = NULL;
	char **words = splitIntoWords(input, &count, &input_copy);
	if (words == NULL)
	{
		if (count == 0)
		{
			printf(ERROR_NO_WORDS_TO_SORT);
			return input;
		}
		return NULL;
	}
	size_t original_size = strlen(input) + NULL_TERMINATOR_SIZE;

	bubbleSort(words, count);
	rebuildString(words, input, count);

	size_t new_length = strlen(input);
	size_t new_size = new_length + NULL_TERMINATOR_SIZE;

	if (new_size < original_size)
	{
		char *resized_input = resizeString(input, original_size, new_size);
		if (resized_input == NULL)
		{
			free(words);
			free(input_copy);
			return NULL;
		}
		input = resized_input;
	}

	free(words);
	free(input_copy);
	printString(input);
	return input;
}

//----------------------------------------------------------------------------------------------------------------------
/// @brief Splits an input string into an array of words using space as divider
/// @param input The input string to split
/// @param count Pointer to store the number of words found
/// @param input_copy Pointer to store the copy of input string (for strtok because it modifies the string)
/// @return Array of pointers to words, or NULL on error/quit command
char **splitIntoWords(char *input, int *count, char **input_copy)
{
	size_t input_length = strlen(input);
	char **words = resizeString(NULL, 0, input_length * sizeof(char *));
	if (words == NULL)
	{
		return NULL;
	}
	*input_copy = resizeString(NULL, 0, input_length + NULL_TERMINATOR_SIZE);
	if (*input_copy == NULL)
	{
		free(words);
		return NULL;
	}
	strcpy(*input_copy, input);
	char *word = strtok(*input_copy, " ");
	*count = 0;

	while (word != NULL)
	{
		words[*count] = word;
		(*count)++;
		word = strtok(NULL, " ");
	}
	if (*count == 0)
	{
		free(words);
		free(*input_copy);
		*input_copy = NULL;
		return NULL;
	}
	char **new_words_array = resizeString(words, input_length * sizeof(char *), (*count) * sizeof(char *));
	if (new_words_array == NULL)
	{
		free(*input_copy);
		free(words);
		return NULL;
	}
	words = new_words_array;
	return words;
}

//---------------------------------------------------------------------------------------------------------------------
/// @brief Sorts an array of words alphabetically using bubble sort algorithm
/// @param words Array of pointers to words (to sort)
/// @param words_number Number of words in the array (to iterate through)
void bubbleSort(char **words, int words_number)
{
	for (int count = 0; count < words_number; count++)
	{
		for (int index = 0; index < words_number - count - 1; index++)
		{
			if (strcmp(words[index], words[index + 1]) > 0)
			{
				char *temp_word = words[index];
				words[index] = words[index + 1];
				words[index + 1] = temp_word;
			}
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------
/// @brief Rebuilds a string from an array of words and separates them with spaces
/// @param words Array of pointers to words (to rebuild)
/// @param input Buffer to write the rebuilt string into (to write into)
/// @param words_number Number of words in the array (to iterate through)
void rebuildString(char **words, char *input, int words_number)
{
	for (int index = 0; index < words_number; index++)
	{
		strcpy(input, words[index]);
		input += strlen(words[index]);
		if (index < words_number - 1)
		{
			*input = ' ';
			input++;
		}
	}
	*input = '\0';
}

//----------------------------------------------------------------------------------------------------------------------
/// @brief Prompts the user for a command and reads it from stdin
/// @param command_result Pointer to the command result
/// @return The entered command string, or NULL on error/quit
char *takeCommand(CommandResult *command_result)
{
	printf("\n"
				 "Choose a command:\n"
				 " a: append text\n"
				 " r: search and replace\n"
				 " s: split and sort\n"
				 " u: unique\n"
				 " q: quit\n"
				 "\n"
				 " > ");
	return getString(command_result);
}

//----------------------------------------------------------------------------------------------------------------------
/// @brief Processes a command and executes operations chosen by user
/// @param command The command character to execute
/// @param input The current input string
/// @param command_result Pointer to the command result (okay, quit, quit and print)
/// @return The modified input string, or original input on error/unknown command
char *processCommand(char *command, char *input, CommandResult *command_result)
{
	if (strlen(command) != 1)
	{
		printf(ERROR_COMMAND_UNKNOWN);
		return input;
	}

	switch (command[0])
	{
	case COMMAND_APPEND:
		return appendText(input, command_result);
	case COMMAND_REPLACE:
		return stringReplacement(input, command_result);
	case COMMAND_SORT:
		return splitAndSort(input);
	case COMMAND_UNIQUE:
		return deleteDuplicates(input);
	default:
		printf(ERROR_COMMAND_UNKNOWN);
		return input;
	}
}

//----------------------------------------------------------------------------------------------------------------------
/// @brief Processes quit commands ("q" or "quit") and updates the command result
/// @param input The input string to check for quit commands
/// @param command_result Pointer to the command result
/// @return NULL if quit command detected, otherwise returns the input string
char *processQuitCommand(char *input, CommandResult *command_result)
{
	if (strcmp(input, "q") == 0)
	{
		*command_result = RESULT_QUIT_AND_PRINT;
		free(input);
		return NULL;
	}

	if (strcmp(input, "quit") == 0)
	{
		*command_result = RESULT_QUIT;
		free(input);
		return NULL;
	}

	*command_result = RESULT_OKAY;
	return input;
}

//----------------------------------------------------------------------------------------------------------------------
/// @brief Removes duplicate words from the input string (case-insensitive)
/// @param input The input string to process
/// @return The modified input string with duplicates removed, or original input on error
char *deleteDuplicates(char *input)
{
	if (strlen(input) == 0)
	{
		printf(ERROR_ALL_WORDS_UNIQUE);
		return input;
	}
	char **words;
	char *input_copy = NULL;
	int count = 0;

	words = splitIntoWords(input, &count, &input_copy);
	if (words == NULL)
	{
		if (count == 0)
		{
			printf(ERROR_ALL_WORDS_UNIQUE);
			return input;
		}
		return NULL;
	}

	size_t original_size = strlen(input) + NULL_TERMINATOR_SIZE;

	int duplicates_found = 0;
	for (int index = 0; index < count - 1; index++)
	{
		for (int next_index = index + 1; next_index < count; next_index++)
		{
			if (compareWords(words[index], words[next_index]) == 0)
			{
				duplicates_found = 1;
				memmove(&words[next_index], &words[next_index + 1], (count - next_index - 1) * sizeof(char *));
				count--;
				next_index--;
			}
		}
	}

	if (!duplicates_found)
	{
		free(words);
		free(input_copy);
		printf(ERROR_ALL_WORDS_UNIQUE);
		return input;
	}

	rebuildString(words, input, count);

	size_t new_length = strlen(input);
	char *resized_input = resizeString(input, original_size, new_length + NULL_TERMINATOR_SIZE);
	if (resized_input == NULL)
	{
		free(words);
		free(input_copy);
		return NULL;
	}
	input = resized_input;

	free(words);
	free(input_copy);
	printString(input);
	return input;
}

//----------------------------------------------------------------------------------------------------------------------
/// @brief Compares two words not considering the case
/// @param word_a First word to compare
/// @param word_b Second word to compare
/// @return Negative value if word_a < word_b, positive if word_a > word_b, 0 if equal due to return value of tolower
int compareWords(char *word_a, char *word_b)
{
	int index;
	for (index = 0; word_a[index] && word_b[index]; index++)
	{
		char character_a = tolower(word_a[index]);
		char character_b = tolower(word_b[index]);
		if (character_a != character_b)
			return character_a - character_b;
	}
	int result = tolower(word_a[index]) - tolower(word_b[index]);
	return result;
}
