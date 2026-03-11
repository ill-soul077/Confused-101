%{
#include <stdio.h>
%}
%%
"hello"  { printf("got hello\n"); }
%%
int yywrap() { return 1; }