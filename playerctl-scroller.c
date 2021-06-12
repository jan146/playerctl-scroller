#include "playerctl-scroller.h"

int originalLen = 25;
int i3 = 0;
int icons = 0;
int commandLength = 250;

char* pid = NULL;
char* module = NULL;
char* player = NULL;
char* script = NULL;
char* prefix = NULL;

void _printHelp_(){
    printf(
        "Usage: playerctl-scroller [OPTIONS] [TEXT]\n\
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
            -i:         Enable icons in front of text\n\
            --icon      Default: off\n\
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

void seti3(char* i){
    if (atoi(i) <= 0){
        char* reason = (char*) malloc(30 * sizeof(char) + strlen(i));
        sprintf(reason, "invalid i3 parameter \"%s\"\n", i);
        invalidArgs(reason);
    }
    i3 = atoi(i);
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

void _printArgs_(int argc, char* argv[]){

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
    printf("Icons: [%s]\n", icons);

}

void updatei3(){

    char* i3Output = getStdout("i3-msg -t get_workspaces 2> /dev/null | grep -o \"num\" | wc -l");
    int workspaceCount = atoi(i3Output);
    if (workspaceCount <= 0){
        char* reason = (char*) malloc(35 * sizeof(char) + strlen(i3Output));
        sprintf(reason, "Something went wrong with i3: \"%s\"\n", i3Output);
        invalidArgs(reason);
    }
    len = originalLen - workspaceCount * i3;
    free(i3Output);

}

void _updateArgs_(int argc, char* argv[], char* dest){

    char* temp = (char*) malloc(maxLength*sizeof(char));
    char* updateCommand = (char*) malloc(commandLength*sizeof(char));

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

    if (icons)
        updatePrefix();

}

void updateButton(int playing, int paused){

    if (strcmp(module, "") == 0)
        return;
    
    if (playing == 0 || paused == 0){

        char* message = (char*) malloc(commandLength*sizeof(char));
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

void updatePrefix(){

    char* message = (char*) malloc(commandLength*sizeof(char)); 
    strcpy(message, script);
    strcat(message, " --prefix");
    
    if (prefix != NULL)
        free(prefix);
    prefix = getStdout(message);
    free(message);

}

void _parseArgs_(int argc, char* argv[]){

    int newArgCount = 1;
    char** newArgs = malloc(20*sizeof(char*));

    for (int i = 1; i < argc; i++){
        if (argv[i][0] == '-' && argv[i][1] == '-'){
            char* str = argv[i] + 2;
            if (strcmp(str, "help") == 0)
                printHelp();
            else if (strcmp(str, "i3") == 0)
                seti3(argv[++i]);
            else if (strcmp(str, "player") == 0)
                setPlayer(argv[++i]);
            else if (strcmp(str, "pid") == 0)
                setPid(argv[++i]);
            else if (strcmp(str, "module") == 0)
                setModule(argv[++i]);
            else if (strcmp(str, "icon") == 0)
                icons = 1;
            else if (strcmp(str, "script") == 0)
                setScript(argv[++i]);
            else {
                newArgs[newArgCount++] = argv[i++];
                newArgs[newArgCount++] = argv[i];
            }
            continue;
        }
        else if (argv[i][0] == '-'){
            switch (argv[i][1]){
                case 'h':   _printHelp_();
                            break;
                case '3':   seti3(argv[++i]);
                            break;
                case 'p':   setPlayer(argv[++i]);
                            break;
                case 'i':   setPid(argv[++i]);
                            break;
                case 'm':   setModule(argv[++i]);
                            break;
                case 'o':   icons = 1;
                            break;
                case 'r':   setScript(argv[++i]);
                            break;
                default: {
                    newArgs[newArgCount++] = argv[i++];
                    newArgs[newArgCount++] = argv[i];
                }
            }
        }
        else
            strcat(full, argv[i]);
    }

    parseArgs(newArgCount, newArgs);
    free(newArgs);

    //if ((strlen(full) > len || forceRotate) && separator != NULL)
    //    strcat(full, separator);

    if (pid == NULL)
        setPid(getStdout("pgrep polybar"));

}

int main(int argc, char* argv[]){

    setlocale(LC_ALL, "");
    full = (char*) calloc(maxLength, sizeof(char));


    _parseArgs_(argc, argv);
    // _printArgs_(argc, argv);
    
    char* statusCommand = (char*) malloc(commandLength*sizeof(char));
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
            _updateArgs_(argc, argv, dest);

        int playing = strcmp(status, "Playing");
        int paused = strcmp(status, "Paused");
        int offline = strcmp(status, "OFFLINE");

        if (update > 0 && time % update == 0 && offline != 0)
            _updateArgs_(argc, argv, dest);

        if (playing == 0 || paused == 0) {
            if (icons)
                printf("%s ", prefix);
            if (forceRotate || strlen(full) > len) {
                if (playing == 0) {
                    rotateText(2);
                    offset++;
                }
                else
                    rotateText(0);
            }
            else
                rotateText(1);
        }
        else
            printf("No player is running");

        time++;
        if (offset >= strlen(full))
            offset -= strlen(full); 

        if (update > 0 && time % update == 0 && offline != 0)
            _updateArgs_(argc, argv, dest);
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
