
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 1 "confused.y"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>  /* for pow() in exponential operator */
extern int yylex();
extern FILE *yyin;
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
    NODE_COMPOUND_ADD,NODE_EXPONENTIAL
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
        case NODE_EXPONENTIAL: {
            float base = eval(n->left);   /* evaluate the base expression     */
            float exp  = eval(n->right);  /* evaluate the exponent expression */
            return pow(base, exp);        /* call C's math library pow()      */
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
            } else if (n->value == 2) {
                int bval = (val != 0) ? 1 : 0;
                insert(n->name, "bool", bval);
                printf("Stored: %s = %s (type: bool)\n", n->name, bval ? "true" : "false");
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

        case NODE_ASK: {
            /* Look up the variable in the symbol table first.
            We need to know its type so we can call scanf correctly. */
            Symbol *sym = get_symbol(n->name);
            if (sym == NULL) {
                printf("Error: Variable '%s' not declared. Declare it before using Ask().\n", n->name);
                break;
            }
            printf("Enter value for %s: ", n->name);   /* prompt the user */
            
            if (strcmp(sym->type, "float") == 0) {
                scanf("%f", &sym->value.fval);          /* read directly into the symbol's float slot */
            } else {
                scanf("%d", &sym->value.ival);          /* read directly into the symbol's int slot */
            }
            printf("Read: %s = ", n->name);
            if (strcmp(sym->type, "float") == 0)
                printf("%.2f\n", sym->value.fval);
            else
                printf("%d\n", sym->value.ival);
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

/* ══════════════════════════════
   TAC HELPERS
   ══════════════════════════════ */

int temp_count  = 0;   /* counts t1, t2, t3 ... */
int label_count = 0;   /* counts L1, L2, L3 ... */

/* Returns a new unique temporary name like "t1", "t2" etc.
   malloc gives it its own memory so it persists after the function returns. */
char *new_temp() {
    char *buf = malloc(16);
    sprintf(buf, "t%d", ++temp_count);
    return buf;
}

/* Returns a new unique label name like "L1", "L2" etc.
   Used for Iff, wlp, flp branch targets. */
char *new_label() {
    char *buf = malloc(16);
    sprintf(buf, "L%d", ++label_count);
    return buf;
}

/* ══════════════════════════════
   gen() — TAC for EXPRESSIONS
   Takes a node, prints TAC instructions for everything inside it,
   and returns a string: either a variable name, a literal, or a
   new temporary that holds the result.
   ══════════════════════════════ */

/* Forward declaration so gen() and gen_stmt() can call each other */
void gen_stmt(Node *n);

char *gen(Node *n) {
    if (n == NULL) return strdup("0");

    switch (n->type) {

        /* ── LEAF NODES ── no instruction needed, just return the name/value */
        case NODE_NUM: {
            /* It's a literal number like 3 or 5.0
               We format it as a string and return it directly.
               No temporary needed — the value is already "known". */
            char *buf = malloc(32);
            if (n->value == (int)n->value)
                sprintf(buf, "%d", (int)n->value);
            else
                sprintf(buf, "%.2f", n->value);
            return buf;
        }

        case NODE_IDENT:
            /* It's a variable like x or result.
               The "address" of this value is just the variable's name.
               Return it directly — no instruction, no temporary. */
            return strdup(n->name);

        /* ── ARITHMETIC NODES ── 
           Pattern is always: gen left, gen right, make temp, print instruction, return temp */
        case NODE_ADD: {
            char *l = gen(n->left);   /* generate TAC for left side  */
            char *r = gen(n->right);  /* generate TAC for right side */
            char *t = new_temp();     /* fresh bucket for the result  */
            printf("%s = %s + %s\n", t, l, r);
            return t;
        }
        case NODE_SUB: {
            char *l = gen(n->left);
            char *r = gen(n->right);
            char *t = new_temp();
            printf("%s = %s - %s\n", t, l, r);
            return t;
        }
        case NODE_MUL: {
            char *l = gen(n->left);
            char *r = gen(n->right);
            char *t = new_temp();
            printf("%s = %s * %s\n", t, l, r);
            return t;
        }
        case NODE_DIV: {
            char *l = gen(n->left);
            char *r = gen(n->right);
            char *t = new_temp();
            printf("%s = %s / %s\n", t, l, r);
            return t;
        }
        case NODE_MOD: {
            char *l = gen(n->left);
            char *r = gen(n->right);
            char *t = new_temp();
            printf("%s = %s %% %s\n", t, l, r);  /* %% prints a literal % */
            return t;
        }

        /* ── RELATIONAL NODES ──
           These produce a 0 or 1 result, stored in a temporary.
           Used as conditions in Iff and wlp. */
        case NODE_GT: {
            char *l = gen(n->left);
            char *r = gen(n->right);
            char *t = new_temp();
            printf("%s = %s > %s\n", t, l, r);
            return t;
        }
        case NODE_LT: {
            char *l = gen(n->left);
            char *r = gen(n->right);
            char *t = new_temp();
            printf("%s = %s < %s\n", t, l, r);
            return t;
        }
        case NODE_EQ: {
            char *l = gen(n->left);
            char *r = gen(n->right);
            char *t = new_temp();
            printf("%s = %s == %s\n", t, l, r);
            return t;
        }
        case NODE_NEQ: {
            char *l = gen(n->left);
            char *r = gen(n->right);
            char *t = new_temp();
            printf("%s = %s != %s\n", t, l, r);
            return t;
        }
        case NODE_GEQ: {
            char *l = gen(n->left);
            char *r = gen(n->right);
            char *t = new_temp();
            printf("%s = %s >= %s\n", t, l, r);
            return t;
        }
        case NODE_LEQ: {
            char *l = gen(n->left);
            char *r = gen(n->right);
            char *t = new_temp();
            printf("%s = %s <= %s\n", t, l, r);
            return t;
        }

        case NODE_EXPONENTIAL: {
            char *base = gen(n->left);    /* generate TAC for the base     */
            char *exp  = gen(n->right);   /* generate TAC for the exponent */
            char *t    = new_temp();
            printf("%s = %s ^ %s\n", t, base, exp);  /* ^ is TAC notation for power */
            return t;
        }

        /* ── FUNCTION CALL ──
           TAC convention: push each argument with "param", then "call".
           The return value lands in a new temporary. */
        case NODE_FUNC_CALL: {
            /* First, generate TAC for every argument and emit param instructions.
               Arguments are chained as NODE_ARG nodes via the right pointer. */
            Node *arg = n->left;
            while (arg != NULL) {
                char *a = gen(arg->left);   /* evaluate the argument expression */
                printf("param %s\n", a);    /* push it onto the param list */
                arg = arg->right;
            }
            /* Now emit the call instruction and capture the return value */
            char *t = new_temp();
            printf("%s = call %s\n", t, n->name);
            return t;
        }

        default:
            return strdup("?");
    }
}

/* ══════════════════════════════
   gen_stmt() — TAC for STATEMENTS
   Walks statement nodes and emits TAC.
   Returns nothing — statements are actions, not values.
   ══════════════════════════════ */
void gen_stmt(Node *n) {
    if (n == NULL) return;

    switch (n->type) {

        /* NODE_SEQ is the backbone of the program — a chain of statements.
           Just recurse left then right, same as execute() does. */
        case NODE_SEQ:
            gen_stmt(n->left);
            gen_stmt(n->right);
            break;

        /* Declaration: evaluate the right-hand expression, assign to variable.
           Example:  In result @ myAdd(3 $ 5)#
           TAC:      param 3
                     param 5
                     t1 = call myAdd
                     result = t1              */
        case NODE_DECL: {
            char *t = gen(n->left);        /* generate TAC for the expression */
            printf("%s = %s\n", n->name, t); /* assign result to the variable */
            break;
        }

        /* Show(): evaluate the expression, then print it.
           Example:  Show(result)#
           TAC:      t2 = result
                     print t2               */
        case NODE_SHOW: {
            char *t = gen(n->left);
            printf("print %s\n", t);
            break;
        }

        /* Compound assignment: x @@ expr  means  x = x + expr
           TAC:  t1 = x + expr_result
                 x  = t1                   */
        case NODE_COMPOUND_ADD: {
            char *t_val = gen(n->left);     /* generate the right-hand expr */
            char *t_res = new_temp();
            printf("%s = %s + %s\n", t_res, n->name, t_val);
            printf("%s = %s\n", n->name, t_res);
            break;
        }

        /* Return statement inside a function.
           TAC:  t1 = expr_result
                 return t1                 */
        case NODE_RETURN: {
            char *t = gen(n->left);
            printf("return %s\n", t);
            break;
        }

        /* Function definition: print a label marking where the function starts,
           then generate TAC for the body.
           NODE_FUNC_DEF itself doesn't execute — but in TAC we want to show
           the function's code as a labeled section. */
        case NODE_FUNC_DEF: {
            printf("\n; --- function %s ---\n", n->name);
            /* Print parameter names so the TAC is readable */
            FuncEntry *fn = lookup_func(n->name);
            if (fn) {
                for (int i = 0; i < fn->param_count; i++)
                    printf("; param %s\n", fn->param_names[i]);
            }
            gen_stmt(n->middle);   /* middle child = function body (the block) */
            printf("; --- end %s ---\n\n", n->name);
            break;
        }

        /* Iff statement — this is where labels become essential.
           Pattern:
               <condition TAC>
               if t1 goto L1
               goto L2
           L1:
               <true body TAC>
           L2:                         */
        case NODE_IF: {
            char *t   = gen(n->left);     /* generate condition expression TAC */
            char *l_true = new_label();   /* label for the true branch */
            char *l_end  = new_label();   /* label for after the whole Iff */

            printf("if %s goto %s\n", t, l_true);
            printf("goto %s\n", l_end);
            printf("%s:\n", l_true);
            gen_stmt(n->middle);           /* true branch body */
            printf("%s:\n", l_end);
            if (n->right != NULL)
                gen_stmt(n->right);        /* or/oriff chain if it exists */
            break;
        }

        /* wlp (while loop).
           Pattern:
           L1:                    <- loop back here each iteration
               <condition TAC>
               if t1 goto L2      <- enter body if condition true
               goto L3            <- exit loop if condition false
           L2:
               <body TAC>
               goto L1            <- always jump back to check condition
           L3:                    <- loop exits here               */
        case NODE_WHILE: {
            char *l_start = new_label();   /* top of loop — re-evaluated each time */
            char *l_body  = new_label();   /* enter body */
            char *l_end   = new_label();   /* exit loop  */

            printf("%s:\n", l_start);
            char *t = gen(n->left);        /* condition expression */
            printf("if %s goto %s\n", t, l_body);
            printf("goto %s\n", l_end);
            printf("%s:\n", l_body);
            gen_stmt(n->middle);           /* loop body */
            printf("goto %s\n", l_start); /* jump back to re-check condition */
            printf("%s:\n", l_end);
            break;
        }

        /* flp (range-based for loop).
           We expand it into TAC using i as the loop variable.
           Pattern:
               i = start
           L1:
               t1 = i < end
               if t1 goto L2
               goto L3
           L2:
               <body TAC>
               i = i + 1
               goto L1
           L3:                                                    */
        case NODE_FOR: {
            char *start   = gen(n->left);    /* start expression */
            char *end_val = gen(n->middle);  /* end expression   */
            char *l_start = new_label();
            char *l_body  = new_label();
            char *l_end   = new_label();

            printf("i = %s\n", start);        /* initialize loop variable */
            printf("%s:\n", l_start);
            char *t_cond = new_temp();
            printf("%s = i < %s\n", t_cond, end_val);  /* check i < end */
            printf("if %s goto %s\n", t_cond, l_body);
            printf("goto %s\n", l_end);
            printf("%s:\n", l_body);
            gen_stmt(n->right);                /* loop body */
            char *t_inc = new_temp();
            printf("%s = i + 1\n", t_inc);     /* increment i */
            printf("i = %s\n", t_inc);
            printf("goto %s\n", l_start);
            printf("%s:\n", l_end);
            break;
        }

        case NODE_FUNC_CALL:
            /* A standalone function call statement — generate it as an expression
               but discard the return value since nobody is using it. */
            gen(n);
            break;
        case NODE_ASK: {
            /* TAC for input is simply a 'read' instruction targeting the variable.
            This is the TAC equivalent of scanf — a backend would translate
            this into a system call or library call to read from stdin. */
            printf("read %s\n", n->name);
            break;
        }


        default:
            break;
    }
}



/* Line 189 of yacc.c  */
#line 822 "confused.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     IDENT = 258,
     STRING_LIT = 259,
     INT_LIT = 260,
     TRUE_LIT = 261,
     FALSE_LIT = 262,
     FLOAT_LIT = 263,
     CHAR_LIT = 264,
     MFUN = 265,
     IN = 266,
     FLT = 267,
     CH = 268,
     BOOL = 269,
     RETURN = 270,
     SHOW = 271,
     ASK = 272,
     IFF = 273,
     ORIFF = 274,
     OR = 275,
     FLP = 276,
     WLP = 277,
     INCIDENT = 278,
     CHECK = 279,
     NOP = 280,
     STK = 281,
     LINE = 282,
     TAKEPART = 283,
     MY_ARRAY = 284,
     EXPONENTIAL = 285,
     INC = 286,
     NAINAI = 287,
     LASTADD = 288,
     LASTDRAG = 289,
     STRJOIN = 290,
     STRLENF = 291,
     ADD = 292,
     SUB = 293,
     MUL = 294,
     DIV = 295,
     MOD = 296,
     AND = 297,
     BOR = 298,
     XOR = 299,
     BNOT = 300,
     GRT = 301,
     LRT = 302,
     EQL = 303,
     EQGRT = 304,
     EQSML = 305,
     NEQL = 306,
     AT = 307,
     COMPOUNDADD = 308,
     HASH = 309,
     DOLLAR = 310,
     COLON = 311
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 751 "confused.y"

    char        *sval;
    int          ival;
    float        fval;
    char         cval;
    struct Node *nval;



/* Line 214 of yacc.c  */
#line 924 "confused.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 936 "confused.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  42
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   508

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  61
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  19
/* YYNRULES -- Number of rules.  */
#define YYNRULES  59
/* YYNRULES -- Number of states.  */
#define YYNSTATES  148

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   311

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      57,    58,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    59,     2,    60,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     5,     8,    10,    12,    14,    16,    18,
      20,    22,    24,    26,    29,    35,    39,    41,    44,    47,
      50,    51,    53,    57,    58,    60,    64,    73,    82,    87,
      97,   105,   110,   116,   124,   133,   138,   146,   155,   161,
     167,   173,   175,   177,   179,   181,   183,   187,   191,   195,
     199,   203,   207,   211,   215,   219,   223,   227,   229,   233
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      62,     0,    -1,    63,    -1,    62,    63,    -1,    78,    -1,
      75,    -1,    76,    -1,    73,    -1,    72,    -1,    74,    -1,
      70,    -1,    65,    -1,    64,    -1,    71,    54,    -1,    17,
      57,     3,    58,    54,    -1,    15,    79,    54,    -1,    63,
      -1,    66,    63,    -1,    11,     3,    -1,    12,     3,    -1,
      -1,    67,    -1,    68,    55,    67,    -1,    -1,    79,    -1,
      69,    55,    79,    -1,    11,     3,    57,    68,    58,    59,
      66,    60,    -1,    12,     3,    57,    68,    58,    59,    66,
      60,    -1,     3,    57,    69,    58,    -1,    21,    57,    79,
      56,    79,    58,    59,    66,    60,    -1,    22,    57,    79,
      58,    59,    66,    60,    -1,     3,    53,    79,    54,    -1,
      16,    57,    79,    58,    54,    -1,    18,    57,    79,    58,
      59,    66,    60,    -1,    18,    57,    79,    58,    59,    66,
      60,    77,    -1,    20,    59,    66,    60,    -1,    19,    57,
      79,    58,    59,    66,    60,    -1,    19,    57,    79,    58,
      59,    66,    60,    77,    -1,    11,     3,    52,    79,    54,
      -1,    12,     3,    52,    79,    54,    -1,    14,     3,    52,
      79,    54,    -1,     5,    -1,     8,    -1,     6,    -1,     7,
      -1,     3,    -1,    79,    37,    79,    -1,    79,    38,    79,
      -1,    79,    39,    79,    -1,    79,    40,    79,    -1,    79,
      41,    79,    -1,    79,    46,    79,    -1,    79,    47,    79,
      -1,    79,    48,    79,    -1,    79,    51,    79,    -1,    79,
      49,    79,    -1,    79,    50,    79,    -1,    71,    -1,    57,
      79,    58,    -1,    30,    57,    79,    55,    79,    58,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   794,   794,   795,   799,   800,   801,   802,   803,   804,
     805,   806,   807,   808,   812,   820,   825,   826,   829,   835,
     844,   845,   846,   856,   857,   858,   869,   876,   888,   896,
     903,   909,   917,   924,   928,   935,   939,   943,   950,   956,
     962,   971,   972,   973,   974,   975,   976,   977,   978,   979,
     980,   981,   982,   983,   984,   985,   986,   987,   988,   989
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "IDENT", "STRING_LIT", "INT_LIT",
  "TRUE_LIT", "FALSE_LIT", "FLOAT_LIT", "CHAR_LIT", "MFUN", "IN", "FLT",
  "CH", "BOOL", "RETURN", "SHOW", "ASK", "IFF", "ORIFF", "OR", "FLP",
  "WLP", "INCIDENT", "CHECK", "NOP", "STK", "LINE", "TAKEPART", "MY_ARRAY",
  "EXPONENTIAL", "INC", "NAINAI", "LASTADD", "LASTDRAG", "STRJOIN",
  "STRLENF", "ADD", "SUB", "MUL", "DIV", "MOD", "AND", "BOR", "XOR",
  "BNOT", "GRT", "LRT", "EQL", "EQGRT", "EQSML", "NEQL", "AT",
  "COMPOUNDADD", "HASH", "DOLLAR", "COLON", "'('", "')'", "'{'", "'}'",
  "$accept", "program", "statement", "ask_stmt", "return_stmt", "block",
  "param", "param_list", "arg_list", "func_def", "func_call", "flp_stmt",
  "wlp_stmt", "compound_stmt", "show_stmt", "iff_stmt", "or_chain",
  "declaration", "expr", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,    40,    41,   123,
     125
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    61,    62,    62,    63,    63,    63,    63,    63,    63,
      63,    63,    63,    63,    64,    65,    66,    66,    67,    67,
      68,    68,    68,    69,    69,    69,    70,    70,    71,    72,
      73,    74,    75,    76,    76,    77,    77,    77,    78,    78,
      78,    79,    79,    79,    79,    79,    79,    79,    79,    79,
      79,    79,    79,    79,    79,    79,    79,    79,    79,    79
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     5,     3,     1,     2,     2,     2,
       0,     1,     3,     0,     1,     3,     8,     8,     4,     9,
       7,     4,     5,     7,     8,     4,     7,     8,     5,     5,
       5,     1,     1,     1,     1,     1,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     1,     3,     6
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     2,    12,    11,    10,     0,     8,     7,     9,
       5,     6,     4,     0,    23,     0,     0,     0,    45,    41,
      43,    44,    42,     0,     0,    57,     0,     0,     0,     0,
       0,     0,     1,     3,    13,     0,     0,    24,     0,    20,
       0,    20,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    15,     0,     0,     0,
       0,     0,    31,     0,    28,     0,     0,     0,    21,     0,
       0,     0,     0,     0,    58,    46,    47,    48,    49,    50,
      51,    52,    53,    55,    56,    54,     0,     0,     0,     0,
       0,    25,    38,    18,    19,     0,     0,    39,     0,    40,
       0,    32,    14,     0,     0,     0,    22,     0,     0,     0,
      16,     0,     0,     0,     0,     0,    59,    33,    17,     0,
      30,    26,    27,     0,     0,    34,     0,     0,     0,    29,
       0,     0,     0,    35,     0,     0,    36,    37
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    11,   120,    13,    14,   121,    78,    79,    46,    15,
      35,    17,    18,    19,    20,    21,   135,    22,    36
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -50
static const yytype_int16 yypact[] =
{
     362,   -43,    12,    26,    28,   211,    -4,    -3,     1,    14,
      15,   221,   -50,   -50,   -50,   -50,    10,   -50,   -50,   -50,
     -50,   -50,   -50,   211,   211,   -49,   -48,    -2,    18,   -50,
     -50,   -50,   -50,    32,   211,   -50,   367,   211,    48,   211,
     211,   211,   -50,   -50,   -50,   385,   -38,   457,   211,   -10,
     211,   -10,   211,   211,   207,   211,   211,   211,   211,   211,
     211,   211,   211,   211,   211,   211,   -50,   223,    40,   238,
     313,   253,   -50,   211,   -50,   403,    83,    99,   -50,   -22,
     421,    -6,   439,   348,   -50,    29,    29,   -50,   -50,   -50,
     -13,   -13,   -13,   -13,   -13,   -13,    49,    50,    47,   211,
      51,   457,   -50,   -50,   -50,   -10,    52,   -50,    53,   -50,
     211,   -50,   -50,   362,   268,   362,   -50,   362,   362,   283,
     -50,    45,    57,    62,    79,   139,   -50,    -7,   -50,   362,
     -50,   -50,   -50,    63,    67,   -50,   155,   211,   362,   -50,
     298,   171,    68,   -50,   362,   191,    -7,   -50
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -50,   -50,     7,   -50,   -50,   -30,     2,    58,   -50,   -50,
       0,   -50,   -50,   -50,   -50,   -50,   -12,   -50,   -18
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      16,    76,    77,    48,    50,    45,    47,    12,    49,    51,
      23,    16,   133,   134,    24,    25,    54,    73,    43,    67,
      74,    69,    70,    71,    55,    56,    57,    58,    59,    26,
      75,    27,    80,   105,    82,    83,   106,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,     1,   105,
      52,    68,   108,    37,    38,   101,     2,     3,    39,     4,
       5,     6,     7,     8,    44,     1,     9,    10,    57,    58,
      59,    40,    41,     2,     3,    24,     4,     5,     6,     7,
       8,   114,     1,     9,    10,   123,   103,   124,   125,    53,
       2,     3,   119,     4,     5,     6,     7,     8,    97,   136,
       9,    10,   104,   111,   112,   127,   113,   116,   141,    81,
     115,   117,   118,    16,   145,    16,   129,    16,    16,   140,
     137,    16,   130,    16,    16,    16,   138,   144,   128,    16,
     128,   128,   128,     0,   147,     0,    16,     0,    16,   131,
       0,    16,     1,   128,    16,    16,     0,     0,   128,     0,
       2,     3,   128,     4,     5,     6,     7,     8,     1,     0,
       9,    10,     0,     0,     0,     0,     2,     3,     0,     4,
       5,     6,     7,     8,     1,     0,     9,    10,     0,     0,
       0,     0,     2,     3,     0,     4,     5,     6,     7,     8,
       0,     0,     9,    10,     1,     0,     0,     0,     0,   132,
       0,     0,     2,     3,     0,     4,     5,     6,     7,     8,
       0,     0,     9,    10,    28,   139,    29,    30,    31,    32,
       0,    42,     0,     0,     1,     0,     0,     0,     0,     0,
       0,   143,     2,     3,     0,     4,     5,     6,     7,     8,
       0,    33,     9,    10,    55,    56,    57,    58,    59,     0,
       0,   146,     0,    60,    61,    62,    63,    64,    65,     0,
      55,    56,    57,    58,    59,    84,     0,     0,    34,    60,
      61,    62,    63,    64,    65,    55,    56,    57,    58,    59,
       0,    96,     0,     0,    60,    61,    62,    63,    64,    65,
      55,    56,    57,    58,    59,     0,    98,     0,     0,    60,
      61,    62,    63,    64,    65,    55,    56,    57,    58,    59,
       0,   100,     0,     0,    60,    61,    62,    63,    64,    65,
      55,    56,    57,    58,    59,     0,   122,     0,     0,    60,
      61,    62,    63,    64,    65,    55,    56,    57,    58,    59,
       0,   126,     0,     0,    60,    61,    62,    63,    64,    65,
      55,    56,    57,    58,    59,     0,   142,     0,     0,    60,
      61,    62,    63,    64,    65,     1,     0,     0,     0,    99,
       0,     0,     0,     2,     3,     0,     4,     5,     6,     7,
       8,     0,     0,     9,    10,    55,    56,    57,    58,    59,
       0,     0,     0,     0,    60,    61,    62,    63,    64,    65,
       0,     0,     0,   110,    55,    56,    57,    58,    59,     0,
       0,     0,     0,    60,    61,    62,    63,    64,    65,     0,
       0,    66,    55,    56,    57,    58,    59,     0,     0,     0,
       0,    60,    61,    62,    63,    64,    65,     0,     0,    72,
      55,    56,    57,    58,    59,     0,     0,     0,     0,    60,
      61,    62,    63,    64,    65,     0,     0,   102,    55,    56,
      57,    58,    59,     0,     0,     0,     0,    60,    61,    62,
      63,    64,    65,     0,     0,   107,    55,    56,    57,    58,
      59,     0,     0,     0,     0,    60,    61,    62,    63,    64,
      65,     0,     0,   109,    55,    56,    57,    58,    59,     0,
       0,     0,     0,    60,    61,    62,    63,    64,    65
};

static const yytype_int16 yycheck[] =
{
       0,    11,    12,    52,    52,    23,    24,     0,    57,    57,
      53,    11,    19,    20,    57,     3,    34,    55,    11,    37,
      58,    39,    40,    41,    37,    38,    39,    40,    41,     3,
      48,     3,    50,    55,    52,    53,    58,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,     3,    55,
      52,     3,    58,    57,    57,    73,    11,    12,    57,    14,
      15,    16,    17,    18,    54,     3,    21,    22,    39,    40,
      41,    57,    57,    11,    12,    57,    14,    15,    16,    17,
      18,    99,     3,    21,    22,   115,     3,   117,   118,    57,
      11,    12,   110,    14,    15,    16,    17,    18,    58,   129,
      21,    22,     3,    54,    54,    60,    59,   105,   138,    51,
      59,    59,    59,   113,   144,   115,    59,   117,   118,   137,
      57,   121,    60,   123,   124,   125,    59,    59,   121,   129,
     123,   124,   125,    -1,   146,    -1,   136,    -1,   138,    60,
      -1,   141,     3,   136,   144,   145,    -1,    -1,   141,    -1,
      11,    12,   145,    14,    15,    16,    17,    18,     3,    -1,
      21,    22,    -1,    -1,    -1,    -1,    11,    12,    -1,    14,
      15,    16,    17,    18,     3,    -1,    21,    22,    -1,    -1,
      -1,    -1,    11,    12,    -1,    14,    15,    16,    17,    18,
      -1,    -1,    21,    22,     3,    -1,    -1,    -1,    -1,    60,
      -1,    -1,    11,    12,    -1,    14,    15,    16,    17,    18,
      -1,    -1,    21,    22,     3,    60,     5,     6,     7,     8,
      -1,     0,    -1,    -1,     3,    -1,    -1,    -1,    -1,    -1,
      -1,    60,    11,    12,    -1,    14,    15,    16,    17,    18,
      -1,    30,    21,    22,    37,    38,    39,    40,    41,    -1,
      -1,    60,    -1,    46,    47,    48,    49,    50,    51,    -1,
      37,    38,    39,    40,    41,    58,    -1,    -1,    57,    46,
      47,    48,    49,    50,    51,    37,    38,    39,    40,    41,
      -1,    58,    -1,    -1,    46,    47,    48,    49,    50,    51,
      37,    38,    39,    40,    41,    -1,    58,    -1,    -1,    46,
      47,    48,    49,    50,    51,    37,    38,    39,    40,    41,
      -1,    58,    -1,    -1,    46,    47,    48,    49,    50,    51,
      37,    38,    39,    40,    41,    -1,    58,    -1,    -1,    46,
      47,    48,    49,    50,    51,    37,    38,    39,    40,    41,
      -1,    58,    -1,    -1,    46,    47,    48,    49,    50,    51,
      37,    38,    39,    40,    41,    -1,    58,    -1,    -1,    46,
      47,    48,    49,    50,    51,     3,    -1,    -1,    -1,    56,
      -1,    -1,    -1,    11,    12,    -1,    14,    15,    16,    17,
      18,    -1,    -1,    21,    22,    37,    38,    39,    40,    41,
      -1,    -1,    -1,    -1,    46,    47,    48,    49,    50,    51,
      -1,    -1,    -1,    55,    37,    38,    39,    40,    41,    -1,
      -1,    -1,    -1,    46,    47,    48,    49,    50,    51,    -1,
      -1,    54,    37,    38,    39,    40,    41,    -1,    -1,    -1,
      -1,    46,    47,    48,    49,    50,    51,    -1,    -1,    54,
      37,    38,    39,    40,    41,    -1,    -1,    -1,    -1,    46,
      47,    48,    49,    50,    51,    -1,    -1,    54,    37,    38,
      39,    40,    41,    -1,    -1,    -1,    -1,    46,    47,    48,
      49,    50,    51,    -1,    -1,    54,    37,    38,    39,    40,
      41,    -1,    -1,    -1,    -1,    46,    47,    48,    49,    50,
      51,    -1,    -1,    54,    37,    38,    39,    40,    41,    -1,
      -1,    -1,    -1,    46,    47,    48,    49,    50,    51
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,    11,    12,    14,    15,    16,    17,    18,    21,
      22,    62,    63,    64,    65,    70,    71,    72,    73,    74,
      75,    76,    78,    53,    57,     3,     3,     3,     3,     5,
       6,     7,     8,    30,    57,    71,    79,    57,    57,    57,
      57,    57,     0,    63,    54,    79,    69,    79,    52,    57,
      52,    57,    52,    57,    79,    37,    38,    39,    40,    41,
      46,    47,    48,    49,    50,    51,    54,    79,     3,    79,
      79,    79,    54,    55,    58,    79,    11,    12,    67,    68,
      79,    68,    79,    79,    58,    79,    79,    79,    79,    79,
      79,    79,    79,    79,    79,    79,    58,    58,    58,    56,
      58,    79,    54,     3,     3,    55,    58,    54,    58,    54,
      55,    54,    54,    59,    79,    59,    67,    59,    59,    79,
      63,    66,    58,    66,    66,    66,    58,    60,    63,    59,
      60,    60,    60,    19,    20,    77,    66,    57,    59,    60,
      79,    66,    58,    60,    59,    66,    60,    77
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1455 of yacc.c  */
#line 794 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); root = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 795 "confused.y"
    { (yyval.nval) = makeNode(NODE_SEQ, (yyvsp[(1) - (2)].nval), NULL, (yyvsp[(2) - (2)].nval)); root = (yyval.nval); ;}
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 799 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 800 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 801 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 802 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 803 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 804 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 805 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 806 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 807 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 808 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (2)].nval); ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 812 "confused.y"
    {
        Node *n = makeNode(NODE_ASK, NULL, NULL, NULL);
        n->name = strdup((yyvsp[(3) - (5)].sval));
        (yyval.nval) = n;
    ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 820 "confused.y"
    {
        (yyval.nval) = makeNode(NODE_RETURN, (yyvsp[(2) - (3)].nval), NULL, NULL);
    ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 825 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 826 "confused.y"
    { (yyval.nval) = makeNode(NODE_SEQ, (yyvsp[(1) - (2)].nval), NULL, (yyvsp[(2) - (2)].nval)); ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 829 "confused.y"
    {
        Node *n = makeNode(NODE_PARAM, NULL, NULL, NULL);
        n->name  = strdup((yyvsp[(2) - (2)].sval));
        n->value = 0;   /* 0 = int parameter */
        (yyval.nval) = n;
    ;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 835 "confused.y"
    {
        Node *n = makeNode(NODE_PARAM, NULL, NULL, NULL);
        n->name  = strdup((yyvsp[(2) - (2)].sval));
        n->value = 1;   /* 1 = float parameter */
        (yyval.nval) = n;
    ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 844 "confused.y"
    { (yyval.nval) = NULL; ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 845 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 846 "confused.y"
    {
        /* chain params using the right pointer */
        Node *p = (yyvsp[(1) - (3)].nval);
        while (p->right != NULL) p = p->right;
        p->right = (yyvsp[(3) - (3)].nval);
        (yyval.nval) = (yyvsp[(1) - (3)].nval);
    ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 856 "confused.y"
    { (yyval.nval) = NULL; ;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 857 "confused.y"
    { (yyval.nval) = makeNode(NODE_ARG, (yyvsp[(1) - (1)].nval), NULL, NULL); ;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 858 "confused.y"
    {
        /* chain args using the right pointer */
        Node *a = makeNode(NODE_ARG, (yyvsp[(3) - (3)].nval), NULL, NULL);
        Node *p = (yyvsp[(1) - (3)].nval);
        while (p->right != NULL) p = p->right;
        p->right = a;
        (yyval.nval) = (yyvsp[(1) - (3)].nval);
    ;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 869 "confused.y"
    {
        Node *n = makeNode(NODE_FUNC_DEF, (yyvsp[(4) - (8)].nval), (yyvsp[(7) - (8)].nval), NULL);
        n->name  = strdup((yyvsp[(2) - (8)].sval));
        n->value = 0;
        insert_func((yyvsp[(2) - (8)].sval), "int", (yyvsp[(4) - (8)].nval), (yyvsp[(7) - (8)].nval));
        (yyval.nval) = n;
    ;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 876 "confused.y"
    {
        Node *n = makeNode(NODE_FUNC_DEF, (yyvsp[(4) - (8)].nval), (yyvsp[(7) - (8)].nval), NULL);
        n->name  = strdup((yyvsp[(2) - (8)].sval));
        n->value = 1;
        insert_func((yyvsp[(2) - (8)].sval), "float", (yyvsp[(4) - (8)].nval), (yyvsp[(7) - (8)].nval));
        (yyval.nval) = n;
    ;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 888 "confused.y"
    {
        Node *n = makeNode(NODE_FUNC_CALL, (yyvsp[(3) - (4)].nval), NULL, NULL);
        n->name = strdup((yyvsp[(1) - (4)].sval));
        (yyval.nval) = n;
    ;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 896 "confused.y"
    {
        /* left = start expr, middle = end expr, right = loop body */
        Node *n = makeNode(NODE_FOR, (yyvsp[(3) - (9)].nval), (yyvsp[(5) - (9)].nval), (yyvsp[(8) - (9)].nval));
        (yyval.nval) = n;
    ;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 903 "confused.y"
    {
        /* condition in left, loop body in middle, no right needed */
        (yyval.nval) = makeNode(NODE_WHILE, (yyvsp[(3) - (7)].nval), (yyvsp[(6) - (7)].nval), NULL);
    ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 909 "confused.y"
    {
        Node *n = makeNode(NODE_COMPOUND_ADD, (yyvsp[(3) - (4)].nval), NULL, NULL);
        n->name = strdup((yyvsp[(1) - (4)].sval));
        (yyval.nval) = n;
    ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 917 "confused.y"
    {
        Node *n = makeNode(NODE_SHOW, (yyvsp[(3) - (5)].nval), NULL, NULL);
        (yyval.nval) = n;
    ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 924 "confused.y"
    {
        /* Iff with no else branch */
        (yyval.nval) = makeNode(NODE_IF, (yyvsp[(3) - (7)].nval), (yyvsp[(6) - (7)].nval), NULL);
    ;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 928 "confused.y"
    {
        /* Iff with or/oriff chain */
        (yyval.nval) = makeNode(NODE_IF, (yyvsp[(3) - (8)].nval), (yyvsp[(6) - (8)].nval), (yyvsp[(8) - (8)].nval));
    ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 935 "confused.y"
    {
        /* plain or — the else branch */
        (yyval.nval) = (yyvsp[(3) - (4)].nval);
    ;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 939 "confused.y"
    {
        /* oriff with no further chain */
        (yyval.nval) = makeNode(NODE_IF, (yyvsp[(3) - (7)].nval), (yyvsp[(6) - (7)].nval), NULL);
    ;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 943 "confused.y"
    {
        /* oriff chained with more oriff/or */
        (yyval.nval) = makeNode(NODE_IF, (yyvsp[(3) - (8)].nval), (yyvsp[(6) - (8)].nval), (yyvsp[(8) - (8)].nval));
    ;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 950 "confused.y"
    {
        Node *n  = makeNode(NODE_DECL, (yyvsp[(4) - (5)].nval), NULL, NULL);
        n->name  = strdup((yyvsp[(2) - (5)].sval));
        n->value = 0;   /* 0 = int */
        (yyval.nval) = n;
    ;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 956 "confused.y"
    {
        Node *n  = makeNode(NODE_DECL, (yyvsp[(4) - (5)].nval), NULL, NULL);
        n->name  = strdup((yyvsp[(2) - (5)].sval));
        n->value = 1;   /* 1 = float */
        (yyval.nval) = n;
    ;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 962 "confused.y"
    {
    Node *n  = makeNode(NODE_DECL, (yyvsp[(4) - (5)].nval), NULL, NULL);
    n->name  = strdup((yyvsp[(2) - (5)].sval));
    n->value = 2;   /* 2 = bool flag */
    (yyval.nval) = n;
    ;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 971 "confused.y"
    { (yyval.nval) = makeNum((yyvsp[(1) - (1)].ival));    ;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 972 "confused.y"
    { (yyval.nval) = makeNum((yyvsp[(1) - (1)].fval));    ;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 973 "confused.y"
    { (yyval.nval) = makeNum(1);     ;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 974 "confused.y"
    { (yyval.nval) = makeNum(0);     ;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 975 "confused.y"
    { (yyval.nval) = makeIdent((yyvsp[(1) - (1)].sval));  ;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 976 "confused.y"
    { (yyval.nval) = makeNode(NODE_ADD, (yyvsp[(1) - (3)].nval), NULL, (yyvsp[(3) - (3)].nval)); ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 977 "confused.y"
    { (yyval.nval) = makeNode(NODE_SUB, (yyvsp[(1) - (3)].nval), NULL, (yyvsp[(3) - (3)].nval)); ;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 978 "confused.y"
    { (yyval.nval) = makeNode(NODE_MUL, (yyvsp[(1) - (3)].nval), NULL, (yyvsp[(3) - (3)].nval)); ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 979 "confused.y"
    { (yyval.nval) = makeNode(NODE_DIV, (yyvsp[(1) - (3)].nval), NULL, (yyvsp[(3) - (3)].nval)); ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 980 "confused.y"
    { (yyval.nval) = makeNode(NODE_MOD, (yyvsp[(1) - (3)].nval), NULL, (yyvsp[(3) - (3)].nval)); ;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 981 "confused.y"
    { (yyval.nval) = makeNode(NODE_GT,  (yyvsp[(1) - (3)].nval), NULL, (yyvsp[(3) - (3)].nval)); ;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 982 "confused.y"
    { (yyval.nval) = makeNode(NODE_LT,  (yyvsp[(1) - (3)].nval), NULL, (yyvsp[(3) - (3)].nval)); ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 983 "confused.y"
    { (yyval.nval) = makeNode(NODE_EQ,  (yyvsp[(1) - (3)].nval), NULL, (yyvsp[(3) - (3)].nval)); ;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 984 "confused.y"
    { (yyval.nval) = makeNode(NODE_NEQ, (yyvsp[(1) - (3)].nval), NULL, (yyvsp[(3) - (3)].nval)); ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 985 "confused.y"
    { (yyval.nval) = makeNode(NODE_GEQ, (yyvsp[(1) - (3)].nval), NULL, (yyvsp[(3) - (3)].nval)); ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 986 "confused.y"
    { (yyval.nval) = makeNode(NODE_LEQ, (yyvsp[(1) - (3)].nval), NULL, (yyvsp[(3) - (3)].nval)); ;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 987 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 988 "confused.y"
    { (yyval.nval) = (yyvsp[(2) - (3)].nval); ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 989 "confused.y"
    { (yyval.nval) = makeNode(NODE_EXPONENTIAL, (yyvsp[(3) - (6)].nval), NULL, (yyvsp[(5) - (6)].nval));;}
    break;



/* Line 1455 of yacc.c  */
#line 2817 "confused.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 992 "confused.y"


int yyerror(char *msg) {
    printf("Syntax Error: %s\n", msg);
    return 0;
}

int main(int argc, char *argv[]) {
    /* If a filename argument was given, open it and point the lexer at it.
       This frees stdin so that Ask() / scanf() can read from the keyboard. */
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f == NULL) {
            printf("Error: Cannot open file '%s'\n", argv[1]);
            return 1;
        }
        yyin = f;   /* yyin is Flex's input file pointer — redirect it to our file */
    }
    /* If no argument given, fall back to stdin as before */

    push_frame("global");
    printf("=== Parsing ===\n");
    yyparse();
    printf("=== Executing ===\n");
    if (root == NULL)
        printf("Error: root is NULL — nothing was parsed!\n");
    else
        execute(root);

    printf("\n=== Three Address Code ===\n");
    if (root != NULL)
        gen_stmt(root);
    printf("=== End TAC ===\n");
    printf("=== Done ===\n");
    return 0;
}
