#include "quakedef.h"
#include "winquake.h"

/*

key up events are sent even if in console mode

*/


#define		MAXCMDLINE	256
char	key_lines[32][MAXCMDLINE];
int		key_linepos;
int		shift_down = FALSE;
int		key_lastpress;

int		edit_line = 0;
int		history_line = 0;

keydest_t	key_dest;

int		key_count;			// incremented every key event

char* keybindings[256];
qboolean	consolekeys[256];	// if true, can't be rebound while in console
qboolean	menubound[256];	// if true, can't be rebound while in menu
int		keyshift[256];		// key to map to if shift held down in console
int		key_repeats[256];	// if > 1, it is autorepeating
qboolean	keydown[256];

typedef struct
{
	char* name;
	int		keynum;
} keyname_t;

keyname_t keynames[] =
{
	{"TAB", K_TAB},
	{"ENTER", K_ENTER},
	{"ESCAPE", K_ESCAPE},
	{"SPACE", K_SPACE},
	{"BACKSPACE", K_BACKSPACE},
	{"UPARROW", K_UPARROW},
	{"DOWNARROW", K_DOWNARROW},
	{"LEFTARROW", K_LEFTARROW},
	{"RIGHTARROW", K_RIGHTARROW},

	{"ALT", K_ALT},
	{"CTRL", K_CTRL},
	{"SHIFT", K_SHIFT},

	{"F1", K_F1},
	{"F2", K_F2},
	{"F3", K_F3},
	{"F4", K_F4},
	{"F5", K_F5},
	{"F6", K_F6},
	{"F7", K_F7},
	{"F8", K_F8},
	{"F9", K_F9},
	{"F10", K_F10},
	{"F11", K_F11},
	{"F12", K_F12},

	{"INS", K_INS},
	{"DEL", K_DEL},
	{"PGDN", K_PGDN},
	{"PGUP", K_PGUP},
	{"HOME", K_HOME},
	{"END", K_END},

	{"MOUSE1", K_MOUSE1},
	{"MOUSE2", K_MOUSE2},
	{"MOUSE3", K_MOUSE3},

	{"JOY1", K_JOY1},
	{"JOY2", K_JOY2},
	{"JOY3", K_JOY3},
	{"JOY4", K_JOY4},

	{"AUX1", K_AUX1},
	{"AUX2", K_AUX2},
	{"AUX3", K_AUX3},
	{"AUX4", K_AUX4},
	{"AUX5", K_AUX5},
	{"AUX6", K_AUX6},
	{"AUX7", K_AUX7},
	{"AUX8", K_AUX8},
	{"AUX9", K_AUX9},
	{"AUX10", K_AUX10},
	{"AUX11", K_AUX11},
	{"AUX12", K_AUX12},
	{"AUX13", K_AUX13},
	{"AUX14", K_AUX14},
	{"AUX15", K_AUX15},
	{"AUX16", K_AUX16},
	{"AUX17", K_AUX17},
	{"AUX18", K_AUX18},
	{"AUX19", K_AUX19},
	{"AUX20", K_AUX20},
	{"AUX21", K_AUX21},
	{"AUX22", K_AUX22},
	{"AUX23", K_AUX23},
	{"AUX24", K_AUX24},
	{"AUX25", K_AUX25},
	{"AUX26", K_AUX26},
	{"AUX27", K_AUX27},
	{"AUX28", K_AUX28},
	{"AUX29", K_AUX29},
	{"AUX30", K_AUX30},
	{"AUX31", K_AUX31},
	{"AUX32", K_AUX32},

	{"MWHEELUP", K_MWHEELUP},
	{"MWHEELDOWN", K_MWHEELDOWN},

	{"PAUSE", K_PAUSE},

	{"SEMICOLON", ';'},	// because a raw semicolon seperates commands

	{NULL, 0}
};

/*
==============================================================================

			LINE TYPING INTO THE CONSOLE

==============================================================================
*/

qboolean CheckForCommand( void )
{
	char command[128];
	char* cmd, * s;
	int i;

	for (i = 0; i < sizeof(command) - 1; i++)
	{
		s = key_lines[edit_line];
		if (s[i + 1] <= ' ')
			break;
		command[i] = s[i + 1];
	}
	command[i] = 0;

	cmd = Cmd_CompleteCommand(command);
	if (!cmd || strcmp(cmd, command) != 0)
		cmd = Cvar_CompleteVariable(command);
	if (!cmd || strcmp(cmd, command) != 0)
		return FALSE; // just a chat message
	return TRUE;
}

void CompleteCommand( void )
{
	char* cmd, * s;

	s = key_lines[edit_line] + 1;
	if (*s == '\\' || *s == '/')
		s++;

	// command completion
	cmd = Cmd_CompleteCommand(s);
	if (!cmd)
		cmd = Cvar_CompleteVariable(s);
	if (cmd)
	{
		key_lines[edit_line][1] = '/';
		Q_strcpy(key_lines[edit_line] + 2, cmd);
		key_linepos = Q_strlen(cmd) + 2;
		key_lines[edit_line][key_linepos] = ' ';
		key_linepos++;
		key_lines[edit_line][key_linepos] = 0;
	}
}

DLL_EXPORT void Key_UpLine( void )
{
	do
	{
		history_line = (history_line - 1) & 31;
	} while (history_line != edit_line
		&& !key_lines[history_line][1]);
	if (history_line == edit_line)
		history_line = (edit_line + 1) & 31;
	Q_strcpy(key_lines[edit_line], key_lines[history_line]);
	key_linepos = Q_strlen(key_lines[edit_line]);
}

/*
====================
Key_Console

Interactive line editing and console scrollback
====================
*/
void Key_Console( int key )
{
	char* cmd;

	if (key == K_ENTER)
	{
		if (key_lines[edit_line][1] == '\\' || key_lines[edit_line][1] == '/')
		{
			Cbuf_AddText(key_lines[edit_line] + 2);
		}
		else
		{
			if (!CheckForCommand() && cls.state >= ca_connected)
			{
				Cbuf_AddText("say ");
			}
			Cbuf_AddText(key_lines[edit_line] + 1);	// skip the >
		}
		Cbuf_AddText("\n");
		Con_Printf("%s\n", key_lines[edit_line]);
		edit_line = (edit_line + 1) & 31;
		history_line = edit_line;
		key_lines[edit_line][0] = ']';
		key_linepos = 1;
		if (cls.state == ca_disconnected)
			SCR_UpdateScreen();	// force an update, because the command
								// may take some time
		return;
	}

	if (key == K_TAB)
	{
		CompleteCommand();
		return;
	}

	if (key == K_BACKSPACE || key == K_LEFTARROW)
	{
		if (key_linepos > 1)
			key_linepos--;
		return;
	}

	if (key == K_UPARROW)
	{
		do
		{
			history_line = (history_line - 1) & 31;
		} while (history_line != edit_line
			&& !key_lines[history_line][1]);
		if (history_line == edit_line)
			history_line = (edit_line + 1) & 31;
		Q_strcpy(key_lines[edit_line], key_lines[history_line]);
		key_linepos = Q_strlen(key_lines[edit_line]);
		return;
	}

	if (key == K_DOWNARROW)
	{
		if (history_line == edit_line) return;
		do
		{
			history_line = (history_line + 1) & 31;
		} while (history_line != edit_line
			&& !key_lines[history_line][1]);
		if (history_line == edit_line)
		{
			key_lines[edit_line][0] = ']';
			key_linepos = 1;
		}
		else
		{
			Q_strcpy(key_lines[edit_line], key_lines[history_line]);
			key_linepos = Q_strlen(key_lines[edit_line]);
		}
		return;
	}

	if (key == K_PGUP)
	{
		// TODO: Implement
	}

	if (key == K_PGDN)
	{
		// TODO: Implement
	}

	if (key == K_HOME)
	{
		// TODO: Implement
	}

	if (key == K_END)
	{
		// TODO: Implement
	}

	// TODO: Implement
}




// TODO: Implement