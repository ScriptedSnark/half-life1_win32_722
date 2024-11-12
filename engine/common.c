// common.c -- misc functions used in client and server

#include "quakedef.h"
#include "winquake.h"

#define NUM_SAFE_ARGVS  7

static char* largv[MAX_NUM_ARGVS + NUM_SAFE_ARGVS + 1];
static char* argvdummy = " ";

static char* safeargvs[NUM_SAFE_ARGVS] =
	{"-stdvid", "-nolan", "-nosound", "-nocdaudio", "-nojoy", "-nomouse", "-dibonly"};

qboolean com_ignorecolons = FALSE;  // YWB:  Ignore colons as token separators in COM_Parse

cvar_t  registered = { "registered","0" };
cvar_t  cmdline = { "cmdline","0", FALSE, TRUE };

qboolean        com_modified;   // set true if using non-id files

int             static_registered = 1;  // only for startup check, then set

char	com_token[1024];
int		com_argc;
char** com_argv;

#define CMDLINE_LENGTH	256
char	com_cmdline[CMDLINE_LENGTH];

qboolean		standard_quake = TRUE, rogue, hipnotic;

// this graphic needs to be in the pak file to use registered features
unsigned short pop[] =
{
 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
,0x0000,0x0000,0x6600,0x0000,0x0000,0x0000,0x6600,0x0000
,0x0000,0x0066,0x0000,0x0000,0x0000,0x0000,0x0067,0x0000
,0x0000,0x6665,0x0000,0x0000,0x0000,0x0000,0x0065,0x6600
,0x0063,0x6561,0x0000,0x0000,0x0000,0x0000,0x0061,0x6563
,0x0064,0x6561,0x0000,0x0000,0x0000,0x0000,0x0061,0x6564
,0x0064,0x6564,0x0000,0x6469,0x6969,0x6400,0x0064,0x6564
,0x0063,0x6568,0x6200,0x0064,0x6864,0x0000,0x6268,0x6563
,0x0000,0x6567,0x6963,0x0064,0x6764,0x0063,0x6967,0x6500
,0x0000,0x6266,0x6769,0x6a68,0x6768,0x6a69,0x6766,0x6200
,0x0000,0x0062,0x6566,0x6666,0x6666,0x6666,0x6562,0x0000
,0x0000,0x0000,0x0062,0x6364,0x6664,0x6362,0x0000,0x0000
,0x0000,0x0000,0x0000,0x0062,0x6662,0x0000,0x0000,0x0000
,0x0000,0x0000,0x0000,0x0061,0x6661,0x0000,0x0000,0x0000
,0x0000,0x0000,0x0000,0x0000,0x6500,0x0000,0x0000,0x0000
,0x0000,0x0000,0x0000,0x0000,0x6400,0x0000,0x0000,0x0000
};

/*


All of Quake's data access is through a hierchal file system, but the contents of the file system can be transparently merged from several sources.

The "base directory" is the path to the directory holding the quake.exe and all game directories.  The sys_* files pass this to host_init in quakeparms_t->basedir.  This can be overridden with the "-basedir" command line parm to allow code debugging in a different directory.  The base directory is
only used during filesystem initialization.

The "game directory" is the first tree on the search path and directory that all generated files (savegames, screenshots, demos, config files) will be saved to.  This can be overridden with the "-game" command line parameter.  The game directory can never be changed while quake is executing.  This is a precacution against having a malicious server instruct clients to write files over areas they shouldn't.

The "cache directory" is only used during development to save network bandwidth, especially over ISDN / T1 lines.  If there is a cache directory
specified, when a file is found by the normal search path, it will be mirrored
into the cache directory, then opened there.



FIXME:
The file "parms.txt" will be read out of the game directory and appended to the current command line arguments to allow different games to initialize startup parms differently.  This could be used to add a "-sspeed 22050" for the high quality sound edition.  Because they are added at the end, they will not override an explicit setting on the original command line.

*/

//============================================================================


// ClearLink is used for new headnodes
void ClearLink( link_t* l )
{
	l->prev = l->next = l;
}

void RemoveLink( link_t* l )
{
	l->next->prev = l->prev;
	l->prev->next = l->next;
}

void InsertLinkBefore( link_t* l, link_t* before )
{
	l->next = before;
	l->prev = before->prev;
	l->prev->next = l;
	l->next->prev = l;
}
void InsertLinkAfter( link_t* l, link_t* after )
{
	l->next = after->next;
	l->prev = after;
	l->prev->next = l;
	l->next->prev = l;
}

/*
============================================================================

					LIBRARY REPLACEMENT FUNCTIONS

============================================================================
*/

void Q_memset( void* dest, int fill, int count )
{
	int             i;

	if ((((long)dest | count) & 3) == 0)
	{
		count >>= 2;
		fill = fill | (fill << 8) | (fill << 16) | (fill << 24);
		for (i = 0; i < count; i++)
			((int*)dest)[i] = fill;
	}
	else
		for (i = 0; i < count; i++)
			((byte*)dest)[i] = fill;
}

void Q_memcpy( void* dest, void* src, int count )
{
	int             i;

	if ((((long)dest | (long)src | count) & 3) == 0)
	{
		count >>= 2;
		for (i = 0; i < count; i++)
			((int*)dest)[i] = ((int*)src)[i];
	}
	else
		for (i = 0; i < count; i++)
			((byte*)dest)[i] = ((byte*)src)[i];
}

int Q_memcmp( void* m1, void* m2, int count )
{
	while (count)
	{
		count--;
		if (((byte*)m1)[count] != ((byte*)m2)[count])
			return -1;
	}
	return 0;
}

void Q_strcpy( char* dest, char* src )
{
	while (*src)
	{
		*dest++ = *src++;
	}
	*dest++ = 0;
}

void Q_strncpy( char* dest, char* src, int count )
{
	while (*src && count--)
	{
		*dest++ = *src++;
	}
	if (count)
		*dest++ = 0;
}

int Q_strlen( char* str )
{
	int             count;
	
	count = 0;
	while (str[count])
		count++;

	return count;
}

char* Q_strrchr( char* s, char c )
{
	int len = Q_strlen(s);
	s += len;
	while (len--)
		if (*--s == c) return s;
	return 0;
}

void Q_strcat( char* dest, char* src )
{
	dest += Q_strlen(dest);
	Q_strcpy(dest, src);
}

int Q_strcmp( char* s1, char* s2 )
{
	while (1)
	{
		if (*s1 != *s2)
			return -1;              // strings not equal    
		if (!*s1)
			return 0;               // strings are equal
		s1++;
		s2++;
	}

	return -1;
}

int Q_strncmp( char* s1, char* s2, int count )
{
	while (1)
	{
		if (!count--)
			return 0;
		if (*s1 != *s2)
			return -1;              // strings not equal    
		if (!*s1)
			return 0;               // strings are equal
		s1++;
		s2++;
	}
	
	return -1;
}

int Q_strncasecmp( char* s1, char* s2, int n )
{
	int             c1, c2;

	while (1)
	{
		c1 = *s1++;
		c2 = *s2++;

		if (!n--)
			return 0;               // strings are equal until end point

		if (c1 != c2)
		{
			if (c1 >= 'a' && c1 <= 'z')
				c1 -= ('a' - 'A');
			if (c2 >= 'a' && c2 <= 'z')
				c2 -= ('a' - 'A');
			if (c1 != c2)
				return -1;              // strings not equal
		}
		if (!c1)
			return 0;               // strings are equal
//              s1++;
//              s2++;
	}

	return -1;
}

int Q_strcasecmp( char* s1, char* s2 )
{
	return Q_strncasecmp(s1, s2, 99999);
}

int Q_atoi( char* str )
{
	int		val;
	int		sign;
	int		c;

	if (*str == '-')
	{
		sign = -1;
		str++;
	}
	else
		sign = 1;

	val = 0;

//
// check for hex
//
	if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
	{
		str += 2;
		while (1)
		{
			c = *str++;
			if (c >= '0' && c <= '9')
				val = (val << 4) + c - '0';
			else if (c >= 'a' && c <= 'f')
				val = (val << 4) + c - 'a' + 10;
			else if (c >= 'A' && c <= 'F')
				val = (val << 4) + c - 'A' + 10;
			else
				return val * sign;
		}
	}

//
// check for character
//
	if (str[0] == '\'')
	{
		return sign * str[1];
	}

//
// assume decimal
//
	while (1)
	{
		c = *str++;
		if (c < '0' || c > '9')
			return val * sign;
		val = val * 10 + c - '0';
	}

	return 0;
}


float Q_atof( char* str )
{
	double	val;
	int		sign;
	int		c;
	int		decimal, total;

	if (*str == '-')
	{
		sign = -1;
		str++;
	}
	else
		sign = 1;

	val = 0;

//
// check for hex
//
	if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
	{
		str += 2;
		while (1)
		{
			c = *str++;
			if (c >= '0' && c <= '9')
				val = (val * 16) + c - '0';
			else if (c >= 'a' && c <= 'f')
				val = (val * 16) + c - 'a' + 10;
			else if (c >= 'A' && c <= 'F')
				val = (val * 16) + c - 'A' + 10;
			else
				return val * sign;
		}
	}

//
// check for character
//
	if (str[0] == '\'')
	{
		return sign * str[1];
	}

//
// assume decimal
//
	decimal = -1;
	total = 0;
	while (1)
	{
		c = *str++;
		if (c == '.')
		{
			decimal = total;
			continue;
		}
		if (c < '0' || c > '9')
			break;
		val = val * 10 + c - '0';
		total++;
	}

	if (decimal == -1)
		return val * sign;
	while (total > decimal)
	{
		val /= 10;
		total--;
	}

	return val * sign;
}

/*
==============================
Q_FileNameCmp

this is a specific string compare to use for filenames.
 it treats all slashes as the same character. so you can compare
paths.  (Quake uses / internally, while NT uses \ for it's path-seperator thing)
NOTE: this function uses the same return value conventions as strcmp.
so 0 signals a match.
==============================
*/
int Q_FileNameCmp( char* file1, char* file2 )
{
	do
	{
		if (*file1 == '/' && *file2 == '\\')
			continue;
		if (tolower(*file1) != tolower(*file2))
			return -1;
		if (!*file1)
			return 0;
	} while (*file1++ && *file2++);
	return 0; // fixed bogus warning
}

/*
============================================================================

					BYTE ORDER FUNCTIONS

============================================================================
*/

qboolean bigendien;

short	(*BigShort) (short l);
short	(*LittleShort) (short l);
int		(*BigLong) (int l);
int		(*LittleLong) (int l);
float	(*BigFloat) (float l);
float	(*LittleFloat) (float l);

short ShortSwap( short l )
{
	byte    b1, b2;

	b1 = l & 255;
	b2 = (l >> 8) & 255;

	return (b1 << 8) + b2;
}

short ShortNoSwap( short l )
{
	return l;
}

int LongSwap( int l )
{
	byte    b1, b2, b3, b4;

	b1 = l & 255;
	b2 = (l >> 8) & 255;
	b3 = (l >> 16) & 255;
	b4 = (l >> 24) & 255;

	return ((int)b1 << 24) + ((int)b2 << 16) + ((int)b3 << 8) + b4;
}

int LongNoSwap( int l )
{
	return l;
}

float FloatSwap( float f )
{
	union
	{
		float	f;
		byte	b[4];
	} dat1, dat2;


	dat1.f = f;
	dat2.b[0] = dat1.b[3];
	dat2.b[1] = dat1.b[2];
	dat2.b[2] = dat1.b[1];
	dat2.b[3] = dat1.b[0];
	return dat2.f;
}

float FloatNoSwap( float f )
{
	return f;
}

/*
==============================================================================

			MESSAGE IO FUNCTIONS

Handles byte ordering and avoids alignment errors
==============================================================================
*/

//
// writing functions
//

void MSG_WriteChar( sizebuf_t* sb, int c )
{
	byte* buf;

#ifdef PARANOID
	if (c < -128 || c > 127)
		Sys_Error("MSG_WriteChar: range error");
#endif

	buf = SZ_GetSpace(sb, 1);
	buf[0] = c;
}

void MSG_WriteByte( sizebuf_t* sb, int c )
{
	byte* buf;

#ifdef PARANOID
	if (c < 0 || c > 255)
		Sys_Error("MSG_WriteByte: range error");
#endif

	buf = SZ_GetSpace(sb, 1);
	buf[0] = c;
}

void MSG_WriteShort( sizebuf_t* sb, int c )
{
	byte* buf;

#ifdef PARANOID
	if (c < ((short)0x8000) || c >(short)0x7fff)
		Sys_Error("MSG_WriteShort: range error");
#endif

	buf = SZ_GetSpace(sb, 2);
	buf[0] = c & 0xff;
	buf[1] = c >> 8;
}

void MSG_WriteWord( sizebuf_t* sb, int c )
{
	byte* buf;

	buf = SZ_GetSpace(sb, 2);
	buf[0] = c & 0xff;
	buf[1] = (c >> 8) & 0xff;
}

void MSG_WriteLong( sizebuf_t* sb, int c )
{
	byte* buf;

	buf = (byte*)SZ_GetSpace(sb, 4);
	buf[0] = c & 0xff;
	buf[1] = (c >> 8) & 0xff;
	buf[2] = (c >> 16) & 0xff;
	buf[3] = c >> 24;
}

void MSG_WriteFloat( sizebuf_t* sb, float f )
{
	union
	{
		float   f;
		int     l;
	} dat;


	dat.f = f;
	dat.l = LittleLong(dat.l);

	SZ_Write(sb, &dat.l, 4);
}

void MSG_WriteString( sizebuf_t* sb, char* s )
{
	if (!s)
		SZ_Write(sb, "", 1);
	else
		SZ_Write(sb, s, Q_strlen(s) + 1);
}

void MSG_WriteBuf( sizebuf_t* sb, int iSize, void* buf )
{
	if (!buf)
		return;

	SZ_Write(sb, buf, iSize);
}

void MSG_WriteCoord( sizebuf_t* sb, float f )
{
	MSG_WriteShort(sb, (int)(f * 8.0));
}

void MSG_WriteAngle( sizebuf_t* sb, float f )
{
	MSG_WriteByte(sb, ((int)f * 256 / 360) & 255);
}

void MSG_WriteHiresAngle( sizebuf_t* sb, float f )
{
	MSG_WriteShort(sb, ((int)f * 65536 / 360) & 65535);
}

void MSG_WriteUsercmd( sizebuf_t* buf, usercmd_t* from, usercmd_t* cmd )
{
	int		bits;

//
// send the movement message
//
	bits = 0;
	if (cmd->angles[0] != from->angles[0])
		bits |= CM_ANGLE1;
	if (cmd->angles[1] != from->angles[1])
		bits |= CM_ANGLE2;
	if (cmd->angles[2] != from->angles[2])
		bits |= CM_ANGLE3;
	if (cmd->forwardmove != from->forwardmove)
		bits |= CM_FORWARD;
	if (cmd->sidemove != from->sidemove)
		bits |= CM_SIDE;
	if (cmd->upmove != from->upmove)
		bits |= CM_UP;
	if (cmd->buttons != from->buttons)
		bits |= CM_BUTTONS;
	if (cmd->impulse != from->impulse)
		bits |= CM_IMPULSE;

	MSG_WriteByte(buf, bits);

	if (bits & CM_ANGLE1)
		MSG_WriteHiresAngle(buf, from->angles[0]);
	if (bits & CM_ANGLE2)
		MSG_WriteHiresAngle(buf, from->angles[1]);
	if (bits & CM_ANGLE3)
		MSG_WriteAngle(buf, from->angles[2]);

	if (bits & CM_FORWARD)
		MSG_WriteFloat(buf, from->forwardmove);
	if (bits & CM_SIDE)
		MSG_WriteFloat(buf, from->sidemove);
	if (bits & CM_UP)
		MSG_WriteFloat(buf, from->upmove);

	if (bits & CM_BUTTONS)
		MSG_WriteShort(buf, from->buttons);
	if (bits & CM_IMPULSE)
		MSG_WriteByte(buf, from->impulse);
	MSG_WriteByte(buf, from->lightlevel);
	MSG_WriteByte(buf, from->msec);
}

//
// reading functions
//
int				msg_readcount;
qboolean		msg_badread;

void MSG_BeginReading( void )
{
	msg_readcount = 0;
	msg_badread = FALSE;
}

// returns -1 and sets msg_badread if no more characters are available
int MSG_ReadChar( void )
{
	int     c;

	if (msg_readcount + 1 > net_message.cursize)
	{
		msg_badread = TRUE;
		return -1;
	}

	c = (signed char)net_message.data[msg_readcount];
	msg_readcount++;

	return c;
}

int MSG_ReadByte( void )
{
	int     c;

	if (msg_readcount + 1 > net_message.cursize)
	{
		msg_badread = TRUE;
		return -1;
	}

	c = (unsigned char)net_message.data[msg_readcount];
	msg_readcount++;

	return c;
}

int MSG_ReadShort( void )
{
	int     c;

	if (msg_readcount + 2 > net_message.cursize)
	{
		msg_badread = TRUE;
		return -1;
	}

	c = (short)(net_message.data[msg_readcount]
	+ (net_message.data[msg_readcount + 1] << 8));

	msg_readcount += 2;

	return c;
}

int MSG_ReadWord( void )
{
	int     c;

	if (msg_readcount + 2 > net_message.cursize)
	{
		msg_badread = TRUE;
		return -1;
	}

	c = net_message.data[msg_readcount]
	+ (net_message.data[msg_readcount + 1] << 8);

	msg_readcount += 2;

	return c;
}

int MSG_ReadLong( void )
{
	int     c;

	if (msg_readcount + 4 > net_message.cursize)
	{
		msg_badread = TRUE;
		return -1;
	}

	c = net_message.data[msg_readcount]
	+ (net_message.data[msg_readcount + 1] << 8)
	+ (net_message.data[msg_readcount + 2] << 16)
	+ (net_message.data[msg_readcount + 3] << 24);

	msg_readcount += 4;

	return c;
}

float MSG_ReadFloat( void )
{
	union
	{
		byte	b[4];
		float	f;
		int	l;
	} dat;

	dat.b[0] = net_message.data[msg_readcount];
	dat.b[1] = net_message.data[msg_readcount + 1];
	dat.b[2] = net_message.data[msg_readcount + 2];
	dat.b[3] = net_message.data[msg_readcount + 3];
	msg_readcount += 4;

	dat.l = LittleLong(dat.l);

	return dat.f;
}

int MSG_ReadBuf( int iSize, void* pbuf )
{
	if (msg_readcount + iSize > net_message.cursize)
	{
		msg_badread = TRUE;
		return -1;
	}

	memcpy(pbuf, &net_message.data[msg_readcount], iSize);
	msg_readcount += iSize;

	return 1;
}

char* MSG_ReadString( void )
{
	static char     string[2048];
	int             l, c;

	l = 0;
	do
	{
		c = MSG_ReadChar();
		if (c == -1 || c == 0)
			break;
		string[l] = c;
		l++;
	} while (l < sizeof(string) - 1);

	string[l] = 0;

	return string;
}

char* MSG_ReadStringLine( void )
{
	static char     string[2048];
	int             l, c;

	l = 0;
	do
	{
		c = MSG_ReadChar();
		if (c == -1 || c == 0 || c == '\n')
			break;
		string[l] = c;
		l++;
	} while (l < sizeof(string) - 1);

	string[l] = 0;

	return string;
}

float MSG_ReadCoord( void )
{
	return MSG_ReadShort() * (1.0 / 8);
}

float MSG_ReadAngle( void )
{
	return MSG_ReadChar() * (360.0 / 256);
}

float MSG_ReadHiresAngle( void )
{
	return MSG_ReadShort() * (360.0 / 65536);
}

void MSG_ReadUsercmd( usercmd_t* from, usercmd_t* move )
{
	int bits;

	memcpy(from, move, sizeof(usercmd_t));

	bits = MSG_ReadByte();

// read current angles
	if (bits & CM_ANGLE1)
		from->angles[0] = MSG_ReadHiresAngle();
	if (bits & CM_ANGLE2)
		from->angles[1] = MSG_ReadHiresAngle();
	if (bits & CM_ANGLE3)
		from->angles[2] = MSG_ReadAngle();

// read movement
	if (bits & CM_FORWARD)
		from->forwardmove = MSG_ReadFloat();
	if (bits & CM_SIDE)
		from->sidemove = MSG_ReadFloat();
	if (bits & CM_UP)
		from->upmove = MSG_ReadFloat();

// read buttons
	if (bits & CM_BUTTONS)
		from->buttons = MSG_ReadShort();

	if (bits & CM_IMPULSE)
		from->impulse = MSG_ReadByte();

// read lightlevel
	from->lightlevel = MSG_ReadByte();

// read time to run command
	from->msec = MSG_ReadByte();
}


//===========================================================================

void SZ_Alloc( sizebuf_t* buf, int startsize )
{
	if (startsize < 256)
		startsize = 256;
	buf->data = Hunk_AllocName(startsize, "sizebuf");
	buf->maxsize = startsize;
	buf->cursize = 0;
}

void SZ_Clear( sizebuf_t* buf )
{
	buf->overflowed = FALSE;
	buf->cursize = 0;
}

void* SZ_GetSpace( sizebuf_t* buf, int length )
{
	void* data;

	if (buf->cursize + length > buf->maxsize)
	{
		if (!buf->allowoverflow)
		{
			if (!buf->maxsize)
				Sys_Error("SZ_GetSpace:  Tried to write to an uninitialized sizebuf_t");

			Sys_Error("SZ_GetSpace: overflow without allowoverflow set");
		}

		if (length > buf->maxsize)
			Sys_Error("SZ_GetSpace: %i is > full buffer size", length);

		Con_Printf("SZ_GetSpace: overflow\n");

		SZ_Clear(buf);
		buf->overflowed = TRUE;
	}

	data = buf->data + buf->cursize;
	buf->cursize += length;

	return data;
}

void SZ_Write( sizebuf_t* buf, void* data, int length )
{
	Q_memcpy(SZ_GetSpace(buf, length), data, length);
}

void SZ_Print( sizebuf_t* buf, char* data )
{
	int             len;

	len = Q_strlen(data) + 1;

// byte * cast to keep VC++ happy
	if (buf->data[buf->cursize - 1])
		Q_memcpy((byte*)SZ_GetSpace(buf, len), data, len); // no trailing 0
	else
		Q_memcpy((byte*)SZ_GetSpace(buf, len - 1) - 1, data, len); // write over trailing 0
}


//============================================================================


/*
============
COM_SkipPath
============
*/
char* COM_SkipPath( char* pathname )
{
	char* last;

	last = pathname;
	while (*pathname)
	{
		if (*pathname == '/')
			last = pathname + 1;
		pathname++;
	}
	return last;
}

/*
============
COM_StripExtension
============
*/
void COM_StripExtension( char* in, char* out )
{
	while (*in && *in != '.')
		*out++ = *in++;
	*out = 0;
}

/*
============
COM_FileExtension
============
*/
char* COM_FileExtension( char* in )
{
	static char exten[8];
	int		i;

	while (*in && *in != '.')
		in++;
	if (!*in)
		return "";
	in++;
	for (i = 0; i < 7 && *in; i++, in++)
		exten[i] = *in;
	exten[i] = 0;
	return exten;
}

/*
============
COM_FileBase
============
*/
// Extracts the base name of a file (no path, no extension, assumes '/' as path separator)
void COM_FileBase( char* in, char* out )
{
	int len, start, end;

	len = strlen(in);

	// scan backward for '.'
	end = len - 1;
	while (end && in[end] != '.')
		end--;

	if (in[end] != '.')		// no '.', copy to end
		end = len - 1;
	else
		end--;					// Found ',', copy to left of '.'


	// Scan backward for '/'
	start = len - 1;
	while (start >= 0 && in[start] != '/')
		start--;

	if (start < 0 || (in[start] != '/'))
		start = 0;
	else
		start++;

	// Length of new sting
	len = end - start + 1;

	// Copy partial string
	strncpy(out, &in[start], len);
	out[len] = '\0';
}


/*
==================
COM_DefaultExtension
==================
*/
void COM_DefaultExtension( char* path, char* extension )
{
	char* src;
//
// if path doesn't have a .EXT, append extension
// (extension should include the .)
//
	src = path + strlen(path) - 1;

	while (*src != '/' && src != path)
	{
		if (*src == '.')
			return;                 // it has an extension
		src--;
	}

	strcat(path, extension);
}


/*
==============
COM_Parse

Parse a token out of a string
==============
*/
char* COM_Parse( char* data )
{
	int             c;
	int             len;

	len = 0;
	com_token[0] = 0;

	if (!data)
		return NULL;

	// skip whitespace
skipwhite:
	while ((c = *data) <= ' ')
	{
		if (c == 0)
			return NULL;                    // end of file;
		data++;
	}

	// skip // comments
	if (c == '/' && data[1] == '/')
	{
		while (*data && *data != '\n')
			data++;
		goto skipwhite;
	}


	// handle quoted strings specially
	if (c == '\"')
	{
		data++;
		while (1)
		{
			c = *data++;
			if (c == '\"' || !c)
			{
				com_token[len] = 0;
				return data;
			}
			com_token[len] = c;
			len++;
		}
	}

	// parse single characters
	if (c == '{' || c == '}' || c == ')' || c == '(' || c == '\'' || c == ':')
	{
		com_token[len] = c;
		len++;
		com_token[len] = 0;
		return data + 1;
	}

	// parse a regular word
	do
	{
		com_token[len] = c;
		data++;
		len++;
		c = *data;
		if (c == '{' || c == '}' || c == ')' || c == '(' || c == '\'' || c == ':')
			break;
	} while (c > 32);

	com_token[len] = 0;
	return data;
}


/*
================
COM_CheckParm

Returns the position (1 to argc-1) in the program's argument list
where the given parameter apears, or 0 if not present
================
*/
int COM_CheckParm( char* parm )
{
	int             i;

	for (i = 1; i < com_argc; i++)
	{
		if (!com_argv[i])
			continue;               // NEXTSTEP sometimes clears appkit vars.
		if (!Q_strcmp(parm, com_argv[i]))
			return i;
	}

	return 0;
}

DLL_EXPORT int COM_CheckParmEx( char* parm, char** argv )
{
	int             i;

	for (i = 1; i < com_argc; i++)
	{
		if (!com_argv[i])
			continue;               // NEXTSTEP sometimes clears appkit vars.
		if (!Q_strcmp(parm, com_argv[i]))
		{
			if (argv)
			{
				if (i < com_argc - 1)
				{
					*argv = com_argv[i + 1];
					return i;
				}
				*argv = 0;
			}

			return i;
		}
	}

	return 0;
}

/*
================
COM_CheckRegistered

Looks for the pop.txt file and verifies it.
Sets the "registered" cvar.
Immediately exits out if an alternate game was attempted to be started without
being registered.
================
*/
void COM_CheckRegistered( void )
{
/*
	int             h;
	unsigned short  check[128];
	int                     i;

	COM_OpenFile("gfx/pop.lmp", &h);
	static_registered = 0;

	if (h == -1)
	{
#if WINDED
		Sys_Error("This dedicated server requires a full registered copy of Quake");
#endif
		Con_Printf("Playing shareware version.\n");
		if (com_modified)
			Sys_Error("You must have the registered version to use modified games");
		return;
	}

	Sys_FileRead(h, check, sizeof(check));
	COM_CloseFile(h);

	for (i = 0; i < 128; i++)
		if (pop[i] != (unsigned short)BigShort(check[i]))
			Sys_Error("Corrupted data file.");

	Cvar_Set("cmdline", com_cmdline);
	Cvar_Set("registered", "1");
	static_registered = 1;
	Con_Printf("Playing registered version.\n");
*/
}


/*
================
COM_InitArgv
================
*/
void COM_InitArgv( int argc, char** argv )
{
	qboolean        safe;
	int             i, j, n;

// reconstitute the command line for the cmdline externally visible cvar
	n = 0;

	for (j = 0; (j < MAX_NUM_ARGVS) && (j < argc); j++)
	{
		i = 0;

		while ((n < (CMDLINE_LENGTH - 1)) && argv[j][i])
		{
			com_cmdline[n++] = argv[j][i++];
		}

		if (n < (CMDLINE_LENGTH - 1))
			com_cmdline[n++] = ' ';
		else
			break;
	}

	com_cmdline[n] = 0;

	safe = FALSE;

	for (com_argc = 0; (com_argc < MAX_NUM_ARGVS) && (com_argc < argc);
		com_argc++)
	{
		largv[com_argc] = argv[com_argc];
		if (!Q_strcmp("-safe", argv[com_argc]))
			safe = TRUE;
	}

	if (safe)
	{
	// force all the safe-mode switches. Note that we reserved extra space in
	// case we need to add these, so we don't need an overflow check
		for (i = 0; i < NUM_SAFE_ARGVS; i++)
		{
			largv[com_argc] = safeargvs[i];
			com_argc++;
		}
	}

	largv[com_argc] = argvdummy;
	com_argv = largv;

	if (COM_CheckParm("-rogue"))
	{
		rogue = TRUE;
		standard_quake = FALSE;
	}

	if (COM_CheckParm("-hipnotic"))
	{
		hipnotic = TRUE;
		standard_quake = FALSE;
	}
}





// TODO: Implement