/* -*- mode: text -*- */
%option noyywrap

%{
#include <stdio.h>
#include <string.h>

int calling_state;
double accu = 0;

typedef enum E_OP { E_OP_ADD='+', E_OP_SUB='-', E_OP_MUL='*' } E_OP;

E_OP op;

void handle_op_tail(void);

%}

dig    [0-9]
exp    [Ee][+-]?{dig}+
uflt   ({dig}+{exp})|({dig}*"."{dig}+{exp}?)|({dig}+"."{dig}*{exp}?)|({dig}+)
flt    [+-]?{uflt}
sigflt [+-]{uflt}+
op     [+*-]
eq     =
ws     [ \t\r\n]

%x MLCOMMENT
%x OP_TAIL
%x AFTER_FLT

%%

<INITIAL,AFTER_FLT,OP_TAIL>{
{ws}				{ }
"//".*$				{ }
}

<INITIAL,AFTER_FLT>{
{eq}				{ printf("%f\n", accu); accu = 0; BEGIN(INITIAL); }
}

<INITIAL,MLCOMMENT,AFTER_FLT,OP_TAIL>{
"\n"				{ }
}

<INITIAL>{
"/*"				{ calling_state=INITIAL; BEGIN(MLCOMMENT); }
{flt}				{ accu += atof(yytext); BEGIN(AFTER_FLT); }
}

<AFTER_FLT>{
"/*"				{ calling_state=AFTER_FLT; BEGIN(MLCOMMENT); }
{sigflt}			{ accu += atof(yytext); }
{op}				{ op = yytext[0]; BEGIN(OP_TAIL); }
.				{ fprintf(stderr, "Unexpected character %c. Restarting.\n", yytext[0]);
				  accu=0;
				  BEGIN(INITIAL);
				}
}

<OP_TAIL>{
"/*"				{ calling_state=OP_TAIL; BEGIN(MLCOMMENT); }
{flt}				{ handle_op_tail(); BEGIN(AFTER_FLT); }
.				{ fprintf(stderr, "Unexpected character %c. Restarting.\n", yytext[0]);
				  accu=0;
				  BEGIN(INITIAL);
				}
}

<MLCOMMENT>{
"/*"				{ }
"*/"				{ BEGIN(calling_state); }
.				{ }
}

%%

void print_result(void);

int main(int argc, char **argv)
{
	yylex();
	return 0;
}

void handle_op_tail(void)
{
	double num = atof(yytext);
	switch(op)
	{
	    case E_OP_ADD:
	      accu += num;
	      break;
	    case E_OP_SUB:
	      accu -= num;
	      break;
	    case E_OP_MUL:
	      accu *= num;
	      break;
	}
}
