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
		con_backscroll += V_max(con_rows / 2, 4);
		if (con_backscroll > con_totallines - (vid.height >> 3) - 1)
			con_backscroll = con_totallines - (vid.height >> 3) - 1;
		return;
	}

	if (key == K_PGDN)
	{
		con_backscroll -= V_max(con_rows / 2, 4);
		if (con_backscroll < 0)
			con_backscroll = 0;
		return;
	}

	if (key == K_HOME)
	{
		con_backscroll = con_totallines - (vid.height >> 3) - 1;
		return;
	}

	if (key == K_END)
	{
		con_backscroll = 0;
		return;
	}

#ifdef _WIN32
	if ((key == 'v' || key == 'V'))
	{
		if ((GetKeyState(VK_CONTROL) & 0x8000) && OpenClipboard(NULL))
		{
			HANDLE	th;
			int	nLength;
			char* clipText, * textCopied;

			th = GetClipboardData(CF_TEXT);
			if (th)
			{
				clipText = GlobalLock(th);
				if (clipText)
				{
					textCopied = malloc(GlobalSize(th) + 1);
					strcpy(textCopied, clipText);
					strtok(textCopied, "\n\r\b");

					nLength = V_min(strlen(textCopied), 256 - key_linepos);
					if (nLength > 0)
					{
						textCopied[nLength] = 0;
						strcat(key_lines[edit_line], textCopied);
						key_linepos += nLength;
					}
					free(textCopied);
				}
				GlobalUnlock(th);
			}
			CloseClipboard();
			return;
		}
	}
#endif

	if (key < 32 || key > 127)
		return;	// non printable

	if (key_linepos < MAXCMDLINE - 1)
	{
		key_lines[edit_line][key_linepos] = key;
		key_linepos++;
		key_lines[edit_line][key_linepos] = 0;
	}
}

//============================================================================

char chat_buffer[120 + 1];
char message_type[32] = "say";
int chat_bufferlen = 0;

void Key_Message( int key )
{
	if (key == K_ENTER)
	{
		Cbuf_AddText("say");
		Cbuf_AddText(" \"");
		Cbuf_AddText(chat_buffer);
		Cbuf_AddText("\"\n");

		key_dest = key_game;
		chat_bufferlen = 0;
		chat_buffer[0] = 0;
		strcpy(message_type, "say");
		return;
	}

	if (key == K_ESCAPE)
	{
		key_dest = key_game;
		chat_bufferlen = 0;
		chat_buffer[0] = 0;
		return;
	}

	if (key < 32 || key > 127)
		return;	// non printable

	if (key == K_BACKSPACE)
	{
		if (chat_bufferlen)
		{
			chat_bufferlen--;
			chat_buffer[chat_bufferlen] = 0;
		}
		return;
	}

	if (chat_bufferlen == 120)
		return; // all full

	chat_buffer[chat_bufferlen++] = key;
	chat_buffer[chat_bufferlen] = 0;
}

//============================================================================


/*
===================
Key_StringToKeynum

Returns a key number to be used to index keybindings[] by looking at
the given string.  Single ascii characters return themselves, while
the K_* names are matched up.
===================
*/
int Key_StringToKeynum( char* str )
{
	keyname_t* kn;

	if (!str || !str[0])
		return -1;
	if (!str[1])
		return str[0];

	for (kn = keynames; kn->name; kn++)
	{
		if (!Q_strcasecmp(str, kn->name))
			return kn->keynum;
	}
	return -1;
}

/*
===================
Key_KeynumToString

Returns a string (either a single ascii char, or a K_* name) for the
given keynum.
FIXME: handle quote special (general escape sequence?)
===================
*/
char* Key_KeynumToString( int keynum )
{
	keyname_t* kn;
	static	char	tinystr[2];

	if (keynum == -1)
		return "<KEY NOT FOUND>";
	if (keynum > 32 && keynum < 127)
	{	// printable ascii
		tinystr[0] = keynum;
		tinystr[1] = 0;
		return tinystr;
	}

	for (kn = keynames; kn->name; kn++)
		if (keynum == kn->keynum)
			return kn->name;

	return "<UNKNOWN KEYNUM>";
}


/*
===================
Key_SetBinding
===================
*/
void Key_SetBinding( int keynum, char* binding )
{
	char*  _new;
	int		l;

	if (keynum == -1)
		return;

// free old bindings
	if (keybindings[keynum])
	{
		Z_Free(keybindings[keynum]);
		keybindings[keynum] = NULL;
	}
			
// allocate memory for new binding
	l = Q_strlen(binding);
	_new = Z_Malloc(l + 1);
	Q_strcpy(_new, binding);
	_new[l] = 0;
	keybindings[keynum] = _new;
}

/*
===================
Key_Unbind_f
===================
*/
void Key_Unbind_f( void )
{
	int		b;

	if (Cmd_Argc() != 2)
	{
		Con_Printf("unbind <key> : remove commands from a key\n");
		return;
	}

	b = Key_StringToKeynum(Cmd_Argv(1));
	if (b == -1)
	{
		Con_Printf("\"%s\" isn't a valid key\n", Cmd_Argv(1));
		return;
	}

	Key_SetBinding(b, "");
}

void Key_Unbindall_f( void )
{
	int		i;

	for (i = 0; i < 256; i++)
		if (keybindings[i])
			Key_SetBinding(i, "");
}

void Key_Escape_f( void )
{
	if (giSubState & 0x10)
	{
		extern int giStateInfo;

		Cbuf_AddText("disconnect\n");
		giActive = DLL_PAUSED;
		giStateInfo = 2;
		giSubState = 1;
		Cbuf_Execute();
	}
	else if (key_dest == key_game)
	{
		giActive = DLL_PAUSED;
	}
	else if (key_dest == key_console)
	{
		Con_ToggleConsole_f();
	}
	else if (key_dest == key_message)
	{
		Key_Message(K_ESCAPE);
	}
	else
	{
		Sys_Error("Bad key_dest");
	}
}

/*
===================
Key_Bind_f
===================
*/
void Key_Bind_f( void )
{
	int			i, c, b;
	char		cmd[1024];

	c = Cmd_Argc();

	if (c != 2 && c != 3)
	{
		Con_Printf("bind <key> [command] : attach a command to a key\n");
		return;
	}
	b = Key_StringToKeynum(Cmd_Argv(1));
	if (b == -1)
	{
		Con_Printf("\"%s\" isn't a valid key\n", Cmd_Argv(1));
		return;
	}

	if (c == 2)
	{
		if (keybindings[b])
			Con_Printf("\"%s\" = \"%s\"\n", Cmd_Argv(1), keybindings[b]);
		else
			Con_Printf("\"%s\" is not bound\n", Cmd_Argv(1));
		return;
	}

// copy the rest of the command line
	cmd[0] = 0;		// start out with a null string
	for (i = 2; i < c; i++)
	{
		if (i > 2)
			strcat(cmd, " ");
		strcat(cmd, Cmd_Argv(i));
	}

	Key_SetBinding(b, cmd);
}

/*
============
Key_WriteBindings

Writes lines containing "bind key value"
============
*/
void Key_WriteBindings(FILE* f)
{
	int		i;

	for (i = 0; i < 256; i++)
		if (keybindings[i])
			if (*keybindings[i])
				fprintf(f, "bind \"%s\" \"%s\"\n", Key_KeynumToString(i), keybindings[i]);
}


/*
===================
Key_Init
===================
*/
void Key_Init( void )
{
	int		i;

	for (i = 0; i < 32; i++)
	{
		key_lines[i][0] = ']';
		key_lines[i][1] = 0;
	}
	key_linepos = 1;

//
// init ascii characters in console mode
//
	for (i = 32; i < 128; i++)
		consolekeys[i] = TRUE;
	consolekeys[K_ENTER] = TRUE;
	consolekeys[K_TAB] = TRUE;
	consolekeys[K_LEFTARROW] = TRUE;
	consolekeys[K_RIGHTARROW] = TRUE;
	consolekeys[K_UPARROW] = TRUE;
	consolekeys[K_DOWNARROW] = TRUE;
	consolekeys[K_BACKSPACE] = TRUE;
	consolekeys[K_PGUP] = TRUE;
	consolekeys[K_PGDN] = TRUE;
	consolekeys[K_SHIFT] = TRUE;
	consolekeys[K_MWHEELUP] = TRUE;
	consolekeys[K_MWHEELDOWN] = TRUE;
	consolekeys['`'] = FALSE;
	consolekeys['~'] = FALSE;

	for (i = 0; i < 256; i++)
		keyshift[i] = i;
	for (i = 'a'; i <= 'z'; i++)
		keyshift[i] = i - 'a' + 'A';
	keyshift['1'] = '!';
	keyshift['2'] = '@';
	keyshift['3'] = '#';
	keyshift['4'] = '$';
	keyshift['5'] = '%';
	keyshift['6'] = '^';
	keyshift['7'] = '&';
	keyshift['8'] = '*';
	keyshift['9'] = '(';
	keyshift['0'] = ')';
	keyshift['-'] = '_';
	keyshift['='] = '+';
	keyshift[','] = '<';
	keyshift['.'] = '>';
	keyshift['/'] = '?';
	keyshift[';'] = ':';
	keyshift['\''] = '"';
	keyshift['['] = '{';
	keyshift[']'] = '}';
	keyshift['`'] = '~';
	keyshift['\\'] = '|';

	menubound[K_ESCAPE] = TRUE;
	for (i = 0; i < 12; i++)
		menubound[K_F1 + i] = TRUE;

//
// register our functions
//
	Cmd_AddCommand("bind", Key_Bind_f);
	Cmd_AddCommand("unbind", Key_Unbind_f);
	Cmd_AddCommand("unbindall", Key_Unbindall_f);
	Cmd_AddCommand("escape", Key_Escape_f);
}

/*
===================
Key_Event

Called by the system between frames for both key up and key down events
Should NOT be called during an interrupt!
===================
*/
void Key_Event( int key, qboolean down )
{
	char* kb;
	char	cmd[1024];

	keydown[key] = down;

	if (keydown[K_CTRL] && keydown[K_ALT] && keydown[K_DEL])
	{
		Sys_Error("ctrl-alt-del pressed");
	}

	if (!down)
		key_repeats[key] = 0;

	key_lastpress = key;
	key_count++;
	if (key_count <= 0)
	{
		return;		// just catching keys for Con_NotifyBox
	}

// update auto-repeat status
	if (down)
	{
		key_repeats[key]++;
		if (key != K_BACKSPACE && key != K_PAUSE && key_repeats[key] > 1)
		{
			return;	// ignore most autorepeats
		}

		if (key >= 200 && !keybindings[key])
			Con_Printf("%s is unbound.\n", Key_KeynumToString(key));
	}

	if (key == K_SHIFT)
		shift_down = down;

//
// key up events only generate commands if the game key binding is
// a button command (leading + sign).  These will occur even in console mode,
// to keep the character from continuing an action started before a console
// switch.  Button commands include the kenum as a parameter, so multiple
// downs can be matched with ups
//
	if (!down)
	{
		kb = keybindings[key];
		if (kb && kb[0] == '+')
		{
			sprintf(cmd, "-%s %i\n", kb + 1, key);
			Cbuf_AddText(cmd);
		}
		if (keyshift[key] != key)
		{
			kb = keybindings[keyshift[key]];
			if (kb && kb[0] == '+')
			{
				sprintf(cmd, "-%s %i\n", kb + 1, key);
				Cbuf_AddText(cmd);
			}
		}
		return;
	}

	if ((giSubState & 0x10) && key != K_ESCAPE)
		return;

//
// during demo playback, most keys bring up the main menu
//
	if (cls.demoplayback && down && consolekeys[key] && key_dest == key_game)
	{
		Con_ToggleConsole_f();
		return;
	}

//
// if not a consolekey, send to the interpreter no matter what mode is
//
	if ((key_dest == key_menu && menubound[key])
		|| (key_dest == key_console && !consolekeys[key])
		|| (key_dest == key_game && (!con_forcedup || !consolekeys[key])))
	{
		kb = keybindings[key];
		if (kb)
		{
			if (kb[0] == '+')
			{	// button commands add keynum as a parm
				sprintf(cmd, "%s %i\n", kb, key);
				Cbuf_AddText(cmd);
			}
			else
			{
				Cbuf_AddText(keybindings[key]);
				Cbuf_AddText("\n");
			}
		}
		return;
	}

	if (!down)
		return;		// other systems only care about key down events

	if (shift_down)
	{
		key = keyshift[key];
	}

	switch (key_dest)
	{
	case key_message:
		Key_Message(key);
		break;

	case key_game:
	case key_console:
		Key_Console(key);
		break;
	default:
		Sys_Error("Bad key_dest");
	}
}


/*
===================
Key_ClearStates
===================
*/
void Key_ClearStates( void )
{
	int		i;

	for (i = 0; i < 256; i++)
	{
		keydown[i] = FALSE;
		key_repeats[i] = 0;
	}
}