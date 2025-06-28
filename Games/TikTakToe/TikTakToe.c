#include "TikTakToe.h"
#include <stdio.h>
#include <stdlib.h> // for maloc und free
#include <string.h>
#include <strings.h>

char player_1[20];
char player_2[20];
static char ground[3][3];
const int player1 = 1;
const int player2 = 2;
int pNumber;

void choose_name(int playerNumber) { // Umbenennung zu playerNumber für Klarheit
    char *playerName;
    size_t bufferSize;

    // Bestimme, welchen Spielernamen wir bearbeiten
    if (playerNumber == 1) {
        playerName = player_1;
        bufferSize = sizeof(player_1);
        printf("Gamer-Tag Player 1 (max %zu Zeichen): ", bufferSize - 1);
    } else if (playerNumber == 2) {
        playerName = player_2;
        bufferSize = sizeof(player_2);
        printf("Gamer-Tag Player 2 (max %zu Zeichen): ", bufferSize - 1);
    } else {
        printf("Fehler: Ungültige Spielernummer bei der Namen-Initialisierung.\n");
        return; // Funktion beenden bei Fehler
    }

    // Schleife, bis eine gültige Eingabe erfolgt
    while (1) {
        // Lese die Eingabe
        if (fgets(playerName, bufferSize, stdin) == NULL) {
            // Fehler beim Lesen der Eingabe (z.B. EOF)
            printf("Fehler beim Lesen der Eingabe.\n");
            return; // Funktion beenden bei schwerwiegendem Fehler
        }

        // Überprüfe, ob ein Zeilenumbruch gelesen wurde (d.h. die Eingabe war nicht zu lang)
        if (playerName[bufferSize - 1] != '\n' && playerName[bufferSize - 1] != '\0') {
            // Der Puffer war nicht groß genug für die gesamte Zeile
            printf("WOW, stop there Bro. Du hast zu viele Zeichen eingegeben.\n");

            // Puffer leeren, um Überlauf zu vermeiden
            int c;
            int overflow = 0;
            while ((c = fgetc(stdin)) != '\n' && c != EOF) {
                overflow++;
            }
            if (overflow > 0) {
                 printf("Du hast %i Zeichen zu viel geschrieben.\n", overflow);
            }
            // Schleife läuft weiter, um erneute Eingabe anzufordern
        } else {
            // Eingabe war im Limit, entferne den Zeilenumbruch, falls vorhanden
            // (oder ersetze ihn mit '\0' für Stringende)
            // strcspn findet die Länge des Strings vor dem ersten '\n'
            playerName[strlen(playerName)-1] = '\0';
            break; // Gültige Eingabe, Schleife verlassen
        }
    }
}

void init_ground(){
    for(int i = 0; i < 3; i++){
        for(int m = 0; m < 3; m++){
            ground[i][m] = ' '; 
        }
    }
}

// player1 : return 1
// player1 : return 2
// no winner : return 0
int win(){
    // top line
    if(ground[0][0] == 'x' && ground[0][1] == 'x' && ground[0][2] == 'x'){
        return 1;
    }else if(ground[0][0] == 'o' && ground[0][1] == 'o' && ground[0][2] == 'o'){
        return 2;
    }
    // mid line
    if(ground[1][0] == 'x' && ground[1][1] == 'x' && ground[1][2] == 'x'){
        return 1;
    }else if(ground[1][0] == 'o' && ground[1][1] == 'o' && ground[1][2] == 'o'){
        return 2;
    }
    // bottom line
    if(ground[2][0] == 'x' && ground[2][1] == 'x' && ground[2][2] == 'x'){
        return 1;
    }else if(ground[2][0] == 'o' && ground[2][1] == 'o' && ground[2][2] == 'o'){
        return 2;
    }
    // left column
    if(ground[0][0] == 'x' && ground[1][0] == 'x' && ground[2][0] == 'x'){
        return 1;
    }else if(ground[0][0] == 'o' && ground[1][0] == 'o' && ground[2][0] == 'o'){
        return 2;
    }
    // mid column
    if(ground[0][1] == 'x' && ground[1][1] == 'x' && ground[2][1] == 'x'){
        return 1;
    }else if(ground[0][1] == 'o' && ground[1][1] == 'o' && ground[2][1] == 'o'){
        return 2;
    }
    // right column
    if(ground[0][2] == 'x' && ground[1][2] == 'x' && ground[2][2] == 'x'){
        return 1;
    }else if(ground[0][2] == 'o' && ground[1][2] == 'o' && ground[2][2] == 'o'){
        return 2;
    }
    // vertical left to right
    if(ground[0][0] == 'x' && ground[1][1] == 'x' && ground[2][2] == 'x'){
        return 1;
    }else if(ground[0][0] == 'o' && ground[1][1] == 'o' && ground[2][2] == 'o'){
        return 2;
    }
    // vertical right to left
    if(ground[0][2] == 'x' && ground[1][1] == 'x' && ground[2][0] == 'x'){
        return 1;
    }else if(ground[0][2] == 'o' && ground[1][1] == 'o' && ground[2][0] == 'o'){
        return 2;
    // no winner
    }else {
        return 0;
    }
}

int move(int i, int player){
    // i : blocks 1 - 9
    int row = (i-1) / 3;
    int column = (i-1) % 3;
    if ((i < 1) || (i > 9)) {
        printf("Bitte geben sie einen gültigen Wert an!");
        return -1;
    }else if(ground[row][column] != ' '){
        printf("Dises Feld ist schon gesetzt!\n:");
        return -1;      
    }else if (player == 1) {
        ground[row][column] = 'x';
        return 1;
    }else if (player == 2) {
        ground[row][column] = 'o';
        return 1;
    }else {
        printf("Etwas ist schief gelaufen...");
        return -1;
    }
}

char* show(){

    int max_len = 50;
    char* gameView = (char*)malloc(max_len * sizeof(char)); //Alloziere den Speicher mit malloc(). 
                                                            //Der Aufrufer der Funktion ist dann dafür verantwortlich, den Speicher mit free() freizugeben
    gameView[0] = '\0'; // String deklarieren
    char* current_pos = gameView; // ein hilfszieger um zu navigieren
    int index_line = 0;
    for(int i = 1; i <= 5; i++){
        if(i % 2 != 0){
            index_line = (i-1) / 2;
        }
        for(int m = 1; m <= 3; m++){
           if(i % 2 != 0){ // Wenn es eine Spielfeld-Zeile ist
                if(m == 1 || m == 2){
                    current_pos += sprintf(current_pos, " %c |", ground[index_line][m-1]);
                } else { // m == 3
                    current_pos += sprintf(current_pos, " %c",ground[index_line][m-1]);
                }
            } else { // Wenn es eine Trennlinie ist (i % 2 == 0)
                current_pos += sprintf(current_pos, "--- ") ;
            }
        }
        current_pos += sprintf(current_pos, "\n") ;
    }  
    *current_pos = '\0';
    return gameView;
}
char* preview(){

    int max_len = 50;
    char* gameView = (char*)malloc(max_len * sizeof(char)); //Alloziere den Speicher mit malloc(). 
                                                            //Der Aufrufer der Funktion ist dann dafür verantwortlich, den Speicher mit free() freizugeben
    gameView[0] = '\0'; // String deklarieren
    char* current_pos = gameView; // ein hilfszieger um zu navigieren
    int index = 1;
    for(int i = 1; i <= 5; i++){
        for(int m = 1; m <= 3; m++){
           if(i % 2 != 0){ // Wenn es eine Spielfeld-Zeile ist
                if(m == 1 || m == 2){
                    current_pos += sprintf(current_pos, "%i  |", index);
                    index++;
                } else { // m == 3
                    current_pos += sprintf(current_pos, "%i", index);
                    index++;
                }
            } else { // Wenn es eine Trennlinie ist (i % 2 == 0)
                current_pos += sprintf(current_pos, "--- ") ;
            }
        }
        current_pos += sprintf(current_pos, "\n") ;
    }  
    *current_pos = '\0';
    return gameView;
}
