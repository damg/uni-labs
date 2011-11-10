/* -*- mode: text -*- */
%option noyywrap

%{
#include <stdio.h>
#include <string.h>
#include <ctype.h>

unsigned int counts[26][26];

void handle_digram(char c1, char c2);

%}

letter [A-Za-z]
ws [ \t\r\n]

%%

{letter}{letter}	{ handle_digram(yytext[0], yytext[1]);
			  REJECT;
			}

.			{ }
\n			{ }

%%

#include "../common.inc.c"
