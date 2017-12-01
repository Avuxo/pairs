#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

/* 
   _____      _          
  |  __ \    (_)         
  | |__) |_ _ _ _ __ ___ 
  |  ___/ _` | | '__/ __|
  | |  | (_| | | |  \__ \
  |_|   \__,_|_|_|  |___/
  
  By: Avuxo
  
  Pairs is an esoteric programming language based on creating binary numbers using characters.
  To create a 1 use two of the same character in a sequence -- Ex: 'ff', '$$', or '55'.
  To create a 0 use two different characters in a sequence  -- Ex: 'e#', 'rd', or 'n4'.

  Opcodes and operands are formed in 8-bit sequences (16 characters) and each is separated by a space
  Ex: 0 is HALT, 1 is store <value> <register> .. 10 is add <value> <register>, 11 is add <register(src)> <register(dest)>, etc.
  
  Pairs is something of a reverse-golf language as it produces abnormally large programs (each binary digit requires 2 characters)
  This means that each bit representated takes 16 bits to be represented

*/



/*
  there are 256 general purpose registers in Pairs.
  Given that they can only be indexed up to 255 (maximum number to be expressed in an 8-bit sequence), there is no reason for more.
  There is one extra register for CLASSIFIED reasons
 */
int registers[256];

/*
  There are 256 possible jump locations (thus there is a limit of 255 subroutines
  that can exist at the same time).
  The label index is the label name and the value is the pc value.
*/
int labels[256];

/* read file into buffer */
char *readFile(char *filename){
    char *ret = NULL; /* return value, defaulted to null for error handling*/
    FILE *file = fopen(filename, "r");
    if(file){
        fseek(file, 0, SEEK_END); /* go to the end of the file */
        long fileSize = ftell(file); /* get number of bytes between start and current postition */

        fseek(file, 0, SEEK_SET); /* return to start */
        ret = malloc(sizeof(char) * fileSize + 1); /* allocate enough space for the file */
        fread(ret, fileSize, 1, file); /*read the file into memory*/
        fclose(file); /*close the file read stream*/

        ret[fileSize] = '\0'; /*null terminator for string*/
    }

    return ret;
}

/* convert the given token into an integer */
int convertToInt(char *charStr){
    int ret = 0b00000000; /*the return value*/
    /* for loop for both the current bit in the return value and the current char pair */
    for(int curBit=0, curPair=0; curBit<8 && curPair<16; curBit++, curPair+=2){
        if(charStr[curPair] == charStr[curPair+1]) { /*check if two characters are the same*/
            ret |= 1 << curBit; /*make current bit 1*/
        } 
    }

    return ret;
}

/* convert the given program into tokens to be used by the convertToInt() function */
int *tokenize(char *program){
    /*
      The default size of the program is 1024 (total numbers for the program to be made up of).
      If the program is longer than 1024 tokens then it is resized
     */

    /*conver the const string into a mutable string*/
    
    unsigned int size = 2;
    int *ret = malloc(sizeof(int) * size);
    char *currentToken = strtok(program, " ");

    int numOfTokens = 0;
    while(currentToken != NULL){
        /* make sure there is space in the buffer*/
        if(numOfTokens >= size){
            printf("too many ints (%d) reallocating...\n", size);
            size += 1024; /*request 1024 for new buffer*/
            int *tmp = realloc(ret, size); /* reallocate buffer with new size */
            if(tmp == NULL){ /* check if reallocation was successful */
                printf("ERROR: Could not allocate additional memory\n");
                exit(1);
            }
            printf("Buffer is now %d wide\n", size);
            ret = tmp; /* assign buffer to new buffer */
            
        }

        /*assign into the return value the token converted to an integer*/
        ret[numOfTokens] = convertToInt(currentToken);
        numOfTokens++;
        currentToken = strtok(NULL, " ");
    }

    ret[size] = 0;
    return ret;
    
}

/* The main interpreter of Pairs */
void interpretProgram(int *program){
    int programCounter = 0; /*current instruction*/
    while(program[programCounter] != 0) {
        
        switch(program[programCounter]){
        case 1: /*store <value> <register>*/
            registers[program[programCounter+2]] = program[programCounter+1];
            programCounter+=2;
            break;
        case 2: /*store <register(src)> <register(src)>*/
            registers[program[programCounter+2]] = registers[program[programCounter+1]];
            programCounter+=2;
            break;
        case 10: /*add <value> <register>*/
            registers[program[programCounter+2]] += program[programCounter+1];
            programCounter+=2;
            break;
        case 11: /*add <register(src)> <register(dest)>*/
            registers[program[programCounter+2]] += registers[program[programCounter+1]];
            programCounter+=2;
            break;
        case 20: /*print <register>*/
            printf("%d\n",
                   registers[program[programCounter+1]]);
            programCounter++;
            break;
        case 21: /*print <value as ascii>*/
            printf("%c\n",
                   (unsigned char) program[programCounter+1]);
            programCounter++;
            break;
        case 40: /*labelset <register>*/
            /*label is set after the operand for jumps*/
            labels[program[programCounter+1]] = registers[programCounter+2];
            break;
        case 41: /*labelset <register>*/
            labels[program[programCounter+1]] = programCounter+2;
            break;

        case 42: /*jump <register>*/
            programCounter = labels[registers[program[programCounter+1]]];
            break;
        case 43: /*jump <value>*/
            programCounter = labels[program[programCounter+1]];
            break;
        case 44: /*jumptrue <compare1(val)> <compare2(val)> <register(jmp)>*/
            // TODO
            break;
        default:
            printf("ERROR: %d is not a known opcode (or the previous instruction is malformed)\n",
                   program[programCounter]);
            exit(1);
            
        }
        
        programCounter++; /*get the next instruction*/
    }
}

int main(int argc, char **argv){
    if(argc < 2){
        printf("Usage: pairs <filename>\n");
        exit(1);
    }
    char *file = readFile(argv[1]);
    if(file){
        int *program = tokenize(file); /*the array of instructions*/
        interpretProgram(program); /*main portion of program*/
        free(program); /*free the allocated memory from tokenize()*/
        free(file);
    }
    return 0;
}
