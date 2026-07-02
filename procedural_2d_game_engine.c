#include <stdio.h>
#include <stdlib.h>

/* Cross-platform header resolution */
#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
    #include <conio.h>
    #define PLATFORM_WINDOWS 1
#else
    #include <unistd.h>
    #include <termios.h>
    #include <fcntl.h>
    #include <sys/select.h>
    #define PLATFORM_WINDOWS 0
#endif

#define V 21
#define H 75

/* Forward Declarations of Engine Subroutines */
void initialize_board(char map[V][H]);
void render_borders(char map[V][H]);
void render_player_paddle(char map[V][H], int jini, int jfinal);
void render_ai_paddle(char map[V][H], int iaini, int iafinal);
void render_ball(char map[V][H], int py, int px);
void run_engine_loop(char map[V][H], int jini, int jfinal, int iaini, int iafinal, int px, int py, int modx, int mody, int modia);
void draw_frame(char map[V][H]);
void display_buffer(char map[V][H]);
void process_input_and_physics(char map[V][H], int *jini, int *jfinal, int *iaini, int *iafinal, int *px, int *py, int *modx, int *mody, int *modia, int *gol);
void update_state(char map[V][H], int jini, int jfinal, int iaini, int iafinal, int py, int px);
void clear_screen(void);
void precise_sleep(int milliseconds);
int detect_keypress(void);
char read_keypress(void);

/* Global State Variables */
int player_victory = 0;
int ai_victory = 0;

int main(void)
{
    char map[V][H];

    /* Dynamic coordinate variables for entities */
    int player_paddle_start = 8;
    int player_paddle_end = 12;

    int ai_paddle_start = 8;
    int ai_paddle_end = 12;

    /* Ball coordinates */
    int ball_y = 10;
    int ball_x = 37;

    /* Velocity Vectors */
    int velocity_x = -1;
    int velocity_y = -1;
    int ai_velocity_y = -1;

    clear_screen();
    initialize_board(map);
    render_player_paddle(map, player_paddle_start, player_paddle_end);
    render_ai_paddle(map, ai_paddle_start, ai_paddle_end);
    render_ball(map, ball_y, ball_x);

    /* Start execution loop */
    run_engine_loop(map, player_paddle_start, player_paddle_end, ai_paddle_start, ai_paddle_end, ball_x, ball_y, velocity_x, velocity_y, ai_velocity_y);

    clear_screen();
    printf("\n=============================================\n");
    printf("                  GAME OVER                  \n");
    printf("=============================================\n\n");

    if (player_victory)
    {
        printf("🎉 Congratulations! You defeated the AI Engine!\n\n");
    }
    else if (ai_victory)
    {
        printf("🤖 AI Victory. Try again to beat the controller!\n\n");
    }

    return 0;
}

/* Builds the initial static borders */
void initialize_board(char map[V][H])
{
    render_borders(map);
}

void render_borders(char map[V][H])
{
    int i, j;
    for (i = 0; i < V; i++)
    {
        for (j = 0; j < H; j++)
        {
            if (i == 0 || i == V - 1)
            {
                map[i][j] = '-';
            }
            else if (j == 0 || j == H - 1)
            {
                map[i][j] = '|';
            }
            else
            {
                map[i][j] = ' ';
            }
        }
    }
}

/* Buffered Frame Renderer */
void display_buffer(char map[V][H])
{
    int i, j;
    for (i = 0; i < V; i++)
    {
        for (j = 0; j < H; j++)
        {
            putchar(map[i][j]);
        }
        putchar('\n');
    }
}

/* Renders Player Racket */
void render_player_paddle(char map[V][H], int jini, int jfinal)
{
    int i, j;
    for (i = jini; i < jfinal; i++)
    {
        for (j = 2; j <= 3; j++)
        {
            map[i][j] = 'X';
        }
    }
}

/* Renders Automated AI Enemy Racket */
void render_ai_paddle(char map[V][H], int iaini, int iafinal)
{
    int i, j;
    for (i = iaini; i < iafinal; i++)
    {
        for (j = H - 4; j <= H - 3; j++)
        {
            map[i][j] = 'X';
        }
    }
}

/* Renders Ball Entity */
void render_ball(char map[V][H], int py, int px)
{
    if (py >= 0 && py < V && px >= 0 && px < H)
    {
        map[py][px] = 'O';
    }
}

/* Main Loop Control Block */
void run_engine_loop(char map[V][H], int jini, int jfinal, int iaini, int iafinal, int px, int py, int modx, int mody, int modia)
{
    int goal_scored = 0;

    while (goal_scored == 0)
    {
        draw_frame(map);
        process_input_and_physics(map, &jini, &jfinal, &iaini, &iafinal, &px, &py, &modx, &mody, &modia, &goal_scored);
        update_state(map, jini, jfinal, iaini, iafinal, px, py);
        precise_sleep(45); /* Sleep execution to match standard 60Hz physics update ticks */
    }
}

/* Zero-Flicker Renderer utilizing Console Terminal Escape Codes */
void draw_frame(char map[V][H])
{
    /* Moves the terminal cursor back to top-left (0,0) without clearing screen buffer to avoid screen flicker */
    printf("\033[H");
    display_buffer(map);
}

/* Non-Blocking Physics Evaluator & Collision Vector Handler */
void process_input_and_physics(char map[V][H], int *jini, int *jfinal, int *iaini, int *iafinal, int *px, int *py, int *modx, int *mody, int *modia, int *gol)
{
    int i;
    char key;

    /* Boundary Collision Detection (Top/Bottom Walls) */
    if (*py == 1 || *py == V - 2)
    {
        *mody *= -1; /* Reverse vertical velocity vector */
    }

    /* Left Boundary Breach (AI Scores Point) */
    if (*px == 1)
    {
        ai_victory = 1;
        *gol = 1;
    }

    /* Right Boundary Breach (Player Scores Point) */
    if (*px == H - 2)
    {
        player_victory = 1;
        *gol = 1;
    }

    /* Collision Vector Logic with Player Paddle */
    if (*px == 4)
    {
        for (i = (*jini); i <= (*jfinal); i++)
        {
            if (i == (*py))
            {
                *modx *= -1; /* Bounce horizontally */
            }
        }
    }

    /* Collision Vector Logic with AI Paddle */
    if (*px == H - 5)
    {
        for (i = (*iaini); i <= (*iafinal); i++)
        {
            if (i == (*py))
            {
                *modx *= -1; /* Bounce horizontally */
            }
        }
    }

    /* AI Paddle Patrol Limits */
    if (*iaini == 1 || *iafinal == V - 1)
    {
        *modia *= -1; /* Reverse automatic movement tracking */
    }

    if (*gol == 0)
    {
        /* Advance ball through 2D space */
        *px += (*modx);
        *py += (*mody);

        /* Advance AI paddle tracker */
        *iaini += (*modia);
        *iafinal += (*modia);

        /* Process Non-Blocking Keyboard Input */
        if (detect_keypress())
        {
            key = read_keypress();

            if (key == 'w' || key == 'W')
            {
                if (*jini != 1)
                {
                    *jini -= 1;
                    *jfinal -= 1;
                }
            }
            if (key == 's' || key == 'S')
            {
                if (*jini != V - 5)
                {
                    *jini += 1;
                    *jfinal += 1;
                }
            }
        }
    }
}

/* Performs a redraw update sequence into the 2D mapping grid array */
void update_state(char map[V][H], int jini, int jfinal, int iaini, int iafinal, int py, int px)
{
    render_borders(map);
    render_player_paddle(map, jini, jfinal);
    render_ai_paddle(map, iaini, iafinal);
    render_ball(map, px, py);
}

/* Completely resets terminal console screen */
void clear_screen(void)
{
    #if PLATFORM_WINDOWS
        system("cls");
    #else
        printf("\033[2J\033[H");
    #endif
}

/* Safe Cross-Platform sleep microsecond wrapper */
void precise_sleep(int milliseconds)
{
    #if PLATFORM_WINDOWS
        Sleep(milliseconds);
    #else
        usleep(milliseconds * 1000);
    #endif
}

/* Cross-Platform, Non-Blocking Keyboard Listener */
int detect_keypress(void)
{
    #if PLATFORM_WINDOWS
        return kbhit();
    #else
        struct timeval tv;
        fd_set fds;
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
        return FD_ISSET(STDIN_FILENO, &fds);
    #endif
}

/* Cross-Platform Unbuffered input character stream reader */
char read_keypress(void)
{
    #if PLATFORM_WINDOWS
        return getch();
    #else
        struct termios oldt, newt;
        char ch;
        int oldf;

        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

        ch = getchar();

        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        fcntl(STDIN_FILENO, F_SETFL, oldf);

        return ch;
    #endif
}