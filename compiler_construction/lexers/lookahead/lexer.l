/* -*- mode: text -*- */
%option noyywrap

%{

#include <stdio.h>
#include <string.h>

char prev_char = 0;
unsigned int counts[26*2][26*2];

void handle_digram(char c1, char c2);

%}

letter [A-Za-z]
ws [ \t\r\n]

%%

{letter}/{letter}	{ if (prev_char)
			    handle_digram(prev_char, yytext[0]);
			  prev_char = yytext[0];
			}

{letter}		{ if (prev_char)
			    handle_digram(prev_char, yytext[0]);
                          prev_char = 0;
			}

.			{ prev_char = 0; }

%%

void print_result(void);

int main(int argc, char **argv)
{
	memset(counts, 0, sizeof(counts));
	yylex();
	print_result();
	return 0;
}

size_t get_count_index_by_char(char c)
{
	if (c >= 'a' && c <= 'z')
	   return c - 'a';

	if (c >= 'A' && c <= 'Z')
	   return c - 'A' + 26;
}

char get_char_by_count_index(size_t i)
{
	if (i >= 0 && i < 26)
	   return 'a' + i;

	if (i >= 26 && i < (26*2))
	   return 'A'+i-26;

	return 0;
}

void handle_digram(char c1, char c2)
{
	size_t i, j;

	i = get_count_index_by_char(c1);
	j = get_count_index_by_char(c2);

	counts[i][j]++;
}

void print_result(void)
{
	size_t i, j;
	for(i = 0; i < 26*2; ++i)
	  for(j = 0; j < 26*2; ++j)
	    if (counts[i][j])
	       printf("%c%c: %d\n", get_char_by_count_index(i), get_char_by_count_index(j), counts[i][j]);
}
