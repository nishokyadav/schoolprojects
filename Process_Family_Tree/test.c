/*
Name:		test.c
Description:	Prints out a family tree based on data in input.txt
		Note: If a couple has more than 9 kids, this program is going to fail.
		Note: If the sum of the lengths of couple names is greater than 15, the formatting will look strange
*/

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAX_NAME_SIZE 15

const char *FILENAME = "input.txt";
const int MAX_NUM_PEOPLE = 50;
const int MAX_CHAR_PER_LINE = 50;

struct person
{
	char nameArr[MAX_NAME_SIZE];
	char spouseArr[MAX_NAME_SIZE];
	int numKids;
	char kidsArr[9][MAX_NAME_SIZE];
};


/*
Name:		printTabs
Inputs:		(int) number of tabs to be printed to screen
Process:	Prints the number of given tabs to the screen
Returns:	Nothing
Description:	As long as the number of tabs is greater than zero, the function works normally.
		Otherwise, nothing is printed to screen.
*/
void printTabs(int);


/*
Name:		removePerson
Inputs:		(char[]) the name of the person to remove,
		(struct person*) an array of people to search through,
		(int*) the number of people in the array
Process:	Searches through the array for a person and extracts his/her info.
		The person is consequently removed from the array.
Returns:	(struct person) the person we want to remove
Description:	IMPORTANT! This function assumes the person you want to remove is actually in the array.
		If you use this function without knowing whether your target is in the array or not,
		you're likely going to get a nasty error (i.e. Segmentation Fault).
		Make sure the person you want to remove is in the array first.
*/
struct person removePerson(char [MAX_NAME_SIZE] , struct person*, int*);


/*
Name:		existsInList
Inputs:		(char[]) name of person you're searching for,
		(struct person*) an array of people to search through,
		(int) number of people in the array
Process:	Searches through the array for a person who matches the given name
Returns:	(int) If person found, 1. Else, 0.
Description:	This function just checks whether someone is in an array or not.
*/
int existsInList(char [MAX_NAME_SIZE], struct person*, int);

int main()
{
	// Initialize variables
	FILE *textfile;
	struct person peopleList[MAX_NUM_PEOPLE];
	int listSize = 0;
	char lineContent[MAX_CHAR_PER_LINE];
	int index;
	int child;
	int lineIndex = 0;
	int nameIndex = 0;
	char tempChar1[MAX_CHAR_PER_LINE], tempChar2[MAX_CHAR_PER_LINE], tempChar3[MAX_CHAR_PER_LINE];
	int tempInt;
	pid_t pid, ppid;
	char currentName[MAX_NAME_SIZE];
	int currentNumKids;
	struct person currentPerson;
	int numTabs = 0;

	// Open the file with the data we want
	textfile = fopen(FILENAME, "r");

	// If the file was opened successfully, start storing its data
	if (textfile != NULL)
	{
		// Get a line of data as long as there is still data to get 
		while ( fgets (lineContent, sizeof lineContent, textfile) != NULL)
		{
			nameIndex = 0;
			lineIndex = 0;
			index = 0;
			

            
			//Store the name and spouse into a person struct 
			
			//Get the person name, char by char, until a ' ' is reached in the lineContent
			while ( lineContent[lineIndex] != ' ' )
			{
				peopleList[listSize].nameArr[nameIndex] = lineContent[lineIndex];
				lineIndex++;
				nameIndex++;
			}
			
                        printf("%s\n", peopleList[listSize].nameArr);
            
			//Append a null char to the cstring, reset the name index, and increment line index
			peopleList[listSize].nameArr[nameIndex] = '\0';
			nameIndex = 0;
			lineIndex++;
			
			//Get the spouse name, char by char, until a ' ' is reached in the lineContent
			while ( lineContent[lineIndex] != ' ' )
			{
				peopleList[listSize].spouseArr[nameIndex] = lineContent[lineIndex];
				lineIndex++;
				nameIndex++;
			}
            
            
            
            
			//Append a null char to the cstring, reset the name index, and increment line index
			peopleList[listSize].spouseArr[nameIndex] = '\0';
			nameIndex = 0;
			lineIndex++;

			// Store the number of kids into a person struct
			peopleList[listSize].numKids = 0;
			index = 0;
			child = 0;
            
			// Extract the kids' names and store them
			// Continue getting children until the line and/or file is done
			while ( (lineContent[lineIndex] != '\0') && (lineContent[lineIndex] != '\n') )
			{
				//Get the child name, char by char, until a ' ' is reached in the lineContent
				while ( (lineContent[lineIndex] != ' ' ) && (lineContent[lineIndex] != '\0' ) && (lineContent[lineIndex] != '\n'))
				{
					peopleList[listSize].kidsArr[index][nameIndex] = lineContent[lineIndex];
					lineIndex++;
					nameIndex++;
					child = 1;
					
				}
				//Append a null char to the cstring, reset the name index, increment the child index and increment line index
				peopleList[listSize].kidsArr[index][nameIndex] = '\0';
				nameIndex = 0;
				lineIndex++;
				
				//Only increment the index if there was at least one child
				if (child==1)
					index++;
			}
			
			//Store the number of children in the person struct
			peopleList[listSize].numKids = index; 
			
			// We're storing things in a person array.
			// When we finish with one person, increment the number of people in the array.
			listSize++;
		} 

		// Close the file. We have all the data.
		fclose(textfile);
	} 

	// Else, print an error message and exit
	else
	{
		fprintf(stderr, "File \"%s\" not found!\n", FILENAME);
		return 1;
	}

	// Now, it's time to process the data
	// Reset the index. We're going to use it in a short while
	index = 0;

	
    
    
    
    
    
    
    
    
    
    
    
    // Prime the pump. We have to start with the first person in the list
	// Get the name of the first person and load their info into a variable
	strcpy( currentName, peopleList[0].nameArr );
	currentPerson = removePerson(currentName, peopleList, &listSize);
    
	// Print the married couple to the screen
	ppid = getppid();
	printf("%s(%d)-%s\n", currentPerson.nameArr, ppid, currentPerson.spouseArr);

	// Keep track of the number of kids they have
	currentNumKids = currentPerson.numKids;

	// The tricky part; forking kids!
	// While there are still kids to process, do the following . . . 
	while (index < currentNumKids)
	{
		// Fork! The parent process will have pid > 0.
		// The child process will have pid = 0
		pid = fork();

		// If forking failed, print an error and exit
		if (pid < 0)
		{
			fprintf(stderr, "Fork failed!\n");
			return 1;
		}

		// Else, if you are a child process, do the following . . .
		else if (pid == 0)
		{
			// We'll need to tab once for format purposes
			numTabs++;

			// We're now working on a new person
			strcpy( currentName, currentPerson.kidsArr[index]);

			// Print the appropriate number of tabs and the name of the person
			printTabs(numTabs);
			
			//Get the parent pid
			ppid = getppid();
			
			printf("%s(%d)", currentName, ppid);

			// Reset the index; we'll need this to check for kids of this person
			index = 0;
			
			// If this person is in the list of people we have, then they at least have a spouse
			// They may have 0 to any number of children
			if (existsInList(currentName, peopleList, listSize))
			{
				// Extract person's info from the list
				currentPerson = removePerson(currentName, peopleList, &listSize);

				// Print the spouse's name
				printf("-%s\n", currentPerson.spouseArr);

				// Update the current number of kids we need to process
				currentNumKids = currentPerson.numKids;
			}

			// Else, forever alone! This person has no spouse or kids. We're done with him/her.
			else
			{
				printf("\n");
				currentNumKids = 0;
			}
		}
		
		// Else, you are parent process. Wait until you're child is done.
		// Then move onto the next child, if any are left to do.
		else
		{
			waitpid(pid, NULL, 0);
			index++;
		}
	} 

	// The process is complete
	return 0;
}

void printTabs(int numTabs)
{
	// Initialize variables
	int index;
	
	// Only print tabs if the number entered is a positive integer
	if (numTabs > 0)
	{
		// Loop until all tabs have been printed
		for (index = 0; index < numTabs; index++)
		{
			printf("\t \t");
		}
	}
}

struct person removePerson(char name[MAX_NAME_SIZE], struct person *array, int *arrLen)
{
	// Initialize variables
        struct person target;
        int index = -1;
        int found = 0;

	// Go through each person in the array until we find who we're looking for
        while (!found)
        {
                index++;

		// We've found our person when their name matches the target name
                if (strcmp( array[index].nameArr, name) == 0 )
                {
                        found = 1;
                }
        }

	// At this point, we know the index of the target in the array
	// Copy the target's info
        target = array[index];
    
    
	// Now, we need to erase the target and shift people in the array to fill the gap
	// We don't actually delete the target, we just overwrite his data
        while (index < (*arrLen) - 1)
        {
                array[index] = array[index+1];
                index++;
        }

	// One less person in the array, so decrement array length
        (*arrLen)--;

        return target;
}

int existsInList(char name[MAX_NAME_SIZE], struct person *array, int arrLen)
{
	// Initialize variables
        int index;

	// Go through each person in the array
        for (index = 0; index < arrLen; index++)
        {
		// If a name in the array matches the target name, we found the person.
                if (strcmp( array[index].nameArr, name) == 0 )
                {
                        return 1;
                }
        }

	// If we never found a match, then the person is not in the array.
        return 0;
}
