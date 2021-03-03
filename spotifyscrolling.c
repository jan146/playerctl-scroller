#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <ctype.h>
#include <unistd.h>

char* getStdout(char *command){

    FILE* fp;
    char path[100];

    /* Open the command for reading. */
    fp = popen(command, "r");

        if (fp == NULL) {
            printf("Failed to run command\n" );
                exit(1);
        }
    
    /* Read the output a line at a time - output it. */
    if (fgets(path, sizeof(path), fp) != NULL) {
        // printf("%s", path);
        // strcpy(artist, path);
        pclose(fp);
        char* ret = malloc(strlen(path) + 1);
        strcpy(ret, path);
        return (ret);
    }

    /* close */
    pclose(fp);

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

int main(int argc, char *argv[]){

    setlocale(LC_ALL, "");

    char* artist = getStdout("playerctl -p spotify metadata -f \"{{ artist }}\"");
    char* title = getStdout("playerctl -p spotify metadata -f \"{{ title }}\"");

    artist[strlen(artist)-1] = '\0';
    title[strlen(title)-1] = '\0';

    char middle[] = "—";
    char end[] = "";
    printf("%d\n", end);
    int requiredLength = strlen(artist)+strlen(title)+strlen(middle)+strlen(end)+1;
    char* full = malloc(requiredLength);

    strcpy(full, artist);
    strcat(full, middle);
    strcat(full, title);
    strcat(full, end);
    int len = 28;

    // printf("%s%s\n", full, ".");
    // printf("middle: %d %d %d [%c%c%c]\n", middle[0], middle[1], middle[2], middle[0], middle[1], middle[2]);
    // printf("end: %d %d %d [%c%c%c]\n", end[0], end[1], end[2], end[0], end[1], end[2]);
    // printf("null: %s %d\n", '\0', '\0');

    char* ptr = full;
    for (int i = 0; 1; i++){
        for (int j = 0; j < len; j++){
            char* c = ptr+(i+j)%strlen(full);
            if (isprint(*c))
                printf("%c", *c);
            else {
                if (invalidCharsBefore(c) == invalidCharsAfter(c)){
                    char* first = c - invalidCharsBefore(c);
                    char* last = c + invalidCharsAfter(c); 
                    char wideChar[last - first + 1];
                    for (int i = 0; i < (last - first + 1); i++){
                        printf("%c", *(first+i));
                        wideChar[i] = *(first+i);
                    }
                }
                else
                    printf("%c", ' ');
            }
        }
        if (i >= strlen(full))  // prevent integer overflow
            i -= strlen(full);
        usleep(1000000*0.1);
        printf("\n");
        fflush(stdout);
    }

}
