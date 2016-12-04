%{
#include <stdio.h>
#include <stdlib.h>
#include "y.tab.h"

extern int is_fin;
extern int stddbmsstart;
int lineno = 1;
void yyerror(char *s);
char *strdupxy(char *s);
%}
%%
	/* reserved keywords */
CREATE	|
create		return CREATE;
TABLE	|
table		return TABLE;
DROP	|
drop		return DROP;
INDEX	|
index		return INDEX;
SELECT	|
select		return SELECT;
FROM	|
from		return FROM;
WHERE	|
where		return WHERE;
ORDER	|
order		return ORDER;
BY	|
by		return BY;
ASC	|
asc		return ASC;
DESC	|
desc		return DESC;
UNIQUE	|
unique		return UNIQUE;
ALL	|
all		return ALL;
DISTINCT |
distinct	return DISTINCT;
INSERT	|
insert		return INSERT;
INTO	|
into		return INTO;
VALUES	|
values		return VALUES;
DELETE	|
delete		return DELETE;
AND	|
and		return AND;
INT(EGER)? |
int(eger)?	return INTEGER;
CHAR(ACTER)? |
char(acter)?	return CHARACTER;
EXIT	|
exit	|
QUIT	|
quit		return EXIT;
DATE	|
date		return DATE;
SHOW	|
show		return SHOW;
TABLES	|
tables		return TABLES;

	/* punctation */
"="	|
"<>"	|
"<"	|
">"	|
"<="	|
">="		yylval.strval = strdupxy(yytext); return COMPARISION;
[-+*/:(),.;]	return yytext[0];

	/* names */
[a-zA-Z][a-zA-Z0-9_]*	yylval.strval = strdupxy(yytext); return NAME;

	/* numbers */
[0-9]+		yylval.intval = atoi(yytext); return NUMBER;

	/* strings */
'[^'\n]*'	yylval.strval = strdupxy(yytext); strcpy(yylval.strval, yytext + 1); *(yylval.strval + strlen(yytext) - 2) = '\0'; return STRING;

	/* whitespace */
\n		if (stddbmsstart == 0 || is_fin == 1) /* printf("       \033[34m>\033[0m ") */ printf("       > "); stddbmsstart = is_fin = 1; lineno++;
[ \t\r]+	;

	/* anything else */
.		yyerror("invalid character");
%%

void yyerror(char *s)
{
	/* printf("%d: %s at %s\n", lineno, s, yytext); */
	printf("SQL syntax error....\n");
}

/* make a duplicate of s */
char *strdupxy(char *s)
{
	char *p;
	p = (char *) malloc(strlen(s) + 1);
	if (p != NULL)
		strcpy(p, s);
	return p;
}

int yywrap()
{
	extern int ctb;
	extern int inittb();
	if (yyin == stdin) {
		return 1;
	} else {
		fclose(yyin);
		if (ctb == 1) {
			inittb();
			ctb = 0;
			/* color prompt
			printf("\033[1;33ms\033[1;31mt\033[1;32md\033[1;34mD\033[1;33mB\033[1;36mM\033[1;35mS\033[0;34m>\033[0m ");
			*//* no color prompt */
			printf("stdDBMS> ");
		}
		yyin = stdin;
		return 0;
	}
}
