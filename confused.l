%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "confused.tab.h"
%}

DIGIT   [0-9]
ID      [a-zA-Z_][a-zA-Z0-9_]*

%%

"Mfun"          { return MFUN;        }
"In"            { return IN;          }
"flt"           { return FLT;         }
"ch"            { return CH;          }
"bool"          { return BOOL;        }
"true"          { yylval.ival = 1; return TRUE_LIT;  }
"false"         { yylval.ival = 0; return FALSE_LIT; }
"return"        { return RETURN;      }
"Show"          { return SHOW;        }
"Ask"           { return ASK;         }
"Iff"           { return IFF;         }
"oriff"         { return ORIFF;       }
"or"            { return OR;          }
"flp"           { return FLP;         }
"wlp"           { return WLP;         }
"incident"      { return INCIDENT;    }
"check"         { return CHECK;       }
"nop"           { return NOP;         }
"stk"           { return STK;         }
"line"          { return LINE;        }
"takepart"      { return TAKEPART;    }
"MY_ARRAY"      { return MY_ARRAY;    }
"Exp"           { return EXPONENTIAL; }
"Inc"           { return INC;         }
"Nainai"        { return NAINAI;      }
"lastadd"       { return LASTADD;     }
"lastdrag"      { return LASTDRAG;    }
"strjoin"       { return STRJOIN;     }
"strlen"        { return STRLENF;     }
"add"           { return ADD;         }
"sub"           { return SUB;         }
"mul"           { return MUL;         }
"div"           { return DIV;         }
"mod"           { return MOD;         }
"AND"           { return AND;         }
"OR"            { return BOR;         }
"XOR"           { return XOR;         }
"NOT"           { return BNOT;        }
"grt"           { return GRT;         }
"lrt"           { return LRT;         }
"eql"           { return EQL;         }
"eqgrt"         { return EQGRT;       }
"eqsml"         { return EQSML;       }
"neql"          { return NEQL;        }
"@@"            { return COMPOUNDADD; }
"@"             { return AT;          }
"#"             { return HASH;        }
"$"             { return DOLLAR;      }
":"             { return COLON;       }
"("             { return '(';         }
")"             { return ')';         }
"{"             { return '{';         }
"}"             { return '}';         }
"["             { return '['; }
"]"             { return ']'; }
"."             { return '.';         }

{DIGIT}+"."{DIGIT}+  {
    yylval.fval = atof(yytext);
    return FLOAT_LIT;
}
{DIGIT}+  {
    yylval.ival = atoi(yytext);
    return INT_LIT;
}
\"[^\"]*\"  {
    int len = strlen(yytext) - 2;
    yylval.sval = malloc(len + 1);
    strncpy(yylval.sval, yytext + 1, len);
    yylval.sval[len] = '\0';
    return STRING_LIT;
}
'.'  {
    yylval.cval = yytext[1];
    return CHAR_LIT;
}
{ID}  {
    yylval.sval = strdup(yytext);
    return IDENT;
}
[ \t\n\r]   ;
"//"[^\n]*  ;
.  { printf("Lexical Error: Unknown character '%s'\n", yytext); }

%%

int yywrap() { return 1; }