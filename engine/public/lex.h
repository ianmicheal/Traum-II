#pragma once

#include <stdlib.h>
#include <setjmp.h>

#define LEX_UNK			-2
#define LEX_EOF			-1
#define LEX_SYMBOL		0
#define LEX_STRING		1
#define LEX_IDENT		2
#define LEX_NUMBER		3

#define LEX_ERR_TRUNC	-1

#define LEX_MAX_TOKEN	64
typedef struct _Lex_state
{
	const char *script;
	const char *pos;
	int error;
	int type;
	size_t len;
	char token[LEX_MAX_TOKEN];
}Lex_state;

static inline const char *
lex_token (Lex_state *lex, size_t *len)
{
	if (len != NULL) *len = lex->len;
	return lex->token;
}
int lex_next (Lex_state *lex);
void lex_init (Lex_state *lex, const char *script);

/*Should be put in its own file, but hey we're punks*/
typedef struct _Parser_state
{
	Lex_state lex;
	int (*trace) (const char *restrict, ...);
	jmp_buf err;
}Parser_state;

int parser_check (Parser_state *ps, const char *str);
int parser_expect (Parser_state *ps, const char *str);
const char *parser_expect_type (Parser_state *ps, int type);
void parser_consume (Parser_state *ps);
void parser_init (Parser_state *ps, const char *script);
