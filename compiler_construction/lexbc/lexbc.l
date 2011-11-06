/* -*- mode: text -*- */
%option noyywrap

%{
#include <stdio.h>
#include <string.h>

int calling_state;
int comment_deepness = 0;
int accu = 0;

typedef enum E_OP { E_OP_ADD='+', E_OP_SUB='-', E_OP_MUL='*' } E_OP;

E_OP op;

%}

digit  [0-9]
num    {digit}+
int    [+-]?{num}
sigint [+-]{num}
op     [+*-]
eq     =
ws     [ \t\r\n]

%s MLCOMMENT
%s OP_TAIL
%s AFTER_INT

%%

<INITIAL>{
"/*"				{ calling_state=INITIAL; BEGIN(MLCOMMENT); }
{int}				{ accu += atoi(yytext); BEGIN(AFTER_INT); }
}

<AFTER_INT>{
"/*"				{ calling_state=AFTER_INT; BEGIN(MLCOMMENT); }
{sigint}			{ accu += atoi(yytext); }
{op}				{ op = yytext[0]; BEGIN(OP_TAIL); }
}

<OP_TAIL>{
"/*"				{ calling_state=OP_TAIL; BEGIN(MLCOMMENT); }
{int}				{ handle_op_tail(); BEGIN(AFTER_INT); }
.				{ fprintf(stderr, "<^Melchior^> MY NAMES BUSH!... "
				  		  "HELP MY FIND IRAQ'S NUCLEAR WEAPONS, "
						  "ORDER YOUR VERY OWN \"FIND MR. SADDAMS WEAPONS "
						  "OF MASS DESTRUCTION DETECTION KIT\", Kit Includes: "
						  "Darts and a Map\n");
				  accu=0;
				  BEGIN(INITIAL);
				}
}

<MLCOMMENT>{
"/*"				{ comment_deepness++; }
"*/"				{ comment_deepness--;
				  if (!comment_deepness)
				    BEGIN(calling_state);
				}
.				{ }
}

<INITIAL,AFTER_INT>{
{eq}				{ printf("%d\n", accu); accu = 0; BEGIN(INITIAL); }
}

<INITIAL,AFTER_INT,OP_TAIL>{
{ws}				{ }
"//".*$				{ }
}

<INITIAL,MLCOMMENT,AFTER_INT,OP_TAIL>{
\n				{ }
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
	int num = atoi(yytext);
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
