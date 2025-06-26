#include "TikTakToe.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h> // for free


char* show();
char* preview();
void choose_name();
void init_ground();
int win();
int move(int i, int player); // Der Prototyp in deinem Kommentar war 'int move(int i)'. Ergänzt.

int main(){
    
    choose_name();
    init_ground();
    // game intro preview
    char* board = preview();
    if (board != NULL) { // Immer gut, NULL-Check zu machen
        printf("\n%s", board);
        free(board); // Speicher freigeben
    }
    
    int rounds = 1; 
    int winner;
    while((winner = win()) == 0) {

        char* board = show();
        if (board != NULL) { // Immer gut, NULL-Check zu machen
            printf("\n%s", board);
            free(board); // Speicher freigeben
        }
        printf("\n-------------\n");
        printf("%s", board);
        if(rounds % 2 != 0){
            int i;
            printf("%s ist am Zug!\n", player_1);
            scanf("%i", &i);
            move(i, 1);
            rounds++;
        }else {
            int i;
            printf("%s ist am Zug!\n", player_2);
            scanf("%i", &i);
            move(i, 2);
            rounds++;
        }
    }
    if(winner == 1){
        printf("~~~ %s hat Gewonnen! ~~~", player_1);
    }else if(winner == 2){
        printf("~~~ %s hat Gewonnen! ~~~", player_2);
    }else {
        printf("Etwas ist schief gelaufen");
    }

    return 0;
}
