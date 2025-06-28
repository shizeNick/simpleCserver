#include "TikTakToe.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h> // for free
#include <sys/wait.h>

void choose_name(int playerNumber);
char* show();
char* preview();
void init_ground();
int win();
int move(int i, int player); // Der Prototyp in deinem Kommentar war 'int move(int i)'. Ergänzt.

void update_tty(){
    //  \033[H --> set cursor top left
    //  \033[2J --> clear screen
    printf("\033[H\033[2J");
    fflush(stdout); // Eingabe puffer sofort leeren, also ohne buffer quasi direkt ans tty
}

int main(){
    
    // Deaktiviere die Pufferung für stdout, wie fflush nur dass es direkt verarbeitet wird uns somit zeit hat am anfang ausgeführt zu werden
    setvbuf(stdout, NULL, _IONBF, 0);
    printf("\033[H\033[2J"); // direkt am anfang damit genug zeit bleibt bis es weiter zur schleife geht

    choose_name(1);
    choose_name(2);

    init_ground();
    int rounds = 1; 
    int winner;



    while((winner = win()) == 0) {
        
        // preview board for help
        char* preview_board = preview();
        if(preview_board != NULL){
            printf("\n%s", preview_board);
            free(preview_board); // Speicher freigeben
        }
        // update play board
        char* board = show();
        if (board != NULL) { // Immer gut, NULL-Check zu machen
            printf("\n\n%s", board);
            free(board); // Speicher freigeben
        }

        printf("\n-------------\n");

        if(rounds % 2 != 0){
            int i;
            int check;
            int outcome;

            printf("%s ist am Zug!\n:", player_1);
            while(1){
               
                check = scanf("%i", &i);
                //Puffer leeren, egal ob scanf erfolgreich war oder nicht
                int c;
                while ((c = getchar()) != '\n' && c != EOF);

                if(check == 1){
                   if((i >= 1) && (i <= 9)) {
                     outcome = move(i, 1);
                     if(outcome == 1){
                         rounds++;
                         break;
                     }
                   }
                }
            }// Wichtig: Leere den Eingabepuffer!
            // Lies und verwerfe alle Zeichen im Puffer bis zum nächsten Newline-Zeichen oder EOF
        }else {
            int i;
            int check;
            int outcome;
            printf("%s ist am Zug!\n:", player_2);
            
            while(1){
               
                check = scanf("%i", &i);
                //Puffer leeren, egal ob scanf erfolgreich war oder nicht
                int c;
                while ((c = getchar()) != '\n' && c != EOF);

                if(check == 1){
                   if((i >= 1) && (i <= 9)) {
                     outcome = move(i, 2);
                     if(outcome == 1){
                         rounds++;
                         break;
                     }
                   }
                }
            }// Wichtig: Leere den Eingabepuffer!
        }
        // Terminal clearen
        update_tty();
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
