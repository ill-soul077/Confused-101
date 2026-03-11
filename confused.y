%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
extern int yylex();
int yyerror(char *msg);

/* ══════════════════════════════
   SYMBOL TABLE
   ══════════════════════════════ */
typedef struct {
    char name[64];
    char type[16];
    union {
        int   ival;
        float fval;
        char  cval;
    } value;
} Symbol;

typedef struct Frame {
    char   func_name[64];
    Symbol locals[100];
    int    local_count;
    float  return_value;
    struct Frame *prev;
} Frame;

Frame *call_stack = NULL;

void push_frame(const char *func_name) {
    Frame *f = malloc(sizeof(Frame));
    strncpy(f->func_name, func_name, 64);
    f->local_count  = 0;
    f->return_value = 0;
    f->prev         = call_stack;
    call_stack      = f;
}

void pop_frame() {
    if (call_stack != NULL) {
        Frame *temp = call_stack;
        call_stack  = call_stack->prev;
        free(temp);
    }
}

Symbol *get_symbol(char *name) {
    Frame *f = call_stack;
    while (f != NULL) {
        for (int i = 0; i < f->local_count; i++)
            if (strcmp(f->locals[i].name, name) == 0)
                return &f->locals[i];
        f = f->prev;
    }
    return NULL;
}

int lookup(char *name) {
    return get_symbol(name) != NULL ? 1 : -1;
}

void insert(char *name, char *type, int value) {
    
    if (lookup(name) != -1) {
        printf("Error: Variable '%s' already declared.\n", name);
        return;
    }
    Frame *f = call_stack;
    strncpy(f->locals[f->local_count].name,  name, 64);
    strncpy(f->locals[f->local_count].type,  type, 16);
    f->locals[f->local_count].value.ival = value;
    f->local_count++;
}
void insertFloat(char *name, char *type, float value) {
    if (lookup(name) != -1) {
        printf("Error: Variable '%s' already declared.\n", name);
        return;
    }
    Frame *f = call_stack;
    strncpy(f->locals[f->local_count].name,  name, 64);
    strncpy(f->locals[f->local_count].type,  type, 16);
    f->locals[f->local_count].value.fval = value;
    f->local_count++;
}

/* ══════════════════════════════
   AST NODE TYPES
   ══════════════════════════════ */
typedef enum {
    NODE_IF, NODE_WHILE, NODE_FOR,
    NODE_SEQ, NODE_DECL,
    NODE_ADD, NODE_SUB, NODE_MUL, NODE_DIV, NODE_MOD,
    NODE_GT,  NODE_LT,  NODE_EQ,  NODE_NEQ,
    NODE_LEQ, NODE_GEQ,
    NODE_AND, NODE_OR,  NODE_XOR,
    NODE_NUM, NODE_IDENT,
    NODE_FUNC_DEF, NODE_FUNC_CALL,
    NODE_RETURN,
    NODE_ARG, NODE_PARAM,
    NODE_SHOW, NODE_ASK,
    NODE_COMPOUND_ADD,
} NodeType;

typedef struct Node {
    NodeType     type;
    struct Node *left;
    struct Node *middle;
    struct Node *right;
    float        value;
    char        *name;
} Node;

Node *root = NULL;

/* ══════════════════════════════
   FUNCTION TABLE
   ══════════════════════════════ */
typedef struct {
    char  name[64];
    char  return_type[16];
    char  param_names[10][64];
    char  param_types[10][16];
    int   param_count;
    struct Node *body;
} FuncEntry;

FuncEntry func_table[50];
int func_count = 0;

void insert_func(char *name, char *return_type, struct Node *params, struct Node *body) {
    if (func_count >= 50) { printf("Function table full!\n"); return; }
    strncpy(func_table[func_count].name,        name,        64);
    strncpy(func_table[func_count].return_type, return_type, 16);
    func_table[func_count].body        = body;
    func_table[func_count].param_count = 0;
    /* walk the param chain and store each parameter */
    struct Node *p = params;
    while (p != NULL) {
        int i = func_table[func_count].param_count;
        strncpy(func_table[func_count].param_names[i], p->name, 64);
        strncpy(func_table[func_count].param_types[i],
                p->value == 0 ? "int" : "float", 16);
        func_table[func_count].param_count++;
        p = p->right;
    }
    func_count++;
}


FuncEntry *lookup_func(char *name) {
    for (int i = 0; i < func_count; i++)
        if (strcmp(func_table[i].name, name) == 0)
            return &func_table[i];
    return NULL;
}



/* ══════════════════════════════
   NODE CONSTRUCTORS
   ══════════════════════════════ */
Node *makeNode(NodeType type, Node *left, Node *middle, Node *right) {
    Node *n  = malloc(sizeof(Node));
    n->type  = type;
    n->left  = left;
    n->middle= middle;
    n->right = right;
    n->value = 0;
    n->name  = NULL;
    return n;
}

Node *makeNum(float value) {
    Node *n  = makeNode(NODE_NUM, NULL, NULL, NULL);
    n->value = value;
    return n;
}

Node *makeIdent(char *name) {
    Node *n = makeNode(NODE_IDENT, NULL, NULL, NULL);
    n->name = strdup(name);
    return n;
}

/* ══════════════════════════════
   EVAL
   ══════════════════════════════ */
void execute(Node *n);  /* forward declaration */



float eval(Node *n) {
    if (n == NULL) return 0;
    switch (n->type) {
        case NODE_NUM:   return n->value;
        case NODE_IDENT: {
            Symbol *sym = get_symbol(n->name);
            if (sym != NULL) {
                if (strcmp(sym->type, "float") == 0) return sym->value.fval;
                if (strcmp(sym->type, "char")  == 0) return sym->value.cval;
                return sym->value.ival;
            }
            printf("Error: Undefined variable '%s'.\n", n->name);  /* already there */
            return 0;
        }
        case NODE_ADD: return eval(n->left) + eval(n->right);
        case NODE_SUB: return eval(n->left) - eval(n->right);
        case NODE_MUL: return eval(n->left) * eval(n->right);
        case NODE_DIV:
            if (eval(n->right) == 0) { printf("Error: Division by zero.\n"); return 0; }
            return eval(n->left) / eval(n->right);
        case NODE_MOD: return (int)eval(n->left) % (int)eval(n->right);
        case NODE_GT:  return eval(n->left) >  eval(n->right);
        case NODE_LT:  return eval(n->left) <  eval(n->right);
        case NODE_EQ:  return eval(n->left) == eval(n->right);
        case NODE_NEQ: return eval(n->left) != eval(n->right);
        case NODE_LEQ: return eval(n->left) <= eval(n->right);
        case NODE_GEQ: return eval(n->left) >= eval(n->right);
        case NODE_AND: return (int)eval(n->left) & (int)eval(n->right);
        case NODE_OR:  return (int)eval(n->left) | (int)eval(n->right);
        case NODE_XOR: return (int)eval(n->left) ^ (int)eval(n->right);
        case NODE_FUNC_CALL: {
            FuncEntry *fn = lookup_func(n->name);
            if (fn == NULL) {
                printf("Error: Undefined function '%s'.\n", n->name);
                return 0;
            }
            push_frame(n->name);
            /* bind arguments to parameter names */
            Node *arg = n->left;
            for (int i = 0; i < fn->param_count && arg != NULL; i++) {
                float val = eval(arg->left);
                if (strcmp(fn->param_types[i], "float") == 0)
                    insertFloat(fn->param_names[i], "float", val);
                else
                    insert(fn->param_names[i], "int", (int)val);
                arg = arg->right;
            }
            execute(fn->body);
            float ret = call_stack->return_value;
            pop_frame();
            printf("Function '%s' returned: %.2f\n", n->name, ret);
            return ret;
        }
        default: return 0;
    }
}

/* ══════════════════════════════
   EXECUTE (forward declaration)
   ══════════════════════════════ */


void execute(Node *n) {
    if (n == NULL) return;
    switch (n->type) {

        case NODE_SEQ:
            execute(n->left);
            execute(n->right);
            break;

        case NODE_DECL: {
            float val = eval(n->left);
            if (n->value == 0) {
                if (val != (int)val)
                    printf("Warning: float %.2f truncated to int for '%s'.\n", val, n->name);
                insert(n->name, "int", (int)val);
                printf("Stored: %s = %d (type: int)\n", n->name, (int)val);
            } else if (n->value == 1) {
                insertFloat(n->name, "float", val);
                printf("Stored: %s = %.2f (type: float)\n", n->name, val);
            }
            break;
        }

        case NODE_IF: {
            int cond = (int)eval(n->left);
            printf("Iff condition = %d (%s)\n", cond, cond ? "TRUE" : "FALSE");
            if (cond) execute(n->middle);
            else      execute(n->right);
            break;
        }

        case NODE_WHILE: {
            printf("wlp starting...\n");
            int iterations = 0;
            while ((int)eval(n->left)) {
                iterations++;
                execute(n->middle);
                if (iterations > 1000) {
                    printf("Error: Loop exceeded 1000 iterations.\n");
                    break;
                }
            }
            printf("wlp done (%d iterations)\n", iterations);
            break;
        }

        case NODE_SHOW: {
            float val = eval(n->left);
            if (n->left && n->left->type == NODE_IDENT) {
                Symbol *sym = get_symbol(n->left->name);
                if (sym && strcmp(sym->type, "float") == 0)
                    printf("%.2f\n", val);
                else
                    printf("%d\n", (int)val);
            } else {
                /* check if result is a whole number */
                if (val == (int)val) printf("%d\n", (int)val);
                else                 printf("%.2f\n", val);
            }
            break;
        }

        case NODE_RETURN: {
            float val = eval(n->left);
            call_stack->return_value = val;
            printf("Return: %.2f\n", val);
            break;
        }

        case NODE_FOR: {
            int start = (int)eval(n->left);
            int end   = (int)eval(n->middle);
            printf("flp starting: %d to %d\n", start, end);
            /* automatically create the loop variable i in the current frame */
            insert("i", "int", start);
            Symbol *i_sym = get_symbol("i");
            for (i_sym->value.ival = start; i_sym->value.ival < end; i_sym->value.ival++) {
                execute(n->right);   /* run the loop body */
            }
            printf("flp done\n");
            break;
        }

        case NODE_FUNC_DEF:
            break; /* already stored in func_table during parsing */

        case NODE_COMPOUND_ADD: {
            /* x @@ expr  means  x = x + expr */
            Symbol *sym = get_symbol(n->name);
            if (sym == NULL) {
                printf("Error: Undefined variable '%s'.\n", n->name);
                break;
            }
            float val = eval(n->left);
            if (strcmp(sym->type, "float") == 0)
                sym->value.fval += val;
            else
                sym->value.ival += (int)val;
            printf("Updated: %s += %.2f\n", n->name, val);
            break;
        }

        default: break;
    }
}
%}


/* ── Value types ── */
%union {
    char        *sval;
    int          ival;
    float        fval;
    char         cval;
    struct Node *nval;
}

/* ── Tokens with values ── */
%token <sval> IDENT STRING_LIT
%token <ival> INT_LIT TRUE_LIT FALSE_LIT
%token <fval> FLOAT_LIT
%token <cval> CHAR_LIT

/* ── Keywords ── */
%token MFUN IN FLT CH BOOL RETURN SHOW ASK
%token IFF ORIFF OR FLP WLP
%token INCIDENT CHECK NOP
%token STK LINE TAKEPART MY_ARRAY
%token EXPONENTIAL INC NAINAI LASTADD LASTDRAG
%token STRJOIN STRLENF

/* ── Operators ── */
%token ADD SUB MUL DIV MOD
%token AND BOR XOR BNOT
%token GRT LRT EQL EQGRT EQSML NEQL
%token AT COMPOUNDADD HASH DOLLAR COLON

/* ── Precedence: lowest first, highest last ── */
%left GRT LRT EQGRT EQSML EQL NEQL
%left ADD SUB
%left MUL DIV MOD



%type <nval> program statement expr declaration show_stmt iff_stmt or_chain
             wlp_stmt flp_stmt compound_stmt func_def func_call arg_list param_list param return_stmt block


%%

/* stub — just enough to compile cleanly for now */
program
    : statement             { $$ = $1; root = $1; }
    | program statement     { $$ = makeNode(NODE_SEQ, $1, NULL, $2); root = $$; }
    ;

statement
    : declaration        { $$ = $1; }
    | show_stmt          { $$ = $1; }
    | iff_stmt           { $$ = $1; }
    | wlp_stmt           { $$ = $1; }
    | flp_stmt           { $$ = $1; }
    | compound_stmt      { $$ = $1; }
    | func_def           { $$ = $1; }
    | return_stmt        { $$ = $1; }
    | func_call HASH     { $$ = $1; }  /* function call as a standalone statement */
    ;

return_stmt
    : RETURN expr HASH {
        $$ = makeNode(NODE_RETURN, $2, NULL, NULL);
    }
    ;
block
    : statement              { $$ = $1; }
    | block statement        { $$ = makeNode(NODE_SEQ, $1, NULL, $2); }
    ;  
param
    : IN IDENT {
        Node *n = makeNode(NODE_PARAM, NULL, NULL, NULL);
        n->name  = strdup($2);
        n->value = 0;   /* 0 = int parameter */
        $$ = n;
    }
    | FLT IDENT {
        Node *n = makeNode(NODE_PARAM, NULL, NULL, NULL);
        n->name  = strdup($2);
        n->value = 1;   /* 1 = float parameter */
        $$ = n;
    }
    ;

param_list
    : /* empty */            { $$ = NULL; }
    | param                  { $$ = $1; }
    | param_list DOLLAR param {
        /* chain params using the right pointer */
        Node *p = $1;
        while (p->right != NULL) p = p->right;
        p->right = $3;
        $$ = $1;
    }
    ;

arg_list
    : /* empty */            { $$ = NULL; }
    | expr                   { $$ = makeNode(NODE_ARG, $1, NULL, NULL); }
    | arg_list DOLLAR expr   {
        /* chain args using the right pointer */
        Node *a = makeNode(NODE_ARG, $3, NULL, NULL);
        Node *p = $1;
        while (p->right != NULL) p = p->right;
        p->right = a;
        $$ = $1;
    }
    ;

func_def
    : IN IDENT '(' param_list ')' '{' block '}' {
        Node *n = makeNode(NODE_FUNC_DEF, $4, $7, NULL);
        n->name  = strdup($2);
        n->value = 0;
        insert_func($2, "int", $4, $7);
        $$ = n;
    }
    | FLT IDENT '(' param_list ')' '{' block '}' {
        Node *n = makeNode(NODE_FUNC_DEF, $4, $7, NULL);
        n->name  = strdup($2);
        n->value = 1;
        insert_func($2, "float", $4, $7);
        $$ = n;
    }
    ;

  

func_call
    : IDENT '(' arg_list ')' {
        Node *n = makeNode(NODE_FUNC_CALL, $3, NULL, NULL);
        n->name = strdup($1);
        $$ = n;
    }
    ;

flp_stmt
    : FLP '(' expr COLON expr ')' '{' block '}' {
        /* left = start expr, middle = end expr, right = loop body */
        Node *n = makeNode(NODE_FOR, $3, $5, $8);
        $$ = n;
    }
    ;
wlp_stmt
    : WLP '(' expr ')' '{' block '}' {
        /* condition in left, loop body in middle, no right needed */
        $$ = makeNode(NODE_WHILE, $3, $6, NULL);
    }
    ;
compound_stmt
    : IDENT COMPOUNDADD expr HASH {
        Node *n = makeNode(NODE_COMPOUND_ADD, $3, NULL, NULL);
        n->name = strdup($1);
        $$ = n;
    }
    ;    

show_stmt
    : SHOW '(' expr ')' HASH {
        Node *n = makeNode(NODE_SHOW, $3, NULL, NULL);
        $$ = n;
    }
    ;

iff_stmt
    : IFF '(' expr ')' '{' block '}' {
        /* Iff with no else branch */
        $$ = makeNode(NODE_IF, $3, $6, NULL);
    }
    | IFF '(' expr ')' '{' block '}' or_chain {
        /* Iff with or/oriff chain */
        $$ = makeNode(NODE_IF, $3, $6, $8);
    }
    ;

or_chain
    : OR '{' block '}' {
        /* plain or — the else branch */
        $$ = $3;
    }
    | ORIFF '(' expr ')' '{' block '}' {
        /* oriff with no further chain */
        $$ = makeNode(NODE_IF, $3, $6, NULL);
    }
    | ORIFF '(' expr ')' '{' block '}' or_chain {
        /* oriff chained with more oriff/or */
        $$ = makeNode(NODE_IF, $3, $6, $8);
    }
    ;

declaration
    : IN IDENT AT expr HASH {
        Node *n  = makeNode(NODE_DECL, $4, NULL, NULL);
        n->name  = strdup($2);
        n->value = 0;   /* 0 = int */
        $$ = n;
    }
    | FLT IDENT AT expr HASH {
        Node *n  = makeNode(NODE_DECL, $4, NULL, NULL);
        n->name  = strdup($2);
        n->value = 1;   /* 1 = float */
        $$ = n;
    }
    ;

expr
    : INT_LIT       { $$ = makeNum($1);    }
    | FLOAT_LIT     { $$ = makeNum($1);    }
    | IDENT         { $$ = makeIdent($1);  }
    | expr ADD expr { $$ = makeNode(NODE_ADD, $1, NULL, $3); }
    | expr SUB expr { $$ = makeNode(NODE_SUB, $1, NULL, $3); }
    | expr MUL expr { $$ = makeNode(NODE_MUL, $1, NULL, $3); }
    | expr DIV expr { $$ = makeNode(NODE_DIV, $1, NULL, $3); }
    | expr MOD expr { $$ = makeNode(NODE_MOD, $1, NULL, $3); }
    | expr GRT expr { $$ = makeNode(NODE_GT,  $1, NULL, $3); }
    | expr LRT expr { $$ = makeNode(NODE_LT,  $1, NULL, $3); }
    | expr EQL expr { $$ = makeNode(NODE_EQ,  $1, NULL, $3); }
    | expr NEQL expr{ $$ = makeNode(NODE_NEQ, $1, NULL, $3); }
    | expr EQGRT expr { $$ = makeNode(NODE_GEQ, $1, NULL, $3); }
    | expr EQSML expr { $$ = makeNode(NODE_LEQ, $1, NULL, $3); }
    | func_call  { $$ = $1; }
    | '(' expr ')'  { $$ = $2; }
    ;

%%

int yyerror(char *msg) {
    printf("Syntax Error: %s\n", msg);
    return 0;
}

int main() {
    push_frame("global");   /* create global scope for variables */
    printf("=== Parsing ===\n");
    yyparse();              /* Phase 1: build the AST tree */
    printf("=== Executing ===\n");
    if (root == NULL)
        printf("Error: root is NULL — nothing was parsed!\n");
    else
        execute(root);      /* Phase 2: walk the tree and run it */
    printf("=== Done ===\n");
    return 0;
}