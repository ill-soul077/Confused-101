%{
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
    NODE_STRING_LIT,
    NODE_SWITCH,
    NODE_CASE,
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

        case NODE_CASE:
            break; /* never executed directly — always walked via NODE_SWITCH */

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
        // case NODE_SHOW: {
        //     char *t = gen(n->left);
        //     printf("print %s\n", t);
        //     break;
        // }

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
        case NODE_SHOW: {
            if (n->left && n->left->type == NODE_STRING_LIT) {
                printf("print_str \"%s\"\n", n->left->name);
                break;
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

            /* Two-pass approach:
            Pass 1 — generate condition checks and collect case labels.
            Pass 2 — emit each block under its label. */
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


        default:
            break;
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



%type <nval> program statement expr declaration show_stmt ask_stmt iff_stmt or_chain
             wlp_stmt flp_stmt compound_stmt func_def func_call arg_list param_list
             param return_stmt block stk_decl queue_decl push_stmt pop_stmt inc_stmt
             incident_stmt case_list


%%

/* stub — just enough to compile cleanly for now */
program
    : statement             { $$ = $1; root = $1; }
    | program statement     { $$ = makeNode(NODE_SEQ, $1, NULL, $2); root = $$; }
    | MFUN '{' block '}'    { $$ = $3; root = $3; }
    ;

statement
    : declaration        { $$ = $1; }
    | show_stmt          { $$ = $1; }
    | ask_stmt           { $$ = $1; }
    | iff_stmt           { $$ = $1; }
    | wlp_stmt           { $$ = $1; }
    | flp_stmt           { $$ = $1; }
    | compound_stmt      { $$ = $1; }
    | func_def           { $$ = $1; }
    | return_stmt        { $$ = $1; }
    | stk_decl           { $$ = $1; }
    | queue_decl         { $$ = $1; }
    | push_stmt          { $$ = $1; }
    | pop_stmt           { $$ = $1; }
    | inc_stmt           { $$ = $1; }
    | incident_stmt      { $$ = $1; }
    | func_call HASH     { $$ = $1; }
    ;


inc_stmt
    : INC '(' IDENT ')' HASH {
        Node *n = makeNode(NODE_INC, NULL, NULL, NULL);
        n->name = strdup($3);
        $$ = n;
    }
    ;

ask_stmt
    : ASK '(' IDENT ')' HASH {
        Node *n = makeNode(NODE_ASK, NULL, NULL, NULL);
        n->name = strdup($3);
        $$ = n;
    }
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

stk_decl
    : STK IDENT HASH {
        Node *n = makeNode(NODE_STK_DECL, NULL, NULL, NULL);
        n->name = strdup($2);
        $$ = n;
    }
    ;

queue_decl
    : LINE IDENT HASH {
        Node *n = makeNode(NODE_QUEUE_DECL, NULL, NULL, NULL);
        n->name = strdup($2);
        $$ = n;
    }
    ;

push_stmt
    : LASTADD '(' IDENT DOLLAR expr ')' HASH {
        Node *n = makeNode(NODE_PUSH, $5, NULL, NULL);
        n->name = strdup($3);
        $$ = n;
    }
    ;

pop_stmt
    : LASTDRAG '(' IDENT ')' HASH {
        Node *n = makeNode(NODE_POP, NULL, NULL, NULL);
        n->name = strdup($3);
        $$ = n;
    }
    ;    

incident_stmt
    : INCIDENT '(' expr ')' '{' case_list '}' {
        $$ = makeNode(NODE_SWITCH, $3, $6, NULL);
    }
    ;

case_list
    : /* empty */                              { $$ = NULL; }
    | case_list CHECK expr COLON '{' block '}' {
        /* left=block, middle=value expr to match, right=next case */
        Node *c = makeNode(NODE_CASE, $6, $3, NULL);
        if ($1 == NULL) {
            $$ = c;
        } else {
            Node *p = $1;
            while (p->right != NULL) p = p->right;
            p->right = c;
            $$ = $1;
        }
    }
    | case_list NOP '{' block '}' {
        /* nop = default, middle is NULL = always matches */
        Node *c = makeNode(NODE_CASE, $4, NULL, NULL);
        if ($1 == NULL) {
            $$ = c;
        } else {
            Node *p = $1;
            while (p->right != NULL) p = p->right;
            p->right = c;
            $$ = $1;
        }
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
    | BOOL IDENT AT expr HASH {
    Node *n  = makeNode(NODE_DECL, $4, NULL, NULL);
    n->name  = strdup($2);
    n->value = 2;   /* 2 = bool flag */
    $$ = n;
    }
    ;

expr
    : INT_LIT       { $$ = makeNum($1);    }
    | FLOAT_LIT     { $$ = makeNum($1);    }
    | TRUE_LIT      { $$ = makeNum(1);     }
    | FALSE_LIT     { $$ = makeNum(0);     }
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
    | EXPONENTIAL '(' expr DOLLAR expr ')' { $$ = makeNode(NODE_EXPONENTIAL, $3, NULL, $5);}
    | NAINAI '(' IDENT ')' {
        Node *n = makeNode(NODE_EMPTY_CHECK, NULL, NULL, NULL);
        n->name = strdup($3);
        $$ = n;
    }
    | STRING_LIT  { Node *n = makeNode(NODE_STRING_LIT, NULL, NULL, NULL);
                n->name = strdup($1);
                $$ = n; }
    | func_call  { $$ = $1; }
    | '(' expr ')'  { $$ = $2; }
    ;

%%

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