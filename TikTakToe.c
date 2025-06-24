#include <stdio.h>
#include <string.h>

int winner = 0;
char player_1[];
char player_2[];
static int ground[3][3];
int player1 = 1;
int player2 = 2;
char* gameView;

void choose_name(){
    printf("Gamer-Tag Player 1: \n");
    fgets(player_1, 12, stdin);
    if(strcmp(&player_1[0], " ")) {
        printf("Fehler beim eingeben des Namens von Player 1.\n(mind. 12 Zeichen!)");
    }
    printf("------\n");
    printf("Gamer-Tag Player 1: \n");
    fgets(player_2, 12, stdin);
    if(strcmp(&player_2[0], " ")) {
        printf("Fehler beim eingeben des Namens von Player 2.\n(mind. 12 Zeichen!)");
    }
}

void init_ground(){
    for(int i = 0; i < 3; i++){
        for(int m = 0; m < 3; m++){
            ground[i][m] = 0; 
        }
    }
}
// player1 : return 1
// player1 : return 2
// no winner : return 0
int win(){
    // top line
    if(ground[0][0] == 1 && ground[0][1] == 1 && ground[0][2] == 1){
        return 1;
    }else if(ground[0][0] == 2 && ground[0][1] == 2 && ground[0][2] == 2){
        return 2;
    }
    // mid line
    if(ground[1][0] == 1 && ground[1][1] == 1 && ground[1][2] == 1){
        return 1;
    }else if(ground[1][0] == 2 && ground[1][1] == 2 && ground[1][2] == 2){
        return 2;
    }
    // bottom line
    if(ground[2][0] == 1 && ground[2][1] == 1 && ground[2][2] == 1){
        return 1;
    }else if(ground[2][0] == 2 && ground[2][1] == 2 && ground[2][2] == 2){
        return 2;
    }
    // left column
    if(ground[0][0] == 1 && ground[1][0] == 1 && ground[2][0] == 1){
        return 1;
    }else if(ground[0][0] == 2 && ground[1][0] == 2 && ground[2][0] == 2){
        return 2;
    }
    // mid column
    if(ground[0][1] == 1 && ground[1][1] == 1 && ground[2][1] == 1){
        return 1;
    }else if(ground[0][1] == 2 && ground[1][1] == 2 && ground[2][1] == 2){
        return 2;
    }
    // right column
    if(ground[0][2] == 1 && ground[1][2] == 1 && ground[2][2] == 1){
        return 1;
    }else if(ground[0][2] == 2 && ground[1][2] == 2 && ground[2][2] == 2){
        return 2;
    }
    // vertical left to right
    if(ground[0][0] == 1 && ground[1][1] == 1 && ground[2][2] == 1){
        return 1;
    }else if(ground[0][0] == 2 && ground[1][1] == 2 && ground[2][2] == 2){
        return 2;
    }
    // vertical right to left
    if(ground[0][2] == 1 && ground[1][1] == 1 && ground[2][0] == 1){
        return 1;
    }else if(ground[0][2] == 2 && ground[1][1] == 2 && ground[2][0] == 2){
        return 2;
    // no winner
    }else {
        return 0;
    }
}

int move(int i, int player){
    int row = (i-1) / 3;
    int column = (i-1) % 3;
    if(ground[row][column] != 0){
        printf("Dises Feld ist schon gesetzt!");
        return -1;      
    }
}

void toString(){
    int index_line = 0;
    for(int i = 1; i <= 5; i++){
        if(i % 2 != 0){
            index_line ++;
        }
        for(int m = 1; m <= 3; m++){
            if(i % 2 != 0 && (m == (1 || 2)) ){
                gameView += ground[index_line][m-1]+'|';
            }else if (i % 2 != 0 && (m == 3)) {
                gameView +=  ground[index_line][m-1]+'|';
            }else if (i % 2 == 0) {
                gameView += '_' ;
            }
        }
        gameView += '\n';
    }
}





