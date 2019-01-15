#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
//#include <unistd.h>
#define MAX_READ_SIZE 100



// binbin() takes a pointer to this structure as argument.
// matchPtr is a pointer that locates the starting byte of the first matched string
// If no match is found, it is set to NULL
// matchOffset is an int that indicates the offset from the beginning of the string
// at which the first match is found.
struct binbin_result
{
   char *matchPtr;
   int matchOffset;
};


// cmpBin() compares two binary strings (byte by byte) of length int len bytes
// it return true if they are identical and false otherwise.
bool cmpBin(char *w1 , char *w2, int len);
// binbin() searches for a binary string in a longer binary string
// it sets the members of binbin_result structure as an output
void binbin(char* haystack, const size_t len_haystack, char* needle, struct binbin_result* binbin_resultPtr);
// This prints out the Usage of the program if an incorrect number of arguments is passed.
void Usage(int argc, char *filename);
// most processing happens in this function
void count(char *filename, char *searchString, char *filename_out);
// This function prints out a binary string (byte by byte)
// It is mostly used for debugging purposes.
void printBin(char* binToPrint, size_t len);



int main(int argc, char *argv[])
{
    if (argc > 4 || argc < 4){
        Usage(argc, argv[0]);
        exit(1);
    }

    /* the count function will count how many times <search-string>
       has appeared (aka the frequency) in <input-filename>
       then will print its results to the screen and create a file
       <output-filename> to which the same results are copied */

    count(argv[1] ,argv[2], argv[3]);

    return 0;
}



//*********************
// Function Definitions
//*********************


// Put function description here ...

void Usage(int argc, char *filename){
    printf("Expected 3 command line arguments but received %d\n", argc-1);
    printf("Usage: %s <input-filename> <search-string> <output-filename>\n", filename);
}


void count(char *filename, char *searchString, char *filename_out){
    long int fileSize = 0;
    int frequency = 0;
    int searchStringLength = strlen(searchString);

    struct binbin_result resValue;
    struct binbin_result* res = &resValue;


    FILE *fp_read;
    FILE *fp_write;


    int const readSize = MAX_READ_SIZE;

    // The file is read in chunks of length readSize.
    char readChunk[readSize];

    // opens input file and generates error messages if any
    fp_read = fopen(filename, "rb");
    if(fp_read == NULL) {
        perror("Error opening file");
        printf("Error number = %d\n", errno);
        exit(1);
    }

    // these lines find the size of the file
    //--------------------------------------
    fseek(fp_read, 0L, SEEK_END);
    fileSize = ftell(fp_read);
    rewind(fp_read);
    //--------------------------------------




    // Since the <search-string> can occur in two parts
    // where the first part lies at the end of one chunk
    // and the second part at the beginning of the next chunk
    // I rewind the file position indicator such that it read
    // the last (searchStringLength - 1) bytes of the previous
    // chunk again.
    long int offset = -1*(searchStringLength - 1);

    // pointer to the begnning of the first matched string
    // in the read chunk
    char *strMatchPtr;

    size_t temp_size;
    char *temp;
    int strMatchOffset;

    // fread() returns the number of the bytes it had read in readChunk_size
    size_t readChunk_size;
    readChunk_size = fread(readChunk, sizeof(readChunk[0]), readSize, fp_read);

    do{
        temp = readChunk;
        temp_size = readChunk_size;

        binbin(temp, temp_size, searchString, res);
        strMatchPtr = res->matchPtr;
        strMatchOffset = res->matchOffset;
        while(strMatchPtr){
            frequency = frequency + 1;
            temp = &strMatchPtr[0] + 1; // temp points to the remaining part of the readChunk
            temp_size = temp_size - strMatchOffset - 1;
            binbin(temp, temp_size, searchString, res); // looks for more possible string matches in temp
            strMatchPtr = res->matchPtr;
            strMatchOffset = res->matchOffset;
        }

	// to read the last searchStringLength - 1) bytes of the previous
	// chunk again. (will be the beginning of the new readChunk).
        fseek(fp_read, offset, SEEK_CUR);
        readChunk_size = fread(readChunk, sizeof(readChunk[0]), readSize, fp_read);
    }while(!feof(fp_read));


    // if end of file (FEOF) is reached but there was some part of the file that was read before
    // we got to FEOF, then the next lines repeats what the do{} while(); statement
    // statement does but only once.
    if(feof(fp_read) && readChunk_size>0){
        temp = readChunk;
        temp_size = readChunk_size;
        binbin(temp, temp_size, searchString, res);
        strMatchPtr = res->matchPtr;
        strMatchOffset = res->matchOffset;
        while(strMatchPtr){
            frequency = frequency + 1;
            temp = &strMatchPtr[0] + 1;
            temp_size = temp_size - strMatchOffset - 1;
            binbin(temp, temp_size, searchString, res);
            strMatchPtr = res->matchPtr;
            strMatchOffset = res->matchOffset;
        }
    }


    printf("Size of file is %li\n", fileSize);
    printf("Number of matches = %d\n", frequency);

    if( access( filename_out, F_OK ) != -1 ) {
        // file exists
        printf("Output File already exists. It will be overwritten!!\n");
    } else {
        // file doesn't exist
    }

    fp_write = fopen(filename_out, "w");
    if(fp_write == NULL) {
        perror("Error opening output file");
        printf("Error number = %d\n", errno);
        exit(1);
    }
    fprintf(fp_write, "Size of file is %li\nNumber of matches = %d", fileSize, frequency);

    if(fclose(fp_read) < 0){
        perror("Error closing input file");
        printf("Error number = %d\n", errno);
        exit(1);
    }

    if(fclose(fp_write) < 0){
        perror("Error closing output file");
        printf("Error number = %d\n", errno);
        exit(1);
    }
}




bool cmpBin(char *w1 , char *w2, int len){
int i;
int matchLevel = 0;
for(i = 0; i<len ; i++){
    if(matchLevel!=i)
        return(false);
    if((w1[i] & 0xff) == (w2[i] & 0xff) )
        matchLevel+=1;
}
if (matchLevel == len)
    return (true);
else
    return (false);
}


void binbin(char* haystack, const size_t len_haystack, char* needle, struct binbin_result* binbin_resultPtr){
    const size_t len_needle = strlen(needle);
    if (len_haystack<len_needle){
        binbin_resultPtr->matchPtr = NULL;
        return;
    }

    int scanPos = 0;
    char *temp;
    temp = &haystack[scanPos];
    bool compare = cmpBin(temp, needle, len_needle);
    while(!compare){
        scanPos+=1;
        if(scanPos> len_haystack - len_needle){
            binbin_resultPtr->matchPtr = NULL;
            return;
        }
        temp = &haystack[scanPos];
        compare = cmpBin(temp, needle, len_needle);
    }
    binbin_resultPtr->matchOffset = scanPos;
    binbin_resultPtr->matchPtr = temp;
    return;
}

void printBin(char* binToPrint, size_t len){
    int i;
    for(i = 0; i<len; i++){
        printf("%c",binToPrint[i]);
    }
    printf("\n");
}
