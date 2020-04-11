#ifndef __ATR__
#define __ATR__

#define ESC "\033"

#define RESET		0
#define BRIGHT		1
#define DIM			2
#define UNDERSCORE	3
#define BLINK		4
#define REVERSE		5
#define HIDDEN		6

//foreground

#define F_BLACK		30
#define F_RED		31
#define F_GREEN		32
#define F_YELLOW	33
#define F_BLUE		34
#define F_MAGENTA	35
#define F_CYAN		36
#define F_WHITE		37

//background

#define B_BLACK		40
#define B_RED		41
#define B_GREEN		42
#define B_YELLOW	43
#define B_BLUE		44
#define B_MAGENTA	45
#define B_CYAN		46
#define B_WHITE		47


#define home()					printf(ESC "[H")
#define clrscr()				printf(ESC "[2J")
#define gotoxy(x, y)			printf(ESC "[%d;%dH", y, x)
#define visible_cursor()		printf(ESC "[?25l")
#define resetcolor() 			printf(ESC "[0m")
#define set_display_atr(color)	printf(ESC "[%dm", color)

#endif
