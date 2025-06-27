#include "TikTakToe.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h> // for free
#include <sys/wait.h>


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
    char* preview_board = preview();
    if (preview_board != NULL) { // Immer gut, NULL-Check zu machen
        printf("\n%s", preview_board);
        free(preview_board); // Speicher freigeben
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
            int wait = 1;
            int check;
            printf("%s ist am Zug!\n", player_1);
            while(1){
                check = scanf("%i", &i);
                move(i, 1);
                //Puffer leeren, egal ob scanf erfolgreich war oder nicht
                while ((getchar()) != '\n' && getchar() != EOF);
                if(check == 1){
                    if((i > 1) || (i < 9)){
                        break;
                    }
                }
            }// Wichtig: Leere den Eingabepuffer!
            // Lies und verwerfe alle Zeichen im Puffer bis zum nächsten Newline-Zeichen oder EOF
                rounds++;
        }else {
            int i;
            int wait = 1;
            int check;
            printf("%s ist am Zug!\n", player_2);
            while(1){
                check = scanf("%i", &i);
                move(i, 2);
                //Puffer leeren, egal ob scanf erfolgreich war oder nicht
                while ((getchar()) != '\n' && getchar() != EOF);
                if(check == 1){
                    if((i > 1) || (i < 9)){
                        break;
                    }
                }
            }
                rounds++;
        }
    }
    char* win_board = show();
    printf("\n%s", win_board);
    if(winner == 1){
        printf("~~~ %s hat Gewonnen! ~~~", player_1);
    }else if(winner == 2){
        printf("~~~ %s hat Gewonnen! ~~~", player_2);
    }else {
        printf("Etwas ist schief gelaufen");
    }

    return 0;
}
