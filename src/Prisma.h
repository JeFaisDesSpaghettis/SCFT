#ifndef PRISMA_H
#define PRISMA_H

/**
 * @file Prisma.h
 * @brief Header only definition, and some functions, helping with ANSI escape codes @n
 * Based off information from https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797
*/

#ifdef __cplusplus
	extern "C"
	{
#endif

#include <stdio.h>
#include <string.h>

typedef struct _prsm_xypos
{
	int x;
	int y;
}prsm_xypos;

#ifdef _WIN32
	#include <windows.h>
	#include <conio.h>
	/**
	 * @brief Exclusive to Windows systems for enabling virtual processing (VT100)
	*/
	static void prsm_enable_ansi_codes()
	{
	    HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
		DWORD dw_mode = 0;
		GetConsoleMode(handle_out, &dw_mode);
		dw_mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		SetConsoleMode(handle_out, dw_mode);
	}

	static void prsm_enable_utf8()
	{
		SetConsoleOutputCP(CP_UTF8);
	}

	static void prsm_enable_echo()
	{
		HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
		DWORD dw_mode = 0;
		GetConsoleMode(handle_in, &dw_mode);
		dw_mode |= ENABLE_ECHO_INPUT;
		SetConsoleMode(handle_in, dw_mode);
	}

	static void prsm_disable_echo()
	{
		HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
		DWORD dw_mode = 0;
		GetConsoleMode(handle_in, &dw_mode);
		dw_mode &= ~ENABLE_ECHO_INPUT;
		SetConsoleMode(handle_in, dw_mode);
	}

	static void prsm_enable_ctrl_c()
	{
		SetConsoleCtrlHandler(NULL, 0);
	}

	static void prsm_disable_ctrl_c()
	{
		SetConsoleCtrlHandler(NULL, 1);
	}

	static unsigned int prsm_get_terminal_w()
	{
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
		return csbi.srWindow.Right - csbi.srWindow.Left + 1;
	}

	static unsigned int prsm_get_terminal_h()
	{
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
		return csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	}

	static int prsm_getch()
	{
		return _getch();
	}

	static int prsm_getche()
	{
		return _getche();
	}

#elif __linux__
	#include <sys/ioctl.h>
	#include <termios.h>
	#include <unistd.h>

	static void prsm_enable_ansi_codes() {}

	static void prsm_enable_utf8() {}

	static void prsm_enable_echo()
	{
		struct termios term_info;
		tcgetattr(0, &term_info);
		term_info.c_lflag |= ECHO;
	}

	static void prsm_disable_echo()
	{
		struct termios term_info;
		tcgetattr(0, &term_info);
		term_info.c_lflag &= ~ECHO;
	}

	static void prsm_enable_ctrl_c()
	{
		struct termios term_info;
		tcgetattr(0, &term_info);
		term_info.c_lflag |= VINTR;
	}

	static void prsm_disable_ctrl_c()
	{
		struct termios term_info;
		tcgetattr(0, &term_info);
		term_info.c_lflag &= ~VINTR;
	}

	static unsigned int prsm_get_terminal_w()
	{
		struct winsize wsz;
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &wsz);
		return wsz.ws_col;
	}

	static unsigned int prsm_get_terminal_h()
	{
		struct winsize wsz;
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &wsz);
		return wsz.ws_row;
	}

	// https://stackoverflow.com/questions/7469139/what-is-the-equivalent-to-getch-getche-in-linux
	static struct termios _prsm_old, _prsm_current;
	static void _prsm_init_termios(int echo)
	{
		tcgetattr(0, &_prsm_old);
		_prsm_current = _prsm_old;
		_prsm_current.c_lflag &= ~ICANON;
		if (echo)
			_prsm_current.c_lflag |= ECHO;
		else
			_prsm_current.c_lflag &= ~ECHO;
		tcsetattr(0, TCSANOW, &_prsm_current);
	}

	static void _prsm_reset_termios()
	{
		tcsetattr(0, TCSANOW, &_prsm_old);
	}

	static char _prsm_getch(int echo)
	{
		char ch;
		_prsm_init_termios(echo);
		ch = getchar();
		_prsm_reset_termios();
		return ch;
	}

	static int prsm_getch()
	{
		return _prsm_getch(0);
	}

	static int prsm_getche()
	{
		return _prsm_getch(1);
	}
#endif

/* -------------------- MACROS -------------------- */
#define PRSM_OCT_ESCP "\033"
#define PRSM_HEX_ESCP "\x1B"
#define PRSM_DEC_ESCP (char)27
#define PRSM_UTF_ESCP "\u001B"
#define PRSM_CHAR_ESCP '\e'

#ifndef PRSM_ESCP_FMT
	#define PRSM_ESCP_FMT PRSM_HEX_ESCP
#endif

#define PRSM_CHAR_BELL '\a'
#define PRSM_CHAR_BACK '\b'
#define PRSM_CHAR_HTAB '\t'
#define PRSM_CHAR_LFNL '\n'
#define PRSM_CHAR_VTAB '\v'
#define PRSM_CHAR_NPAG '\f'
#define PRSM_CHAR_CRET '\r'

#define PRSM_CURS_RESET 			"\x1B[H"
#define PRSM_CURS_GOTO(X, Y) 		"\x1B["#X";"#Y"f"
#define PRSM_CURS_UP(N) 			"\x1B["#N"A"
#define PRSM_CURS_DOWN(N) 			"\x1B["#N"B"
#define PRSM_CURS_RIGHT(N) 			"\x1B["#N"C"
#define PRSM_CURS_LEFT(N) 			"\x1B["#N"D"
#define PRSM_CURS_LINE_DOWN(N) 		"\x1B["#N"E"
#define PRSM_CURS_LINE_UP(N) 		"\x1B["#N"F"
#define PRSM_CURS_COLUMN_GOTO(N) 	"\x1B["#N"G"

#define PRSM_CURS_POS_SAVE_WIN 		"\x1B7"
#define PRSM_CURS_POS_RESTORE_WIN 	"\x1B8"

#define PRSM_CURS_POS_SAVE_UNIX 	"\x1B[s"
#define PRSM_CURS_POS_RESTORE_UNIX 	"\x1B[u"

#define PRSM_SCR_CLEAR_AFTER_CURS 	"\x1B[0J"
#define PRSM_SCR_CLEAR_BEFORE_CURS 	"\x1B[1J"
#define PRSM_SCR_CLEAR_FULL 		"\x1B[2J"

#define PRSM_LINE_CLEAR_AFTER_CURS 	"\x1B[0K"
#define PRSM_LINE_CLEAR_BEFORE_CURS "\x1B[1K"
#define PRSM_LINE_CLEAR 			"\x1B[2K"

#define PRSM_EFCT_RESET 				"\x1B[0m"
#define PRSM_EFCT_BOLD_ENABLE 			"\x1B[1m"
#define PRSM_EFCT_DIM_ENABLE 			"\x1B[2m"
#define PRSM_EFCT_ITALIC_ENABLE 		"\x1B[3m"
#define PRSM_EFCT_UNDERLINE_ENABLE 		"\x1B[4m"
#define PRSM_EFCT_UNDERLINE_2_ENABLE	"\x1B[21m"
#define PRSM_EFCT_BLINK_ENABLE 			"\x1B[5m"
#define PRSM_EFCT_REVERSE_ENABLE 		"\x1B[7m"
#define PRSM_EFCT_HIDE_ENABLE 			"\x1B[8m"
#define PRSM_EFCT_STRIKE_ENABLE 		"\x1B[9m"

#define PRSM_EFCT_BOLD_DISABLE 			"\x1B[22m"
#define PRSM_EFCT_DIM_DISABLE 			"\x1B[22m"
#define PRSM_EFCT_ITALIC_DISABLE 		"\x1B[23m"
#define PRSM_EFCT_UNDERLINE_DISABLE 	"\x1B[24m"
#define PRSM_EFCT_BLINK_DISABLE 		"\x1B[25m"
#define PRSM_EFCT_REVERSE_DISABLE 		"\x1B[27m"
#define PRSM_EFCT_HIDE_DISABLE 			"\x1B[28m"
#define PRSM_EFCT_STRIKE_DISABLE 		"\x1B[29m"

#define PRSM_COLOR_4_FG_BLACK 	"\x1B[30m"
#define PRSM_COLOR_4_FG_RED		"\x1B[31m"
#define PRSM_COLOR_4_FG_GREEN 	"\x1B[32m"
#define PRSM_COLOR_4_FG_YELLOW 	"\x1B[33m"
#define PRSM_COLOR_4_FG_BLUE 	"\x1B[34m"
#define PRSM_COLOR_4_FG_MAGENTA	"\x1B[35m"
#define PRSM_COLOR_4_FG_CYAN	"\x1B[36m"
#define PRSM_COLOR_4_FG_WHITE	"\x1B[37m"
#define PRSM_COLOR_4_FG_DEFAULT	"\x1B[39m"

#define PRSM_COLOR_4_BG_BLACK 	"\x1B[40m"
#define PRSM_COLOR_4_BG_RED		"\x1B[41m"
#define PRSM_COLOR_4_BG_GREEN 	"\x1B[42m"
#define PRSM_COLOR_4_BG_YELLOW 	"\x1B[43m"
#define PRSM_COLOR_4_BG_BLUE 	"\x1B[44m"
#define PRSM_COLOR_4_BG_MAGENTA	"\x1B[45m"
#define PRSM_COLOR_4_BG_CYAN	"\x1B[46m"
#define PRSM_COLOR_4_BG_WHITE	"\x1B[47m"
#define PRSM_COLOR_4_BG_DEFAULT	"\x1B[49m"

#define PRSM_COLOR_4_FG_BRIGHT_BLACK 	"\x1B[90m"
#define PRSM_COLOR_4_FG_BRIGHT_RED		"\x1B[91m"
#define PRSM_COLOR_4_FG_BRIGHT_GREEN 	"\x1B[92m"
#define PRSM_COLOR_4_FG_BRIGHT_YELLOW 	"\x1B[93m"
#define PRSM_COLOR_4_FG_BRIGHT_BLUE 	"\x1B[94m"
#define PRSM_COLOR_4_FG_BRIGHT_MAGENTA	"\x1B[95m"
#define PRSM_COLOR_4_FG_BRIGHT_CYAN		"\x1B[96m"
#define PRSM_COLOR_4_FG_BRIGHT_WHITE	"\x1B[97m"
#define PRSM_COLOR_4_FG_BRIGHT_DEFAULT	"\x1B[99m"

#define PRSM_COLOR_4_BG_BRIGHT_BLACK 	"\x1B[100m"
#define PRSM_COLOR_4_BG_BRIGHT_RED		"\x1B[101m"
#define PRSM_COLOR_4_BG_BRIGHT_GREEN 	"\x1B[102m"
#define PRSM_COLOR_4_BG_BRIGHT_YELLOW 	"\x1B[103m"
#define PRSM_COLOR_4_BG_BRIGHT_BLUE 	"\x1B[104m"
#define PRSM_COLOR_4_BG_BRIGHT_MAGENTA	"\x1B[105m"
#define PRSM_COLOR_4_BG_BRIGHT_CYAN		"\x1B[106m"
#define PRSM_COLOR_4_BG_BRIGHT_WHITE	"\x1B[107m"
#define PRSM_COLOR_4_BG_BRIGHT_DEFAULT	"\x1B[109m"

#define PRSM_COLOR_8_FG_FMT(DEC) "\x1B[38;5;"#DEC"m"
#define PRSM_COLOR_8_BG_FMT(DEC) "\x1B[48;5;"#DEC"m"

#define PRSM_COLOR_RGB_FG_FMT(DECR, DECG, DECB) "\x1B[38;2;"#DECR";"#DECG";"#DECB"m"
#define PRSM_COLOR_RGB_BG_FMT(DECR, DECG, DECB) "\x1B[48;2;"#DECR";"#DECG";"#DECB"m"

#define PRSM_EXTRA_CURS_HIDE 		"\x1B[?25l"
#define PRSM_EXTRA_CURS_SHOW 		"\x1B[?25h"
#define PRSM_EXTRA_SCR_RESTORE 		"\x1B[?47l"
#define PRSM_EXTRA_SCR_SAVE 		"\x1B[?47h"
#define PRSM_EXTRA_ALT_BUF_ENABLE 	"\x1B[?1049h"
#define PRSM_EXTRA_ALT_BUF_DISABLE 	"\x1B[?1049l"

#ifdef __cplusplus
	}
#endif

#endif /* PRISMA_H */