/*
Assignment 5: Multi-threaded Producer Consumer Pipeline
By: Frankie Herbert
--I am a repeat of this course
*/

How to Compile:
gcc --std=gnu99 -g -pthread -o line_processor main.c

How to Run Input Files and Redirect to Output Files:

1) ./line_processor < input1.txt > output1.txt

2) ./line_processor < input2.txt > output2.txt

3) ./line_processor < input3.txt > output3.txt





Cited Sources:

URL: https://www.geeksforgeeks.org/c-program-to-find-and-replace-a-word-in-a-file-by-another-given-word/
Author: ab1998dob
Code: 
// Function to find and
// replace a word in File
void findAndReplaceInFile()
{
    FILE *ifp, *ofp;
    char word[100], ch, read[100], replace[100];
    int word_len, i, p = 0;
 
    ifp = fopen("file_search_input.txt", "r");
    ofp = fopen("file_replace_output.txt", "w+");
    if (ifp == NULL || ofp == NULL) {
        printf("Can't open file.");
        exit(0);
    }
    puts("THE CONTENTS OF THE FILE ARE SHOWN BELOW :\n");
 
    // displaying file contents
    while (1) {
        ch = fgetc(ifp);
        if (ch == EOF) {
            break;
        }
        printf("%c", ch);
    }
 
    puts("\n\nEnter the word to find:");
    fgets(word, 100, stdin);
 
    // removes the newline character from the string
    word[strlen(word) - 1] = word[strlen(word)];
 
    puts("Enter the word to replace it with :");
    fgets(replace, 100, stdin);
 
    // removes the newline character from the string
    replace[strlen(replace) - 1] = replace[strlen(replace)];
 
    fprintf(ofp, "%s - %s\n", word, replace);
 
    // comparing word with file
    rewind(ifp);
    while (!feof(ifp)) {
 
        fscanf(ifp, "%s", read);
 
        if (strcmp(read, word) == 0) {
 
            // for deleting the word
            strcpy(read, replace);
        }
 
        // In last loop it runs twice
        fprintf(ofp, "%s ", read);
    }
 
    // Printing the content of the Output file
    rewind(ofp);
    while (1) {
        ch = fgetc(ofp);
        if (ch == EOF) {
            break;
        }
        printf("%c", ch);
    }
 
    fclose(ifp);
    fclose(ofp);
}
 
// Driver code
void main()
{
    findAndReplaceInFile();
}
_________________________________________________________________________________
URL: https://linuxhint.com/c-command-line-argument-processing/
Author: Kalsoom Bibi
Code: 
int main(int argc, char *argv[])

{

    int w;
    for(w=1;w<argc;w++)
    {
    printf("%s\n\n",argv[w]);
    }
    return 0;


}
int main(int argc, char *argv[])

{

    int b;
    for(b=0;b<argc;b++)
    {
    printf("%s",argv[b]);
    }
    return 0;


}
______________________________________________________________________________________
URL: https://www.tutorialspoint.com/multithreading-in-c
Author: TutorialPoint

_______________________________________________________________________________________
URL: https://replit.com/@cs344/65prodconspipelinec#main.c
Author: cs344
Code: 
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h> // must link with -lm

/*
A program with a pipeline of 3 threads that interact with each other as producers and consumers.
- Input thread is the first thread in the pipeline. It gets input from the user and puts it in a buffer it shares with the next thread in the pipeline.
- Square root thread is the second thread in the pipeline. It consumes items from the buffer it shares with the input thread. It computes the square root of this item. It puts the computed value in a buffer it shares with the next thread in the pipeline. Thus this thread implements both consumer and producer functionalities.
- Output thread is the third thread in the pipeline. It consumes items from the buffer it shares with the square root thread and prints the items.

*/

// Size of the buffers
#define SIZE 10

// Number of items that will be produced. This number is less than the size of the buffer. Hence, we can model the buffer as being unbounded.
#define NUM_ITEMS 6

// Buffer 1, shared resource between input thread and square-root thread
int buffer_1[SIZE];
// Number of items in the buffer
int count_1 = 0;
// Index where the input thread will put the next item
int prod_idx_1 = 0;
// Index where the square-root thread will pick up the next item
int con_idx_1 = 0;
// Initialize the mutex for buffer 1
pthread_mutex_t mutex_1 = PTHREAD_MUTEX_INITIALIZER;
// Initialize the condition variable for buffer 1
pthread_cond_t full_1 = PTHREAD_COND_INITIALIZER;


// Buffer 2, shared resource between square root thread and output thread
double buffer_2[SIZE];
// Number of items in the buffer
int count_2 = 0;
// Index where the square-root thread will put the next item
int prod_idx_2 = 0;
// Index where the output thread will pick up the next item
int con_idx_2 = 0;
// Initialize the mutex for buffer 2
pthread_mutex_t mutex_2 = PTHREAD_MUTEX_INITIALIZER;
// Initialize the condition variable for buffer 2
pthread_cond_t full_2 = PTHREAD_COND_INITIALIZER;


/*
Get input from the user.
This function doesn't perform any error checking.
*/
int get_user_input(){
  int value;
  printf("Enter a positive integer: ");
  scanf("%d", &value);
  return value;
}

/*
 Put an item in buff_1
*/
void put_buff_1(int item){
  // Lock the mutex before putting the item in the buffer
  pthread_mutex_lock(&mutex_1);
  // Put the item in the buffer
  buffer_1[prod_idx_1] = item;
  // Increment the index where the next item will be put.
  prod_idx_1 = prod_idx_1 + 1;
  count_1++;
  // Signal to the consumer that the buffer is no longer empty
  pthread_cond_signal(&full_1);
  // Unlock the mutex
  pthread_mutex_unlock(&mutex_1);
}

/*
 Function that the input thread will run.
 Get input from the user.
 Put the item in the buffer shared with the square_root thread.
*/
void *get_input(void *args)
{
    for (int i = 0; i < NUM_ITEMS; i++)
    {
      // Get the user input
      int item = get_user_input();
      put_buff_1(item);
    }
    return NULL;
}

/*
Get the next item from buffer 1
*/
int get_buff_1(){
  // Lock the mutex before checking if the buffer has data
  pthread_mutex_lock(&mutex_1);
  while (count_1 == 0)
    // Buffer is empty. Wait for the producer to signal that the buffer has data
    pthread_cond_wait(&full_1, &mutex_1);
  int item = buffer_1[con_idx_1];
  // Increment the index from which the item will be picked up
  con_idx_1 = con_idx_1 + 1;
  count_1--;
  // Unlock the mutex
  pthread_mutex_unlock(&mutex_1);
  // Return the item
  return item;
}

/*
 Put an item in buff_2
*/
void put_buff_2(double item){
  // Lock the mutex before putting the item in the buffer
  pthread_mutex_lock(&mutex_2);
  // Put the item in the buffer
  buffer_2[prod_idx_2] = item;
  // Increment the index where the next item will be put.
  prod_idx_2 = prod_idx_2 + 1;
  count_2++;
  // Signal to the consumer that the buffer is no longer empty
  pthread_cond_signal(&full_2);
  // Unlock the mutex
  pthread_mutex_unlock(&mutex_2);
}

/*
 Function that the square root thread will run. 
 Consume an item from the buffer shared with the input thread.
 Compute the square root of the item.
 Produce an item in the buffer shared with the output thread.

*/
void *compute_square_root(void *args)
{
    int item = 0;
    double square_root;
    for (int i = 0; i < NUM_ITEMS; i++)
    {
      item = get_buff_1();
      square_root = sqrt(item);
      put_buff_2(square_root);
    }
    return NULL;
}

/*
Get the next item from buffer 2
*/
double get_buff_2(){
  // Lock the mutex before checking if the buffer has data
  pthread_mutex_lock(&mutex_2);
  while (count_2 == 0)
    // Buffer is empty. Wait for the producer to signal that the buffer has data
    pthread_cond_wait(&full_2, &mutex_2);
  double item = buffer_2[con_idx_2];
  // Increment the index from which the item will be picked up
  con_idx_2 = con_idx_2 + 1;
  count_2--;
  // Unlock the mutex
  pthread_mutex_unlock(&mutex_2);
  // Return the item
  return item;
}


/*
 Function that the output thread will run. 
 Consume an item from the buffer shared with the square root thread.
 Print the item.
*/
void *write_output(void *args)
{
    double item;
    for (int i = 0; i < NUM_ITEMS; i++)
    {
      item = get_buff_2();
      printf("\nOutput: %.4f\n", item);
    }
    return NULL;
}

int main()
{
    srand(time(0));
    pthread_t input_t, square_root_t, output_t;
    // Create the threads
    pthread_create(&input_t, NULL, get_input, NULL);
    pthread_create(&square_root_t, NULL, compute_square_root, NULL);
    pthread_create(&output_t, NULL, write_output, NULL);
    // Wait for the threads to terminate
    pthread_join(input_t, NULL);
    pthread_join(square_root_t, NULL);
    pthread_join(output_t, NULL);
    return EXIT_SUCCESS;
}
________________________________________________________________________________________________
URL: https://aticleworld.com/remove-trailing-newline-character-from-fgets/
Author: AticleWorld
Code: 
#define  BUFFER_SIZE 24
int main(void)
{
    char buf[BUFFER_SIZE];
    printf("Enter the data = ");
    if (fgets(buf, sizeof(buf), stdin) == NULL)
    {
        printf("Fail to read the input stream");
    }
    else
    {
        buf[strlen(buf) - 1] = '\0';
    }
    printf("Entered Data = %s\n",buf);
    return 0;
}
_________________________________________________________________________________________
Code: 
#define  BUFFER_SIZE 24
int main(void)
{
    char buf[BUFFER_SIZE];
    printf("Enter the data = ");
    if (fgets(buf, sizeof(buf), stdin) == NULL)
    {
        printf("Fail to read the input stream");
    }
    else
    {
        //find new line
        char *ptr = strchr(buf, '\n');
        if (ptr)
        {
            //if new line found replace with null character
            *ptr  = '\0';
        }
    }
    printf("Entered Data = %s\n",buf);
    return 0;
}
___________________________________________________________________________________________
Code:
#define  BUFFER_SIZE 24
int main(void)
{
    char buf[BUFFER_SIZE];
    printf("Enter the data = ");
    if (fgets(buf, sizeof(buf), stdin) == NULL)
    {
        printf("Fail to read the input stream");
    }
    else
    {
        buf[strcspn(buf, "\n")] = '\0';
    }
    printf("Entered Data = %s\n",buf);
    return 0;
}
_____________________________________________________________________________________________
Code:
#define  BUFFER_SIZE 24
int main(void)
{
    char buf[BUFFER_SIZE];
    printf("Enter the data = ");
    if (fgets(buf, sizeof(buf), stdin) == NULL)
    {
        printf("Fail to read the input stream");
    }
    else
    {
        strtok(buf,"\n");
    }
    printf("Entered Data = %s\n",buf);
    return 0;
}
_______________________________________________________________________________________
Code: 
#define  BUFFER_SIZE 24
void removeNewLineChar(char *ptr)
{
    while((ptr != NULL) && (*ptr != '\n'))
    {
        ++ptr;
    }
    *ptr = '\0';
}
int main(void)
{
    char buf[BUFFER_SIZE];
    printf("Enter the data = ");
    if (fgets(buf, sizeof(buf), stdin) == NULL)
    {
        printf("Fail to read the input stream");
    }
    else
    {
      removeNewLineChar(buf);
    }
    printf("Entered Data = %s\n",buf);
    return 0;
}
_________________________________________________________________________________________
URL: https://www.geeksforgeeks.org/program-to-replace-every-space-in-a-string-with-hyphen/
Author: Geeks-for-Geeks
Code:
// C++ program for the above approach
#include <iostream>
#include <string>
 
using namespace std;
 
string printHyphen(string str)
{
 
  // Split by space and converting
  // String to list and
  for(int i=0; i<str.length(); i++)
  {
    if(str[i] == ' ')
      str[i] = '-';
  }
  return str;
}
 
// Driver code
int main()
{
  string str = "Text contains malayalam and level words";
  cout << printHyphen(str);
  return 0;
}
______________________________________________________________________________________
URL that was good to look over multiple different examples for replacing chars:
https://stackoverflow.com/questions/779875/what-function-is-to-replace-a-substring-from-a-string-in-c
__________________________________________________________________________________________________________
