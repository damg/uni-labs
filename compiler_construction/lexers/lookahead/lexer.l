/* -*- mode: text -*- */
%option noyywrap

%{

#include <stdio.h>
#include <string.h>
#include <ctype.h>

char prev_char = 0;
unsigned int counts[26][26];

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
\n			{ prev_char = 0; }

%%

#include "../common.inc.c"
