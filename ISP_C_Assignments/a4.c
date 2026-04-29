//----------------------------------------------------------------------------------------------------------------------
/// Image Structuring Program - command-line image editor that allows users to load BMP images,
/// place them onto a canvas as layers, and render the final result
///
/// Author: 12347025
//----------------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include "bmp.h"
#include <string.h>
#include <math.h>

#define INITIAL_CAPACITY 10
#define BYTES_PER_PIXEL 4
#define COLOR_CHANNELS 3
#define MAX_ALPHA 255
#define ROOT_LAYER_ID 0
#define EXPECTED_ARGC 3
#define MIN_CANVAS_SIZE 1
#define BMP_MAGIC_B 'B'
#define BMP_MAGIC_M 'M'
#define CMD_COUNT_HELP 1
#define CMD_COUNT_LOAD 2
#define CMD_COUNT_CROP 6
#define CMD_COUNT_PLACE 5
#define CMD_COUNT_SWITCH 2
#define CMD_COUNT_SAVE 2
#define CMD_COUNT_SINGLE 1
#define INDENT_MULTIPLIER 3
#define CAPACITY_MULTIPLIER 2
#define BMP_MAGIC_BUFFER_SIZE 2
#define FILE_START 0
#define NULL_TERMINATOR_SIZE 1

typedef enum _ColorChannel_
{
  CHANNEL_BLUE,
  CHANNEL_GREEN,
  CHANNEL_RED,
  CHANNEL_ALPHA
} ColorChannel;

typedef enum _ReturnCode_
{
  RETURN_SUCCESS,
  RETURN_MEMORY_ERROR,
  RETURN_INVALID_ARGC,
  RETURN_INVALID_CANVAS_SIZE
} ReturnCode;

typedef enum _FunctionResult_
{
  FUNCTION_SUCCESS,
  FUNCTION_ERROR
} FunctionResult;

typedef enum _OperationResult_
{
  OPERATION_SUCCESS = 1,
  OPERATION_FAILURE = 0
} OperationResult;

typedef enum _CommandType_
{
  CMD_BMPS,
  CMD_CROP,
  CMD_HELP,
  CMD_LOAD,
  CMD_PLACE,
  CMD_PRINT,
  CMD_SAVE,
  CMD_SWITCH,
  CMD_TREE,
  CMD_UNDO,
  CMD_UNKNOWN
} CommandType;

typedef struct _BmpInfo_
{
  int id_;
  int width_;
  int height_;
  uint8_t *pixels_;
} BmpInfo;

typedef struct _BmpList_
{
  BmpInfo *bmp_;
  struct _BmpList_ *next_;
} BmpList;

typedef struct _LayerInfo_
{
  int id_;
  int bmp_id_;
  int coordinate_x_;
  int coordinate_y_;
  char blend_mode_;
  struct _LayerInfo_ *parent_;
  struct _LayerInfo_ **children_;
  int children_count_;
  int children_capacity_;
} LayerInfo;

typedef struct _CanvasInfo_
{
  int canvas_width_;
  int canvas_height_;
  LayerInfo *canvas_root_;
  LayerInfo *current_canvas_layer_;
  BmpList *bmp_list_head_;
  int next_bmp_id_;
  int next_layer_id_;
} CanvasInfo;

void load(CanvasInfo *canvas, const char *path);
void addBmpToList(BmpList *bmpList, CanvasInfo *canvas);
int addLayerToParent(LayerInfo *layer, LayerInfo *parent);
void place(int bmp_id, int canvas_x, int canvas_y, const char *blend_mode_str, CanvasInfo *canvas);
BmpList *findID(BmpList *bmp_list_head, int id);
void tree(int depth, LayerInfo *layer);
void crop(int bmp_id, int top_x, int top_y, int bottom_x, int bottom_y, CanvasInfo *canvas);
LayerInfo *findLayer(int layer_id, LayerInfo *layer);
void switch_layer(int layer_id, CanvasInfo *canvas);
void help(void);
void bmps(CanvasInfo *canvas);
void undo(CanvasInfo *canvas);
int validateAndExecuteCommand(char **words, int count, CanvasInfo *canvas);
static int validateArgCount(CommandType cmd, int count);
static int executeCommand(CommandType cmd, char **words, CanvasInfo *canvas);
char **splitIntoWords(char *input, int *count, char **input_copy);
char *getString(void);
char *find(char *buffer, char c);
int takeCommand(CanvasInfo *canvas);
void printCanvas(uint8_t *buffer, CanvasInfo *canvas);
void renderLayer(LayerInfo *layer, uint8_t *buffer, CanvasInfo *canvas);
uint8_t blend(uint8_t A, uint8_t B, uint8_t alpha_value, char blend_mode);
LayerInfo **makePathForPrint(CanvasInfo *canvas, int *layers_count);
void print(CanvasInfo *canvas);
uint8_t *makeCanvasWhite(CanvasInfo *canvas);
void save(CanvasInfo *canvas, const char *file_path);
void freeBmpList(BmpList *bmp_list_head);
void freeLayerTree(LayerInfo *layer);
void cleanupCanvas(CanvasInfo *canvas);
CanvasInfo *createCanvas(int width, int height);
LayerInfo *createRootLayer(void);
void printWelcomeMessage(int width, int height);

//----------------------------------------------------------------------------------------------------------------------
/// @brief Main function that initializes the image editor and runs the command loop
/// @param argc Number of command line arguments
/// @param argv Array of command line arguments
/// @return 0 if execution was successful, error code otherwise
int main(int argc, char *argv[])
{
  if (argc != EXPECTED_ARGC)
  {
    printf("[ERROR] Invalid amount of command line parameters!\n");
    return RETURN_INVALID_ARGC;
  }
  int width = atoi(argv[1]);
  int height = atoi(argv[2]);

  if ((width < MIN_CANVAS_SIZE) || (height < MIN_CANVAS_SIZE))
  {
    printf("[ERROR] Invalid canvas size specified!\n");
    return RETURN_INVALID_CANVAS_SIZE;
  }
  CanvasInfo *canvas = createCanvas(width, height);
  if (canvas == NULL)
  {
    return RETURN_MEMORY_ERROR;
  }
  LayerInfo *root_layer = createRootLayer();
  if (root_layer == NULL)
  {
    free(canvas);
    return RETURN_MEMORY_ERROR;
  }
  canvas->canvas_root_ = root_layer;
  canvas->current_canvas_layer_ = root_layer;
  printWelcomeMessage(width, height);
  int result = takeCommand(canvas);
  cleanupCanvas(canvas);
  free(canvas);
  return result;
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief Creates and initializes a new canvas structure
/// @param width Canvas width in pixels
/// @param height Canvas height in pixels
/// @return Pointer to the created canvas, or NULL on error
CanvasInfo *createCanvas(int width, int height)
{
  CanvasInfo *canvas = malloc(sizeof(CanvasInfo));
  if (canvas == NULL)
  {
    printf("[ERROR] Memory allocation failed!\n");
    return NULL;
  }
  canvas->canvas_root_ = NULL;
  canvas->current_canvas_layer_ = NULL;
  canvas->bmp_list_head_ = NULL;
  canvas->next_bmp_id_ = 0;
  canvas->next_layer_id_ = 1;
  canvas->canvas_width_ = width;
  canvas->canvas_height_ = height;
  return canvas;
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief Creates and initializes the root layer (layer 0)
/// @return Pointer to the created root layer, or NULL on error
LayerInfo *createRootLayer(void)
{
  LayerInfo *layer = malloc(sizeof(LayerInfo));
  if (layer == NULL)
  {
    printf("[ERROR] Memory allocation failed!\n");
    return NULL;
  }
  layer->id_ = ROOT_LAYER_ID;
  layer->parent_ = NULL;
  layer->children_ = NULL;
  layer->children_count_ = 0;
  layer->children_capacity_ = 0;
  return layer;
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief Prints the welcome message with canvas dimensions
/// @param width Canvas width in pixels
/// @param height Canvas height in pixels
void printWelcomeMessage(int width, int height)
{
  printf("\n"
         "Welcome to Image Structuring Program!\n"
         "The canvas is %d x %d pixels.\n"
         "\n", width, height);
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief Frees resources allocated for command processing
/// @param string Input string
/// @param words Array of words
/// @param input_copy Copy of input string
static void freeCommandInput(char *string, char **words, char *input_copy)
{
  free(string);
  free(words);
  free(input_copy);
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief Main command loop that reads and processes user commands
/// @param canvas Pointer to the canvas structure
/// @return 0 on success, error code on failure
int takeCommand(CanvasInfo *canvas)
{
  int count = 0;
  char *input_copy = NULL;
  char *string = NULL;
  char **words = NULL;

  while (1)
  {
    printf(" > ");
    string = getString();
    if (string == NULL)
    {
      return RETURN_MEMORY_ERROR;
    }
    words = splitIntoWords(string, &count, &input_copy);
    if (words == NULL)
    {
      printf("[ERROR] Memory allocation failed!\n");
      free(input_copy);
      free(string);
      return RETURN_MEMORY_ERROR;
    }
    if (count == 0)
    {
      printf("[ERROR] Command unknown!\n");
      freeCommandInput(string, words, input_copy);
      continue;
    }
    if (strcmp(words[0], "quit") == 0)
    {
      freeCommandInput(string, words, input_copy);
      return RETURN_SUCCESS;
    }

    int result = validateAndExecuteCommand(words, count, canvas);
    freeCommandInput(string, words, input_copy);
    if (result == FUNCTION_ERROR)
    {
      continue;
    }
  }
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief Determines the command type from command string
/// @param cmd Command string
/// @return CommandType enum value corresponding to the command, or CMD_UNKNOWN if not found
static CommandType getCommandType(const char *cmd)
{
  if (strcmp(cmd, "bmps") == 0) return CMD_BMPS;
  if (strcmp(cmd, "crop") == 0) return CMD_CROP;
  if (strcmp(cmd, "help") == 0) return CMD_HELP;
  if (strcmp(cmd, "load") == 0) return CMD_LOAD;
  if (strcmp(cmd, "place") == 0) return CMD_PLACE;
  if (strcmp(cmd, "print") == 0) return CMD_PRINT;
  if (strcmp(cmd, "save") == 0) return CMD_SAVE;
  if (strcmp(cmd, "switch") == 0) return CMD_SWITCH;
  if (strcmp(cmd, "tree") == 0) return CMD_TREE;
  if (strcmp(cmd, "undo") == 0) return CMD_UNDO;
  return CMD_UNKNOWN;
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief Validates the argument count for a given command
/// @param cmd Command type
/// @param count Number of arguments provided
/// @return FUNCTION_SUCCESS if count matches expected, FUNCTION_ERROR otherwise
static int validateArgCount(CommandType cmd, int count)
{
  switch (cmd)
  {
    case CMD_BMPS: return (count == CMD_COUNT_SINGLE) ? FUNCTION_SUCCESS : FUNCTION_ERROR;
    case CMD_CROP: return (count == CMD_COUNT_CROP) ? FUNCTION_SUCCESS : FUNCTION_ERROR;
    case CMD_HELP: return (count == CMD_COUNT_HELP) ? FUNCTION_SUCCESS : FUNCTION_ERROR;
    case CMD_LOAD: return (count == CMD_COUNT_LOAD) ? FUNCTION_SUCCESS : FUNCTION_ERROR;
    case CMD_PLACE: return (count == CMD_COUNT_PLACE) ? FUNCTION_SUCCESS : FUNCTION_ERROR;
    case CMD_PRINT: return (count == CMD_COUNT_SINGLE) ? FUNCTION_SUCCESS : FUNCTION_ERROR;
    case CMD_SAVE: return (count == CMD_COUNT_SAVE) ? FUNCTION_SUCCESS : FUNCTION_ERROR;
    case CMD_SWITCH: return (count == CMD_COUNT_SWITCH) ? FUNCTION_SUCCESS : FUNCTION_ERROR;
    case CMD_TREE: return (count == CMD_COUNT_SINGLE) ? FUNCTION_SUCCESS : FUNCTION_ERROR;
    case CMD_UNDO: return (count == CMD_COUNT_SINGLE) ? FUNCTION_SUCCESS : FUNCTION_ERROR;
    case CMD_UNKNOWN: return FUNCTION_ERROR;
  }
  return FUNCTION_ERROR;
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief Executes the specified command
/// @param cmd Command type to execute
/// @param words Array of command words
/// @param canvas Pointer to the canvas structure
/// @return FUNCTION_SUCCESS on success, FUNCTION_ERROR on failure
static int executeCommand(CommandType cmd, char **words, CanvasInfo *canvas)
{
  switch (cmd)
  {
    case CMD_BMPS:
      bmps(canvas);
      return FUNCTION_SUCCESS;

    case CMD_CROP:
      crop(atoi(words[1]), atoi(words[2]),
           atoi(words[3]), atoi(words[4]),
           atoi(words[5]), canvas);
      return FUNCTION_SUCCESS;

    case CMD_HELP:
      help();
      return FUNCTION_SUCCESS;

    case CMD_LOAD:
      load(canvas, words[1]);
      return FUNCTION_SUCCESS;

    case CMD_PLACE:
      place(atoi(words[1]), atoi(words[2]),
            atoi(words[3]), words[4], canvas);
      return FUNCTION_SUCCESS;

    case CMD_PRINT:
      print(canvas);
      return FUNCTION_SUCCESS;

    case CMD_SAVE:
      save(canvas, words[1]);
      return FUNCTION_SUCCESS;

    case CMD_SWITCH:
      switch_layer(atoi(words[1]), canvas);
      return FUNCTION_SUCCESS;

    case CMD_TREE:
      tree(ROOT_LAYER_ID, canvas->canvas_root_);
      return FUNCTION_SUCCESS;

    case CMD_UNDO:
      undo(canvas);
      return FUNCTION_SUCCESS;

    case CMD_UNKNOWN:
      printf("[ERROR] Command unknown!\n");
      return FUNCTION_ERROR;
  }
  return FUNCTION_ERROR;
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief Validates and executes a command
/// @param words Array of command words
/// @param count Number of words
/// @param canvas Pointer to the canvas structure
/// @return FUNCTION_SUCCESS on success, FUNCTION_ERROR on failure
int validateAndExecuteCommand(char **words, int count, CanvasInfo *canvas)
{
  if (count == 0) return FUNCTION_ERROR;
  CommandType cmd = getCommandType(words[0]);

  if (cmd == CMD_UNKNOWN)
  {
    printf("[ERROR] Command unknown!\n");
    return FUNCTION_ERROR;
  }

  if (validateArgCount(cmd, count) == FUNCTION_ERROR)
  {
    printf("[ERROR] Wrong number of arguments!\n");
    return FUNCTION_ERROR;
  }

  return executeCommand(cmd, words, canvas);
}


//The following function was taken from my assignment 3
//----------------------------------------------------------------------------------------------------------------------
/// @brief Finds the first occurrence of a character in a string
/// @param buffer String to search in
/// @param c Character to find
/// @return Pointer to the first occurrence of the character, or NULL if not found
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


//The following function was taken from my assignment 3
//----------------------------------------------------------------------------------------------------------------------
/// @brief Reads a string from stdin with dynamic buffer allocation
/// @return Pointer to the read string, or NULL on error
char *getString(void)
{
  int buffer_size = INITIAL_CAPACITY;

  char *initial_buffer = malloc(buffer_size * sizeof(char));
  if (initial_buffer == NULL)
  {
    return NULL;
  }

  fgets(initial_buffer, buffer_size, stdin);

  while (find(initial_buffer, '\n') == NULL)
  {
    size_t current_length = strlen(initial_buffer);
    buffer_size += INITIAL_CAPACITY;
    size_t new_size = buffer_size * sizeof(char);

    char *new_buffer = realloc(initial_buffer, new_size);

    if (new_buffer == NULL)
    {
      free(initial_buffer);
      return NULL;
    }
    initial_buffer = new_buffer;
    fgets(initial_buffer + current_length, INITIAL_CAPACITY + NULL_TERMINATOR_SIZE, stdin);
  }
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
    char *resized_buffer = realloc(initial_buffer, final_size);
    if (resized_buffer == NULL)
    {
      free(initial_buffer);
      return NULL;
    }
    initial_buffer = resized_buffer;
  }

  return initial_buffer;
}


//The following function was taken from my assignment 3
//----------------------------------------------------------------------------------------------------------------------
/// @brief Splits an input string into words using space as delimiter
/// @param input Input string to split
/// @param count Pointer to store the number of words found
/// @param input_copy Pointer to store the copy of input string (for strtok)
/// @return Array of pointers to words, or NULL on error
char **splitIntoWords(char *input, int *count, char **input_copy)
{
  size_t input_length = strlen(input);
  char **words = malloc (input_length * sizeof(char *));
  if (words == NULL)
  {
    return NULL;
  }
  *input_copy = malloc(input_length + NULL_TERMINATOR_SIZE);
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
    return words;
  }
  char **new_words_array = realloc(words, (*count) * sizeof(char *));
  if (new_words_array == NULL)
  {
    free(*input_copy);
    free(words);
    return NULL;
  }
  words = new_words_array;
  return words;
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief Adds a BMP to the canvas's BMP list
/// @param bmpList BMP list node to add
/// @param canvas Pointer to the canvas structure
void addBmpToList(BmpList *bmpList, CanvasInfo *canvas)
{
  if (canvas->bmp_list_head_ == NULL)
  {
    canvas->bmp_list_head_ = bmpList;
  }
  else
  {
    BmpList *current = canvas->bmp_list_head_;
    while (current->next_ != NULL)
    {
      current = current->next_;
    }
    current->next_ = bmpList;
  }
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief Adds a layer as a child to a parent layer
/// @param layer Layer to add
/// @param parent Parent layer
/// @return OPERATION_SUCCESS on success, OPERATION_FAILURE on error
int addLayerToParent(LayerInfo *layer, LayerInfo *parent)
{
  if (parent->children_ == NULL)
  {
    parent->children_capacity_ = INITIAL_CAPACITY;
    parent->children_ = malloc(INITIAL_CAPACITY * sizeof(LayerInfo*));
    if (parent->children_ == NULL)
    {
      return OPERATION_FAILURE;
    }
    parent->children_[ROOT_LAYER_ID] = layer;
    parent->children_count_ = 1;
  }
  else if (parent->children_count_ >= parent->children_capacity_)
  {
    int updated_capacity = parent->children_capacity_ * CAPACITY_MULTIPLIER;
    LayerInfo ** temp = realloc(parent->children_, updated_capacity * sizeof(LayerInfo*));
    if (temp == NULL)
    {
      return OPERATION_FAILURE;
    }
    parent->children_ = temp;
    parent->children_capacity_ = updated_capacity;
    parent->children_[parent->children_count_] = layer;
    parent->children_count_++;
  }
  else
  {
    parent->children_[parent->children_count_] = layer;
    parent->children_count_++;
  }
  return OPERATION_SUCCESS;
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief Loads a BMP image from a file and adds it to the canvas
/// @param canvas Pointer to the canvas structure
/// @param path Path to the BMP file
void load(CanvasInfo *canvas, const char *path)
{
  FILE *file = fopen(path, "rb");
  if (file == NULL)
  {
    printf("[ERROR] Cannot open file!\n");
    return;
  }
  char buffer[BMP_MAGIC_BUFFER_SIZE];
  fread(buffer, 1, BMP_MAGIC_BUFFER_SIZE, file);
  if (buffer[0] != BMP_MAGIC_B || buffer[1] != BMP_MAGIC_M)
  {
    fclose(file);
    printf("[ERROR] Invalid file!\n");
    return;
  }
  fseek(file, FILE_START, SEEK_SET);
  BmpHeader header;
  fread(&header, sizeof(BmpHeader), 1, file);
  BmpInfo *bmp = malloc(sizeof(BmpInfo));
  if (bmp == NULL)
  {
    fclose(file);
    printf("[ERROR] Memory allocation failed!\n");
    return;
  }
  bmp->width_ = header.width_;
  bmp->height_ = header.height_;
  int bytes_amount = bmp->width_ * bmp->height_ * BYTES_PER_PIXEL;
  bmp->pixels_ = malloc(bytes_amount);
  if (bmp->pixels_ == NULL)
  {
    free(bmp);
    fclose(file);
    printf("[ERROR] Memory allocation failed!\n");
    return;
  }
  bmp->id_ = canvas->next_bmp_id_;
  fseek(file, header.offset_pixel_array_, SEEK_SET);
  fread(bmp->pixels_, 1, bytes_amount, file);
  BmpList *bmpList = malloc(sizeof(BmpList));
  if (bmpList == NULL)
  {
    free(bmp->pixels_);
    free(bmp);
    fclose(file);
    printf("[ERROR] Memory allocation failed!\n");
    return;
  }
  bmpList->bmp_ = bmp;
  bmpList->next_ = NULL;
  addBmpToList(bmpList, canvas);
  canvas->next_bmp_id_++;
  printf("Loaded %s with ID %d and dimensions %d %d\n",
         path, bmp->id_, bmp->width_, bmp->height_);
  fclose(file);
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief Places a BMP image on the canvas at specified coordinates with a blend mode
/// @param bmp_id ID of the BMP to place
/// @param canvas_x X coordinate on canvas (1-based)
/// @param canvas_y Y coordinate on canvas (1-based)
/// @param blend_mode_str Blend mode string ('n', 'm', or 's')
/// @param canvas Pointer to the canvas structure
void place(int bmp_id, int canvas_x, int canvas_y, const char *blend_mode_str, CanvasInfo *canvas)
{
  BmpList *result = findID(canvas->bmp_list_head_, bmp_id);
  if (result == NULL)
  {
    printf("[ERROR] BMP ID not found!\n");
    return;
  }

  int canvas_x_0 = canvas_x - 1;
  int canvas_y_0 = canvas_y - 1;
  if (canvas_x < MIN_CANVAS_SIZE || canvas_x > canvas->canvas_width_ ||
     canvas_y < MIN_CANVAS_SIZE || canvas_y > canvas->canvas_height_)
  {
    printf("[ERROR] Canvas coordinates are invalid!\n");
    return;
  }
  if (canvas_x_0 + result->bmp_->width_ > canvas->canvas_width_ ||
     canvas_y_0 + result->bmp_->height_ > canvas->canvas_height_)
  {
    printf("[ERROR] BMP does not fit on the canvas!\n");
    return;
  }
  if (strlen(blend_mode_str) != 1 ||
      (blend_mode_str[0] != 'n' && blend_mode_str[0] != 'm' &&
       blend_mode_str[0] != 's'))
  {
    printf("[ERROR] Invalid blend mode!\n");
    return;
  }
  char blend_mode = blend_mode_str[0];

  LayerInfo *layer = malloc(sizeof(LayerInfo));
  if (layer == NULL)
  {
    printf("[ERROR] Memory allocation failed!\n");
    return;
  }
  layer->id_ = canvas->next_layer_id_;
  layer->bmp_id_ = result->bmp_->id_;
  layer->coordinate_x_ = canvas_x;
  layer->coordinate_y_ = canvas_y;
  layer->blend_mode_ = blend_mode;
  layer->children_count_ = 0;
  layer->parent_ = canvas->current_canvas_layer_;
  layer->children_ = NULL;
  layer->children_capacity_ = 0;

  if (!addLayerToParent(layer, canvas->current_canvas_layer_))
  {
    printf("[ERROR] Memory allocation failed!\n");
    free(layer);
    return;
  }

  canvas->current_canvas_layer_ = layer;
  canvas->next_layer_id_++;
  printf("Switched to layer %d\n", layer->id_);
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief Finds a BMP in the list by its ID
/// @param bmp_list_head Head of the BMP list
/// @param id BMP ID to find
/// @return Pointer to the BMP list node containing the BMP, or NULL if not found
BmpList *findID(BmpList *bmp_list_head, int id)
{
  BmpList *current = bmp_list_head;
  while (current != NULL)
  {
    if (current->bmp_->id_ == id)
    {
      return current;
    }
    current = current->next_;
  }
  return NULL;
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief Prints the layer tree structure recursively
/// @param depth Current depth in the tree
/// @param layer Current layer to print
void tree(int depth, LayerInfo *layer)
{
  int indent = depth * INDENT_MULTIPLIER;

  if (layer->id_ == ROOT_LAYER_ID)
  {
    printf("Layer %d\n", ROOT_LAYER_ID);
  }
  else
  {
    for (int count = 0; count < indent; count++)
    {
      printf(" ");
    }
    printf("Layer %d renders BMP %d at %d %d\n", 
    layer->id_, layer->bmp_id_, layer->coordinate_x_, layer->coordinate_y_);
  }

  if (layer->children_ != NULL && layer->children_count_ > 0)
  {
    for (int count = 0; count < layer->children_count_ ;count++)
    {
      tree(depth+1, layer->children_[count]);
    }
  }
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief Crops a BMP image to specified coordinates and creates a new BMP
/// @param bmp_id ID of the BMP to crop
/// @param top_x Top-left X coordinate (1-based)
/// @param top_y Top-left Y coordinate (1-based)
/// @param bottom_x Bottom-right X coordinate (1-based)
/// @param bottom_y Bottom-right Y coordinate (1-based)
/// @param canvas Pointer to the canvas structure
void crop(int bmp_id, int top_x, int top_y, int bottom_x, int bottom_y, CanvasInfo *canvas)
{
  BmpList *result = findID(canvas->bmp_list_head_, bmp_id);
  if (result == NULL)
  {
    printf("[ERROR] BMP ID not found!\n");
    return;
  }

  int top_x_0 = top_x - 1;
  int top_y_0 = top_y - 1;
  int bottom_x_0 = bottom_x - 1;
  int bottom_y_0 = bottom_y - 1;

  if (top_x_0 < 0 || top_x_0 >= result->bmp_->width_ ||
      top_y_0 < 0 || top_y_0 >= result->bmp_->height_ ||
      bottom_x_0 < 0 || bottom_x_0 >= result->bmp_->width_ ||
      bottom_y_0 < 0 || bottom_y_0 >= result->bmp_->height_)
  {
    printf("[ERROR] Crop coordinates are outside the BMP!\n");
    return;
  }
  if (top_x_0 > bottom_x_0 || top_y_0 > bottom_y_0)
  {
    printf("[ERROR] Crop coordinates do not form a valid rectangle!\n");
    return;
  }

  int new_width = bottom_x_0 - top_x_0 + 1;
  int new_height = bottom_y_0 - top_y_0 + 1;
  BmpInfo *bmp_cropped = malloc(sizeof(BmpInfo));
  if (bmp_cropped == NULL)
  {
    printf("[ERROR] Memory allocation failed!\n");
    return;
  }
  bmp_cropped->pixels_ = malloc(new_width * new_height * BYTES_PER_PIXEL);
  if (bmp_cropped->pixels_ == NULL)
  {
    printf("[ERROR] Memory allocation failed!\n");
    free(bmp_cropped);
    return;
  }
  bmp_cropped->id_ = canvas->next_bmp_id_++;
  bmp_cropped->width_ = new_width;
  bmp_cropped->height_ = new_height;

  for (int line_count = top_y_0; line_count <= bottom_y_0; line_count++)
  {
    int bmp_line_index = (result->bmp_->height_ - 1 - line_count);
    uint8_t *original_start = result->bmp_->pixels_ +
      (bmp_line_index * result->bmp_->width_ + top_x_0) * BYTES_PER_PIXEL;
    int new_line_index = (new_height - 1 - (line_count - top_y_0));
    uint8_t *new_start = bmp_cropped->pixels_ + (new_line_index * new_width) * BYTES_PER_PIXEL;
    memcpy(new_start, original_start, new_width * BYTES_PER_PIXEL);
  }

  BmpList *list = malloc(sizeof(BmpList));
  if (list == NULL)
  {
    printf("[ERROR] Memory allocation failed!\n");
    free(bmp_cropped->pixels_);
    free(bmp_cropped);
    return;
  }
  list->bmp_ = bmp_cropped;
  list->next_ = NULL;
  addBmpToList(list, canvas);
  printf("Cropped ID %d to new ID %d with dimensions %d x %d\n",
         result->bmp_->id_, bmp_cropped->id_, bmp_cropped->width_, bmp_cropped->height_);
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief Switches the current canvas layer to the specified layer
/// @param layer_id ID of the layer to switch to
/// @param canvas Pointer to the canvas structure
void switch_layer(int layer_id, CanvasInfo *canvas)
{
  LayerInfo *result = findLayer(layer_id, canvas->canvas_root_);
  if (result == NULL)
  {
    printf("[ERROR] Layer ID not found!\n");
    return;
  }

  canvas->current_canvas_layer_ = result;
  printf("Switched to layer %d\n", result->id_);
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief Finds a layer in the layer tree by its ID
/// @param layer_id ID of the layer to find
/// @param layer Root layer to start searching from
/// @return Pointer to the found layer, or NULL if not found
LayerInfo *findLayer(int layer_id, LayerInfo *layer)
{
  if (layer == NULL)
  {
    return NULL;
  }
  if (layer_id == layer->id_)
  {
    return layer;
  }
  if (layer->children_ != NULL && layer->children_count_ > 0)
  {
    for (int count = 0; count < layer->children_count_ ;count++)
    {
      LayerInfo * found = findLayer(layer_id, layer->children_[count]);
      if (found != NULL)
      {
        return found;
      }
    }
  }
  return NULL;
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief Prints the help message with available commands
void help(void)
{
  printf(
    "\n"
    "Available commands:\n"
    " help\n"
    " load <PATH>\n"
    " crop <BMP_ID> <TOP_X> <TOP_Y> <BOTTOM_X> <BOTTOM_Y>\n"
    " place <BMP_ID> <CANVAS_X> <CANVAS_Y> <BLEND_MODE>\n"
    " undo\n"
    " print\n"
    " switch <LAYER_ID>\n"
    " tree\n"
    " bmps\n"
    " save <FILE_PATH>\n"
    " quit\n"
    "\n");
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief Lists all loaded BMP images with their IDs and dimensions
/// @param canvas Pointer to the canvas structure
void bmps(CanvasInfo *canvas)
{
  BmpList *current = canvas->bmp_list_head_;
  while (current != NULL)
  {
    printf("BMP %d has dimensions %d x %d\n", current->bmp_->id_, current->bmp_->width_, current->bmp_->height_);
    current = current->next_;
  }
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief Undoes the last layer operation by switching to the parent layer
/// @param canvas Pointer to the canvas structure
void undo(CanvasInfo *canvas)
{
  if (canvas->current_canvas_layer_->id_ == ROOT_LAYER_ID || canvas->current_canvas_layer_ == canvas->canvas_root_)
  {
    printf("[ERROR] Already at root layer!\n");
    return;
  }
  canvas->current_canvas_layer_ = canvas->current_canvas_layer_->parent_;
  printf("Switched to layer %d\n", canvas->current_canvas_layer_->id_);
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief Renders and prints the canvas with all layers
/// @param canvas Pointer to the canvas structure
void print(CanvasInfo *canvas)
{
  int layers_count;
  LayerInfo **path = makePathForPrint(canvas, &layers_count);
  if (path == NULL)
  {
    return;
  }

  uint8_t *buffer = makeCanvasWhite(canvas);
  if (buffer == NULL)
  {
    free(path);
    return;
  }

  for (int count = 1; count < layers_count; count++)
  {
    renderLayer(path[count], buffer, canvas);
  }
  printCanvas(buffer, canvas);

  free(path);
  free(buffer);
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief Creates a path array from root to current layer for rendering
/// @param canvas Pointer to the canvas structure
/// @param layers_count Pointer to store the number of layers in the path
/// @return Array of layer pointers, or NULL on error
LayerInfo **makePathForPrint(CanvasInfo *canvas, int *layers_count)
{
  *layers_count = 0;
  LayerInfo *current = canvas->current_canvas_layer_;
  while (current != canvas->canvas_root_)
  {
    current = current->parent_;
    (*layers_count)++;
  }
  (*layers_count)++;

  LayerInfo **print_path = malloc((*layers_count) * sizeof(LayerInfo*));
  if (print_path == NULL)
  {
    printf("[ERROR] Memory allocation failed!\n");
    return NULL;
  }
  int index = (*layers_count) - 1;

  current = canvas->current_canvas_layer_;
  while (current != canvas->canvas_root_)
  {
    print_path[index] = current;
    current = current->parent_;
    index--;
  }
  print_path[ROOT_LAYER_ID] = canvas->canvas_root_;

  return print_path;
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief Blends two color values using the specified blend mode
/// @param A Background color value
/// @param B Foreground color value
/// @param alpha_value Alpha channel value (0-255)
/// @param blend_mode Blend mode character ('n', 'm', or 's')
/// @return Blended color value
uint8_t blend(uint8_t A, uint8_t B, uint8_t alpha_value, char blend_mode)
{
  uint8_t R = 0;
  double alpha = alpha_value / (double)MAX_ALPHA;

  switch (blend_mode)
  {
    case 'n':  
    {
      R = alpha * B + (1.0 - alpha) * A;
      break;
    }

    case 'm':  
    {
      R = (A * B) / MAX_ALPHA;
      break;
    }

    case 's':  
    {
      R = fmax(A, B) - fmin(A, B);
      break;
    }

    default:
      R = A;
      break;
  }

  return R;
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief Creates a white canvas buffer
/// @param canvas Pointer to the canvas structure
/// @return Pointer to the allocated buffer filled with white pixels, or NULL on error
uint8_t *makeCanvasWhite(CanvasInfo *canvas)
{
  uint8_t *buffer = malloc(canvas->canvas_width_ * canvas->canvas_height_ * BYTES_PER_PIXEL);
  if (buffer == NULL)
  {
    printf("[ERROR] Memory allocation failed!\n");
    return NULL;
  }
  memset(buffer, MAX_ALPHA, canvas->canvas_width_ * canvas->canvas_height_ * BYTES_PER_PIXEL);
  return buffer;
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief Renders a layer onto the canvas buffer
/// @param layer Layer to render
/// @param buffer Canvas buffer to render onto
/// @param canvas Pointer to the canvas structure
void renderLayer(LayerInfo *layer, uint8_t *buffer, CanvasInfo *canvas)
{
  BmpList *result = findID(canvas->bmp_list_head_, layer->bmp_id_);

  int coordinate_x = layer->coordinate_x_;
  int coordinate_y = layer->coordinate_y_;
  char blend_mode = layer->blend_mode_;
  int width = result->bmp_->width_;
  int height = result->bmp_->height_;

  for (int x = 0; x < width; x++)
  {
    for (int y = 0; y < height; y++)
    {
      int canvas_x = coordinate_x - 1 + x;
      int canvas_y = coordinate_y - 1 + y;

      int bmp_offset = ((height - 1 - y) * width + x) * BYTES_PER_PIXEL;
      uint8_t *bmp_pixel = result->bmp_->pixels_ + bmp_offset;

      int canvas_offset = (canvas_y * canvas->canvas_width_ + canvas_x) * BYTES_PER_PIXEL;
      uint8_t *canvas_pixel = buffer + canvas_offset;

      for (int channel = 0; channel < COLOR_CHANNELS; channel++)
      {
        canvas_pixel[channel] = blend(canvas_pixel[channel], bmp_pixel[channel],
                                      bmp_pixel[CHANNEL_ALPHA], blend_mode);
      }
    }
  }
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief Prints the canvas buffer as a colored grid using ANSI escape codes
/// @param buffer Canvas buffer to print
/// @param canvas Pointer to the canvas structure
void printCanvas(uint8_t *buffer, CanvasInfo *canvas)
{
  printf("   ");
  for (int column = 1; column <= canvas->canvas_width_; column++)
  {
    printf(" %02d", column);
  }
  printf("\n");

  for (int row = 1; row <= canvas->canvas_height_; row++)
  {
    printf("%02d|", row);

    for (int col = 1; col <= canvas->canvas_width_; col++)
    {
      int x = col - 1;
      int y = row - 1;
      int offset = (y * canvas->canvas_width_ + x) * BYTES_PER_PIXEL;

      uint8_t B = buffer[offset + CHANNEL_BLUE];
      uint8_t G = buffer[offset + CHANNEL_GREEN];
      uint8_t R = buffer[offset + CHANNEL_RED];

      printf("\033[38;2;%d;%d;%dm███\033[0m", R, G, B);
    }
    printf("|\n");
  }

  printf("  ");
  for (int count = 0; count < canvas->canvas_width_ * 3 + 2; count++)
  {
    printf("-");
  }
  printf("\n");
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief Saves the canvas as a BMP file
/// @param canvas Pointer to the canvas structure
/// @param file_path Path to save the BMP file
void save(CanvasInfo *canvas, const char *file_path)
{
  int layers_count;
  LayerInfo **path = makePathForPrint(canvas, &layers_count);
  if (path == NULL) return;

  uint8_t *buffer = makeCanvasWhite(canvas);
  if (buffer == NULL)
  {
    free(path);
    return;
  }

  for (int index = 1; index < layers_count; index++)
  {
    renderLayer(path[index], buffer, canvas);
  }

  BmpHeader header;
  fillBmpHeaderDefaultValues(&header, canvas->canvas_width_, canvas->canvas_height_);
  FILE *file = fopen(file_path, "wb");
  if (file == NULL)
  {
    printf("[ERROR] Invalid file path!\n");
    free(path);
    free(buffer);
    return;
  }
  fwrite(&header, sizeof(BmpHeader), 1, file);

  for (int row = canvas->canvas_height_ - 1; row >= 0; row--)
  {
    uint8_t *row_start = buffer + (row * canvas->canvas_width_ * BYTES_PER_PIXEL);
    fwrite(row_start, canvas->canvas_width_ * BYTES_PER_PIXEL, 1, file);
  }
  fclose(file);

  printf("Successfully saved image to %s\n", file_path);

  free(path);
  free(buffer);
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief Frees all memory allocated for the BMP list
/// @param bmp_list_head Head of the BMP list
void freeBmpList(BmpList *bmp_list_head)
{
  BmpList *current = bmp_list_head;
  while (current != NULL)
  {
    BmpList *next = current->next_;
    if (current->bmp_ != NULL)
    {
      if (current->bmp_->pixels_ != NULL)
      {
        free(current->bmp_->pixels_);
      }
      free(current->bmp_);
    }
    free(current);
    current = next;
  }
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief Frees all memory allocated for the layer tree recursively
/// @param layer Root layer of the tree to free
void freeLayerTree(LayerInfo *layer)
{
  if (layer == NULL)
  {
    return;
  }

  if (layer->children_ != NULL && layer->children_count_ > 0)
  {
    for (int index = 0; index < layer->children_count_; index++)
    {
      freeLayerTree(layer->children_[index]);
    }
    free(layer->children_);
  }

  free(layer);
}


//----------------------------------------------------------------------------------------------------------------------
/// @brief Cleans up all resources allocated for the canvas
/// @param canvas Pointer to the canvas structure
void cleanupCanvas(CanvasInfo *canvas)
{
  if (canvas == NULL)
  {
    return;
  }

  if (canvas->bmp_list_head_ != NULL)
  {
    freeBmpList(canvas->bmp_list_head_);
    canvas->bmp_list_head_ = NULL;
  }

  if (canvas->canvas_root_ != NULL)
  {
    freeLayerTree(canvas->canvas_root_);
    canvas->canvas_root_ = NULL;
    canvas->current_canvas_layer_ = NULL;
  }
}
