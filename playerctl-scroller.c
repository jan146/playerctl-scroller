#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <ctype.h>
#include <unistd.h>

float delay = 0.1;
int update = 0;
int len = 25;
int forceRotate = 0;
char* separator = NULL;
char* statusCommand = NULL;

char* full = NULL;
int offset = 0;
const int maxLength = 1000;

void removeNL(char* arr){
    if (arr[strlen(arr)-1] == '\n')
        arr[strlen(arr)-1] = '\0';
}

char* getStdout(char *command){

    FILE* fp;
    char path[100];

    fp = popen(command, "r");

    if (fp == NULL){
        fprintf(stderr, "Command did not run properly\n");
        exit(1);
    }

    if (fgets(path, sizeof(path), fp) != NULL){
        char* ret = malloc(strlen(path) + 1);
        strcpy(ret, path);
        removeNL(ret);
        return (ret);
    }

    pclose(fp);

}

void printHelp(){
    printf(
        "Usage: cscroll [OPTIONS] [TEXT]\n\
        OPTIONS:\n\
            -h:         Display this page.\n\
            --help      \n\
            -d:         Delay in seconds\n\
            --delay     example: cscroll -d 0.5 [TEXT]\n\
                        this will rotate the text every\n\
                        half of a second (500 ms).\n\
                        Default: 0.1 (100 ms).\n\
            -l:         The maximum length of [TEXT]\n\
            --length    i. e. if the number of characters\n\
                        (in bytes) is equal or greater than\n\
                        this, it will rotate.\n\
                        Default: 25 characters.\n\
            -f:         Force the text to rotate,\n\
            --force     even if it is not too long\n\
                        (longer than -l argument).\n\
                        Default: off.\n\
            -c:         Place your desired command in double\n\
            --command   quotes (\"\"), after the -c argument\n\
                        to add it to [TEXT].\n\
            -u:         The commands will be updated every\n\
            --update    n-th update (set by delay\n\
                        parameter, n should be passed\n\
                        after \"-u\").\n\
                        Default: off (0)\n\
            -s:         If the text should rotate,\n\
            --separator this string will be appended\n\
                        to the end of the text.\n\
            -t:         Set command to get current\n\
            --status    playback status.\n\
        \n\
        TEXT:\n\
            Add either strings or commands (-c) to the text\n\
            that you want to display.\n\
            Both have to be encapsulated by double quotes (\"\").\n\
        \n\
        Example (stupid, but probably necessary):\n\
        cscroll -d 0.8 -l 15 -f \"Today is \" -c \"date +%%A\" \".\"\n\
    \n");
}

void invalidArgs(char* reason){
    fprintf(stderr,
        "Invalid usage of cscroll: %s\
Run \"cscroll -h\" for help.\n\
    \n", reason);
    exit(1);
}

void setDelay(char* d){
    if (atof(d) <= 0){
        char* reason = (char*) malloc(30 * sizeof(char) + sizeof(d));
        sprintf(reason, "invalid delay parameter \"%s\"\n", d);
        invalidArgs(reason);
    }
    delay = atof(d);
}

void setLength(char* l){
    if (atoi(l) <= 0){
        char* reason = (char*) malloc(30 * sizeof(char) + sizeof(l));
        sprintf(reason, "invalid length parameter \"%s\"\n", l);
        invalidArgs(reason);
    }
    len = atoi(l);
}

void setUpdate(char* u){
    if (atoi(u) < 0){
        char* reason = (char*) malloc(30 * sizeof(char) + sizeof(u));
        sprintf(reason, "invalid update parameter \"%s\"\n", u);
        invalidArgs(reason);
    }
    update = atoi(u);
}

int validChar(char c){
    return isprint(c) || (c >= 0 && c <= 31);
}

int invalidCharsBefore(char* c){
    int counter = 0;
    c--;
    while (!validChar(*c)){
        counter++;
        c--;
    }
    return counter;
}

int invalidCharsAfter(char* c){
    int counter = 0;
    c++;
    while (!validChar(*c)){
        counter++;
        c++;
    }
    return counter;
}

void printArgs(int argc, char* argv[]){
    printf("%d: ", argc);
    for (int i = 0; i < argc; i++)
        printf("[%s] ", argv[i]);
    printf("\n");
    printf("Delay: %f\n", delay);
    printf("Length: %d\n", len);
    printf("Force: %d\n", forceRotate);
    printf("Update: %d\n", update);
}

void parseArgs(int argc, char* argv[]){

    for (int i = 1; i < argc; i++){
        if (argv[i][0] == '-' && argv[i][1] == '-'){
            char* str = argv[i] + 2;
            if (strcmp(str, "help") == 0)
                printHelp();
            else if (strcmp(str, "delay") == 0)
                setDelay(argv[++i]);
            else if (strcmp(str, "length") == 0)
                setLength(argv[++i]);
            else if (strcmp(str, "force") == 0)
                forceRotate = 1;
            else if (strcmp(str, "update") == 0)
                setUpdate(argv[++i]);
            else if (strcmp(str, "separator") == 0)
                setUpdate(argv[++i]);
            else if (strcmp(str, "status") == 0)
                statusCommand = argv[++i];
            else if (strcmp(str, "command") == 0)
                strcat(full, getStdout(argv[++i]));
            else{
                char* reason = (char*) malloc(30 * sizeof(char) + sizeof(str));
                sprintf(reason, "invalid argument \"--%s\"\n", str);
                invalidArgs(reason);
            }
            continue;
        }
        else if (argv[i][0] == '-'){
            switch (argv[i][1]){
                case 'h':   printHelp();
                            break;
                case 'd':   setDelay(argv[++i]);
                            break;
                case 'l':   setLength(argv[++i]);
                            break;
                case 'f':   forceRotate = 1;
                            break;
                case 'u':   setUpdate(argv[++i]);
                            break;
                case 's':   separator = argv[++i];
                            break;
                case 't':   statusCommand = argv[++i];
                            break;
                case 'c':   strcat(full, getStdout(argv[++i]));
                            break;
                default:{
                    char* reason = (char*) malloc(30 * sizeof(char) + sizeof(argv[i]));
                    sprintf(reason, "invalid argument \"%s\"\n", argv[i]);
                    invalidArgs(reason);
                }
            }
        }
        else
            strcat(full, argv[i]);
    }

    if (strlen(full) > len && separator != NULL)
        strcat(full, separator);

}

void updateArgs(int argc, char* argv[]){

    char* temp = (char*) malloc(maxLength);
    for (int i = 1; i < argc; i++){
        if (argv[i][0] == '-' && argv[i][1] == '-'){
            char* str = argv[i] + 2;
            if (strcmp(str, "delay") == 0)
                i++;
            else if (strcmp(str, "length") == 0)
                i++;
            else if (strcmp(str, "update") == 0)
                i++;
            else if (strcmp(str, "separator") == 0)
                i++;
            else if (strcmp(str, "status") == 0)
                i++;
            else if (strcmp(str, "command") == 0)
                strcat(temp, getStdout(argv[++i]));
            continue;
        }
        else if (argv[i][0] == '-'){
            switch (argv[i][1]){
                case 'd':
                case 'l':
                case 'u':
                case 's':
                case 't':   i++;
                            break;
                case 'c':   strcat(temp, getStdout(argv[++i]));
                            break;
                default:    break;
            }
        }
        else
            strcat(temp, argv[i]);
    }

    if (strlen(temp) > len && separator != NULL)
        strcat(temp, separator);

    if (strcmp(full, temp) != 0){
        full = temp;
        offset = 0;
    }

}

void rotateText(){

    for (int i = 0; i < len; i++){
        char* c = full+((offset+i)%strlen(full));
        if (isprint(*c))
            printf("%c", *c);
        else {
            if (invalidCharsBefore(c) == invalidCharsAfter(c)){
                char* first = c - invalidCharsBefore(c);
                char* last = c + invalidCharsAfter(c); 
                for (int j = 0; j < (last - first + 1); j++)
                    printf("%c", *(first+j));
            }
            else
                printf("%c", ' ');
        }
    }

}

int main(int argc, char* argv[]){

    setlocale(LC_ALL, "");
    full = (char*) malloc(maxLength);

    parseArgs(argc, argv);
    // printArgs(argc, argv);
    
    if (full == NULL || strcmp(full, "") == 0)
        return(0);

    while (1){
        
        char* status = getStdout(statusCommand);

        if (strcmp(status, "Playing") == 0){

            if (strlen(full) > len || forceRotate)
                rotateText();
            else
                printf(full);

        }
        else if (strcmp(status, "Paused") == 0){

            if (strlen(full) > len || forceRotate){
                rotateText();
                offset--;
            }
            else
                printf(full);

        }
        else
            printf("No player is running");



        offset++;
        if (offset >= strlen(full))
            offset -= strlen(full);
        if (update > 0 && offset % update == 0)
            updateArgs(argc, argv);
        
        printf("\n");
        fflush(stdout);
        usleep(1000000*delay);

    }
    return(0);

}
