
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
   STACK AND QUEUE NODE TYPES
   These are heap-allocated — each push/enqueue creates one new node
   via malloc, each pop/dequeue frees one. No fixed-size array anywhere.
   ══════════════════════════════ */
typedef struct StackNode {
    int              data;
    struct StackNode *next;  /* points to the node pushed before this one */
} StackNode;

typedef struct QueueNode {
    int               data;
    struct QueueNode *next;  /* points forward toward the rear */
} QueueNode;

/* ══════════════════════════════
   SYMBOL TABLE
   ══════════════════════════════ */
typedef struct {
    char name[64];
    char type[16];  /* "int","float","bool","char","stack","queue" */
    union {
        int   ival;
        float fval;
        char  cval;
    } value;
    /* Stack fields — only meaningful when type == "stack" */
    StackNode *stk_top;    /* NULL = empty stack                   */
    int        stk_count;  /* how many nodes are currently linked  */
    /* Queue fields — only meaningful when type == "queue" */
    QueueNode *que_front;  /* oldest node — dequeue from here      */
    QueueNode *que_rear;   /* newest node — enqueue after here     */
    int        que_count;
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
    NODE_LEQ, NODE_GEQ,NODE_INC,
    NODE_AND, NODE_OR,  NODE_XOR,
    NODE_NUM, NODE_IDENT,
    NODE_FUNC_DEF, NODE_FUNC_CALL,
    NODE_RETURN,
    NODE_ARG, NODE_PARAM,
    NODE_SHOW, NODE_ASK,
    NODE_COMPOUND_ADD,NODE_EXPONENTIAL,
    NODE_STK_DECL,    /* stk mystack#           */
    NODE_QUEUE_DECL,  /* line myqueue#          */
    NODE_PUSH,        /* lastadd(name $ expr)#  */
    NODE_POP,         /* lastdrag(name)#        */
    NODE_EMPTY_CHECK, /* Nainai(name)           */
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
        case NODE_EMPTY_CHECK: {
            Symbol *sym = get_symbol(n->name);
            if (sym == NULL) {
                printf("Error: '%s' not declared.\n", n->name);
                return 0;
            }
            if (strcmp(sym->type, "stack") == 0)
                return (sym->stk_count == 0) ? 1 : 0;
            if (strcmp(sym->type, "queue") == 0)
                return (sym->que_count == 0) ? 1 : 0;
            return 0;
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
        case NODE_STK_DECL: {
            insert(n->name, "stack", 0);
            Symbol *sym = get_symbol(n->name);
            if (sym) {
                sym->stk_top   = NULL;  /* NULL top = empty linked list */
                sym->stk_count = 0;
            }
            printf("Stack '%s' declared. (linked-list, unlimited size)\n", n->name);
            break;
        }

        case NODE_QUEUE_DECL: {
            insert(n->name, "queue", 0);
            Symbol *sym = get_symbol(n->name);
            if (sym) {
                sym->que_front = NULL;
                sym->que_rear  = NULL;  /* both NULL = empty queue */
                sym->que_count = 0;
            }
            printf("Queue '%s' declared. (linked-list, unlimited size)\n", n->name);
            break;
        }

        case NODE_PUSH: {
            Symbol *sym = get_symbol(n->name);
            if (sym == NULL) {
                printf("Error: '%s' not declared.\n", n->name);
                break;
            }
            int val = (int)eval(n->left);

            if (strcmp(sym->type, "stack") == 0) {
                /* Allocate a brand new node on the heap */
                StackNode *node = malloc(sizeof(StackNode));
                if (node == NULL) { printf("Out of memory!\n"); break; }
                node->data     = val;         /* store the value          */
                node->next     = sym->stk_top;/* new node points to old top */
                sym->stk_top   = node;        /* top now points to new node */
                sym->stk_count++;
                printf("Pushed %d onto stack '%s'  [size=%d]\n",
                    val, n->name, sym->stk_count);

            } else if (strcmp(sym->type, "queue") == 0) {
                QueueNode *node = malloc(sizeof(QueueNode));
                if (node == NULL) { printf("Out of memory!\n"); break; }
                node->data = val;
                node->next = NULL;  /* new node is always the last in line */
                if (sym->que_rear == NULL) {
                    /* Queue was empty — new node is both front and rear */
                    sym->que_front = node;
                    sym->que_rear  = node;
                } else {
                    /* Attach after current rear, then advance rear */
                    sym->que_rear->next = node;
                    sym->que_rear       = node;
                }
                sym->que_count++;
                printf("Enqueued %d into queue '%s'  [size=%d]\n",
                    val, n->name, sym->que_count);

            } else {
                printf("Error: '%s' is not a stack or queue.\n", n->name);
            }
            break;
        }

        case NODE_POP: {
            Symbol *sym = get_symbol(n->name);
            if (sym == NULL) {
                printf("Error: '%s' not declared.\n", n->name);
                break;
            }

            if (strcmp(sym->type, "stack") == 0) {
                if (sym->stk_top == NULL) {
                    printf("Error: Stack '%s' is empty.\n", n->name);
                    break;
                }
                StackNode *old   = sym->stk_top;  /* save pointer before unlinking */
                int        val   = old->data;     /* read value before freeing     */
                sym->stk_top     = old->next;     /* top retreats to previous node */
                free(old);                        /* return memory to heap         */
                sym->stk_count--;
                printf("Popped %d from stack '%s'  [size=%d]\n",
                    val, n->name, sym->stk_count);

            } else if (strcmp(sym->type, "queue") == 0) {
                if (sym->que_front == NULL) {
                    printf("Error: Queue '%s' is empty.\n", n->name);
                    break;
                }
                QueueNode *old = sym->que_front;
                int        val = old->data;
                sym->que_front = old->next;       /* front advances to next node   */
                if (sym->que_front == NULL)
                    sym->que_rear = NULL;         /* queue emptied — rear must also
                                                    be NULL to avoid dangling ptr  */
                free(old);
                sym->que_count--;
                printf("Dequeued %d from queue '%s'  [size=%d]\n",
                    val, n->name, sym->que_count);
            }
            break;
        }

        case NODE_INC: {
            Symbol *sym = get_symbol(n->name);
            if (sym == NULL) {
                printf("Error: Undefined variable '%s'.\n", n->name);
                break;
            }
            if (strcmp(sym->type, "float") == 0) {
                sym->value.fval += 1;
                printf("Inc: %s is now %.2f\n", n->name, sym->value.fval);
            } else {
                sym->value.ival += 1;
                printf("Inc: %s is now %d\n", n->name, sym->value.ival);
            }
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

        case NODE_EMPTY_CHECK: {
            char *t = new_temp();
            printf("%s = empty_check %s\n", t, n->name);
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
            gen(n);
            break;
        case NODE_ASK: {
            /* TAC for input is simply a 'read' instruction targeting the variable.
            This is the TAC equivalent of scanf — a backend would translate
            this into a system call or library call to read from stdin. */
            printf("read %s\n", n->name);
            break;
        }

        case NODE_STK_DECL:
            printf("stack_decl %s\n", n->name);
            break;

        case NODE_QUEUE_DECL:
            printf("queue_decl %s\n", n->name);
            break;

        case NODE_PUSH: {
            char *val = gen(n->left);
            if (strcmp("stack", "stack") == 0)  /* will refine with type lookup later */
                printf("push %s %s\n", n->name, val);
            else
                printf("enqueue %s %s\n", n->name, val);
            break;
        }



        case NODE_POP:{
            printf("pop %s\n", n->name);
            break;
        }
        case NODE_INC: {
            char *t = new_temp();
            printf("%s = %s + 1\n", t, n->name);
            printf("%s = %s\n", n->name, t);
            break;
        }    


        default:
            break;
    }
}



/* Line 189 of yacc.c  */
#line 1016 "confused.tab.c"

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
#line 945 "confused.y"

    char        *sval;
    int          ival;
    float        fval;
    char         cval;
    struct Node *nval;



/* Line 214 of yacc.c  */
#line 1118 "confused.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 1130 "confused.tab.c"

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
#define YYFINAL  60
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   688

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  61
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  24
/* YYNRULES -- Number of rules.  */
#define YYNRULES  71
/* YYNRULES -- Number of states.  */
#define YYNSTATES  184

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
      59,    60,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    57,     2,    58,     2,     2,     2,     2,
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
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     8,    13,    15,    17,    19,    21,
      23,    25,    27,    29,    31,    33,    35,    37,    39,    41,
      44,    50,    56,    60,    62,    65,    68,    71,    72,    74,
      78,    79,    81,    85,    94,   103,   108,   118,   126,   131,
     137,   141,   145,   153,   159,   167,   176,   181,   189,   198,
     204,   210,   216,   218,   220,   222,   224,   226,   230,   234,
     238,   242,   246,   250,   254,   258,   262,   266,   270,   277,
     282,   284
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      62,     0,    -1,    63,    -1,    62,    63,    -1,    10,    57,
      67,    58,    -1,    83,    -1,    76,    -1,    65,    -1,    81,
      -1,    74,    -1,    73,    -1,    75,    -1,    71,    -1,    66,
      -1,    77,    -1,    78,    -1,    79,    -1,    80,    -1,    64,
      -1,    72,    54,    -1,    31,    59,     3,    60,    54,    -1,
      17,    59,     3,    60,    54,    -1,    15,    84,    54,    -1,
      63,    -1,    67,    63,    -1,    11,     3,    -1,    12,     3,
      -1,    -1,    68,    -1,    69,    55,    68,    -1,    -1,    84,
      -1,    70,    55,    84,    -1,    11,     3,    59,    69,    60,
      57,    67,    58,    -1,    12,     3,    59,    69,    60,    57,
      67,    58,    -1,     3,    59,    70,    60,    -1,    21,    59,
      84,    56,    84,    60,    57,    67,    58,    -1,    22,    59,
      84,    60,    57,    67,    58,    -1,     3,    53,    84,    54,
      -1,    16,    59,    84,    60,    54,    -1,    26,     3,    54,
      -1,    27,     3,    54,    -1,    33,    59,     3,    55,    84,
      60,    54,    -1,    34,    59,     3,    60,    54,    -1,    18,
      59,    84,    60,    57,    67,    58,    -1,    18,    59,    84,
      60,    57,    67,    58,    82,    -1,    20,    57,    67,    58,
      -1,    19,    59,    84,    60,    57,    67,    58,    -1,    19,
      59,    84,    60,    57,    67,    58,    82,    -1,    11,     3,
      52,    84,    54,    -1,    12,     3,    52,    84,    54,    -1,
      14,     3,    52,    84,    54,    -1,     5,    -1,     8,    -1,
       6,    -1,     7,    -1,     3,    -1,    84,    37,    84,    -1,
      84,    38,    84,    -1,    84,    39,    84,    -1,    84,    40,
      84,    -1,    84,    41,    84,    -1,    84,    46,    84,    -1,
      84,    47,    84,    -1,    84,    48,    84,    -1,    84,    51,
      84,    -1,    84,    49,    84,    -1,    84,    50,    84,    -1,
      30,    59,    84,    55,    84,    60,    -1,    32,    59,     3,
      60,    -1,    72,    -1,    59,    84,    60,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   988,   988,   989,   990,   994,   995,   996,   997,   998,
     999,  1000,  1001,  1002,  1003,  1004,  1005,  1006,  1007,  1008,
    1013,  1021,  1029,  1034,  1035,  1038,  1044,  1053,  1054,  1055,
    1065,  1066,  1067,  1078,  1085,  1097,  1105,  1112,  1118,  1126,
    1133,  1141,  1149,  1157,  1165,  1169,  1176,  1180,  1184,  1191,
    1197,  1203,  1212,  1213,  1214,  1215,  1216,  1217,  1218,  1219,
    1220,  1221,  1222,  1223,  1224,  1225,  1226,  1227,  1228,  1229,
    1234,  1235
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
  "COMPOUNDADD", "HASH", "DOLLAR", "COLON", "'{'", "'}'", "'('", "')'",
  "$accept", "program", "statement", "inc_stmt", "ask_stmt", "return_stmt",
  "block", "param", "param_list", "arg_list", "func_def", "func_call",
  "flp_stmt", "wlp_stmt", "compound_stmt", "show_stmt", "stk_decl",
  "queue_decl", "push_stmt", "pop_stmt", "iff_stmt", "or_chain",
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
     305,   306,   307,   308,   309,   310,   311,   123,   125,    40,
      41
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    61,    62,    62,    62,    63,    63,    63,    63,    63,
      63,    63,    63,    63,    63,    63,    63,    63,    63,    63,
      64,    65,    66,    67,    67,    68,    68,    69,    69,    69,
      70,    70,    70,    71,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    81,    82,    82,    82,    83,
      83,    83,    84,    84,    84,    84,    84,    84,    84,    84,
      84,    84,    84,    84,    84,    84,    84,    84,    84,    84,
      84,    84
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     4,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       5,     5,     3,     1,     2,     2,     2,     0,     1,     3,
       0,     1,     3,     8,     8,     4,     9,     7,     4,     5,
       3,     3,     7,     5,     7,     8,     4,     7,     8,     5,
       5,     5,     1,     1,     1,     1,     1,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     6,     4,
       1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     2,    18,
       7,    13,    12,     0,    10,     9,    11,     6,    14,    15,
      16,    17,     8,     5,     0,    30,     0,     0,     0,     0,
      56,    52,    54,    55,    53,     0,     0,     0,    70,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       1,     3,    19,     0,     0,    31,    23,     0,     0,    27,
       0,    27,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    22,     0,     0,
       0,     0,     0,    40,    41,     0,     0,     0,    38,     0,
      35,     4,    24,     0,     0,     0,    28,     0,     0,     0,
       0,     0,     0,    71,    57,    58,    59,    60,    61,    62,
      63,    64,    66,    67,    65,     0,     0,     0,     0,     0,
       0,     0,     0,    32,    49,    25,    26,     0,     0,    50,
       0,    51,     0,    69,    39,    21,     0,     0,     0,    20,
       0,    43,    29,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    68,    44,     0,    37,    42,    33,    34,     0,
       0,    45,     0,     0,     0,    36,     0,     0,     0,    46,
       0,     0,    47,    48
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    17,    66,    19,    20,    21,    67,   106,   107,    64,
      22,    48,    24,    25,    26,    27,    28,    29,    30,    31,
      32,   171,    33,    49
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -48
static const yytype_int16 yypact[] =
{
     365,   -28,   -27,    15,    29,    37,     3,   -15,    -2,    -1,
       4,     6,    56,    57,    10,    11,    12,   340,   -48,   -48,
     -48,   -48,   -48,    26,   -48,   -48,   -48,   -48,   -48,   -48,
     -48,   -48,   -48,   -48,     3,     3,   390,   -47,   -39,    14,
      22,   -48,   -48,   -48,   -48,    25,    27,     3,   -48,   547,
       3,    82,     3,     3,     3,    35,    39,    88,    93,   103,
     -48,   -48,   -48,   565,   -26,   637,   -48,    61,     3,   -10,
       3,   -10,     3,     3,   104,   388,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,   -48,   403,    50,
     418,   508,   433,   -48,   -48,    52,    60,    62,   -48,     3,
     -48,   -48,   -48,   583,   113,   114,   -48,   -17,   601,     1,
     619,   528,    64,   -48,   -13,   -13,   -48,   -48,   -48,    95,
      95,    95,    95,    95,    95,    72,    73,    71,     3,    80,
      75,     3,    84,   637,   -48,   -48,   -48,   -10,    83,   -48,
      86,   -48,     3,   -48,   -48,   -48,   390,   448,   390,   -48,
     463,   -48,   -48,   390,   390,   478,    87,    90,   175,    85,
     201,   227,   -48,    -5,   390,   -48,   -48,   -48,   -48,    91,
      92,   -48,   253,     3,   390,   -48,   493,   279,    98,   -48,
     390,   305,    -5,   -48
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -48,   -48,     7,   -48,   -48,   -48,   -23,    32,    81,   -48,
     -48,     0,   -48,   -48,   -48,   -48,   -48,   -48,   -48,   -48,
     -48,   -38,   -48,   -31
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      23,   104,   105,    63,    65,    68,    40,    18,    41,    42,
      43,    44,    69,    70,   169,   170,    75,    23,    37,    88,
      71,    90,    91,    92,    61,    34,    78,    79,    80,    99,
      36,    35,    38,    45,   100,    46,    23,   103,   137,   108,
      39,   110,   111,   138,    50,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   137,    51,    52,    55,
      56,   140,    47,    53,     1,    54,    72,    23,   133,    57,
      58,    59,     3,     4,   102,     5,     6,     7,     8,     9,
      62,    35,    10,    11,    73,    89,    74,    12,    13,    93,
       1,    95,    14,    94,    15,    16,    96,   147,     3,     4,
     150,     5,     6,     7,     8,     9,    97,   112,    10,    11,
     126,   155,   130,    12,    13,   131,   135,   136,    14,   101,
      15,    16,   132,   156,   143,   158,   144,   145,   146,   149,
     160,   161,    76,    77,    78,    79,    80,   148,   151,   166,
     153,   172,   176,   154,   183,   163,    23,   164,    23,   174,
     173,   177,   109,    23,    23,   180,    23,   181,    23,     0,
      23,    23,     0,   102,    23,   102,     0,   102,   102,   152,
       0,     0,    23,     0,    23,     0,     0,    23,     1,   102,
      23,    23,     0,     0,   102,     0,     3,     4,   102,     5,
       6,     7,     8,     9,     0,     0,    10,    11,     0,     0,
       0,    12,    13,     0,     1,     0,    14,     0,    15,    16,
       0,     0,     3,     4,     0,     5,     6,     7,     8,     9,
       0,     0,    10,    11,     0,     0,     0,    12,    13,     0,
       1,     0,    14,   165,    15,    16,     0,     0,     3,     4,
       0,     5,     6,     7,     8,     9,     0,     0,    10,    11,
       0,     0,     0,    12,    13,     0,     1,     0,    14,   167,
      15,    16,     0,     0,     3,     4,     0,     5,     6,     7,
       8,     9,     0,     0,    10,    11,     0,     0,     0,    12,
      13,     0,     1,     0,    14,   168,    15,    16,     0,     0,
       3,     4,     0,     5,     6,     7,     8,     9,     0,     0,
      10,    11,     0,     0,     0,    12,    13,     0,     1,     0,
      14,   175,    15,    16,     0,     0,     3,     4,     0,     5,
       6,     7,     8,     9,     0,     0,    10,    11,     0,     0,
       0,    12,    13,     0,     0,     0,    14,   179,    15,    16,
      60,     0,     0,     1,     0,     0,     0,     0,     0,     0,
       0,     3,     4,     0,     5,     6,     7,     8,     9,     0,
       0,    10,    11,   182,     0,     0,    12,    13,     1,     0,
       0,    14,     0,    15,    16,     2,     3,     4,     0,     5,
       6,     7,     8,     9,     0,     0,    10,    11,     0,     0,
       0,    12,    13,     1,     0,     0,    14,     0,    15,    16,
       0,     3,     4,     0,     5,     6,     7,     8,     9,     0,
       0,    10,    11,     0,     0,     0,    12,    13,     0,     0,
       0,    14,     0,    15,    16,    76,    77,    78,    79,    80,
       0,     0,     0,     0,    81,    82,    83,    84,    85,    86,
      76,    77,    78,    79,    80,     0,     0,     0,   113,    81,
      82,    83,    84,    85,    86,    76,    77,    78,    79,    80,
       0,     0,     0,   125,    81,    82,    83,    84,    85,    86,
      76,    77,    78,    79,    80,     0,     0,     0,   127,    81,
      82,    83,    84,    85,    86,    76,    77,    78,    79,    80,
       0,     0,     0,   129,    81,    82,    83,    84,    85,    86,
      76,    77,    78,    79,    80,     0,     0,     0,   157,    81,
      82,    83,    84,    85,    86,    76,    77,    78,    79,    80,
       0,     0,     0,   159,    81,    82,    83,    84,    85,    86,
      76,    77,    78,    79,    80,     0,     0,     0,   162,    81,
      82,    83,    84,    85,    86,    76,    77,    78,    79,    80,
       0,     0,     0,   178,    81,    82,    83,    84,    85,    86,
       0,     0,     0,     0,   128,    76,    77,    78,    79,    80,
       0,     0,     0,     0,    81,    82,    83,    84,    85,    86,
       0,     0,     0,   142,    76,    77,    78,    79,    80,     0,
       0,     0,     0,    81,    82,    83,    84,    85,    86,     0,
       0,    87,    76,    77,    78,    79,    80,     0,     0,     0,
       0,    81,    82,    83,    84,    85,    86,     0,     0,    98,
      76,    77,    78,    79,    80,     0,     0,     0,     0,    81,
      82,    83,    84,    85,    86,     0,     0,   134,    76,    77,
      78,    79,    80,     0,     0,     0,     0,    81,    82,    83,
      84,    85,    86,     0,     0,   139,    76,    77,    78,    79,
      80,     0,     0,     0,     0,    81,    82,    83,    84,    85,
      86,     0,     0,   141,    76,    77,    78,    79,    80,     0,
       0,     0,     0,    81,    82,    83,    84,    85,    86
};

static const yytype_int16 yycheck[] =
{
       0,    11,    12,    34,    35,    52,     3,     0,     5,     6,
       7,     8,    59,    52,    19,    20,    47,    17,     3,    50,
      59,    52,    53,    54,    17,    53,    39,    40,    41,    55,
      57,    59,     3,    30,    60,    32,    36,    68,    55,    70,
       3,    72,    73,    60,    59,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    55,    59,    59,     3,
       3,    60,    59,    59,     3,    59,    52,    67,    99,    59,
      59,    59,    11,    12,    67,    14,    15,    16,    17,    18,
      54,    59,    21,    22,    59,     3,    59,    26,    27,    54,
       3,     3,    31,    54,    33,    34,     3,   128,    11,    12,
     131,    14,    15,    16,    17,    18,     3,     3,    21,    22,
      60,   142,    60,    26,    27,    55,     3,     3,    31,    58,
      33,    34,    60,   146,    60,   148,    54,    54,    57,    54,
     153,   154,    37,    38,    39,    40,    41,    57,    54,    54,
      57,   164,   173,    57,   182,    58,   146,    57,   148,    57,
      59,   174,    71,   153,   154,    57,   156,   180,   158,    -1,
     160,   161,    -1,   156,   164,   158,    -1,   160,   161,   137,
      -1,    -1,   172,    -1,   174,    -1,    -1,   177,     3,   172,
     180,   181,    -1,    -1,   177,    -1,    11,    12,   181,    14,
      15,    16,    17,    18,    -1,    -1,    21,    22,    -1,    -1,
      -1,    26,    27,    -1,     3,    -1,    31,    -1,    33,    34,
      -1,    -1,    11,    12,    -1,    14,    15,    16,    17,    18,
      -1,    -1,    21,    22,    -1,    -1,    -1,    26,    27,    -1,
       3,    -1,    31,    58,    33,    34,    -1,    -1,    11,    12,
      -1,    14,    15,    16,    17,    18,    -1,    -1,    21,    22,
      -1,    -1,    -1,    26,    27,    -1,     3,    -1,    31,    58,
      33,    34,    -1,    -1,    11,    12,    -1,    14,    15,    16,
      17,    18,    -1,    -1,    21,    22,    -1,    -1,    -1,    26,
      27,    -1,     3,    -1,    31,    58,    33,    34,    -1,    -1,
      11,    12,    -1,    14,    15,    16,    17,    18,    -1,    -1,
      21,    22,    -1,    -1,    -1,    26,    27,    -1,     3,    -1,
      31,    58,    33,    34,    -1,    -1,    11,    12,    -1,    14,
      15,    16,    17,    18,    -1,    -1,    21,    22,    -1,    -1,
      -1,    26,    27,    -1,    -1,    -1,    31,    58,    33,    34,
       0,    -1,    -1,     3,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    11,    12,    -1,    14,    15,    16,    17,    18,    -1,
      -1,    21,    22,    58,    -1,    -1,    26,    27,     3,    -1,
      -1,    31,    -1,    33,    34,    10,    11,    12,    -1,    14,
      15,    16,    17,    18,    -1,    -1,    21,    22,    -1,    -1,
      -1,    26,    27,     3,    -1,    -1,    31,    -1,    33,    34,
      -1,    11,    12,    -1,    14,    15,    16,    17,    18,    -1,
      -1,    21,    22,    -1,    -1,    -1,    26,    27,    -1,    -1,
      -1,    31,    -1,    33,    34,    37,    38,    39,    40,    41,
      -1,    -1,    -1,    -1,    46,    47,    48,    49,    50,    51,
      37,    38,    39,    40,    41,    -1,    -1,    -1,    60,    46,
      47,    48,    49,    50,    51,    37,    38,    39,    40,    41,
      -1,    -1,    -1,    60,    46,    47,    48,    49,    50,    51,
      37,    38,    39,    40,    41,    -1,    -1,    -1,    60,    46,
      47,    48,    49,    50,    51,    37,    38,    39,    40,    41,
      -1,    -1,    -1,    60,    46,    47,    48,    49,    50,    51,
      37,    38,    39,    40,    41,    -1,    -1,    -1,    60,    46,
      47,    48,    49,    50,    51,    37,    38,    39,    40,    41,
      -1,    -1,    -1,    60,    46,    47,    48,    49,    50,    51,
      37,    38,    39,    40,    41,    -1,    -1,    -1,    60,    46,
      47,    48,    49,    50,    51,    37,    38,    39,    40,    41,
      -1,    -1,    -1,    60,    46,    47,    48,    49,    50,    51,
      -1,    -1,    -1,    -1,    56,    37,    38,    39,    40,    41,
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
       0,     3,    10,    11,    12,    14,    15,    16,    17,    18,
      21,    22,    26,    27,    31,    33,    34,    62,    63,    64,
      65,    66,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    83,    53,    59,    57,     3,     3,     3,
       3,     5,     6,     7,     8,    30,    32,    59,    72,    84,
      59,    59,    59,    59,    59,     3,     3,    59,    59,    59,
       0,    63,    54,    84,    70,    84,    63,    67,    52,    59,
      52,    59,    52,    59,    59,    84,    37,    38,    39,    40,
      41,    46,    47,    48,    49,    50,    51,    54,    84,     3,
      84,    84,    84,    54,    54,     3,     3,     3,    54,    55,
      60,    58,    63,    84,    11,    12,    68,    69,    84,    69,
      84,    84,     3,    60,    84,    84,    84,    84,    84,    84,
      84,    84,    84,    84,    84,    60,    60,    60,    56,    60,
      60,    55,    60,    84,    54,     3,     3,    55,    60,    54,
      60,    54,    55,    60,    54,    54,    57,    84,    57,    54,
      84,    54,    68,    57,    57,    84,    67,    60,    67,    60,
      67,    67,    60,    58,    57,    58,    54,    58,    58,    19,
      20,    82,    67,    59,    57,    58,    84,    67,    60,    58,
      57,    67,    58,    82
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
#line 988 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); root = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 989 "confused.y"
    { (yyval.nval) = makeNode(NODE_SEQ, (yyvsp[(1) - (2)].nval), NULL, (yyvsp[(2) - (2)].nval)); root = (yyval.nval); ;}
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 990 "confused.y"
    { (yyval.nval) = (yyvsp[(3) - (4)].nval); root = (yyvsp[(3) - (4)].nval); ;}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 994 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 995 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 996 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 997 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 998 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 999 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 1000 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 1001 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 1002 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 1003 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 1004 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 1005 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 1006 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 1007 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 1008 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (2)].nval); ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 1013 "confused.y"
    {
        Node *n = makeNode(NODE_INC, NULL, NULL, NULL);
        n->name = strdup((yyvsp[(3) - (5)].sval));
        (yyval.nval) = n;
    ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 1021 "confused.y"
    {
        Node *n = makeNode(NODE_ASK, NULL, NULL, NULL);
        n->name = strdup((yyvsp[(3) - (5)].sval));
        (yyval.nval) = n;
    ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 1029 "confused.y"
    {
        (yyval.nval) = makeNode(NODE_RETURN, (yyvsp[(2) - (3)].nval), NULL, NULL);
    ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 1034 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 1035 "confused.y"
    { (yyval.nval) = makeNode(NODE_SEQ, (yyvsp[(1) - (2)].nval), NULL, (yyvsp[(2) - (2)].nval)); ;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 1038 "confused.y"
    {
        Node *n = makeNode(NODE_PARAM, NULL, NULL, NULL);
        n->name  = strdup((yyvsp[(2) - (2)].sval));
        n->value = 0;   /* 0 = int parameter */
        (yyval.nval) = n;
    ;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 1044 "confused.y"
    {
        Node *n = makeNode(NODE_PARAM, NULL, NULL, NULL);
        n->name  = strdup((yyvsp[(2) - (2)].sval));
        n->value = 1;   /* 1 = float parameter */
        (yyval.nval) = n;
    ;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 1053 "confused.y"
    { (yyval.nval) = NULL; ;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 1054 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 1055 "confused.y"
    {
        /* chain params using the right pointer */
        Node *p = (yyvsp[(1) - (3)].nval);
        while (p->right != NULL) p = p->right;
        p->right = (yyvsp[(3) - (3)].nval);
        (yyval.nval) = (yyvsp[(1) - (3)].nval);
    ;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 1065 "confused.y"
    { (yyval.nval) = NULL; ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 1066 "confused.y"
    { (yyval.nval) = makeNode(NODE_ARG, (yyvsp[(1) - (1)].nval), NULL, NULL); ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 1067 "confused.y"
    {
        /* chain args using the right pointer */
        Node *a = makeNode(NODE_ARG, (yyvsp[(3) - (3)].nval), NULL, NULL);
        Node *p = (yyvsp[(1) - (3)].nval);
        while (p->right != NULL) p = p->right;
        p->right = a;
        (yyval.nval) = (yyvsp[(1) - (3)].nval);
    ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 1078 "confused.y"
    {
        Node *n = makeNode(NODE_FUNC_DEF, (yyvsp[(4) - (8)].nval), (yyvsp[(7) - (8)].nval), NULL);
        n->name  = strdup((yyvsp[(2) - (8)].sval));
        n->value = 0;
        insert_func((yyvsp[(2) - (8)].sval), "int", (yyvsp[(4) - (8)].nval), (yyvsp[(7) - (8)].nval));
        (yyval.nval) = n;
    ;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 1085 "confused.y"
    {
        Node *n = makeNode(NODE_FUNC_DEF, (yyvsp[(4) - (8)].nval), (yyvsp[(7) - (8)].nval), NULL);
        n->name  = strdup((yyvsp[(2) - (8)].sval));
        n->value = 1;
        insert_func((yyvsp[(2) - (8)].sval), "float", (yyvsp[(4) - (8)].nval), (yyvsp[(7) - (8)].nval));
        (yyval.nval) = n;
    ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 1097 "confused.y"
    {
        Node *n = makeNode(NODE_FUNC_CALL, (yyvsp[(3) - (4)].nval), NULL, NULL);
        n->name = strdup((yyvsp[(1) - (4)].sval));
        (yyval.nval) = n;
    ;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 1105 "confused.y"
    {
        /* left = start expr, middle = end expr, right = loop body */
        Node *n = makeNode(NODE_FOR, (yyvsp[(3) - (9)].nval), (yyvsp[(5) - (9)].nval), (yyvsp[(8) - (9)].nval));
        (yyval.nval) = n;
    ;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 1112 "confused.y"
    {
        /* condition in left, loop body in middle, no right needed */
        (yyval.nval) = makeNode(NODE_WHILE, (yyvsp[(3) - (7)].nval), (yyvsp[(6) - (7)].nval), NULL);
    ;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 1118 "confused.y"
    {
        Node *n = makeNode(NODE_COMPOUND_ADD, (yyvsp[(3) - (4)].nval), NULL, NULL);
        n->name = strdup((yyvsp[(1) - (4)].sval));
        (yyval.nval) = n;
    ;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 1126 "confused.y"
    {
        Node *n = makeNode(NODE_SHOW, (yyvsp[(3) - (5)].nval), NULL, NULL);
        (yyval.nval) = n;
    ;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 1133 "confused.y"
    {
        Node *n = makeNode(NODE_STK_DECL, NULL, NULL, NULL);
        n->name = strdup((yyvsp[(2) - (3)].sval));
        (yyval.nval) = n;
    ;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 1141 "confused.y"
    {
        Node *n = makeNode(NODE_QUEUE_DECL, NULL, NULL, NULL);
        n->name = strdup((yyvsp[(2) - (3)].sval));
        (yyval.nval) = n;
    ;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 1149 "confused.y"
    {
        Node *n = makeNode(NODE_PUSH, (yyvsp[(5) - (7)].nval), NULL, NULL);
        n->name = strdup((yyvsp[(3) - (7)].sval));
        (yyval.nval) = n;
    ;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 1157 "confused.y"
    {
        Node *n = makeNode(NODE_POP, NULL, NULL, NULL);
        n->name = strdup((yyvsp[(3) - (5)].sval));
        (yyval.nval) = n;
    ;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 1165 "confused.y"
    {
        /* Iff with no else branch */
        (yyval.nval) = makeNode(NODE_IF, (yyvsp[(3) - (7)].nval), (yyvsp[(6) - (7)].nval), NULL);
    ;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 1169 "confused.y"
    {
        /* Iff with or/oriff chain */
        (yyval.nval) = makeNode(NODE_IF, (yyvsp[(3) - (8)].nval), (yyvsp[(6) - (8)].nval), (yyvsp[(8) - (8)].nval));
    ;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 1176 "confused.y"
    {
        /* plain or — the else branch */
        (yyval.nval) = (yyvsp[(3) - (4)].nval);
    ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 1180 "confused.y"
    {
        /* oriff with no further chain */
        (yyval.nval) = makeNode(NODE_IF, (yyvsp[(3) - (7)].nval), (yyvsp[(6) - (7)].nval), NULL);
    ;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 1184 "confused.y"
    {
        /* oriff chained with more oriff/or */
        (yyval.nval) = makeNode(NODE_IF, (yyvsp[(3) - (8)].nval), (yyvsp[(6) - (8)].nval), (yyvsp[(8) - (8)].nval));
    ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 1191 "confused.y"
    {
        Node *n  = makeNode(NODE_DECL, (yyvsp[(4) - (5)].nval), NULL, NULL);
        n->name  = strdup((yyvsp[(2) - (5)].sval));
        n->value = 0;   /* 0 = int */
        (yyval.nval) = n;
    ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 1197 "confused.y"
    {
        Node *n  = makeNode(NODE_DECL, (yyvsp[(4) - (5)].nval), NULL, NULL);
        n->name  = strdup((yyvsp[(2) - (5)].sval));
        n->value = 1;   /* 1 = float */
        (yyval.nval) = n;
    ;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 1203 "confused.y"
    {
    Node *n  = makeNode(NODE_DECL, (yyvsp[(4) - (5)].nval), NULL, NULL);
    n->name  = strdup((yyvsp[(2) - (5)].sval));
    n->value = 2;   /* 2 = bool flag */
    (yyval.nval) = n;
    ;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 1212 "confused.y"
    { (yyval.nval) = makeNum((yyvsp[(1) - (1)].ival));    ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 1213 "confused.y"
    { (yyval.nval) = makeNum((yyvsp[(1) - (1)].fval));    ;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 1214 "confused.y"
    { (yyval.nval) = makeNum(1);     ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 1215 "confused.y"
    { (yyval.nval) = makeNum(0);     ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 1216 "confused.y"
    { (yyval.nval) = makeIdent((yyvsp[(1) - (1)].sval));  ;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 1217 "confused.y"
    { (yyval.nval) = makeNode(NODE_ADD, (yyvsp[(1) - (3)].nval), NULL, (yyvsp[(3) - (3)].nval)); ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 1218 "confused.y"
    { (yyval.nval) = makeNode(NODE_SUB, (yyvsp[(1) - (3)].nval), NULL, (yyvsp[(3) - (3)].nval)); ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 1219 "confused.y"
    { (yyval.nval) = makeNode(NODE_MUL, (yyvsp[(1) - (3)].nval), NULL, (yyvsp[(3) - (3)].nval)); ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 1220 "confused.y"
    { (yyval.nval) = makeNode(NODE_DIV, (yyvsp[(1) - (3)].nval), NULL, (yyvsp[(3) - (3)].nval)); ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 1221 "confused.y"
    { (yyval.nval) = makeNode(NODE_MOD, (yyvsp[(1) - (3)].nval), NULL, (yyvsp[(3) - (3)].nval)); ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 1222 "confused.y"
    { (yyval.nval) = makeNode(NODE_GT,  (yyvsp[(1) - (3)].nval), NULL, (yyvsp[(3) - (3)].nval)); ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 1223 "confused.y"
    { (yyval.nval) = makeNode(NODE_LT,  (yyvsp[(1) - (3)].nval), NULL, (yyvsp[(3) - (3)].nval)); ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 1224 "confused.y"
    { (yyval.nval) = makeNode(NODE_EQ,  (yyvsp[(1) - (3)].nval), NULL, (yyvsp[(3) - (3)].nval)); ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 1225 "confused.y"
    { (yyval.nval) = makeNode(NODE_NEQ, (yyvsp[(1) - (3)].nval), NULL, (yyvsp[(3) - (3)].nval)); ;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 1226 "confused.y"
    { (yyval.nval) = makeNode(NODE_GEQ, (yyvsp[(1) - (3)].nval), NULL, (yyvsp[(3) - (3)].nval)); ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 1227 "confused.y"
    { (yyval.nval) = makeNode(NODE_LEQ, (yyvsp[(1) - (3)].nval), NULL, (yyvsp[(3) - (3)].nval)); ;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 1228 "confused.y"
    { (yyval.nval) = makeNode(NODE_EXPONENTIAL, (yyvsp[(3) - (6)].nval), NULL, (yyvsp[(5) - (6)].nval));;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 1229 "confused.y"
    {
        Node *n = makeNode(NODE_EMPTY_CHECK, NULL, NULL, NULL);
        n->name = strdup((yyvsp[(3) - (4)].sval));
        (yyval.nval) = n;
    ;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 1234 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 1235 "confused.y"
    { (yyval.nval) = (yyvsp[(2) - (3)].nval); ;}
    break;



/* Line 1455 of yacc.c  */
#line 3183 "confused.tab.c"
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
#line 1238 "confused.y"


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
