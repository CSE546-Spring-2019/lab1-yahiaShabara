#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
//#include <unistd.h>
#define MAX_READ_SIZE 100


struct binbin_result
{
   char *matchPtr;
   int matchOffset;
};



bool cmpBin(char *w1 , char *w2, int len);
void binbin(char* haystack, const size_t len_haystack, char* needle, struct binbin_result* binbin_resultPtr);
void Usage(int argc, char *filename);
void count(char *filename, char *searchString, char *filename_out);
//struct binWordMatch_result* binWordMatch(char* haystack, size_t len_haystack, char* needle, size_t len_needle);
void printBin(char* binToPrint, size_t len);

int main(int argc, char *argv[])
{
    if (argc > 4 || argc < 4){
        Usage(argc, argv[0]);
        exit(1);
    }

    /* the count function will return its results in the int array 'results'
       where result[0] is the file size, and result[1] is the <search-string>
       number of appearances (frequency) in <input-filename> */
    //long int result[2];

    count(argv[1] ,argv[2], argv[3]);

//    printf("Size of file is %li\n", result[0]);
//    printf("Number of matches = %li", result[1]);
/*    char s1[MAX_READ_SIZE];
    char *s2;

    s2 = argv[2];

    printf("s2 = %s\n", s2);

    FILE * fp;
    fp = fopen("hi.txt", "rb");
    if (fp == NULL){
        perror("Error opening file");
        printf("Error number = %d\n", errno);
        exit(1);
    }
    size_t size_s1;
    size_s1 = fread(s1 , sizeof(s1[0]), MAX_READ_SIZE, fp);
    printf("size of read characters = %d\n", size_s1);
    int i;
    for(i = 0; i<MAX_READ_SIZE; i++){
        printf("%c",s1[i]);
    }
    printf("\n");
    bool compare;
    struct binbin_result* matchResult;
    char* strMatchPtr;
    int strMatchOffset;
    // compare = cmpBin(s1,s2,size_s1);
    matchResult = binbin(s1, size_s1, s2);
    strMatchPtr = matchResult->matchPtr;
    strMatchOffset = matchResult->matchOffset;
    if(strMatchPtr!=NULL){
        printf("found!!!!!\n");
    }
    compare = cmpBin(strMatchPtr, s2, strlen(s2));
    printf("comparison result = %d", compare);
*/
    return 0;
}

void Usage(int argc, char *filename){
printf("Expected 3 command line arguments but received %d\n", argc-1);
printf("Usage: %s <input-filename> <search-string> <output-filename>\n", filename);
}



// Function Definition
// Put function description here ...
void count(char *filename, char *searchString, char *filename_out){
long int fileSize = 0;
//long int result;
int frequency = 0;
int searchStringLength = strlen(searchString);
struct binbin_result resValue;
struct binbin_result* res = &resValue;


FILE *fp_read;
FILE *fp_write;


int const readSize = MAX_READ_SIZE;
char readChunk[readSize];

fp_read = fopen(filename, "rb");
if(fp_read == NULL) {
    perror("Error opening file");
    printf("Error number = %d\n", errno);
    exit(1);
}

fseek(fp_read, 0L, SEEK_END);
fileSize = ftell(fp_read);
rewind(fp_read);

long int offset = -1*(searchStringLength - 1);
//printf("Offset = %d\n", offset);


char *strMatchPtr;
size_t temp_size;
char *temp;
int strMatchOffset;

size_t readChunk_size;
//printf("SEEK_CURR = %d\n", ftell(fp_read));
readChunk_size = fread(readChunk, sizeof(readChunk[0]), readSize, fp_read);
//printf("readChunk size = %d\n", readChunk_size);
//printf("After first read SEEK_CURR = %d\n", ftell(fp_read));

do{
    //length_readChunk = strlen(readChunk);
    //printf("readChunk is: %s\n", readChunk);
    temp = readChunk;
    temp_size = readChunk_size;
    //strMatchPtr = strstr(temp, searchString);
    binbin(temp, temp_size, searchString, res);
    strMatchPtr = res->matchPtr;
    strMatchOffset = res->matchOffset;
    while(strMatchPtr){
        frequency = frequency + 1;
//        printf("-----------------\n");
//        printf("Frequency: %d\n", frequency);
//        printf("-----------------\n");
        temp = &strMatchPtr[0] + 1;
        temp_size = temp_size - strMatchOffset - 1;
        binbin(temp, temp_size, searchString, res);
        strMatchPtr = res->matchPtr;
        strMatchOffset = res->matchOffset;
    }

     //if (!feof(fp) && readChunk[length_readChunk-1]!= '\n'){
     //   fseek(fp, offset, SEEK_CUR);
    //}
    //printf("SEEK_CURR = %d\n", ftell(fp_read));
    fseek(fp_read, offset, SEEK_CUR);
    //printf("After fseek(), SEEK_CURR = %d\n", ftell(fp_read));
    readChunk_size = fread(readChunk, sizeof(readChunk[0]), readSize, fp_read);
}while(!feof(fp_read));

if(feof(fp_read) && readChunk_size>0){
    temp = readChunk;
    temp_size = readChunk_size;
    binbin(temp, temp_size, searchString, res);
    strMatchPtr = res->matchPtr;
    strMatchOffset = res->matchOffset;
    while(strMatchPtr){
        frequency = frequency + 1;
//        printf("-----------------\n");
//        printf("Frequency: %d\n", frequency);
//        printf("-----------------\n");
        temp = &strMatchPtr[0] + 1;
        temp_size = temp_size - strMatchOffset - 1;
        binbin(temp, temp_size, searchString, res);
        strMatchPtr = res->matchPtr;
        strMatchOffset = res->matchOffset;
    }
}


//result[0] = fileSize;
//result[1] = (long int) frequency;

printf("Size of file is %li\n", fileSize);
printf("Number of matches = %d\n", frequency);

if( access( filename_out, F_OK ) != -1 ) {
    // file exists
    printf("Output File already exists. It will be overwritten!!\n");
} else {
    // file doesn't exist
}
fp_write = fopen(filename_out, "w");
fprintf(fp_write, "Size of file is %li\nNumber of matches = %d", fileSize, frequency);

fclose(fp_read);
fclose(fp_write);

}


/*
struct binWordMatch_result* binWordMatch(char* haystack, size_t len_haystack, char* needle, size_t len_needle){
    struct binWordMatch_result bwm_result;
    if(len_haystack < len_needle){
        bwm_result.matchPtr = NULL;
        return(&bwm_result);
    }
    int scan_num = 0;
    int matchLevel = 0;
    int needle_index = 0;
    while(scan_num<=len_haystack-len_needle)
        {
        for(needle_index = 0; needle_index<len_needle; needle_index++)
            {
            if(haystack[scan_num+needle_index]	==	(needle[needle_index] &	0xff))
                matchLevel += 1;
            else
                {
                matchLevel = 0;
                scan_num += 1;
                break;
                }
            }
            if (len_needle == matchLevel)
                bwm_result.matchPtr = haystack + scan_num;
                bwm_result.matchOffset = scan_num;
                return(&bwm_result);
        }
}
*/





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
//    printf("====>>haystack: ");
//    printBin(haystack, len_haystack);
//    printf("Length of haystack is: %d\n", len_haystack);
//    printf("needle: ");
//    printBin(needle, len_needle);
//    printf("Length of needle is: %d\n", len_needle);
    if (len_haystack<len_needle){
        binbin_resultPtr->matchPtr = NULL;
        return;
    }

    int scanPos = 0;
    char *temp;
    temp = &haystack[scanPos];
    bool compare = cmpBin(temp, needle, len_needle);
//    printBin(temp, len_needle);
//    printf("compare result = %d\n",compare);
    while(!compare){
        scanPos+=1;
        if(scanPos> len_haystack - len_needle){
            binbin_resultPtr->matchPtr = NULL;
            return;
        }
        temp = &haystack[scanPos];
        compare = cmpBin(temp, needle, len_needle);
//        printBin(temp, len_needle);
//        printf("compare result = %d\n",compare);
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
