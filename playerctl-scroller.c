#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <ctype.h>
#include <unistd.h>
#include <wchar.h>

float delay = 0.1;
int update = 0;
int originalLen = 25;
int len = 25;
int i3 = 0;
int forceRotate = 0;
char* separator = NULL;
char* pid = NULL;
char* module = NULL;
char* player = NULL;
char* script = NULL;

char* full;
int offset = 0;
const int maxLength = 1000;
const int commandLength = 250;

void removeNL(char* arr){
    if (arr[strlen(arr)-1] == '\n')
        arr[strlen(arr)-1] = '\0';
}

char* getStdout(char* command){

    FILE* fp;
    char path[100];

    fp = popen(command, "r");

    if (fp == NULL){
        // fflush(stdout);
        // fprintf(stderr, "Command did not run properly\n");
        pclose(fp);
        exit(1);
    }

    if (fgets(path, sizeof(path), fp) != NULL){

        char* ret = malloc(strlen(path) + 1);
        strcpy(ret, path);
        pclose(fp);
        removeNL(ret);
        return (ret);
    
    }

}

void printHelp(){
    printf(
        "Usage: cscroll [OPTIONS] [TEXT]\n\
        OPTIONS:\n\
            -h:         Display this page.\n\
            --help      \n\
            \n\
            -d [seconds]:\n\
            --delay     Delay in seconds\n\
                        example: cscroll -d 0.5 [TEXT]\n\
                        this will rotate the text every\n\
                        half of a second (500 ms).\n\
                        Default: 0.1 (100 ms).\n\
                        \n\
            -l [number of characters]:\n\
            --length    The maximum length of [TEXT]\n\
                        i. e. if the number of characters\n\
                        (in bytes) is equal or greater than\n\
                        this, it will rotate.\n\
                        Default: 25 characters.\n\
                        \n\
            -f:         Force the text to rotate,\n\
            --force     even if it is not too long\n\
                        (longer than -l argument).\n\
                        Default: off.\n\
                        \n\
            -c [command]:\n\
            --command   Place your desired command in double\n\
                        quotes (\"\"), after the -c argument\n\
                        to add it to [TEXT].\n\
                        \n\
            -u [interval]:\n\
            --update    The commands will be updated every\n\
                        n-th update (set by delay\n\
                        parameter, n should be passed\n\
                        after \"-u\").\n\
                        Default: off (0)\n\
                        \n\
            -s [string]:\n\
            --separator If the text should rotate,\n\
                        this string will be appended\n\
                        to the end of the text.\n\
                        \n\
            -p [player]:\n\
            --player    Set player to get current\n\
                        playback status from.\n\
                        \n\
            -i [pid]:\n\
            --pid       Set the pid of the polybar module.\n\
                        \n\
            -m [module]:\n\
            --module    Set the name of the polybar module.\n\
                        \n\
            -3 [space]: \n\
            --i3        Set the number of spaces an i3\n\
                        workspace element occupies.\n\
                        Default: off (0)\n\
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
    free(reason);
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
    originalLen = len;
}

void seti3(char* i){
    if (atoi(i) <= 0){
        char* reason = (char*) malloc(30 * sizeof(char) + sizeof(i));
        sprintf(reason, "invalid i3 parameter \"%s\"\n", i);
        invalidArgs(reason);
    }
    i3 = atoi(i);
}

void setUpdate(char* u){
    if (atoi(u) < 0){
        char* reason = (char*) malloc(30 * sizeof(char) + sizeof(u));
        sprintf(reason, "invalid update parameter \"%s\"\n", u);
        invalidArgs(reason);
    }
    update = atoi(u);
}

void setSeparator(char* s){
    separator = (char*) malloc(strlen(s)+1);
    strcpy(separator, s);
}

void setPlayer(char* p){
    player = (char*) malloc(strlen(p)+1);
    strcpy(player, p);
    
    // playerctld destionation is not
    // initialized automatically
    if (strstr(p, "playerctl") != NULL) {
        char pctl[] = "dbus-send --print-reply \
        --dest=org.mpris.MediaPlayer2.playerctld \
        /org/mpris/MediaPlayer2 \
        org.freedesktop.DBus.Properties.GetAll \
        string:\"org.mpris.MediaPlayer2.Player\" \
        &> /dev/null";
        system(pctl);
    }

}

void setPid(char* p){
    pid = (char*) malloc(strlen(p)+1);
    strcpy(pid, p);
}

void setModule(char* m){
    module = (char*) malloc(strlen(m)+1);
    strcpy(module, m);
}

void setScript(char* s){
    script = (char*) malloc(strlen(s)+1);
    strcpy(script, s);
}

void printArgs(int argc, char* argv[]){

    printf("%d: ", argc);
    for (int i = 0; i < argc; i++)
        printf("[%s] ", argv[i]);
    printf("\n");
    printf("Delay: [%f]\n", delay);
    printf("Length: [%d]\n", len);
    printf("i3: [%d]\n", i3);
    printf("Force: [%d]\n", forceRotate);
    printf("Update: [%d]\n", update);
    printf("Separator: [%s]\n", separator);
    printf("Player: [%s]\n", player);
    printf("Pid: [%s]\n", pid);
    printf("Module: [%s]\n", module);

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
            else if (strcmp(str, "i3") == 0)
                seti3(argv[++i]);
            else if (strcmp(str, "force") == 0)
                forceRotate = 1;
            else if (strcmp(str, "update") == 0)
                setUpdate(argv[++i]);
            else if (strcmp(str, "separator") == 0)
                setSeparator(argv[++i]);
            else if (strcmp(str, "player") == 0)
                setPlayer(argv[++i]);
            else if (strcmp(str, "pid") == 0)
                setPid(argv[++i]);
            else if (strcmp(str, "module") == 0)
                setModule(argv[++i]);
            else if (strcmp(str, "script") == 0)
                setScript(argv[++i]);
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
                case '3':   seti3(argv[++i]);
                            break;
                case 'f':   forceRotate = 1;
                            break;
                case 'u':   setUpdate(argv[++i]);
                            break;
                case 's':   setSeparator(argv[++i]);
                            break;
                case 'p':   setPlayer(argv[++i]);
                            break;
                case 'i':   setPid(argv[++i]);
                            break;
                case 'm':   setModule(argv[++i]);
                            break;
                case 'r':   setScript(argv[++i]);
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

    if (pid == NULL)
        setPid(getStdout("pgrep polybar"));

}

void updatei3(){

    char* i3Output = getStdout("i3-msg -t get_workspaces 2> /dev/null | grep -o \"num\" | wc -l");
    int workspaceCount = atoi(i3Output);
    if (workspaceCount <= 0){
        char* reason = (char*) malloc(30 * sizeof(char) + sizeof(i3Output));
        sprintf(reason, "Something went wrong with i3: \"%s\"\n", i3Output);
        invalidArgs(reason);
    }
    len = originalLen - workspaceCount * i3;
    free(i3Output);

}

void updateArgs(int argc, char* argv[], char* dest){

    char* temp = (char*) malloc(maxLength);
    char* updateCommand = (char*) malloc(commandLength);

    strcpy(updateCommand, script);
    strcat(updateCommand, " --update ");
    strcat(updateCommand, dest);
    char* ret = getStdout(updateCommand); 
    strcpy(temp, ret);

    if ((strlen(temp) > len || forceRotate) && separator != NULL)
        strcat(temp, separator);

    if (strcmp(full, temp) != 0){
        free(full);
        full = temp;
        offset = 0;
    }

    free(ret);
    free(updateCommand);

}

void updateButton(int playing, int paused){

    if (strcmp(module, "") == 0)
        return;
    
    if (playing == 0 || paused == 0){

        char* message = (char*) malloc(commandLength);
        strcpy(message, "polybar-msg -p ");
        strcat(message, pid);
        strcat(message, " hook ");
        removeNL(module);
        strcat(message, module);
        strcat(message, (playing == 0) ? " 1" : " 2");
        strcat(message, " &> /dev/null ; exit 0");
        system(message);
        free(message);

    }

}

void rotateText(int dontRotate){

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
    
    for (int i = 0; i < printableChars - shortenLength - ((dontRotate) ? wideCharOffset : 0); i++){
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
    offset += (firstCharWidth - 1);

}

int main(int argc, char* argv[]){

    setlocale(LC_ALL, "");
    full = (char*) malloc(maxLength);
    full[0] = '\0';

    parseArgs(argc, argv);
    // printArgs(argc, argv);
    
    char* statusCommand = (char*) malloc(commandLength);
    strcpy(statusCommand, script);
    strcat(statusCommand, " --status");
    int time = 0;

    while (1){
        
        char* status = (char*) getStdout(statusCommand);
        char* dest;
        
        if (strcmp(status, "OFFLINE") != 0){
            int i = strcspn(status, " ");
            dest = status+i+1;
            status[i] = '\0';
        }
        
        if (time == 0)
            updateArgs(argc, argv, dest);

        int playing = strcmp(status, "Playing");
        int paused = strcmp(status, "Paused");
        int offline = strcmp(status, "OFFLINE");

        if (playing == 0){
            if (forceRotate || strlen(full) > len){
                offset++;
                rotateText(0);
            }
            else
                rotateText(1);
        }
        else if (paused == 0){
            if (strlen(full) > len)
                rotateText(0);
            else
                rotateText(1);
        }
        else
            printf("No player is running");

        time++;
        if (offset >= strlen(full))
            offset -= strlen(full); 

        if (update > 0 && time % update == 0 && offline != 0)
            updateArgs(argc, argv, dest);
        if (i3 > 0)
            updatei3();
        updateButton(playing, paused);

        free(status);
        printf("\n");
        fflush(stdout);
        usleep(1000000*delay);

    }
    
    free(separator);
    free(pid);
    free(module);
    free(player);
    free(script);
    free(statusCommand);
    free(full);
    
    return(0);

}
