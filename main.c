#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#define UPDTIME 200000
#define ENDSCORE 324

typedef struct{
    int x;
    int y;
}body_t;

struct termios oldterm;
struct termios newterm;
body_t snake[ENDSCORE];

unsigned char background[20][20] = {0};

void drawbackground(){
    for(int y = 0; y<20; y++){
        for(int x = 0; x<20; x++){
            if(y == 0 || y == 19 || x == 0 || x == 19){
                printf("#");
                continue;
            }
            
            if(background[y][x] == 1)
                printf("@");
            else if(background[y][x] == 2)
                printf("*");

            else printf(" ");
        }
        printf("\n");
    }
}

void spawnfood(int *x, int *y){
    int fruity = (rand() % 18)+1;
    int fruitx = (rand() % 18)+1;

    if(background[fruity][fruitx] == 1){
        spawnfood(x, y);
    }
    else{
        background[fruity][fruitx] = 2;
        *x = fruitx;
        *y = fruity;
    }
}

int isinborder(int x, int y){
    return (y >= 19 || y <= 0 || x >= 19 || x <= 0);
}

int bodycolcheck(int y, int x){
    return (background[y][x] == 1);
}

void writesnake(int snakelen){
    for(int i = 0; i<=snakelen; i++){
        background[snake[i].y][snake[i].x] = 1;
    }
}

void clearsnake(){
    for(int i = 0; i<20; i++){
        for(int j = 0; j<20; j++){
            if(background[i][j] == 1)
                background[i][j] = 0;
        }
    }
}

int movesnake(char keypress, int snakelen){
    int hx = snake[0].x;
    int hy = snake[0].y;
    
    switch(keypress){
        case 'w':
            hy -= 1;
            break;
        case 's':
            hy += 1;
            break;
        case 'a':
            hx -= 1;
            break;
        case 'd':
            hx += 1;
            break;
    }
     for (int i = snakelen; i>0; i--) {
         snake[i].x = snake[i-1].x;
         snake[i].y = snake[i-1].y;
     }

    snake[0].x = hx;
    snake[0].y = hy;

    if(bodycolcheck(hy, hx))
        return 0;
    clearsnake();

    return 1;
}

int main(void){
    int fd = fileno(stdin);
    char key;
    
    snake[0].x = 10;
    snake[0].y = 10;

    int fx, fy;
    
    int score = 0;
    int symbindex = 0;

    tcgetattr(fd, &oldterm);
    newterm = oldterm;
   
    newterm.c_lflag &= ~(ICANON | ECHO); 
    tcsetattr(fd, TCSANOW, &newterm);

    key = 'w';
    srand(time(NULL));
    spawnfood(&fx, &fy);

    while(1){
        system("clear");
        
        if(score >= ENDSCORE)
            break;

        printf("\nScore: %d\n\n", score); 

        drawbackground();
        
        fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);
        read(0, &key, 1);
        sleep(0.5);
       
        if(!movesnake(key, score))
            break;    
        writesnake(score);

        if(isinborder(snake[0].x, snake[0].y)){
            break;
        }
        else if(background[fy][fx] == 1){
            score++;            
            spawnfood(&fx, &fy);
        }

        usleep(UPDTIME);
    }

    printf("\nEnd game!\n\n");
    tcsetattr(fd, TCSANOW, &oldterm);
    return 0;
}
