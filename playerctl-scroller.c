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

char* getStdout(char *command){

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
            -p:         Set player to get current\n\
            --player    playback status from.\n\
            -i:         Set the pid of the polybar module.\n\
            --pid       \n\
            -m:         Set the name of the polybar module.\n\
            --module    \n\
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

void setSeparator(char* s){
    separator = (char*) malloc(strlen(s)+1);
    separator = s;
}

void setPlayer(char* p){
    player = (char*) malloc(strlen(p)+1);
    player = p;
}

void setPid(char* p){
    pid = (char*) malloc(strlen(p)+1);
    pid = p;
}

void setModule(char* m){
    module = (char*) malloc(strlen(m)+1);
    module = m;
}

void setScript(char* s){
    script = (char*) malloc(strlen(s)+1);
    script = s;
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
    printf("Delay: [%f]\n", delay);
    printf("Length: [%d]\n", len);
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

void updateArgs(int argc, char* argv[], char* dest){

    char* temp = (char*) malloc(maxLength);
    char* updateCommand = (char*) malloc(commandLength);

    strcpy(updateCommand, script);
    strcat(updateCommand, " --update ");
    strcat(updateCommand, dest);
    strcpy(temp, getStdout(updateCommand));

    if ((strlen(temp) > len || forceRotate) && separator != NULL)
        strcat(temp, separator);

    if (strcmp(full, temp) != 0){
        full = temp;
        offset = 0;
    }

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

    }

}

void rotateText(){

    for (int i = 0; i < len; i++){
        char* c = full+((offset+i+strlen(full)-1)%strlen(full));
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
    full[0] = '\0';

    parseArgs(argc, argv);
    // printArgs(argc, argv);
    
    char* statusCommand = (char*) malloc(commandLength);
    strcpy(statusCommand, script);
    strcat(statusCommand, " --status");
    int time = 0;

    while (1){

        char* status = getStdout(statusCommand);
        char* dest = (char*) malloc(commandLength);

        if (strcmp(status, "OFFLINE") != 0){
            int i = strcspn(status, " ");
            dest = status+i+1;
            status[i] = '\0';
        }

        int playing = strcmp(status, "Playing");
        int paused = strcmp(status, "Paused");
        int offline = strcmp(status, "OFFLINE");

        if (playing == 0){

            offset++;
            if (strlen(full) > len || forceRotate)
                rotateText();
            else
                printf(full);

        }
        else if (paused == 0){

            if (strlen(full) > len || forceRotate)
                rotateText();
            else
                printf(full);

        }

        else
            printf("No player is running");

        time++;
        if (offset >= strlen(full))
            offset -= strlen(full);

        if (update > 0 && time % update == 0 && offline != 0)
            updateArgs(argc, argv, dest);

        updateButton(playing, paused);
        
        printf("\n");
        fflush(stdout);
        usleep(1000000*delay);

    }
    return(0);

}
