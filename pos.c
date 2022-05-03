/*
These three lines suppress Visual Studio's warnings
regarding certain functions like scanf. On Xcode
and other IDEs, such preprocessor directives are not needed;
however, including these lines does not affect code compilation
in those IDEs.
*/
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define LIST_MAX 2000 // this is the number of distinct item types possible
#define QUIT_PASS "Q" // this is the special employee password to quit the program



/*
We use a struct to represent an item. We will store all possible items in an
array of type struct item.
*/
struct item
{
   	int barcode;
   	char name[41];
   	double price;
};

typedef struct item Item; // define type Item as type struct item



// 	FUNCTION PROTOTYPES

// Descriptions for each function will be provided before the actual function code.
int checkForSentinel(char input[100]);
void printAllItems(Item allItems[], size_t counter); // for debugging purposes only... see function description
int searchByBarcode(int barcode, Item allItems[], size_t end, size_t* indexptr);
int recurse(int barcode, Item allItems[], size_t startIndex, size_t endIndex, size_t* indexptr);
void processItem(Item i, double* grandTotalPtr);



// The main function runs the whole program and calls other functions
int main() {
   	/////////////// OPENING THE FILE AND STORING INFORMATION IN ARRAY ///////////////

   	Item allItems[LIST_MAX]; // array to store items

   	FILE* filePtr = fopen("barcode1.txt", "r"); // open file in reading mode

   	if (filePtr == NULL) { // cannot open file
          	puts("Unfortunately, we cannot scan your prices at the moment. Please call an employee for assistance.");
          	puts("The program will now exit. We're sorry for the inconvenience.");
          	return 1; // program did not execute successfully
   	}
   	// No else statement needed because if filePtr == NULL, we already exited the program

   	// READING THE FILE

   	size_t counter = 0;  // variable used to assign array indices for each item

   	char line[256] = { 0 };	// our string to store a line from the text file

   	while (fgets(line, sizeof line, filePtr) != NULL)  // while not at end of file
   	{
          	int code = atoi(strtok(line, ",")); // tokenize line and retrive barcode

          	/*
          	Assume that barcodes start at 1 and go up. If code turns out to be
          	negative, then we have an invalid barcode.

          	Note that the atoi function will return 0 if the string cannot be
          	represented as a integer.
          	*/
          	if (code <= 0)
                 	continue; // move on to next line


          	char* itemName;  	// a pointer to the string representing the item's name (see next line of code)
          	itemName = strtok(NULL, ","); // tokenize line again to retrieve item name
          	if (itemName == NULL) // error in tokenizing line again (e.g., line improperly formatted)
                 	continue; // move on to next line

          	double price = atof(strtok(NULL, ",")); // tokenize line once more to obtain item price
          	/*
          	We also assume that prices start at 1 and go up. If price turns out
          	to be negative, then we have an invalid item price.

          	Note that the atof function will return 0 if the string cannot be
          	represented as a floating point value.
          	*/
          	if (price <= 0)
                 	continue; // move on to next line

          	allItems[counter].barcode = code; // update current Item's barcode field
          	strcpy(allItems[counter].name, itemName); // update current Item's name field
          	allItems[counter].price = price; // update current Item's price field

          	counter++; // increase array index
   	}

   	// The function call below is for debugging purposes only. Uncomment it to display
   	// the elements in array allItems.

   	//printAllItems(allItems, counter);

   	fclose(filePtr); // we will not be using the file anymore

   	/////////////// CODE FOR CUSTOMER CHECKOUT ///////////////

   	/*
   	Note: each customer will engage in his/her checkout, and then the program
   	will prepare itself for the next customer. However, if an employee enters
   	the special passphrase specified at the top of the code, the program will
   	quit. The employee can only enter this special passphrase at the prompt
   	"Please enter the first barcode (enter X to quit)".
   	*/
   	while (1) {   // program will loop infinitely unless employee enters quit passphrase

          	// CODE FOR EACH CUSTOMER'S CHECKOUT

          	puts("Welcome to C Market!");

          	double finalTotal = 0; // final total amount after shopping

          	char userInput[100] = { 0 }; // our string to store user input

          	printf("Please enter the first barcode (enter X to quit): ");

          	/*
          	The function scanf returns an integer value indicating how many inputs were sucessfully read.
          	Because we are only reading a string, scanf should return 1. If it does not, then there is a
          	problem. This while loop will have the user try again.
          	*/
          	while (scanf("%s", userInput) != 1) {
                 	puts("Error reading input. (If this problem persists, please ask an employee.)");
                 	printf("Please try again (enter X to quit): ");
                 	fflush(stdin); // flush buffer to ensure user can enter new input
          	}
          	fflush(stdin); // flush buffer to ensure user can enter new input

          	if (checkForSentinel(userInput)) // if the user has entered the special employee password
                 	break; // exit program (by first breaking out of the while loop)

          	while (strcmp("X", userInput) != 0 && strcmp("x", userInput) != 0) { // user enters "X" or "x" to quit
                 	int barcode = atoi(userInput); // convert user input to an integer barcode

                 	// We assume that barcodes should be positive. Once again, the atoi function returns 0 if it
                 	// cannot convert its string argument to an integer.
                 	if (barcode > 0) // barcode is valid
                 	{
                       	size_t arrayIndex; // the array index of the Item with the user-specified barcode

                       	if (searchByBarcode(barcode, allItems, counter - 1, &arrayIndex)) // function is successful if it returns 0
                              	printf("Error: barcode cannot be found. Please try again (enter X to quit): ");
                       	else {
                              	processItem(allItems[arrayIndex], &finalTotal);
                              	printf("Anything else? Enter another barcode. (To stop, type X.) ");
                       	}
                 	}

                 	else // barcode is not valid
                       	printf("%s", "Please enter a valid barcode or the letter \"X.\" ");

                 	/*
                 	The function scanf returns an integer value indicating how many inputs were sucessfully read.
                 	Because we are only reading a string, scanf should return 1. If it does not, then there is a
                 	problem. This while loop will have the user try again.
                 	*/
                 	while (scanf("%s", userInput) != 1) {
                       	puts("Error reading input. (If this problem persists, please ask an employee.)");
                       	printf("Please try again (enter X to quit): ");
                       	fflush(stdin); // flush buffer to ensure user can enter new input
                 	}
                 	fflush(stdin); // flush buffer to ensure user can enter new input
          	}

          	puts("\n\n\n***************************************");
          	printf("TOTAL: $%.2f", finalTotal);
          	printf("\nThank you! Please come again next time.");
          	puts("\n***************************************");

          	puts(""); // for spacing
          	puts(""); // for spacing
          	puts(""); // for spacing
   	}

   	return 0; // program executed sucessfully
}



/*
This function simply checks if its argument is equal to the special employee
password specified at the top of the code. If so, it will return 1, which means
that the two strings match. Otherwise, it will return 0.
*/
int checkForSentinel(char input[100]) {
   	if (strcmp(QUIT_PASS, input) == 0) // strcmp returns 0 if strings match
          	return 1; // in this case, 1 signifies that the strings match
   	return 0; // otherwise, return 0 -- note that no else statement is needed
}



// THIS FUNCTION IS FOR DEBUGGING PURPOSES ONLY. It prints out all items listed
// in the array allItems. We use this to check if all information from the text
// file is properly read and stored in memory.
void printAllItems(Item allItems[], size_t counter) {
   	for (size_t i = 0; i < counter; ++i)
          	printf("Barcode: %-12d Name: %-40s Price: $%-12.2f\n", allItems[i].barcode, allItems[i].name, allItems[i].price);
   	puts(""); // for spacing
}



/*
Given a specified barcode, the function searchByBarcode will use pointers to "return"
the index of the item with the specified barcode. This function will also return 0
for a successful search; otherwise, it will return -1 if the barcode cannot be found.

Note that searchByBarcode calls a recursive function to perform a binary search on
its array argument.
*/

// function to search an array
int searchByBarcode(int barcode, Item allItems[], size_t end, size_t* indexptr) {
   	return recurse(barcode, allItems, 0, end, indexptr);
}

// the recursive function that actually contains the binary search algorithm
int recurse(int barcode, Item allItems[], size_t startIndex, size_t endIndex, size_t* indexptr) {

   	if (startIndex > endIndex) {
          	return -1; // let -1 be an error code
   	}

   	size_t mid = (endIndex + startIndex) / 2; // find "midpoint"

   	if (allItems[mid].barcode == barcode) {
          	*indexptr = mid;
          	return 0;
   	}

   	else if (allItems[mid].barcode < barcode)
          	return recurse(barcode, allItems, mid + 1, endIndex, indexptr); // find barcode in upper half
   	else // allItems[mid].barcode is greater than barcode
          	return recurse(barcode, allItems, startIndex, mid - 1, indexptr); // find barcode in lower half
}



/*
This is a function that takes a specified item and asks how many the user wants to buy. It accepts
a pointer to the grand total variable, and once it calculates the amount spent on buying some number
of the specified item, it adds it to the grand total.
*/
void processItem(Item i, double* grandTotalPtr) {
   	int quantity;
   	double tot; // total from buying some quantity of Item i
   	printf("\n=================\n\n");
   	printf("Product: %s\nPrice: $%.2f\n", i.name, i.price);
   	printf("How many do you want to buy? (Enter negative number to subtract from cart.) ");

   	/*
   	The function scanf returns an integer value indicating how many inputs were sucessfully read.
   	Because we are only reading an integer, scanf should return 1. If it does not, then there is
   	a problem. This while loop will have the user try again.
   	*/
   	while (scanf("%d", &quantity) != 1) {
          	puts("Error reading input. (If this problem persists, call an employee.)");
          	printf("Note that you must enter an integer. Please try again: ");
          	fflush(stdin); // flush buffer to ensure user can enter new input
   	};
   	fflush(stdin); // flush buffer to ensure user can enter new input

   	// calculate and add to grand total
   	tot = i.price * quantity;
   	*grandTotalPtr += tot;

   	printf("Cost of %d items: $%.2f\n", quantity, tot);
   	puts("\n=================\n");
}
