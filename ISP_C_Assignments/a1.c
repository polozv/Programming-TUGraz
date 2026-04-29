//---------------------------------------------------------------------------------------------------------------------
/// This program simulates a non-alcoholic Spritzerstand
/// Author: 12347025
//---------------------------------------------------------------------------------------------------------------------
#include <stdio.h>


typedef struct _DepositGlass_{
  int number_of_glasses_;
  int glass_size_;
  double deposit_rate_;
} DepositGlass;


typedef struct _Drink_{
  int glass_size_;
  int tip_;
  double price_;
  char name_[50];
} Drink;


void returnGlass(DepositGlass *deposit_glass);
int getGlassSize(void);
double calculateDeposit(DepositGlass *deposit_glass);
void orderDrink(Drink *drinks, DepositGlass *deposit_glass);
double calculateCocktailPrice(int input, Drink *drinks, DepositGlass *deposit_glass);
double getDepositForSize(int ml);
int tipPercent(void);
void printOrderInfo(Drink *drinks, DepositGlass *deposit_glass, double cocktail_cost, int percentage);


//---------------------------------------------------------------------------------------------------------------------
/// Main function of the program. User is asked to enter a number dependion on what he wants to do. 
/// @return 0 if program terminates succesfully 

int main(void)
{
  DepositGlass deposit_glass = {0, 0, 0.0};
  Drink drinks = {0, 0, 0.0, ""};
  int input = 0;


  printf("Welcome to the Spritzerstand Simulator!!!\n");
  do
  {
    printf("\nHow can I assist you today?\n1. I'd like to return my glasses.\n2. I'd like to order.\n3. Quit\n > ");
    scanf("%d", &input);

    
    if (input == 1)
    {
      returnGlass(&deposit_glass);
    }
    else if (input == 2)
    {
      orderDrink(&drinks, &deposit_glass);
    }
    else if (input == 3)
    {
      return 0;
    }
    else
    {
      printf("[Error]: Please choose a valid option!\n");
    } 
  } while (1);
}


//---------------------------------------------------------------------------------------------------------------------
/// This function asks the user the number of glasses for return and displays result
/// @param *deposit_glass is a pointer to a structure DepositGlass

void returnGlass(DepositGlass *deposit_glass)
{
  do 
  {
    printf("\nHow many glasses do you want to return?\n > ");
    scanf("%d", &deposit_glass->number_of_glasses_);
    if (deposit_glass->number_of_glasses_ <= 0)
    {
      printf("[Error]: How do you plan on returning that?\n");
    }
  } while (deposit_glass->number_of_glasses_ <= 0);

  deposit_glass->glass_size_ = getGlassSize();

  double refund_money = calculateDeposit(deposit_glass);
  printf("\n--------------------------------\n"
         "You returned %d glasses at %.2f € each.\nYour total cashback is: %.2f €"
         "\n--------------------------------\n"
         "Here you go... have a pleasant evening!\n",
         deposit_glass->number_of_glasses_, deposit_glass->deposit_rate_, refund_money);
}


//---------------------------------------------------------------------------------------------------------------------
/// This function asks the user to choose the size of a glass and return a mistake if the input is incorrect 
/// @return the size of the glass the user picked 

int getGlassSize(void)
{
  int size = 0;
  do 
  {
    printf("\nSelect a glass size (in ml)\n- 300\n- 500\n > ");
    scanf("%d", &size);
    if (size != 300 && size != 500)
    {
      printf("[Error]: Sorry that glass size is invalid.\n");
    }
  } while (size != 300 && size != 500);
  return size;
}


//---------------------------------------------------------------------------------------------------------------------
/// This function returns deposit amount depending on the size of a glass
/// @param ml is an integer containing the size of the glass 
/// @return the deposit fee depending on the glass size 

double getDepositForSize(int ml)
{
    switch (ml)
    {
      case 300: 
        return 1.5;
      case 500: 
        return 2.0;
      default: 
        return 0.0;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/// This funtion calculates the whole deposit for returned glasses 
/// @param *deposit_glass is a pointer to a structure DepositGlass
/// @return the whole amount of money the user got for returning cetrain amount of glasses 

double calculateDeposit(DepositGlass *deposit_glass)
{
  deposit_glass->deposit_rate_ = getDepositForSize(deposit_glass->glass_size_);
  return deposit_glass->number_of_glasses_ * deposit_glass->deposit_rate_;
}


//---------------------------------------------------------------------------------------------------------------------
/// This functon displays drink menu, takes user's input and prozess drink order
/// @param *drinks is pointer to a structure Drink
/// @param *deposit_glass is a pointer to a structure DepositGlass


void orderDrink(Drink *drinks, DepositGlass *deposit_glass)
{
  int input = 0;
  int percentage = -1;
  

  do 
  {
    printf("\nWhat are you feeling today?\n"
           "1. Virgin Cuba Libre\n"
           "2. Carbonas rhoncus bibendum\n"
           "3. Fizzy water\n > ");
    scanf("%d", &input);
    if (input < 1 || input > 3)
    {
      printf("[Error]: Hmm I don't seem to have that drink.\n");
    }
  } while (input < 1 || input > 3);

  drinks->glass_size_ = getGlassSize();
  double cocktail_cost = calculateCocktailPrice(input, drinks, deposit_glass);
  percentage = tipPercent();
  printOrderInfo(drinks, deposit_glass,cocktail_cost,percentage);
}


//---------------------------------------------------------------------------------------------------------------------
/// This function asks the user for tip percentage 
/// @return the amount of percentage from 0-100

int tipPercent(void)
{
  int percentage = -1;
  do  
  {
    printf("\nHow much would you like to tip? (in %%)\n > ");
    scanf("%d", &percentage);
    if (percentage < 0 || percentage > 100)
    {
      printf("[Error]: I cannot accept that. Please insert an amount from 0 to 100.\n");
    }
  } while (percentage < 0 || percentage > 100);
  return percentage;
}


//---------------------------------------------------------------------------------------------------------------------
/// This function prints all the details about order
/// @param *drinks is a pointer to a structure Drink
/// @param *deposit_glass is a pointer to a structure Deposit_Glass
/// @param cocktail_cost is the price for ordered cocktails(includind deposit fee)
/// @param percentge is the amount of tip in percents 

void printOrderInfo(Drink *drinks, DepositGlass *deposit_glass, double cocktail_cost, int percentage)
{
  double tip = cocktail_cost * (percentage / 100.0);
  printf("\n--------------------------------\n"
         "- %d ml glass of %s\n"
         "- Drink price: %.2f €\n"
         "- Deposit: %.2f €\n"
         "- Total without tip: %.2f €\n"
         "- Tip: %d %%\n- Total: %.2f €"
         "\n--------------------------------\n"
         "Here you go... have a pleasant evening!\n",
          drinks->glass_size_, drinks->name_, drinks->price_,
          deposit_glass->deposit_rate_, cocktail_cost, percentage, cocktail_cost + tip);
}


//---------------------------------------------------------------------------------------------------------------------
/// This function calculates a cocktail price including deposit fee
/// @param input is the input user typed basen on the type of drink he wants to get
/// @param *drinks is a pointer to a structure Drink
/// @param *deposit_glass is a pointer to a sctucture DepositGlass
/// @return the cocktail price which already includes deposit fee for a glass 

double calculateCocktailPrice(int input, Drink *drinks, DepositGlass *deposit_glass)
{
  deposit_glass->deposit_rate_ = getDepositForSize(drinks->glass_size_);
  if (input == 1) 
  {
    sprintf(drinks->name_, "Virgin Cuba Libre");
    drinks->price_ = 2.5;
  } 
  else if (input == 2) 
  {
    sprintf(drinks->name_, "Carbonas rhoncus bibendum");
    drinks->price_ = 3.0;
  } 
  else 
  {
    sprintf(drinks->name_, "Fizzy water");
    drinks->price_ = 1.0;
  }
    return drinks->price_ + deposit_glass->deposit_rate_;
}
