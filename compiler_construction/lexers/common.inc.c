#ifndef __COM_COMMON_INC_C__
#define __COM_COMMON_INC_C__

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
	return c-'a';
}

char get_char_by_count_index(size_t i)
{
	return i + 'a';
}

void handle_digram(char c1, char c2)
{
	c1 = tolower(c1);
	c2 = tolower(c2);

	size_t i, j;

	i = get_count_index_by_char(c1);
	j = get_count_index_by_char(c2);

	counts[i][j]++;
}

void print_result(void)
{
	size_t i, j;
	for(i = 0; i < 26; ++i)
	  for(j = 0; j < 26; ++j)
	    if (counts[i][j])
	       printf("%c%c: %d\n", get_char_by_count_index(i), get_char_by_count_index(j), counts[i][j]);
}

#endif
