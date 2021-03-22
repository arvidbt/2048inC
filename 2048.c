#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <ctype.h>

#define SIZE 4

typedef struct array
{
    int n1;
    int n2;
    int *data;
} Array;

static Array *board;
static int score;

void app_run(void);
void game_new(void);
void game_quit(void);
int game_get_square(int row, int column);
void game_slide_up(void);
void game_slide_right(void);
void game_slide_down(void);
void game_slide_left(void);
bool game_is_game_over(void);
Array *array_create(int n1, int n2);
void array_destroy(Array *arr);
int array_rows(const Array *arr);
int array_columns(const Array *arr);
int array_get(const Array *arr, int i1, int i2);
void array_set(Array *arr, int i1, int i2, int value);

static void print_board(void)
{
    printf("+--------+--------+--------+--------+\n");
    for(int i = 0; i < 4; i++)
    {
        printf("|        |        |        |        |\n");
        printf("|");
        for(int j = 0; j < 4; j++)
        {
            if(game_get_square(i, j) != 0) 
            {
                printf("%4d    |", game_get_square(i,j));
            }
            else
            {
                printf("        |");
            }
        }
        printf("\n|        |        |        |        |");
        printf("\n+--------+--------+--------+--------+\n");    
    }
}

static void print_menu(void)
{
    printf("SCORE: %d\n", score);
    printf("SELECT ACTION\n");
        printf("0: quit\n");
        printf("w: slide up\n");
        printf("d: slide right\n");
        printf("s: slide down\n");
        printf("a: slide left\n");
        printf("? ");
}

static void game_run(void)
{
    char user_choice;
    bool game_stop = false;

    do
    {
        print_board();

        print_menu();

        user_choice = getchar();
        while('\n'!=getchar());

        switch (user_choice)
        {
            case '0':
                printf("You chose to quit the game.\n");
                game_quit();

                break;
            
            case 'w':
                printf("Sliding up ...\n");
                game_slide_up();

                break;

            case 'd':
                printf("Sliding right ...\n");
                game_slide_right();

                break;

            case 's':
                printf("Sliding down ...\n");
                game_slide_down();

                break;

            case 'a':
                printf("Sliding left ...\n");
                game_slide_left();

                break;
            
            default:
                printf("Not a valid choice. Use WASD.\n");
                break;
        }

        game_stop = game_is_game_over();
        if(game_stop == true)
        {
            game_quit();
        }

    } while(game_stop == false);
}

static int random_values(void)
{
    if(rand()%100 <= 90)
    {
        return 2;
    }
    else
    {
        return 4;
    }
}

static void insert_value(Array *board)
{
    bool insert_success = false;

    while(insert_success == false)
    {
        int row = rand()%4;
        int column = rand()%4;
        if(array_get(board, row, column) == 0)
        {
            array_set(board, row, column, random_values());
            insert_success = true;
        }
    }
}

static bool add_together(void)
{
    bool added = false;

    for(int row = 0; row < 3; row++)
    {
        for(int col = 0; col < 4; col++)
        {
            if(game_get_square(row, col) == game_get_square((row + 1), col))
            {
                if(game_get_square(row, col) != 0)
                {
                    added = true;
                }
                array_set(board, row, col, (game_get_square(row, col) * 2));
                array_set(board, (row + 1), col, 0);
                
                if(game_get_square(row, col) != 0)
                {
                    added = true;
                    score = score + game_get_square(row, col);
                }
            }
        }
    }

    return added;
}

static bool move_and_add(void)
{
    bool move_successful = false;

    for(int row = 1; row < 4; row++)
    {
        for(int col = 0; col < 4; col++)
        {
            if(row > 0)
            {
                if(game_get_square(row, col) != 0 && game_get_square((row - 1), col) == 0 && (row > 1))
                {
                    array_set(board, (row - 1), col, game_get_square(row, col));
                    array_set(board, row, col, 0);

                    row = row - 2;

                    move_successful = true;
                }

                if(row == 1 && game_get_square(row, col) != 0 && game_get_square((row - 1), col) == 0)
                {
                    array_set(board, (row - 1), col, game_get_square(row, col));
                    array_set(board, row, col, 0);

                    move_successful = true;
                }
            }
        }
    }

    if(add_together())
    {
        move_successful = true;
    }

    return move_successful;
}

static void move_and_insert(bool move_successful)
{
    for(int row = 1; row < 4; row++)
    {
        for(int col = 0; col < 4; col++)
        {
            if(row > 0)
            {
                if(game_get_square(row, col) != 0 && game_get_square((row - 1), col) == 0 && (row > 1))
                {
                    array_set(board, (row - 1), col, game_get_square(row, col));
                    array_set(board, row, col, 0);

                    row = row - 2;
                }

                if(row == 1 && game_get_square(row, col) != 0 && game_get_square((row - 1), col) == 0)
                {
                    array_set(board, (row - 1), col, game_get_square(row, col));
                    array_set(board, row, col, 0);

                }
            }
        }
    }

    if(move_successful)
    {
        insert_value(board);
    }
}

static void rotate_board_90deg(void)
{
    int temp[16];
    int index = 0;

    for(int row = 0; row < 4; row++)
    {
        for(int col = 3; col >= 0; col--)
        {
            temp[index] = array_get(board, col, row);
            index++;
        }
    }

    for(int i = 0; i < 16; i++)
    {
        board->data[i] = temp[i];
    }
}

static bool no_moves_possible(void)
{
    bool move_is_not_possible = true;

    for(int row = 0; row < 4; row++)
    {
        for(int col = 3; col >= 0; col--)
        {
            if(row < 3)
            {
                if(game_get_square((row + 1), col) == game_get_square(row, col)) 
                {
                    move_is_not_possible = false;
                }
            }
            if(col < 3)
            {
                if(game_get_square(row, (col + 1)) == game_get_square(row, col))
                {
                    move_is_not_possible = false;
                }
            }
        }
    }

    return move_is_not_possible;
}

static void you_win(void)
{
    printf("\n");

    printf("<=========You reached 2048!=========>\n");
    printf("+--------+--------+--------+--------+\n");
    for(int i = 0; i < 2; i++)
    {
        printf("|        |        |        |        |\n");
        printf("|");
        for(int j = 0; j < 4; j++)
        {
            if(game_get_square(i, j) != 0) 
            {
                printf("%4d    |", game_get_square(i,j));
            }
            else
            {
                printf("        |"); 
            }
        }
        printf("\n|        |        |        |        |\n");
        if(i == 0)
        {
            printf("+--------+--------+--------+--------+\n");   
        }
        if(i == 1) 
        {
            printf("+--------+----YOU WIN !----+--------+\n");
        }
    }
    for(int i = 2; i < 4; i++)
    {
        printf("|        |        |        |        |\n");
        printf("|");
        for(int j = 0; j < 4; j++)
        {
            if(game_get_square(i, j) != 0) 
            {
                printf("%4d    |", game_get_square(i,j));
            }
            else
            {
                printf("        |");
            }
        }
        printf("\n|        |        |        |        |");
        printf("\n+--------+--------+--------+--------+\n");    
    }
}

static void you_lose(void)
{
    printf("\n");

    printf("+--------+--------+--------+--------+\n");
    for(int i = 0; i < 2; i++)
    {
        printf("|        |        |        |        |\n");
        printf("|");
        for(int j = 0; j < 4; j++)
        {
            if(game_get_square(i, j) != 0) 
            {
                printf("%4d    |", game_get_square(i,j));
            }
            else
            {
                printf("        |"); 
            }
        }
        printf("\n|        |        |        |        |\n");
        if(i == 0)
        {
            printf("+--No more moves or empty squares!--+\n");   
        }
        if(i == 1) 
        {
            printf("+------");printf("------SCORE:%5d------", score);printf("------+\n");
        }
    }

    for(int i = 2; i < 4; i++)
    {
        printf("|        |        |        |        |\n");
        printf("|");
        for(int j = 0; j < 4; j++)
        {
            if(game_get_square(i, j) != 0) 
            {
                printf("%4d    |", game_get_square(i,j));
            }
            else
            {
                printf("        |");
            }
        }
        printf("\n|        |        |        |        |\n");
        if(i == 2)
        {
            printf("+--------+----Game over----+--------+\n");
        } 
    }
    printf("+--------+--------+--------+--------+\n"); 
}

int main(void)
{
    score = 0;
    game_new();
    game_run();

    return 0;
}

void game_new(void)
{
    srand(time(NULL));

    board = array_create(SIZE, SIZE);
    for(int i = 0; i < 16; i++)
    {
        board->data[i] = 0;
    }

    for(int i = 0; i < 2; i++)
    {
        insert_value(board);
    }
}

void game_quit(void)
{
    array_destroy(board);
    exit(0);
}

int game_get_square(int row, int column)
{
    return array_get(board, row, column);
}

void game_slide_up(void)
{
    bool move_successful = move_and_add();
    move_and_insert(move_successful);
}

void game_slide_right(void)
{
    for(int i = 0; i < 3; i++)
    {
        rotate_board_90deg();
    }
    
    bool move_successful = move_and_add();
    move_and_insert(move_successful);

    rotate_board_90deg();
}

void game_slide_down(void)
{
    for(int i = 0; i < 2; i++)
    {
        rotate_board_90deg();
    }

    bool move_successful = move_and_add();
    move_and_insert(move_successful);

    for(int i = 0; i < 2; i++)
    {
        rotate_board_90deg();
    }
}

void game_slide_left(void)
{
    rotate_board_90deg();

    bool move_successful = move_and_add();
    move_and_insert(move_successful);
    
    for(int i = 0; i < 3; i++)
    {
        rotate_board_90deg();
    }
}

bool game_is_game_over(void)
{
    bool is_game_over = false;
    int full = 0;

    for(int i = 0; i < 16; i++)
    {
        if(board->data[i] != 0)
        {
            full++;
        }
    }

    if(full == 16 && no_moves_possible())
    {   
        is_game_over = true;
        you_lose();
    }

    for(int i = 0; i < 16; i++)
    {
        if(board->data[i] == 2048)
        {
            is_game_over = true;
            you_win();
        }
    }
    return is_game_over;
}

Array *array_create(int n1, int n2)
{
    Array *return_arr = malloc(sizeof(Array));
    return_arr->data = malloc((n1 * n2) * sizeof(int));

    return_arr->n1 = n1;
    return_arr->n2 = n2;

    return return_arr;
}

void array_destroy(Array *arr)
{
    free(arr->data);
    free(arr);
}

int array_rows(const Array *arr)
{
    return arr->n1;
}

int array_columns(const Array *arr)
{
    return arr->n2;
}

int array_get(const Array *arr, int i1, int i2)
{
    return arr->data[i1 * arr->n2 + i2];
}

void array_set(Array *arr, int i1, int i2, int value)
{
    arr->data[i1 * arr->n2 + i2] = value;
}