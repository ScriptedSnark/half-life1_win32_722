#include "quakedef.h"
#include "cdll_int.h"
#include "draw.h"
#include "tmessage.h"
#include "cl_demo.h"

#define MSGFILE_NAME		0
#define MSGFILE_TEXT		1
#define MAX_MESSAGES		200				// I don't know if this table will balloon like every other feature in Half-Life
												// But, for now, I've set this to a reasonable value

client_textmessage_t	gMessageParms;
client_textmessage_t*	gMessageTable = NULL;
int						gMessageTableCount = 0;

static void TextMessageParse( byte* pMemFile, int fileSize );

// Copied from sound.cpp in the DLL
char* memfgets( byte* pMemFile, int fileSize, int* pFilePos, char* pBuffer, int bufferSize )
{
	int filePos = *pFilePos;
	int i, last, stop;

	// Bullet-proofing
	if (!pMemFile || !pBuffer)
		return NULL;

	if (filePos >= fileSize)
		return NULL;

	i = filePos;
	last = fileSize;

	// fgets always NULL terminates, so only read bufferSize-1 characters
	if (last - filePos > (bufferSize - 1))
		last = filePos + (bufferSize - 1);

	stop = 0;

	// Stop at the next newline (inclusive) or end of buffer
	while (i < last && !stop)
	{
		if (pMemFile[i] == '\n')
			stop = 1;
		i++;
	}


	// If we actually advanced the pointer, copy it over
	if (i != filePos)
	{
		// We read in size bytes
		int size = i - filePos;
		// copy it out
		memcpy(pBuffer, pMemFile + filePos, sizeof(unsigned char) * size);

		// If the buffer isn't full, terminate (this is always true)
		if (size < bufferSize)
			pBuffer[size] = 0;

		// Update file pointer
		*pFilePos = i;
		return pBuffer;
	}

	// No data read, bail
	return NULL;
}

// The string "pText" is assumed to have all whitespace from both ends cut out
int IsComment( char* pText )
{
	if (pText)
	{
		int length = strlen(pText);

		if (length >= 2 && pText[0] == '/' && pText[1] == '/')
			return 1;

		// No text?
		if (length > 0)
			return 0;
	}
	// No text is a comment too
	return 1;
}


// The string "pText" is assumed to have all whitespace from both ends cut out
int IsStartOfText( char* pText )
{
	if (pText)
	{
		if (pText[0] == '{')
			return 1;
	}
	return 0;
}


// The string "pText" is assumed to have all whitespace from both ends cut out
int IsEndOfText( char* pText )
{
	if (pText)
	{
		if (pText[0] == '}')
			return 1;
	}
	return 0;
}


int IsWhiteSpace( char space )
{
	if (space == ' ' || space == '\t' || space == '\r' || space == '\n')
		return 1;
	return 0;
}


const char* SkipSpace( const char* pText )
{
	if (pText)
	{
		int pos = 0;
		while (pText[pos] && IsWhiteSpace(pText[pos]))
			pos++;
		return pText + pos;
	}

	return NULL;
}


const char* SkipText( const char* pText )
{
	if (pText)
	{
		int pos = 0;
		while (pText[pos] && !IsWhiteSpace(pText[pos]))
			pos++;
		return pText + pos;
	}

	return NULL;
}


int ParseFloats( const char* pText, float* pFloat, int count )
{
	const char* pTemp = pText;
	int index = 0;

	while (pTemp && count > 0)
	{
		// Skip current token / float
		pTemp = SkipText(pTemp);
		// Skip any whitespace in between
		pTemp = SkipSpace(pTemp);
		if (pTemp)
		{
			// Parse a float
			pFloat[index] = (float)atof(pTemp);
			count--;
			index++;
		}
	}

	if (count == 0)
		return 1;

	return 0;
}


// Trims all whitespace from the front and end of a string
void TrimSpace( const char* source, char* dest )
{
	int start, end, length;

	start = 0;
	end = strlen(source);

	while (source[start] && IsWhiteSpace(source[start]))
		start++;

	end--;
	while (end > 0 && IsWhiteSpace(source[end]))
		end--;

	end++;

	length = end - start;
	if (length > 0)
		strncpy(dest, source + start, length);
	else
		length = 0;

	// Terminate the dest string
	dest[length] = 0;
}


int IsToken( const char* pText, const char* pTokenName )
{
	if (!pText || !pTokenName)
		return 0;

	if (!_strnicmp(pText + 1, pTokenName, strlen(pTokenName)))
		return 1;

	return 0;
}



int ParseDirective( const char* pText )
{
	if (pText && pText[0] == '$')
	{
		float tempFloat[8];

		if (IsToken(pText, "position"))
		{
			if (ParseFloats(pText, tempFloat, 2))
			{
				gMessageParms.x = tempFloat[0];
				gMessageParms.y = tempFloat[1];
			}
		}
		else if (IsToken(pText, "effect"))
		{
			if (ParseFloats(pText, tempFloat, 1))
			{
				gMessageParms.effect = (int)tempFloat[0];
			}
		}
		else if (IsToken(pText, "fxtime"))
		{
			if (ParseFloats(pText, tempFloat, 1))
			{
				gMessageParms.fxtime = tempFloat[0];
			}
		}
		else if (IsToken(pText, "color2"))
		{
			if (ParseFloats(pText, tempFloat, 3))
			{
				gMessageParms.r2 = (int)tempFloat[0];
				gMessageParms.g2 = (int)tempFloat[1];
				gMessageParms.b2 = (int)tempFloat[2];
			}
		}
		else if (IsToken(pText, "color"))
		{
			if (ParseFloats(pText, tempFloat, 3))
			{
				gMessageParms.r1 = (int)tempFloat[0];
				gMessageParms.g1 = (int)tempFloat[1];
				gMessageParms.b1 = (int)tempFloat[2];
			}
		}
		else if (IsToken(pText, "fadein"))
		{
			if (ParseFloats(pText, tempFloat, 1))
			{
				gMessageParms.fadein = tempFloat[0];
			}
		}
		else if (IsToken(pText, "fadeout"))
		{
			if (ParseFloats(pText, tempFloat, 3))
			{
				gMessageParms.fadeout = tempFloat[0];
			}
		}
		else if (IsToken(pText, "holdtime"))
		{
			if (ParseFloats(pText, tempFloat, 3))
			{
				gMessageParms.holdtime = tempFloat[0];
			}
		}
		else
		{
			Con_DPrintf("Unknown token: %s\n", pText);
		}

		return 1;
	}
	return 0;
}

void TextMessageInit( void )
{
	int fileSize;
	byte *pMemFile;

	pMemFile = COM_LoadTempFile("titles.txt", &fileSize);

	if (pMemFile)
		TextMessageParse(pMemFile, fileSize);
}

#define NAME_HEAP_SIZE 4096

void TextMessageParse( byte* pMemFile, int fileSize )
{
	char		buf[512], trim[512];
	char* pCurrentText = 0, * pNameHeap;
	char		currentName[512], nameHeap[NAME_HEAP_SIZE];
	int			lastNamePos;

	int			mode = MSGFILE_NAME;	// Searching for a message name	
	int			lineNumber, filePos, lastLinePos;
	int			messageCount;

	client_textmessage_t	textMessages[MAX_MESSAGES];

	int			i, nameHeapSize, textHeapSize, messageSize, nameOffset;

	lastNamePos = 0;
	lineNumber = 0;
	filePos = 0;
	lastLinePos = 0;
	messageCount = 0;

	while (memfgets(pMemFile, fileSize, &filePos, buf, 512) != NULL)
	{
		TrimSpace(buf, trim);
		switch (mode)
		{
		case MSGFILE_NAME:
			if (IsComment(trim))	// Skip comment lines
				break;

			if (ParseDirective(trim))	// Is this a directive "$command"?, if so parse it and break
				break;

			if (IsStartOfText(trim))
			{
				mode = MSGFILE_TEXT;
				pCurrentText = (char*)(pMemFile + filePos);
				break;
			}
			if (IsEndOfText(trim))
			{
				Con_DPrintf("Unexpected '}' found, line %d\n", lineNumber);
				return;
			}
			strcpy(currentName, trim);
			break;

		case MSGFILE_TEXT:
			if (IsEndOfText(trim))
			{
				int length = strlen(currentName);

				// Save name on name heap
				if ((lastNamePos + length) > sizeof(nameHeap))
				{
					Con_DPrintf("Error parsing file!\n");
					return;
				}
				strcpy(nameHeap + lastNamePos, currentName);

				// Terminate text in-place in the memory file (it's temporary memory that will be deleted)
				pMemFile[lastLinePos - 1] = 0;

				// Save name/text on heap
				textMessages[messageCount] = gMessageParms;
				textMessages[messageCount].pName = nameHeap + lastNamePos;
				lastNamePos += strlen(currentName) + 1;
				textMessages[messageCount].pMessage = pCurrentText;
				messageCount++;

				// Reset parser to search for names
				mode = MSGFILE_NAME;
				break;
			}
			if (IsStartOfText(trim))
			{
				Con_DPrintf("Unexpected '{' found, line %d\n", lineNumber);
				return;
			}
			break;
		}
		lineNumber++;
		lastLinePos = filePos;
	}

	Con_DPrintf("Parsed %d text messages\n", messageCount);
	nameHeapSize = lastNamePos;
	textHeapSize = 0;
	for (i = 0; i < messageCount; i++)
		textHeapSize += strlen(textMessages[i].pMessage) + 1;


	messageSize = (messageCount * sizeof(client_textmessage_t));

	// Must malloc because we need to be able to clear it after initialization
	gMessageTable = (client_textmessage_t*)Hunk_AllocName(textHeapSize + nameHeapSize + messageSize, "TextMessages");

	// Copy table over
	memcpy(gMessageTable, textMessages, messageSize);

	// Copy Name heap
	pNameHeap = ((char*)gMessageTable) + messageSize;
	memcpy(pNameHeap, nameHeap, nameHeapSize);
	nameOffset = pNameHeap - gMessageTable[0].pName;

	// Copy text & fixup pointers
	pCurrentText = pNameHeap + nameHeapSize;

	for (i = 0; i < messageCount; i++)
	{
		gMessageTable[i].pName += nameOffset;		// Adjust name pointer (parallel buffer)
		strcpy(pCurrentText, gMessageTable[i].pMessage);	// Copy text over
		gMessageTable[i].pMessage = pCurrentText;
		pCurrentText += strlen(pCurrentText) + 1;
	}

#if _DEBUG
	if ((pCurrentText - (char*)gMessageTable) != (textHeapSize + nameHeapSize + messageSize))
		Con_Printf("Overflow text message buffer!!!!!\n");
#endif
	gMessageTableCount = messageCount;
}

void SetDemoMessage( const char* pszMessage, float fFadeInTime, float fFadeOutTime, float fHoldTime )
{
	if (!pszMessage || !pszMessage[0])
		return;

	strcpy((char*)tm_demomessage.pMessage, (char*)pszMessage);
	tm_demomessage.fadein = fFadeInTime;
	tm_demomessage.fadeout = fFadeOutTime;
	tm_demomessage.holdtime = fHoldTime;
}

client_textmessage_t* TextMessageGet( const char* pName )
{
	int i;

	if (!_stricmp(pName, DEMO_MESSAGE))
	{
		return &tm_demomessage;
	}

	for (i = 0; i < gMessageTableCount; i++)
	{
		if (!strcmp(pName, gMessageTable[i].pName))
			return &gMessageTable[i];
	}

	return NULL;
}

// Draw a single character
int TextMessageDrawCharacter( int x, int y, int number, int r, int g, int b )
{
	if (r || g || b)
	{
		return Draw_MessageCharacterAdd(x, y, number, r, g, b);
	}
	return 0;
}