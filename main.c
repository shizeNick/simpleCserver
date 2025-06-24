#include "TikTakToe.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h> // for free


char* show();
void choose_name();
void init_ground();
int win();
int move(int i, int player); // Der Prototyp in deinem Kommentar war 'int move(int i)'. Ergänzt.

int main(){
    
    choose_name();
    init_ground();
    char* board = show();
    if (board != NULL) { // Immer gut, NULL-Check zu machen
        printf("\n%s", board);
        free(board); // Speicher freigeben
    }
    printf("\n%s", board);
    
    return 0;
}
