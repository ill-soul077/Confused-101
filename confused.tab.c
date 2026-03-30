
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


/* STACK AND QUEUE */
typedef struct StackNode {
    int              data;
    struct StackNode *next;  /* points to the node pushed before this one */
} StackNode;

typedef struct QueueNode {
    int               data;
    struct QueueNode *next;  /* points forward toward the rear */
} QueueNode;

/*  SYMBOL TABLE */
typedef struct {
    char name[64];
    char type[16];
    union {
        int   ival;
        float fval;
        char  cval;
    } value;
    StackNode *stk_top;    int stk_count;
    QueueNode *que_front;  QueueNode *que_rear;  int que_count;
    int  *arr_data;   /* heap-allocated int array, NULL if not an array */
    int   arr_size;   /* declared size                                  */
    char *str_val;    /* heap-allocated string, NULL if not a string    */
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

/* AST NODE TYPES*/
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
    NODE_STRING_LIT,
    NODE_SWITCH,
    NODE_CASE,
    NODE_ARRAY_DECL,   /* MY_ARRAY In name(size)#      */
    NODE_ARRAY_ASSIGN, /* name(index) @ expr#           */
    NODE_ARRAY_ACCESS, /* name(index) used as expr      */
    NODE_STR_JOIN,     /* strjoin(a $ b)                */
    NODE_STR_LEN,      /* strlen(a)                     */
    NODE_STR_PART,     /* takepart(a $ start $ len)     */
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


/*  NODE CONSTRUCTORS */
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

/*    EVAL      */
void execute(Node *n);  /* forward declaration */
float eval(Node *n);    /* forward declaration */

/* Returns a malloc'd string for string-valued nodes, or NULL if not a string node */
char *eval_str(Node *n) {
    if (n == NULL) return NULL;
    if (n->type == NODE_STRING_LIT) return strdup(n->name);
    if (n->type == NODE_IDENT) {
        Symbol *sym = get_symbol(n->name);
        if (sym && strcmp(sym->type, "string") == 0 && sym->str_val)
            return strdup(sym->str_val);
        return NULL;
    }
    if (n->type == NODE_STR_JOIN) {
        char *a = eval_str(n->left);
        char *b = eval_str(n->right);
        if (!a || !b) { free(a); free(b); return strdup(""); }
        char *result = malloc(strlen(a) + strlen(b) + 1);
        strcpy(result, a); strcat(result, b);
        free(a); free(b);
        return result;
    }
    if (n->type == NODE_STR_PART) {
        char *a     = eval_str(n->left);
        int   start = (int)eval(n->middle);
        int   len   = (int)eval(n->right);
        if (!a) return strdup("");
        int slen = strlen(a);
        if (start < 0) start = 0;
        if (start >= slen) { free(a); return strdup(""); }
        if (start + len > slen) len = slen - start;
        char *result = malloc(len + 1);
        strncpy(result, a + start, len);
        result[len] = '\0';
        free(a);
        return result;
    }
    return NULL;
}

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

        case NODE_STR_LEN: {
            char *s = eval_str(n->left);
            int len = s ? (int)strlen(s) : 0;
            free(s);
            return (float)len;
        }

        case NODE_ARRAY_ACCESS: {
            Symbol *sym = get_symbol(n->name);
            if (sym == NULL || strcmp(sym->type, "array") != 0) {
                printf("Error: '%s' is not a declared array.\n", n->name);
                return 0;
            }
            int idx = (int)eval(n->left);
            if (idx < 0 || idx >= sym->arr_size) {
                printf("Error: Array '%s' index %d out of bounds.\n", n->name, idx);
                return 0;
            }
            return sym->arr_data[idx];
        }

        default: return 0;
    }
}

/*  EXECUTE  */
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
            } else if (n->value == 3) {
                /* string declaration: ch name @ "literal"# */
                insert(n->name, "string", 0);
                Symbol *sym = get_symbol(n->name);
                if (sym && n->left) {
                    sym->str_val = strdup(n->left->name);
                }
                printf("Stored: %s = \"%s\" (type: string)\n", n->name, 
                    sym ? sym->str_val : "?");
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
                        if (n->left && (n->left->type == NODE_STRING_LIT ||
                            n->left->type == NODE_STR_JOIN   ||
                            n->left->type == NODE_STR_PART)) {
                char *s = eval_str(n->left);
                if (s) { printf("%s\n", s); free(s); }
                break;
            }
            /* Also handle Show(stringVar) where the ident holds a string */
            if (n->left && n->left->type == NODE_IDENT) {
                Symbol *sym = get_symbol(n->left->name);
                if (sym && strcmp(sym->type, "string") == 0) {
                    printf("%s\n", sym->str_val ? sym->str_val : "");
                    break;
                }
            }
            /* Check for string literal FIRST — eval() cannot handle strings */
            if (n->left && n->left->type == NODE_STRING_LIT) {
                printf("%s\n", n->left->name);
                break;
            }
            /* For everything else, eval() gives us the numeric value */
            float val = eval(n->left);
            if (n->left && n->left->type == NODE_IDENT) {
                Symbol *sym = get_symbol(n->left->name);
                if (sym && strcmp(sym->type, "float") == 0)
                    printf("%.2f\n", val);
                else
                    printf("%d\n", (int)val);
            } else {
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

        case NODE_SWITCH: {
            float switch_val = eval(n->left);
            Node *c = n->middle;
            int matched = 0;
            while (c != NULL) {
                /* middle == NULL means this is the nop/default case */
                if (c->middle == NULL || eval(c->middle) == switch_val) {
                    execute(c->left);
                    matched = 1;
                    break;
                }
                c = c->right;
            }
            if (!matched)
                printf("incident: no match for %.2f\n", switch_val);
            break;
        }

        case NODE_ARRAY_DECL: {
            int size = (int)n->value;
            insert(n->name, "array", 0);
            Symbol *sym = get_symbol(n->name);
            if (sym) {
                sym->arr_data = calloc(size, sizeof(int));
                sym->arr_size = size;
            }
            printf("Array '%s' declared. (size=%d)\n", n->name, size);
            break;
        }

        case NODE_ARRAY_ASSIGN: {
            Symbol *sym = get_symbol(n->name);
            if (sym == NULL || strcmp(sym->type, "array") != 0) {
                printf("Error: '%s' is not an array.\n", n->name);
                break;
            }
            int idx = (int)eval(n->left);
            int val = (int)eval(n->right);
            if (idx < 0 || idx >= sym->arr_size) {
                printf("Error: Array '%s' index %d out of bounds.\n", n->name, idx);
                break;
            }
            sym->arr_data[idx] = val;
            printf("Array '%s'[%d] = %d\n", n->name, idx, val);
            break;
        }

        case NODE_CASE:
            break; /* never executed directly — always walked via NODE_SWITCH */

        default: break;
    }
}

/* TAC HELPERS */

int temp_count  = 0;   /* counts t1, t2, t3 ... */
int label_count = 0;   /* counts L1, L2, L3 ... */

char *new_temp() {
    char *buf = malloc(16);
    sprintf(buf, "t%d", ++temp_count);
    return buf;
}


char *new_label() {
    char *buf = malloc(16);
    sprintf(buf, "L%d", ++label_count);
    return buf;
}


void gen_stmt(Node *n);

char *gen(Node *n) {
    if (n == NULL) return strdup("0");

    switch (n->type) {

        case NODE_NUM: {

            char *buf = malloc(32);
            if (n->value == (int)n->value)
                sprintf(buf, "%d", (int)n->value);
            else
                sprintf(buf, "%.2f", n->value);
            return buf;
        }

        case NODE_IDENT:
            return strdup(n->name);


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

        case NODE_FUNC_CALL: {

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

        case NODE_ARRAY_ACCESS: {
            char *idx = gen(n->left);
            char *t   = new_temp();
            printf("%s = %s[%s]\n", t, n->name, idx);
            return t;
        }

        case NODE_STR_JOIN: {
            char *a = gen(n->left);
            char *b = gen(n->right);
            char *t = new_temp();
            printf("%s = strjoin %s %s\n", t, a, b);
            return t;
        }
        case NODE_STR_LEN: {
            char *a = gen(n->left);
            char *t = new_temp();
            printf("%s = strlen %s\n", t, a);
            return t;
        }
        case NODE_STR_PART: {
            char *a   = gen(n->left);
            char *s   = gen(n->middle);
            char *l   = gen(n->right);
            char *t   = new_temp();
            printf("%s = takepart %s %s %s\n", t, a, s, l);
            return t;
        }

        default:
            return strdup("?");
    }
}


void gen_stmt(Node *n) {
    if (n == NULL) return;

    switch (n->type) {


        case NODE_SEQ:
            gen_stmt(n->left);
            gen_stmt(n->right);
            break;


        case NODE_DECL: {
            if (n->value == 3) {
                /* string declaration — emit a string assignment */
                char *strval = (n->left && n->left->name) ? n->left->name : "";
                printf("%s = \"%s\"\n", n->name, strval);
                break;
            }
            char *t = gen(n->left);
            printf("%s = %s\n", n->name, t);
            break;
        }

        case NODE_COMPOUND_ADD: {
            char *t_val = gen(n->left);     /* generate the right-hand expr */
            char *t_res = new_temp();
            printf("%s = %s + %s\n", t_res, n->name, t_val);
            printf("%s = %s\n", n->name, t_res);
            break;
        }


        case NODE_RETURN: {
            char *t = gen(n->left);
            printf("return %s\n", t);
            break;
        }


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
        case NODE_SHOW: {
            if (n->left && n->left->type == NODE_STRING_LIT) {
                printf("print_str \"%s\"\n", n->left->name);
                break;
            }
            if (n->left && (n->left->type == NODE_STR_JOIN ||
                            n->left->type == NODE_STR_PART)) {
                char *t = gen(n->left);
                printf("print_str %s\n", t);
                break;
            }
            if (n->left && n->left->type == NODE_IDENT) {
                Symbol *sym = get_symbol(n->left->name);
                if (sym && strcmp(sym->type, "string") == 0) {
                    printf("print_str %s\n", n->left->name);
                    break;
                }
            }
            char *t = gen(n->left);
            printf("print %s\n", t);
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
        case NODE_SWITCH: {
            char *sv    = gen(n->left);   /* evaluate switch expr once */
            char *l_end = new_label();


            char *case_labels[50];
            int   case_count = 0;
            char *l_default  = NULL;

            /* Pass 1 */
            Node *c = n->middle;
            while (c != NULL) {
                if (c->middle == NULL) {
                    /* nop/default — no condition check, just a label */
                    l_default = new_label();
                    case_labels[case_count++] = l_default;
                } else {
                    char *cv = gen(c->middle);
                    char *t  = new_temp();
                    printf("%s = %s == %s\n", t, sv, cv);
                    char *lbl = new_label();
                    printf("if %s goto %s\n", t, lbl);
                    case_labels[case_count++] = lbl;
                }
                c = c->right;
            }
            /* Fall-through if nothing matched */
            if (l_default) printf("goto %s\n", l_default);
            else           printf("goto %s\n", l_end);

            /* Pass 2 */
            c = n->middle;
            for (int i = 0; i < case_count; i++) {
                printf("%s:\n", case_labels[i]);
                gen_stmt(c->left);
                printf("goto %s\n", l_end);
                c = c->right;
            }
            printf("%s:\n", l_end);
            break;
        }

        case NODE_CASE:
            break;

        case NODE_ARRAY_DECL: {
            printf("array_decl %s size %d\n", n->name, (int)n->value);
            break;
        }

        case NODE_ARRAY_ASSIGN: {
            char *idx = gen(n->left);
            char *val = gen(n->right);
            printf("%s[%s] = %s\n", n->name, idx, val);
            break;
        }

        default:
            break;
    }
}



/* Line 189 of yacc.c  */
#line 1168 "confused.tab.c"

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
#line 1097 "confused.y"

    char        *sval;
    int          ival;
    float        fval;
    char         cval;
    struct Node *nval;



/* Line 214 of yacc.c  */
#line 1270 "confused.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 1282 "confused.tab.c"

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
#define YYFINAL  74
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1076

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  63
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  28
/* YYNRULES -- Number of rules.  */
#define YYNRULES  86
/* YYNRULES -- Number of states.  */
#define YYNSTATES  244

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
       2,    61,     2,    62,     2,     2,     2,     2,     2,     2,
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
      43,    46,    48,    50,    56,    62,    66,    68,    71,    74,
      77,    78,    80,    84,    85,    87,    91,   100,   109,   114,
     124,   132,   137,   143,   147,   151,   159,   165,   173,   174,
     182,   188,   196,   205,   213,   221,   226,   234,   243,   249,
     255,   261,   267,   269,   271,   273,   275,   277,   281,   285,
     289,   293,   297,   301,   305,   309,   313,   317,   321,   328,
     333,   335,   342,   347,   356,   361,   363
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      64,     0,    -1,    65,    -1,    64,    65,    -1,    10,    57,
      69,    58,    -1,    89,    -1,    78,    -1,    67,    -1,    85,
      -1,    76,    -1,    75,    -1,    77,    -1,    73,    -1,    68,
      -1,    79,    -1,    80,    -1,    81,    -1,    82,    -1,    66,
      -1,    83,    -1,    74,    54,    -1,    86,    -1,    87,    -1,
      31,    59,     3,    60,    54,    -1,    17,    59,     3,    60,
      54,    -1,    15,    90,    54,    -1,    65,    -1,    69,    65,
      -1,    11,     3,    -1,    12,     3,    -1,    -1,    70,    -1,
      71,    55,    70,    -1,    -1,    90,    -1,    72,    55,    90,
      -1,    11,     3,    59,    71,    60,    57,    69,    58,    -1,
      12,     3,    59,    71,    60,    57,    69,    58,    -1,     3,
      59,    72,    60,    -1,    21,    59,    90,    56,    90,    60,
      57,    69,    58,    -1,    22,    59,    90,    60,    57,    69,
      58,    -1,     3,    53,    90,    54,    -1,    16,    59,    90,
      60,    54,    -1,    26,     3,    54,    -1,    27,     3,    54,
      -1,    33,    59,     3,    55,    90,    60,    54,    -1,    34,
      59,     3,    60,    54,    -1,    23,    59,    90,    60,    57,
      84,    58,    -1,    -1,    84,    24,    90,    56,    57,    69,
      58,    -1,    84,    25,    57,    69,    58,    -1,    18,    59,
      90,    60,    57,    69,    58,    -1,    18,    59,    90,    60,
      57,    69,    58,    88,    -1,    29,    11,     3,    59,     5,
      60,    54,    -1,     3,    61,    90,    62,    52,    90,    54,
      -1,    20,    57,    69,    58,    -1,    19,    59,    90,    60,
      57,    69,    58,    -1,    19,    59,    90,    60,    57,    69,
      58,    88,    -1,    11,     3,    52,    90,    54,    -1,    12,
       3,    52,    90,    54,    -1,    14,     3,    52,    90,    54,
      -1,    13,     3,    52,     4,    54,    -1,     5,    -1,     8,
      -1,     6,    -1,     7,    -1,     3,    -1,    90,    37,    90,
      -1,    90,    38,    90,    -1,    90,    39,    90,    -1,    90,
      40,    90,    -1,    90,    41,    90,    -1,    90,    46,    90,
      -1,    90,    47,    90,    -1,    90,    48,    90,    -1,    90,
      51,    90,    -1,    90,    49,    90,    -1,    90,    50,    90,
      -1,    30,    59,    90,    55,    90,    60,    -1,    32,    59,
       3,    60,    -1,     4,    -1,    35,    59,    90,    55,    90,
      60,    -1,    36,    59,    90,    60,    -1,    28,    59,    90,
      55,    90,    55,    90,    60,    -1,     3,    61,    90,    62,
      -1,    74,    -1,    59,    90,    60,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,  1142,  1142,  1143,  1144,  1148,  1149,  1150,  1151,  1152,
    1153,  1154,  1155,  1156,  1157,  1158,  1159,  1160,  1161,  1162,
    1163,  1164,  1165,  1170,  1178,  1186,  1191,  1192,  1195,  1201,
    1210,  1211,  1212,  1222,  1223,  1224,  1235,  1242,  1254,  1262,
    1269,  1275,  1283,  1290,  1298,  1306,  1314,  1322,  1328,  1329,
    1341,  1356,  1360,  1367,  1376,  1385,  1389,  1393,  1400,  1406,
    1412,  1418,  1431,  1432,  1433,  1434,  1435,  1436,  1437,  1438,
    1439,  1440,  1441,  1442,  1443,  1444,  1445,  1446,  1447,  1448,
    1453,  1456,  1459,  1462,  1465,  1470,  1471
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
  "'['", "']'", "$accept", "program", "statement", "inc_stmt", "ask_stmt",
  "return_stmt", "block", "param", "param_list", "arg_list", "func_def",
  "func_call", "flp_stmt", "wlp_stmt", "compound_stmt", "show_stmt",
  "stk_decl", "queue_decl", "push_stmt", "pop_stmt", "incident_stmt",
  "case_list", "iff_stmt", "array_decl", "array_assign", "or_chain",
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
      41,    91,    93
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    63,    64,    64,    64,    65,    65,    65,    65,    65,
      65,    65,    65,    65,    65,    65,    65,    65,    65,    65,
      65,    65,    65,    66,    67,    68,    69,    69,    70,    70,
      71,    71,    71,    72,    72,    72,    73,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    84,
      84,    85,    85,    86,    87,    88,    88,    88,    89,    89,
      89,    89,    90,    90,    90,    90,    90,    90,    90,    90,
      90,    90,    90,    90,    90,    90,    90,    90,    90,    90,
      90,    90,    90,    90,    90,    90,    90
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     4,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     1,     1,     5,     5,     3,     1,     2,     2,     2,
       0,     1,     3,     0,     1,     3,     8,     8,     4,     9,
       7,     4,     5,     3,     3,     7,     5,     7,     0,     7,
       5,     7,     8,     7,     7,     4,     7,     8,     5,     5,
       5,     5,     1,     1,     1,     1,     1,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     6,     4,
       1,     6,     4,     8,     4,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     2,    18,     7,    13,    12,     0,    10,     9,    11,
       6,    14,    15,    16,    17,    19,     8,    21,    22,     5,
       0,    33,     0,     0,     0,     0,     0,     0,    66,    80,
      62,    64,    65,    63,     0,     0,     0,     0,     0,     0,
      85,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     1,     3,    20,     0,     0,    34,
       0,    26,     0,     0,    30,     0,    30,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    25,     0,     0,
       0,     0,     0,     0,    43,    44,     0,     0,     0,     0,
      41,     0,    38,     0,     4,    27,     0,     0,     0,    31,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    86,    67,    68,    69,    70,    71,    72,    73,    74,
      76,    77,    75,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    35,     0,    58,    28,    29,     0,     0,
      59,     0,    61,    60,    84,     0,     0,    79,     0,    82,
      42,    24,     0,     0,     0,    48,     0,    23,     0,    46,
       0,    32,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    54,     0,     0,     0,    78,    81,    51,
       0,    40,     0,     0,    47,    53,    45,    36,    37,     0,
       0,     0,    52,     0,     0,     0,    83,     0,     0,    39,
       0,     0,     0,     0,     0,    50,     0,    55,     0,     0,
      49,     0,    56,    57
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    20,    81,    22,    23,    24,    82,   129,   130,    78,
      25,    60,    27,    28,    29,    30,    31,    32,    33,    34,
      35,   200,    36,    37,    38,   222,    39,    61
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -102
static const yytype_int16 yypact[] =
{
     564,   -48,   -51,    25,    27,    44,    45,    71,   -10,    -8,
      -6,    -5,    21,    26,    56,    59,    73,    28,    30,    31,
     539,  -102,  -102,  -102,  -102,  -102,    39,  -102,  -102,  -102,
    -102,  -102,  -102,  -102,  -102,  -102,  -102,  -102,  -102,  -102,
      71,    71,    71,   589,   -49,   -40,    42,    43,   -34,  -102,
    -102,  -102,  -102,  -102,    37,    38,    41,    46,    49,    71,
    -102,   917,    71,    95,    71,    71,    71,    71,    48,    50,
     107,   108,   109,   110,  -102,  -102,  -102,   935,   -39,  1025,
     587,  -102,   137,    71,     3,    71,     3,   111,    71,    71,
      71,    71,   113,    71,    71,   621,    71,    71,    71,    71,
      71,    71,    71,    71,    71,    71,    71,  -102,   636,    54,
     651,   801,   666,   681,  -102,  -102,    58,    60,    63,    61,
    -102,    71,  -102,    67,  -102,  -102,   953,   119,   122,  -102,
     -38,   971,   -37,    72,   989,   604,   841,   860,    69,   879,
     696,  -102,     5,     5,  -102,  -102,  -102,    -1,    -1,    -1,
      -1,    -1,    -1,    78,    80,    79,    71,    82,    84,   130,
      83,    71,    90,  1025,    71,  -102,  -102,  -102,     3,    89,
    -102,    99,  -102,  -102,  -102,    71,    71,  -102,    71,  -102,
    -102,  -102,   589,   711,   589,  -102,    87,  -102,   726,  -102,
    1007,  -102,   589,   589,   898,   741,   756,   237,   100,   270,
     -23,   115,   118,  -102,   303,   336,    71,  -102,  -102,    22,
     589,  -102,    71,   104,  -102,  -102,  -102,  -102,  -102,   771,
     103,   117,  -102,   369,   821,   589,  -102,    71,   589,  -102,
     120,   402,   786,   435,   589,  -102,   121,  -102,   468,   589,
    -102,   501,    22,  -102
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
    -102,  -102,     4,  -102,  -102,  -102,  -101,    -3,    81,  -102,
    -102,     0,  -102,  -102,  -102,  -102,  -102,  -102,  -102,  -102,
    -102,  -102,  -102,  -102,  -102,   -67,  -102,   -33
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      26,   212,   213,    83,    21,    40,    43,    77,    79,    80,
      84,    41,    85,    42,   127,   128,   121,   168,   168,    86,
      26,   122,   169,   171,    75,    41,    95,    89,    44,   108,
      45,   110,   111,   112,   113,   214,    96,    97,    98,    99,
     100,   220,   221,    26,    98,    99,   100,    46,    47,    62,
     126,    63,   131,    64,    65,   134,   135,   136,   137,    68,
     139,   140,    69,   142,   143,   144,   145,   146,   147,   148,
     149,   150,   151,   152,    48,    49,    50,    51,    52,    53,
      66,   197,    26,   199,    70,    67,   125,    71,   163,    72,
      73,   204,   205,    76,    87,    88,    90,    91,   109,    54,
      92,    55,   114,    56,   115,    93,    57,    58,    94,   223,
     116,   117,   118,   119,   154,   133,   138,   159,   161,   164,
     160,   162,   166,   183,   231,   167,   172,   233,   188,   177,
      59,   190,   180,   238,   181,   186,   182,   187,   241,   184,
       1,   185,   194,   195,   189,   196,   192,   201,     3,     4,
       5,     6,     7,     8,     9,    10,   193,   210,    11,    12,
      13,   225,   227,    14,    15,   191,    16,   132,    17,   215,
      18,    19,   216,   219,   228,   243,     0,   234,   239,   224,
       0,     0,    26,     0,    26,     0,     0,     0,     0,     0,
       0,     0,    26,    26,   232,   124,     0,    26,     0,    26,
       0,   125,     0,   125,    26,    26,     0,     0,   125,   125,
      26,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    26,     0,    26,     0,   125,    26,     0,
       0,    26,     0,    26,    26,   125,     0,   125,    26,    26,
       1,    26,   125,     0,     0,   125,     0,     0,     3,     4,
       5,     6,     7,     8,     9,    10,     0,     0,    11,    12,
      13,     0,     0,    14,    15,     0,    16,     0,    17,     0,
      18,    19,     0,     1,     0,     0,     0,     0,     0,     0,
       0,     3,     4,     5,     6,     7,     8,     9,    10,     0,
       0,    11,    12,    13,     0,   209,    14,    15,     0,    16,
       0,    17,     0,    18,    19,     0,     1,     0,     0,     0,
       0,     0,     0,     0,     3,     4,     5,     6,     7,     8,
       9,    10,     0,     0,    11,    12,    13,     0,   211,    14,
      15,     0,    16,     0,    17,     0,    18,    19,     0,     1,
       0,     0,     0,     0,     0,     0,     0,     3,     4,     5,
       6,     7,     8,     9,    10,     0,     0,    11,    12,    13,
       0,   217,    14,    15,     0,    16,     0,    17,     0,    18,
      19,     0,     1,     0,     0,     0,     0,     0,     0,     0,
       3,     4,     5,     6,     7,     8,     9,    10,     0,     0,
      11,    12,    13,     0,   218,    14,    15,     0,    16,     0,
      17,     0,    18,    19,     0,     1,     0,     0,     0,     0,
       0,     0,     0,     3,     4,     5,     6,     7,     8,     9,
      10,     0,     0,    11,    12,    13,     0,   229,    14,    15,
       0,    16,     0,    17,     0,    18,    19,     0,     1,     0,
       0,     0,     0,     0,     0,     0,     3,     4,     5,     6,
       7,     8,     9,    10,     0,     0,    11,    12,    13,     0,
     235,    14,    15,     0,    16,     0,    17,     0,    18,    19,
       0,     1,     0,     0,     0,     0,     0,     0,     0,     3,
       4,     5,     6,     7,     8,     9,    10,     0,     0,    11,
      12,    13,     0,   237,    14,    15,     0,    16,     0,    17,
       0,    18,    19,     0,     1,     0,     0,     0,     0,     0,
       0,     0,     3,     4,     5,     6,     7,     8,     9,    10,
       0,     0,    11,    12,    13,     0,   240,    14,    15,     0,
      16,     0,    17,     0,    18,    19,     0,     0,     0,    74,
       0,     0,     1,     0,     0,     0,     0,     0,     0,     0,
       3,     4,     5,     6,     7,     8,     9,    10,     0,   242,
      11,    12,    13,     0,     0,    14,    15,     1,    16,     0,
      17,     0,    18,    19,     2,     3,     4,     5,     6,     7,
       8,     9,    10,     0,     0,    11,    12,    13,     0,     0,
      14,    15,     1,    16,     0,    17,     0,    18,    19,     0,
       3,     4,     5,     6,     7,     8,     9,    10,     0,     0,
      11,    12,    13,     0,     0,    14,    15,     0,    16,     0,
      17,     0,    18,    19,    96,    97,    98,    99,   100,     0,
       0,     0,     0,   101,   102,   103,   104,   105,   106,     0,
       0,    96,    97,    98,    99,   100,     0,     0,     0,   123,
     101,   102,   103,   104,   105,   106,     0,     0,    96,    97,
      98,    99,   100,     0,     0,     0,   174,   101,   102,   103,
     104,   105,   106,    96,    97,    98,    99,   100,     0,     0,
       0,   141,   101,   102,   103,   104,   105,   106,    96,    97,
      98,    99,   100,     0,     0,     0,   153,   101,   102,   103,
     104,   105,   106,    96,    97,    98,    99,   100,     0,     0,
       0,   155,   101,   102,   103,   104,   105,   106,    96,    97,
      98,    99,   100,     0,     0,     0,   157,   101,   102,   103,
     104,   105,   106,    96,    97,    98,    99,   100,     0,     0,
       0,   158,   101,   102,   103,   104,   105,   106,    96,    97,
      98,    99,   100,     0,     0,     0,   179,   101,   102,   103,
     104,   105,   106,    96,    97,    98,    99,   100,     0,     0,
       0,   198,   101,   102,   103,   104,   105,   106,    96,    97,
      98,    99,   100,     0,     0,     0,   202,   101,   102,   103,
     104,   105,   106,    96,    97,    98,    99,   100,     0,     0,
       0,   207,   101,   102,   103,   104,   105,   106,    96,    97,
      98,    99,   100,     0,     0,     0,   208,   101,   102,   103,
     104,   105,   106,    96,    97,    98,    99,   100,     0,     0,
       0,   226,   101,   102,   103,   104,   105,   106,    96,    97,
      98,    99,   100,     0,     0,     0,   236,   101,   102,   103,
     104,   105,   106,     0,     0,     0,     0,   156,    96,    97,
      98,    99,   100,     0,     0,     0,     0,   101,   102,   103,
     104,   105,   106,     0,     0,     0,     0,   230,    96,    97,
      98,    99,   100,     0,     0,     0,     0,   101,   102,   103,
     104,   105,   106,     0,     0,     0,   175,    96,    97,    98,
      99,   100,     0,     0,     0,     0,   101,   102,   103,   104,
     105,   106,     0,     0,     0,   176,    96,    97,    98,    99,
     100,     0,     0,     0,     0,   101,   102,   103,   104,   105,
     106,     0,     0,     0,   178,    96,    97,    98,    99,   100,
       0,     0,     0,     0,   101,   102,   103,   104,   105,   106,
       0,     0,     0,   206,    96,    97,    98,    99,   100,     0,
       0,     0,     0,   101,   102,   103,   104,   105,   106,     0,
       0,   107,    96,    97,    98,    99,   100,     0,     0,     0,
       0,   101,   102,   103,   104,   105,   106,     0,     0,   120,
      96,    97,    98,    99,   100,     0,     0,     0,     0,   101,
     102,   103,   104,   105,   106,     0,     0,   165,    96,    97,
      98,    99,   100,     0,     0,     0,     0,   101,   102,   103,
     104,   105,   106,     0,     0,   170,    96,    97,    98,    99,
     100,     0,     0,     0,     0,   101,   102,   103,   104,   105,
     106,     0,     0,   173,    96,    97,    98,    99,   100,     0,
       0,     0,     0,   101,   102,   103,   104,   105,   106,     0,
       0,   203,    96,    97,    98,    99,   100,     0,     0,     0,
       0,   101,   102,   103,   104,   105,   106
};

static const yytype_int16 yycheck[] =
{
       0,    24,    25,    52,     0,    53,    57,    40,    41,    42,
      59,    59,    52,    61,    11,    12,    55,    55,    55,    59,
      20,    60,    60,    60,    20,    59,    59,    61,     3,    62,
       3,    64,    65,    66,    67,    58,    37,    38,    39,    40,
      41,    19,    20,    43,    39,    40,    41,     3,     3,    59,
      83,    59,    85,    59,    59,    88,    89,    90,    91,     3,
      93,    94,     3,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,     3,     4,     5,     6,     7,     8,
      59,   182,    82,   184,    11,    59,    82,    59,   121,    59,
      59,   192,   193,    54,    52,    52,    59,    59,     3,    28,
      59,    30,    54,    32,    54,    59,    35,    36,    59,   210,
       3,     3,     3,     3,    60,     4,     3,    59,    55,    52,
      60,    60,     3,   156,   225,     3,    54,   228,   161,    60,
      59,   164,    54,   234,    54,     5,    57,    54,   239,    57,
       3,    57,   175,   176,    54,   178,    57,    60,    11,    12,
      13,    14,    15,    16,    17,    18,    57,    57,    21,    22,
      23,    57,    59,    26,    27,   168,    29,    86,    31,    54,
      33,    34,    54,   206,    57,   242,    -1,    57,    57,   212,
      -1,    -1,   182,    -1,   184,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   192,   193,   227,    58,    -1,   197,    -1,   199,
      -1,   197,    -1,   199,   204,   205,    -1,    -1,   204,   205,
     210,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   223,    -1,   225,    -1,   223,   228,    -1,
      -1,   231,    -1,   233,   234,   231,    -1,   233,   238,   239,
       3,   241,   238,    -1,    -1,   241,    -1,    -1,    11,    12,
      13,    14,    15,    16,    17,    18,    -1,    -1,    21,    22,
      23,    -1,    -1,    26,    27,    -1,    29,    -1,    31,    -1,
      33,    34,    -1,     3,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    11,    12,    13,    14,    15,    16,    17,    18,    -1,
      -1,    21,    22,    23,    -1,    58,    26,    27,    -1,    29,
      -1,    31,    -1,    33,    34,    -1,     3,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    11,    12,    13,    14,    15,    16,
      17,    18,    -1,    -1,    21,    22,    23,    -1,    58,    26,
      27,    -1,    29,    -1,    31,    -1,    33,    34,    -1,     3,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    11,    12,    13,
      14,    15,    16,    17,    18,    -1,    -1,    21,    22,    23,
      -1,    58,    26,    27,    -1,    29,    -1,    31,    -1,    33,
      34,    -1,     3,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      11,    12,    13,    14,    15,    16,    17,    18,    -1,    -1,
      21,    22,    23,    -1,    58,    26,    27,    -1,    29,    -1,
      31,    -1,    33,    34,    -1,     3,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    11,    12,    13,    14,    15,    16,    17,
      18,    -1,    -1,    21,    22,    23,    -1,    58,    26,    27,
      -1,    29,    -1,    31,    -1,    33,    34,    -1,     3,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    11,    12,    13,    14,
      15,    16,    17,    18,    -1,    -1,    21,    22,    23,    -1,
      58,    26,    27,    -1,    29,    -1,    31,    -1,    33,    34,
      -1,     3,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    11,
      12,    13,    14,    15,    16,    17,    18,    -1,    -1,    21,
      22,    23,    -1,    58,    26,    27,    -1,    29,    -1,    31,
      -1,    33,    34,    -1,     3,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    11,    12,    13,    14,    15,    16,    17,    18,
      -1,    -1,    21,    22,    23,    -1,    58,    26,    27,    -1,
      29,    -1,    31,    -1,    33,    34,    -1,    -1,    -1,     0,
      -1,    -1,     3,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      11,    12,    13,    14,    15,    16,    17,    18,    -1,    58,
      21,    22,    23,    -1,    -1,    26,    27,     3,    29,    -1,
      31,    -1,    33,    34,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    -1,    -1,    21,    22,    23,    -1,    -1,
      26,    27,     3,    29,    -1,    31,    -1,    33,    34,    -1,
      11,    12,    13,    14,    15,    16,    17,    18,    -1,    -1,
      21,    22,    23,    -1,    -1,    26,    27,    -1,    29,    -1,
      31,    -1,    33,    34,    37,    38,    39,    40,    41,    -1,
      -1,    -1,    -1,    46,    47,    48,    49,    50,    51,    -1,
      -1,    37,    38,    39,    40,    41,    -1,    -1,    -1,    62,
      46,    47,    48,    49,    50,    51,    -1,    -1,    37,    38,
      39,    40,    41,    -1,    -1,    -1,    62,    46,    47,    48,
      49,    50,    51,    37,    38,    39,    40,    41,    -1,    -1,
      -1,    60,    46,    47,    48,    49,    50,    51,    37,    38,
      39,    40,    41,    -1,    -1,    -1,    60,    46,    47,    48,
      49,    50,    51,    37,    38,    39,    40,    41,    -1,    -1,
      -1,    60,    46,    47,    48,    49,    50,    51,    37,    38,
      39,    40,    41,    -1,    -1,    -1,    60,    46,    47,    48,
      49,    50,    51,    37,    38,    39,    40,    41,    -1,    -1,
      -1,    60,    46,    47,    48,    49,    50,    51,    37,    38,
      39,    40,    41,    -1,    -1,    -1,    60,    46,    47,    48,
      49,    50,    51,    37,    38,    39,    40,    41,    -1,    -1,
      -1,    60,    46,    47,    48,    49,    50,    51,    37,    38,
      39,    40,    41,    -1,    -1,    -1,    60,    46,    47,    48,
      49,    50,    51,    37,    38,    39,    40,    41,    -1,    -1,
      -1,    60,    46,    47,    48,    49,    50,    51,    37,    38,
      39,    40,    41,    -1,    -1,    -1,    60,    46,    47,    48,
      49,    50,    51,    37,    38,    39,    40,    41,    -1,    -1,
      -1,    60,    46,    47,    48,    49,    50,    51,    37,    38,
      39,    40,    41,    -1,    -1,    -1,    60,    46,    47,    48,
      49,    50,    51,    -1,    -1,    -1,    -1,    56,    37,    38,
      39,    40,    41,    -1,    -1,    -1,    -1,    46,    47,    48,
      49,    50,    51,    -1,    -1,    -1,    -1,    56,    37,    38,
      39,    40,    41,    -1,    -1,    -1,    -1,    46,    47,    48,
      49,    50,    51,    -1,    -1,    -1,    55,    37,    38,    39,
      40,    41,    -1,    -1,    -1,    -1,    46,    47,    48,    49,
      50,    51,    -1,    -1,    -1,    55,    37,    38,    39,    40,
      41,    -1,    -1,    -1,    -1,    46,    47,    48,    49,    50,
      51,    -1,    -1,    -1,    55,    37,    38,    39,    40,    41,
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
      -1,    -1,    -1,    46,    47,    48,    49,    50,    51,    -1,
      -1,    54,    37,    38,    39,    40,    41,    -1,    -1,    -1,
      -1,    46,    47,    48,    49,    50,    51
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    21,    22,    23,    26,    27,    29,    31,    33,    34,
      64,    65,    66,    67,    68,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    85,    86,    87,    89,
      53,    59,    61,    57,     3,     3,     3,     3,     3,     4,
       5,     6,     7,     8,    28,    30,    32,    35,    36,    59,
      74,    90,    59,    59,    59,    59,    59,    59,     3,     3,
      11,    59,    59,    59,     0,    65,    54,    90,    72,    90,
      90,    65,    69,    52,    59,    52,    59,    52,    52,    61,
      59,    59,    59,    59,    59,    90,    37,    38,    39,    40,
      41,    46,    47,    48,    49,    50,    51,    54,    90,     3,
      90,    90,    90,    90,    54,    54,     3,     3,     3,     3,
      54,    55,    60,    62,    58,    65,    90,    11,    12,    70,
      71,    90,    71,     4,    90,    90,    90,    90,     3,    90,
      90,    60,    90,    90,    90,    90,    90,    90,    90,    90,
      90,    90,    90,    60,    60,    60,    56,    60,    60,    59,
      60,    55,    60,    90,    52,    54,     3,     3,    55,    60,
      54,    60,    54,    54,    62,    55,    55,    60,    55,    60,
      54,    54,    57,    90,    57,    57,     5,    54,    90,    54,
      90,    70,    57,    57,    90,    90,    90,    69,    60,    69,
      84,    60,    60,    54,    69,    69,    55,    60,    60,    58,
      57,    58,    24,    25,    58,    54,    54,    58,    58,    90,
      19,    20,    88,    69,    90,    57,    60,    59,    57,    58,
      56,    69,    90,    69,    57,    58,    60,    58,    69,    57,
      58,    69,    58,    88
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
#line 1142 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); root = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 1143 "confused.y"
    { (yyval.nval) = makeNode(NODE_SEQ, (yyvsp[(1) - (2)].nval), NULL, (yyvsp[(2) - (2)].nval)); root = (yyval.nval); ;}
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 1144 "confused.y"
    { (yyval.nval) = (yyvsp[(3) - (4)].nval); root = (yyvsp[(3) - (4)].nval); ;}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 1148 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 1149 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 1150 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 1151 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 1152 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 1153 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 1154 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 1155 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 1156 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 1157 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 1158 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 1159 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 1160 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 1161 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 1162 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 1163 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (2)].nval); ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 1164 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 1165 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 1170 "confused.y"
    {
        Node *n = makeNode(NODE_INC, NULL, NULL, NULL);
        n->name = strdup((yyvsp[(3) - (5)].sval));
        (yyval.nval) = n;
    ;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 1178 "confused.y"
    {
        Node *n = makeNode(NODE_ASK, NULL, NULL, NULL);
        n->name = strdup((yyvsp[(3) - (5)].sval));
        (yyval.nval) = n;
    ;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 1186 "confused.y"
    {
        (yyval.nval) = makeNode(NODE_RETURN, (yyvsp[(2) - (3)].nval), NULL, NULL);
    ;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 1191 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 1192 "confused.y"
    { (yyval.nval) = makeNode(NODE_SEQ, (yyvsp[(1) - (2)].nval), NULL, (yyvsp[(2) - (2)].nval)); ;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 1195 "confused.y"
    {
        Node *n = makeNode(NODE_PARAM, NULL, NULL, NULL);
        n->name  = strdup((yyvsp[(2) - (2)].sval));
        n->value = 0;   /* 0 = int parameter */
        (yyval.nval) = n;
    ;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 1201 "confused.y"
    {
        Node *n = makeNode(NODE_PARAM, NULL, NULL, NULL);
        n->name  = strdup((yyvsp[(2) - (2)].sval));
        n->value = 1;   /* 1 = float parameter */
        (yyval.nval) = n;
    ;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 1210 "confused.y"
    { (yyval.nval) = NULL; ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 1211 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 1212 "confused.y"
    {
        /* chain params using the right pointer */
        Node *p = (yyvsp[(1) - (3)].nval);
        while (p->right != NULL) p = p->right;
        p->right = (yyvsp[(3) - (3)].nval);
        (yyval.nval) = (yyvsp[(1) - (3)].nval);
    ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 1222 "confused.y"
    { (yyval.nval) = NULL; ;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 1223 "confused.y"
    { (yyval.nval) = makeNode(NODE_ARG, (yyvsp[(1) - (1)].nval), NULL, NULL); ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 1224 "confused.y"
    {
        /* chain args using the right pointer */
        Node *a = makeNode(NODE_ARG, (yyvsp[(3) - (3)].nval), NULL, NULL);
        Node *p = (yyvsp[(1) - (3)].nval);
        while (p->right != NULL) p = p->right;
        p->right = a;
        (yyval.nval) = (yyvsp[(1) - (3)].nval);
    ;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 1235 "confused.y"
    {
        Node *n = makeNode(NODE_FUNC_DEF, (yyvsp[(4) - (8)].nval), (yyvsp[(7) - (8)].nval), NULL);
        n->name  = strdup((yyvsp[(2) - (8)].sval));
        n->value = 0;
        insert_func((yyvsp[(2) - (8)].sval), "int", (yyvsp[(4) - (8)].nval), (yyvsp[(7) - (8)].nval));
        (yyval.nval) = n;
    ;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 1242 "confused.y"
    {
        Node *n = makeNode(NODE_FUNC_DEF, (yyvsp[(4) - (8)].nval), (yyvsp[(7) - (8)].nval), NULL);
        n->name  = strdup((yyvsp[(2) - (8)].sval));
        n->value = 1;
        insert_func((yyvsp[(2) - (8)].sval), "float", (yyvsp[(4) - (8)].nval), (yyvsp[(7) - (8)].nval));
        (yyval.nval) = n;
    ;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 1254 "confused.y"
    {
        Node *n = makeNode(NODE_FUNC_CALL, (yyvsp[(3) - (4)].nval), NULL, NULL);
        n->name = strdup((yyvsp[(1) - (4)].sval));
        (yyval.nval) = n;
    ;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 1262 "confused.y"
    {
        /* left = start expr, middle = end expr, right = loop body */
        Node *n = makeNode(NODE_FOR, (yyvsp[(3) - (9)].nval), (yyvsp[(5) - (9)].nval), (yyvsp[(8) - (9)].nval));
        (yyval.nval) = n;
    ;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 1269 "confused.y"
    {
        /* condition in left, loop body in middle, no right needed */
        (yyval.nval) = makeNode(NODE_WHILE, (yyvsp[(3) - (7)].nval), (yyvsp[(6) - (7)].nval), NULL);
    ;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 1275 "confused.y"
    {
        Node *n = makeNode(NODE_COMPOUND_ADD, (yyvsp[(3) - (4)].nval), NULL, NULL);
        n->name = strdup((yyvsp[(1) - (4)].sval));
        (yyval.nval) = n;
    ;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 1283 "confused.y"
    {
        Node *n = makeNode(NODE_SHOW, (yyvsp[(3) - (5)].nval), NULL, NULL);
        (yyval.nval) = n;
    ;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 1290 "confused.y"
    {
        Node *n = makeNode(NODE_STK_DECL, NULL, NULL, NULL);
        n->name = strdup((yyvsp[(2) - (3)].sval));
        (yyval.nval) = n;
    ;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 1298 "confused.y"
    {
        Node *n = makeNode(NODE_QUEUE_DECL, NULL, NULL, NULL);
        n->name = strdup((yyvsp[(2) - (3)].sval));
        (yyval.nval) = n;
    ;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 1306 "confused.y"
    {
        Node *n = makeNode(NODE_PUSH, (yyvsp[(5) - (7)].nval), NULL, NULL);
        n->name = strdup((yyvsp[(3) - (7)].sval));
        (yyval.nval) = n;
    ;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 1314 "confused.y"
    {
        Node *n = makeNode(NODE_POP, NULL, NULL, NULL);
        n->name = strdup((yyvsp[(3) - (5)].sval));
        (yyval.nval) = n;
    ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 1322 "confused.y"
    {
        (yyval.nval) = makeNode(NODE_SWITCH, (yyvsp[(3) - (7)].nval), (yyvsp[(6) - (7)].nval), NULL);
    ;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 1328 "confused.y"
    { (yyval.nval) = NULL; ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 1329 "confused.y"
    {
        /* left=block, middle=value expr to match, right=next case */
        Node *c = makeNode(NODE_CASE, (yyvsp[(6) - (7)].nval), (yyvsp[(3) - (7)].nval), NULL);
        if ((yyvsp[(1) - (7)].nval) == NULL) {
            (yyval.nval) = c;
        } else {
            Node *p = (yyvsp[(1) - (7)].nval);
            while (p->right != NULL) p = p->right;
            p->right = c;
            (yyval.nval) = (yyvsp[(1) - (7)].nval);
        }
    ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 1341 "confused.y"
    {
        /* nop = default, middle is NULL = always matches */
        Node *c = makeNode(NODE_CASE, (yyvsp[(4) - (5)].nval), NULL, NULL);
        if ((yyvsp[(1) - (5)].nval) == NULL) {
            (yyval.nval) = c;
        } else {
            Node *p = (yyvsp[(1) - (5)].nval);
            while (p->right != NULL) p = p->right;
            p->right = c;
            (yyval.nval) = (yyvsp[(1) - (5)].nval);
        }
    ;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 1356 "confused.y"
    {
        /* Iff with no else branch */
        (yyval.nval) = makeNode(NODE_IF, (yyvsp[(3) - (7)].nval), (yyvsp[(6) - (7)].nval), NULL);
    ;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 1360 "confused.y"
    {
        /* Iff with or/oriff chain */
        (yyval.nval) = makeNode(NODE_IF, (yyvsp[(3) - (8)].nval), (yyvsp[(6) - (8)].nval), (yyvsp[(8) - (8)].nval));
    ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 1367 "confused.y"
    {
        Node *n  = makeNode(NODE_ARRAY_DECL, NULL, NULL, NULL);
        n->name  = strdup((yyvsp[(3) - (7)].sval));
        n->value = (yyvsp[(5) - (7)].ival);
        (yyval.nval) = n;
    ;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 1376 "confused.y"
    {
        /* left=index expr, right=value expr */
        Node *n = makeNode(NODE_ARRAY_ASSIGN, (yyvsp[(3) - (7)].nval), NULL, (yyvsp[(6) - (7)].nval));
        n->name = strdup((yyvsp[(1) - (7)].sval));
        (yyval.nval) = n;
    ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 1385 "confused.y"
    {
        /* plain or — the else branch */
        (yyval.nval) = (yyvsp[(3) - (4)].nval);
    ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 1389 "confused.y"
    {
        /* oriff with no further chain */
        (yyval.nval) = makeNode(NODE_IF, (yyvsp[(3) - (7)].nval), (yyvsp[(6) - (7)].nval), NULL);
    ;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 1393 "confused.y"
    {
        /* oriff chained with more oriff/or */
        (yyval.nval) = makeNode(NODE_IF, (yyvsp[(3) - (8)].nval), (yyvsp[(6) - (8)].nval), (yyvsp[(8) - (8)].nval));
    ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 1400 "confused.y"
    {
        Node *n  = makeNode(NODE_DECL, (yyvsp[(4) - (5)].nval), NULL, NULL);
        n->name  = strdup((yyvsp[(2) - (5)].sval));
        n->value = 0;   /* 0 = int */
        (yyval.nval) = n;
    ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 1406 "confused.y"
    {
        Node *n  = makeNode(NODE_DECL, (yyvsp[(4) - (5)].nval), NULL, NULL);
        n->name  = strdup((yyvsp[(2) - (5)].sval));
        n->value = 1;   /* 1 = float */
        (yyval.nval) = n;
    ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 1412 "confused.y"
    {
    Node *n  = makeNode(NODE_DECL, (yyvsp[(4) - (5)].nval), NULL, NULL);
    n->name  = strdup((yyvsp[(2) - (5)].sval));
    n->value = 2;   /* 2 = bool flag */
    (yyval.nval) = n;
    ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 1418 "confused.y"
    {
        Node *n = makeNode(NODE_DECL, NULL, NULL, NULL);
        n->name  = strdup((yyvsp[(2) - (5)].sval));
        n->value = 3;   /* 3 = string type */
        /* store the string literal in the node's right child's name */
        Node *s = makeNode(NODE_STRING_LIT, NULL, NULL, NULL);
        s->name = strdup((yyvsp[(4) - (5)].sval));
        n->left = s;
        (yyval.nval) = n;
    ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 1431 "confused.y"
    { (yyval.nval) = makeNum((yyvsp[(1) - (1)].ival));    ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 1432 "confused.y"
    { (yyval.nval) = makeNum((yyvsp[(1) - (1)].fval));    ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 1433 "confused.y"
    { (yyval.nval) = makeNum(1);     ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 1434 "confused.y"
    { (yyval.nval) = makeNum(0);     ;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 1435 "confused.y"
    { (yyval.nval) = makeIdent((yyvsp[(1) - (1)].sval));  ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 1436 "confused.y"
    { (yyval.nval) = makeNode(NODE_ADD, (yyvsp[(1) - (3)].nval), NULL, (yyvsp[(3) - (3)].nval)); ;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 1437 "confused.y"
    { (yyval.nval) = makeNode(NODE_SUB, (yyvsp[(1) - (3)].nval), NULL, (yyvsp[(3) - (3)].nval)); ;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 1438 "confused.y"
    { (yyval.nval) = makeNode(NODE_MUL, (yyvsp[(1) - (3)].nval), NULL, (yyvsp[(3) - (3)].nval)); ;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 1439 "confused.y"
    { (yyval.nval) = makeNode(NODE_DIV, (yyvsp[(1) - (3)].nval), NULL, (yyvsp[(3) - (3)].nval)); ;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 1440 "confused.y"
    { (yyval.nval) = makeNode(NODE_MOD, (yyvsp[(1) - (3)].nval), NULL, (yyvsp[(3) - (3)].nval)); ;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 1441 "confused.y"
    { (yyval.nval) = makeNode(NODE_GT,  (yyvsp[(1) - (3)].nval), NULL, (yyvsp[(3) - (3)].nval)); ;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 1442 "confused.y"
    { (yyval.nval) = makeNode(NODE_LT,  (yyvsp[(1) - (3)].nval), NULL, (yyvsp[(3) - (3)].nval)); ;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 1443 "confused.y"
    { (yyval.nval) = makeNode(NODE_EQ,  (yyvsp[(1) - (3)].nval), NULL, (yyvsp[(3) - (3)].nval)); ;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 1444 "confused.y"
    { (yyval.nval) = makeNode(NODE_NEQ, (yyvsp[(1) - (3)].nval), NULL, (yyvsp[(3) - (3)].nval)); ;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 1445 "confused.y"
    { (yyval.nval) = makeNode(NODE_GEQ, (yyvsp[(1) - (3)].nval), NULL, (yyvsp[(3) - (3)].nval)); ;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 1446 "confused.y"
    { (yyval.nval) = makeNode(NODE_LEQ, (yyvsp[(1) - (3)].nval), NULL, (yyvsp[(3) - (3)].nval)); ;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 1447 "confused.y"
    { (yyval.nval) = makeNode(NODE_EXPONENTIAL, (yyvsp[(3) - (6)].nval), NULL, (yyvsp[(5) - (6)].nval));;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 1448 "confused.y"
    {
        Node *n = makeNode(NODE_EMPTY_CHECK, NULL, NULL, NULL);
        n->name = strdup((yyvsp[(3) - (4)].sval));
        (yyval.nval) = n;
    ;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 1453 "confused.y"
    { Node *n = makeNode(NODE_STRING_LIT, NULL, NULL, NULL);
                n->name = strdup((yyvsp[(1) - (1)].sval));
                (yyval.nval) = n; ;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 1456 "confused.y"
    {
                    (yyval.nval) = makeNode(NODE_STR_JOIN, (yyvsp[(3) - (6)].nval), NULL, (yyvsp[(5) - (6)].nval));
                ;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 1459 "confused.y"
    {
                    (yyval.nval) = makeNode(NODE_STR_LEN, (yyvsp[(3) - (4)].nval), NULL, NULL);
                ;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 1462 "confused.y"
    {
                    (yyval.nval) = makeNode(NODE_STR_PART, (yyvsp[(3) - (8)].nval), (yyvsp[(5) - (8)].nval), (yyvsp[(7) - (8)].nval));
                ;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 1465 "confused.y"
    {
            Node *n = makeNode(NODE_ARRAY_ACCESS, (yyvsp[(3) - (4)].nval), NULL, NULL);
            n->name = strdup((yyvsp[(1) - (4)].sval));
            (yyval.nval) = n;
        ;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 1470 "confused.y"
    { (yyval.nval) = (yyvsp[(1) - (1)].nval); ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 1471 "confused.y"
    { (yyval.nval) = (yyvsp[(2) - (3)].nval); ;}
    break;



/* Line 1455 of yacc.c  */
#line 3604 "confused.tab.c"
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
#line 1474 "confused.y"


int yyerror(char *msg) {
    printf("Syntax Error: %s\n", msg);
    return 0;
}

int main(int argc, char *argv[]) {
    /* argv[1] = input source file, argv[2] = output file (optional) */
    if (argc < 2) {
        printf("Usage: confused.exe input.txt [output.txt]\n");
        return 1;
    }

    /* Open the source file for the lexer */
    FILE *f = fopen(argv[1], "r");
    if (f == NULL) {
        printf("Error: Cannot open file '%s'\n", argv[1]);
        return 1;
    }
    yyin = f;


    if (argc >= 3) {
        if (freopen(argv[2], "w", stdout) == NULL) {
            printf("Error: Cannot open output file '%s'\n", argv[2]);
            return 1;
        }
    }

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
