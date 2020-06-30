#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "public/lex.h"

static inline int
isnumeric (int c)
{
	return ('0' <= c && c <= '9') || '.' == c;
}
static inline int
isprintable (int c)
{
	return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') 
			|| ('0' <= c && c <= '9') || '_' == c;
}
int
lex_next (Lex_state *lex)
{	/*Clear the state*/
	lex->error = 0;
	lex->len = 0;
	lex->token[0] = '\0';
	/*Parse out the next one from the stream*/
	while (*lex->pos)
	{	/*Skip whitespace and comments*/
		if (*lex->pos <= ' ')
		{
			lex->pos++;
			continue;
		}
		if ('#' == *lex->pos)
		{
			do
			{
				if (*lex->pos == '\0')
				{
					break;
				}
				lex->pos++;
			}
			while (*lex->pos != '\n');
			continue;
		}
		/*Symbols*/
		switch (*lex->pos)
		{
		case '{':
		case '}':
		case ',':
			lex->token[lex->len++] = *lex->pos;
			lex->token[lex->len] = '\0';
			lex->pos++;
			lex->type = LEX_SYMBOL;
			return lex->type;
		}
		/*Numbers*/
		if ('.' == lex->pos[0] && isnumeric (lex->pos[1]))
		{
			lex->token[lex->len++] = *lex->pos;
			lex->token[lex->len] = '\0';
			lex->pos++;
			goto Numeric;
		}
		if (isnumeric (*lex->pos))
		{
		Numeric:
			do
			{
				if (lex->len < LEX_MAX_TOKEN-1)
				{
					lex->token[lex->len++] = *lex->pos;
					lex->token[lex->len] = '\0';
				}
				else lex->error = LEX_ERR_TRUNC;
				lex->pos++;
			}
			while (isnumeric (*lex->pos));
			lex->type = LEX_NUMBER;
			return lex->type;
		}
		/*Strings*/
		if ('\"' == *lex->pos)
		{
			do
			{
				if ('\"' == *(++lex->pos))
				{
					lex->pos++;
					break;
				}
				if (lex->len < LEX_MAX_TOKEN-1)
				{
					lex->token[lex->len++] = *lex->pos;
					lex->token[lex->len] = '\0';
				}
				else lex->error = LEX_ERR_TRUNC;
			}
			while (*lex->pos);
			lex->type = LEX_STRING;
			return lex->type;
		}
		/*Identifier*/
		if (isprintable (*lex->pos))
		{
			do
			{
				if (lex->len < LEX_MAX_TOKEN-1)
				{
					lex->token[lex->len++] = *lex->pos;
					lex->token[lex->len] = '\0';
				}
				else lex->error = LEX_ERR_TRUNC;
				lex->pos++;
			}
			while (isprintable (*lex->pos));
			lex->type = LEX_IDENT;
			return lex->type;
		}
		/*Unknown*/
		lex->type = LEX_UNK;
		return lex->type;
	}
	lex->type = LEX_EOF;
	return lex->type;
}
void
lex_init (Lex_state *lex, const char *script)
{
	memset (lex, 0, sizeof (*lex));
	lex->script = lex->pos = script;
}

int
parser_check (Parser_state *ps, const char *str)
{
	if (strcmp (str, lex_token (&ps->lex, NULL)))
	{
		return 0;
	}
	lex_next (&ps->lex);
	return 1;
}
int
parser_expect (Parser_state *ps, const char *str)
{
	const char *token = lex_token (&ps->lex, NULL);
	if (strcmp (str, token))
	{
		ps->trace ("Expected '%s'; got '%s'.\n", str, token);
		longjmp (ps->err, -1);
		return 0;
	}
	lex_next (&ps->lex);
	return 1;
}
const char *
parser_expect_type (Parser_state *ps, int type)
{
	static const char *t[] = {"symbol", "string", "ident", "number"};
	if (ps->lex.type != type)
	{
		ps->trace ("Expected %s; got %s.\n", t[type], t[ps->lex.type]);
		longjmp (ps->err, -1);
		return NULL;
	}
	return ps->lex.token;
}
void
parser_consume (Parser_state *ps)
{
	lex_next (&ps->lex);
}
void
parser_init (Parser_state *ps, const char *script)
{
	memset (ps, 0, sizeof (*ps));
	lex_init (&ps->lex, script);
	ps->trace = printf;
	parser_consume (ps);
}
