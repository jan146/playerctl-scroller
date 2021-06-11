#include "playerctl-scroller.h"

float delay = 0.1;
int update = 0;
int len = 25;
int forceRotate = 0;
char* separator = NULL;

char* full;
int offset = 0;
int maxLength = 1000;

void removeNL(char* arr){
    if (arr[strlen(arr)-1] == '\n')
        arr[strlen(arr)-1] = '\0';
}

char* getStdout(char* command){

    FILE* fp = popen(command, "r");

    if (fp == NULL){
        // fflush(stdout);
        fprintf(stderr, "Command did not run properly\n");
        pclose(fp);
        exit(1);
    }

    char* ret = (char*) malloc(1001*sizeof(char));
    if (fgets(ret, 1000, fp) != NULL){
        pclose(fp);
        removeNL(ret);
        return ret;
    }

    pclose(fp);

}

void printHelp(){
    printf(
        "Usage: cscroll [OPTIONS] [TEXT]\n\
        OPTIONS:\n\
            -h:         Display this page.\n\
            (--help)    \n\
            -d:         Delay in seconds\n\
            (--delay)   example: cscroll -d 0.5 [TEXT]\n\
                        this will rotate the text every\n\
                        half of a second (500 ms).\n\
                        Default: 0.1 (100 ms).\n\
            -l:         The maximum length of [TEXT]\n\
            (--length)  i. e. if the number of characters\n\
                        (in bytes) is equal or greater than\n\
                        this, it will rotate.\n\
                        Default: 25 characters.\n\
            -f:         Force the text to rotate,\n\
            (--force)   even if it is not too long\n\
                        (longer than -l argument).\n\
                        Default: off.\n\
            -c:         Place your desired command in double\n\
            (--command) quotes (\"\"), after the -c argument\n\
                        to add it to [TEXT].\n\
            -u:         The commands will be updated every\n\
            (--update)  n-th update (set by delay\n\
                        parameter, n should be passed\n\
                        after \"-u\").\n\
                        Default: off (0)\n\
            -s:             If the text should rotate,\n\
            (--separator)   this string will be appended\n\
                            to the end of the text.\n\
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
    originalLen = len;
}

void setUpdate(char* u){
    if (atoi(u) < 0){
        char* reason = (char*) malloc(30 * sizeof(char) + sizeof(u));
        sprintf(reason, "invalid update parameter \"%s\"\n", u);
        invalidArgs(reason);
    }
    update = atoi(u);
}

void invalidArgs(char* reason){
    fprintf(stderr,
        "Invalid usage of cscroll: %s\
Run \"cscroll -h\" for help.\n\
    \n", reason);
    free(reason);
    exit(1);
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
            else if (strcmp(str, "command") == 0)
                strcat(temp, getStdout(argv[++i]));
            continue;
        }
        else if (argv[i][0] == '-'){
            switch (argv[i][1]){
                case 'd':
                case 'l':
                case 'u':
                case 's':   i++;
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
        free(full);
        full = temp;
        offset = 0;
    }

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
                separator = argv[++i];
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

    if ((strlen(full) > len || forceRotate) && separator != NULL)
        strcat(full, separator);

}

void rotateText(int status){

    // 0 paused + long
    // print from offset
    // printableChars - shortenLength

    // 1 playing + dont rotate
    // 1 paused + short
    // just print string
    // printableChars - wideCharOffset

    // 2 playing + rotate
    // print from offset
    // increment offset
    // printableChar - shortenLength

    // skip wide character parts 
    // (bytes after first one)
    // and go to next actual character
    int wideCharOffset = 0;

    // increase offset by first
    // character width for next
    // rotation to start at the
    // actual next character
    int firstCharWidth = 1;

    // count any wide characters
    // as double wide to prevent
    // a wide text output
    int shortenLength = 0;

    // Don't print "len" characters
    // if "full" is shorter than
    // the value of "len"
    int printableChars = (strlen(full) < len) ? strlen(full) : len;
    
    for (int i = 0; i < printableChars - (status == 1 ? wideCharOffset : shortenLength); i++){
        char* ptr = full+((offset+i+wideCharOffset)%strlen(full));
        char c = *ptr;

        if (c >= -62 && c <= -33){
            // 2 byte wide character
            wchar_t wideChar[2];
            char* unicodeString = malloc((2+1) * sizeof(char));
            unicodeString[0] = c;
            unicodeString[1] = *(ptr+1);
            unicodeString[2] = L'\0';

            mbstowcs(wideChar, unicodeString, 2);
            printf("%ls", wideChar);
            
            wideCharOffset += 1;
            if (i == 0)
                firstCharWidth = 2;
            
            int width = wcwidth(*wideChar);
            if (width > 1)
                shortenLength += width - 1;
            free(unicodeString);
        }
        else if (c >= -32 && c <= -17){
            // 3 byte wide character
            wchar_t wideChar[2];
            char* unicodeString = malloc((3+1) * sizeof(char));
            unicodeString[0] = c;
            unicodeString[1] = *(ptr+1);
            unicodeString[2] = *(ptr+2);
            unicodeString[3] = L'\0';

            mbstowcs(wideChar, unicodeString, 2);
            printf("%ls", wideChar);
            
            wideCharOffset += 2;
            if (i == 0)
                firstCharWidth = 3;
            
            int width = wcwidth(*wideChar);
            if (width > 1)
                shortenLength += width - 1;
            free(unicodeString);
        }
        else if (c >= -16 && c <= -12){
            // 4 byte wide character
            wchar_t wideChar[2];
            char* unicodeString = malloc((4+1) * sizeof(char));
            unicodeString[0] = c;
            unicodeString[1] = *(ptr+1);
            unicodeString[2] = *(ptr+2);
            unicodeString[3] = *(ptr+3);
            unicodeString[4] = L'\0';

            mbstowcs(wideChar, unicodeString, 2);
            printf("%ls", wideChar);
            
            wideCharOffset += 3;
            if (i == 0)
                firstCharWidth = 4;
            
            int width = wcwidth(*wideChar);
            if (width > 1)
                shortenLength += width - 1;
            free(unicodeString);
        }
        else // default (1 byte wide)
            printf("%c", c);
    }
    if (status == 2)
        offset += (firstCharWidth - 1);

}

int _main_(int argc, char* argv[]){

    setlocale(LC_ALL, "");
    full = (char*) malloc(maxLength*sizeof(char));

    parseArgs(argc, argv);
    // printArgs(argc, argv);
    
    if (full == NULL || strcmp(full, "") == 0)
        return(0);

    while (1){

        if (strlen(full) > len || forceRotate)
            rotateText(0);
        else
            rotateText(1);

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
