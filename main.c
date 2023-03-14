/*
Assignment 5: Multi-threaded Producer Consumer Pipeline
By: Frankie Herbert
*/
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// Constants
#define LINES 50
#define LENGTH 1024
#define LENLINE 50000

// Setup Buffer 1
char buffer1[LINES][LENGTH];
char BUFFline1[LENGTH];
int items_in_buf1 = 0;
int producer_idx_1 = 0;
int consumer_idx_1 = 0;
pthread_mutex_t mutex_1 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full_1 = PTHREAD_COND_INITIALIZER;


// Setup Buffer 2
char buffer2[LINES][LENGTH];
int items_in_buf2 = 0;
int producer_idx_2 = 0;
int consumer_idx_2 = 0;
pthread_mutex_t mutex_2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full_2 = PTHREAD_COND_INITIALIZER;


// Setup Buffer 3
char buffer3[LINES][LENGTH];
char BUFFline3[LENLINE];
char BUFFout[LENLINE];
int items_in_buf3 = 0;
int producer_idx_3 = 0;
int consumer_idx_3 = 0;
pthread_mutex_t mutex_3 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full_3 = PTHREAD_COND_INITIALIZER;


// Put items into Buffer 1
void put_buffer_1(char* line) {
    pthread_mutex_lock(&mutex_1);
    strcpy(buffer1[producer_idx_1], line);
    producer_idx_1++;
    items_in_buf1++;
    pthread_cond_signal(&full_1);
    pthread_mutex_unlock(&mutex_1);
}

// Put items into Buffer 2
void put_buffer_2(char* line) {
    pthread_mutex_lock(&mutex_2);
    strcpy(buffer2[producer_idx_2], line);
    producer_idx_2++;
    items_in_buf2++;
    pthread_cond_signal(&full_2);
    pthread_mutex_unlock(&mutex_2);
}

// Put items into Buffer 3
void put_buffer_3(char* line) {
    pthread_mutex_lock(&mutex_3);
    strcpy(buffer3[producer_idx_3], line);
    producer_idx_3++;
    items_in_buf3++;
    pthread_cond_signal(&full_3);
    pthread_mutex_unlock(&mutex_3);
}

// Get items from Buffer 1
char* get_buffer_1_item() {
    pthread_mutex_lock(&mutex_1);
    while (items_in_buf1 == 0) {
        pthread_cond_wait(&full_1, &mutex_1);
    }
    strcpy(BUFFline1, buffer1[consumer_idx_1]);
    consumer_idx_1++;
    items_in_buf1--;
    pthread_mutex_unlock(&mutex_1);
    return BUFFline1;
}

// Get items from Buffer 2
char* get_buffer_2_item() {
    char* temp = malloc(LENLINE);
    pthread_mutex_lock(&mutex_2);
    while (items_in_buf2 == 0) {
        pthread_cond_wait(&full_2, &mutex_2);
    }
    strcpy(temp, buffer2[consumer_idx_2]);
    consumer_idx_2++;
    items_in_buf2--;
    pthread_mutex_unlock(&mutex_2);
    return temp;
}

// Get items from Buffer 3
char* get_buffer_3_item() {
    pthread_mutex_lock(&mutex_3);
    while (items_in_buf3 == 0) {
        pthread_cond_wait(&full_3, &mutex_3);
    }
    strcpy(BUFFline3, buffer3[consumer_idx_3]);
    consumer_idx_3++;
    items_in_buf3--;
    pthread_mutex_unlock(&mutex_3);
    return BUFFline3;
}

// Get input from input file
char* get_input() {
    size_t size = LENGTH;   // Initialize the size variable
    char* GetBuff = (char*)malloc(size); // Allocate memory for the input
    char** Toinput = &GetBuff;      // Double pointer to the input and set it to the address of the input
    getline(Toinput, &size, stdin);  // Read input from standard input
    return GetBuff;                // Return the input
}

// Process characers STOP
void* InputMulti(void* args) {
    int i;
    char GetBuff[LENGTH];
    for (i = 0; i < LINES; i++) {
        char* Getinput = get_input();  // Get input from user
        strcpy(GetBuff, Getinput);    // Copy the Getinput into GetBuff
        if (!strcmp(Getinput, "STOP\n")) {   // If Getinput is equal to "STOP", add it to the GetBuf and return NULL
            put_buffer_1(Getinput);
            return NULL;
        }

        put_buffer_1(Getinput);   // If not equal to "STOP", add it to the buffer
    }
    return NULL;  // Return NULL
}


void* LineSep(void* args) {
    char* line;
    char* Sepline;
    int i;
    for (i = 0; i < LINES; i++) {
        line = get_buffer_1_item();         // Get an item from buffer 1

        if (!strcmp(line, "STOP\n")) {      // Check if the line is "STOP\n"
            put_buffer_2(line);
            return NULL;
        }
        Sepline = (char*)malloc(strlen(line) + 1);  // Allocate memory for the new line
        if (Sepline == NULL) {
            return NULL;
        }
        int j;
        // Loop through the characters in the line and replace newlines with spaces
        for (j = 0; j < strlen(line); j++) {
            if (line[j] == '\n') {
                Sepline[j] = ' ';
            }
            else {
                Sepline[j] = line[j];
            }
        }
        Sepline[strlen(line)] = '\0';   // Null terminator to the new line
        put_buffer_2(Sepline);          // Add the new line to buffer 2
        free(Sepline);          // Free memory allocated for the new line
    }
    return NULL;     // Return NULL
}

// Process all lines containing the '+' character by replacing '++' with '^'
void* PlusSign(void* args) {
    int i, j;
    char* line;
    size_t len;
    for (i = 0; i < LINES; i++) {
        line = get_buffer_2_item();

        if (!strcmp(line, "STOP\n")) {     // Check if the line is "STOP\n"
            put_buffer_3(line);
            return NULL;
        }
        len = strlen(line);
        for (j = 0; j < len; j++) {      // Loop through each character in the line
            if (line[j] == '\n') {       // Replace new line character with space
                line[j] = ' ';
            }
            // Replace "++" with "^" and move remaining characters to the left
            else if (line[j] == '+' && line[j + 1] == '+') {
                line[j] = '^';
                memmove(line + j + 1, line + j + 2, len - j);
                len--;
            }
        }
        put_buffer_3(line);     // Put line in buffer 2
    }
    return NULL;             // After all lines were processed, return NULL
}

void* OutputMulti(void* args) {
    char BUFFline[82];
    int i;
    char* temp;
    int counter = 0;
    for (i = 0; i < LINES; i++)
    {
        temp = get_buffer_3_item();
        counter += strlen(temp);

        if (!strcmp(temp, "STOP\n"))  // Check if the temp is "STOP\n"
            return NULL;

        strcat(BUFFout, temp);
        if (counter > 79) {   // If counter is greater than 79, proceed 
            int idx = 0;
            while (counter > 79) {
                strncpy(BUFFline, BUFFout + idx, 80);   // First 80 characters are copied into the BUFFline string
                BUFFline[80] = '\n';      // Add newline character 
                printf("%s", BUFFline);   // Print BUFFline string
                idx += 80;        // The idx is incremented by 80
                counter -= 80;   // The counter is decremented by 80
            }
            memmove(BUFFout, BUFFout + idx, counter + 1); // copy any remaining characters
        }
    }
    return NULL;
}

int main() {
    pthread_t InputMulti_t, LineSep_t, PlusSign_t, OutputMulti_t;

    // Create the threads
    pthread_create(&InputMulti_t, NULL, InputMulti, NULL);
    pthread_create(&LineSep_t, NULL, LineSep, NULL);
    pthread_create(&PlusSign_t, NULL, PlusSign, NULL);
    pthread_create(&OutputMulti_t, NULL, OutputMulti, NULL);

    // Wait for the threads to terminate
    pthread_join(InputMulti_t, NULL);
    pthread_join(LineSep_t, NULL);
    pthread_join(PlusSign_t, NULL);
    pthread_join(OutputMulti_t, NULL);
    return EXIT_SUCCESS;
}