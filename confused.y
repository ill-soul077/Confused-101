%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>  
extern int yylex();
extern FILE *yyin;
int yyerror(char *msg);


/* STACK AND QUEUE */
typedef struct StackNode {
    int              data;
    struct StackNode *next;  
} StackNode;

typedef struct QueueNode {
    int               data;
    struct QueueNode *next; 
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
    int  *arr_data;   
    int   arr_size;   
    char *str_val;   
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
    NODE_STK_DECL,    
    NODE_QUEUE_DECL,  
    NODE_PUSH,        
    NODE_POP,         
    NODE_EMPTY_CHECK, 
    NODE_STRING_LIT,
    NODE_SWITCH,
    NODE_CASE,
    NODE_ARRAY_DECL,   
    NODE_ARRAY_ASSIGN, 
    NODE_ARRAY_ACCESS, 
    NODE_STR_JOIN,     
    NODE_STR_LEN,      
    NODE_STR_PART,     
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
void execute(Node *n);  
float eval(Node *n);    

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
            float base = eval(n->left);   
            float exp  = eval(n->right);  
            return pow(base, exp);        
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
            
            if (n->left && n->left->type == NODE_IDENT) {
                Symbol *sym = get_symbol(n->left->name);
                if (sym && strcmp(sym->type, "string") == 0) {
                    printf("%s\n", sym->str_val ? sym->str_val : "");
                    break;
                }
            }
            
            if (n->left && n->left->type == NODE_STRING_LIT) {
                printf("%s\n", n->left->name);
                break;
            }
           
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
            insert("i", "int", start);
            Symbol *i_sym = get_symbol("i");
            for (i_sym->value.ival = start; i_sym->value.ival < end; i_sym->value.ival++) {
                execute(n->right);  
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
            printf("Enter value for %s: ", n->name);   
            
            if (strcmp(sym->type, "float") == 0) {
                scanf("%f", &sym->value.fval);         
            } else {
                scanf("%d", &sym->value.ival);         
            }
            printf("Read: %s = ", n->name);
            if (strcmp(sym->type, "float") == 0)
                printf("%.2f\n", sym->value.fval);
            else
                printf("%d\n", sym->value.ival);
            break;
        }

        case NODE_FUNC_DEF:
            break; 

        case NODE_COMPOUND_ADD: {
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
                sym->stk_top   = NULL;  
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
                sym->que_rear  = NULL;  
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
                StackNode *node = malloc(sizeof(StackNode));
                if (node == NULL) { printf("Out of memory!\n"); break; }
                node->data     = val;         
                node->next     = sym->stk_top;
                sym->stk_top   = node;       
                sym->stk_count++;
                printf("Pushed %d onto stack '%s'  [size=%d]\n",
                    val, n->name, sym->stk_count);

            } else if (strcmp(sym->type, "queue") == 0) {
                QueueNode *node = malloc(sizeof(QueueNode));
                if (node == NULL) { printf("Out of memory!\n"); break; }
                node->data = val;
                node->next = NULL; 
                if (sym->que_rear == NULL) {
                    
                    sym->que_front = node;
                    sym->que_rear  = node;
                } else {
                    
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
                StackNode *old   = sym->stk_top; 
                int        val   = old->data;     
                sym->stk_top     = old->next;     
                free(old);                        
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
                sym->que_front = old->next;       
                if (sym->que_front == NULL)
                    sym->que_rear = NULL;         
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
            break; 

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
            char *l = gen(n->left);   
            char *r = gen(n->right);  
            char *t = new_temp();     
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
            char *base = gen(n->left);    
            char *exp  = gen(n->right);   
            char *t    = new_temp();
            printf("%s = %s ^ %s\n", t, base, exp);  
            return t;
        }

        case NODE_FUNC_CALL: {

            Node *arg = n->left;
            while (arg != NULL) {
                char *a = gen(arg->left);   
                printf("param %s\n", a);    
                arg = arg->right;
            }
            
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
                char *strval = (n->left && n->left->name) ? n->left->name : "";
                printf("%s = \"%s\"\n", n->name, strval);
                break;
            }
            char *t = gen(n->left);
            printf("%s = %s\n", n->name, t);
            break;
        }

        case NODE_COMPOUND_ADD: {
            char *t_val = gen(n->left);    
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
            
            FuncEntry *fn = lookup_func(n->name);
            if (fn) {
                for (int i = 0; i < fn->param_count; i++)
                    printf("; param %s\n", fn->param_names[i]);
            }
            gen_stmt(n->middle);   
            printf("; --- end %s ---\n\n", n->name);
            break;
        }


        case NODE_IF: {
            char *t   = gen(n->left);     
            char *l_true = new_label();   
            char *l_end  = new_label();   

            printf("if %s goto %s\n", t, l_true);
            printf("goto %s\n", l_end);
            printf("%s:\n", l_true);
            gen_stmt(n->middle);           
            printf("%s:\n", l_end);
            if (n->right != NULL)
                gen_stmt(n->right);        
            break;
        }


        case NODE_WHILE: {
            char *l_start = new_label();   
            char *l_body  = new_label();   
            char *l_end   = new_label();   

            printf("%s:\n", l_start);
            char *t = gen(n->left);        
            printf("if %s goto %s\n", t, l_body);
            printf("goto %s\n", l_end);
            printf("%s:\n", l_body);
            gen_stmt(n->middle);           
            printf("goto %s\n", l_start); 
            printf("%s:\n", l_end);
            break;
        }


        case NODE_FOR: {
            char *start   = gen(n->left);    
            char *end_val = gen(n->middle);  
            char *l_start = new_label();
            char *l_body  = new_label();
            char *l_end   = new_label();

            printf("i = %s\n", start);        
            printf("%s:\n", l_start);
            char *t_cond = new_temp();
            printf("%s = i < %s\n", t_cond, end_val); 
            printf("if %s goto %s\n", t_cond, l_body);
            printf("goto %s\n", l_end);
            printf("%s:\n", l_body);
            gen_stmt(n->right);                
            char *t_inc = new_temp();
            printf("%s = i + 1\n", t_inc);     
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
            if (strcmp("stack", "stack") == 0) 
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
            char *sv    = gen(n->left);   
            char *l_end = new_label();


            char *case_labels[50];
            int   case_count = 0;
            char *l_default  = NULL;

           
            Node *c = n->middle;
            while (c != NULL) {
                if (c->middle == NULL) {
           
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
            
            if (l_default) printf("goto %s\n", l_default);
            else           printf("goto %s\n", l_end);

            
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
             incident_stmt case_list array_decl array_assign 


%%


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
    | array_decl   { $$ = $1; }
    | array_assign { $$ = $1; }
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
        
        Node *n = makeNode(NODE_FOR, $3, $5, $8);
        $$ = n;
    }
    ;
wlp_stmt
    : WLP '(' expr ')' '{' block '}' {
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
       
        $$ = makeNode(NODE_IF, $3, $6, NULL);
    }
    | IFF '(' expr ')' '{' block '}' or_chain {
        
        $$ = makeNode(NODE_IF, $3, $6, $8);
    }
    ;

array_decl
    : MY_ARRAY IN IDENT '(' INT_LIT ')' HASH {
        Node *n  = makeNode(NODE_ARRAY_DECL, NULL, NULL, NULL);
        n->name  = strdup($3);
        n->value = $5;
        $$ = n;
    }
    ;

array_assign
    : IDENT '[' expr ']' AT expr HASH {
        
        Node *n = makeNode(NODE_ARRAY_ASSIGN, $3, NULL, $6);
        n->name = strdup($1);
        $$ = n;
    }
    ;    

or_chain
    : OR '{' block '}' {
        $$ = $3;
    }
    | ORIFF '(' expr ')' '{' block '}' {
        
        $$ = makeNode(NODE_IF, $3, $6, NULL);
    }
    | ORIFF '(' expr ')' '{' block '}' or_chain {
        $$ = makeNode(NODE_IF, $3, $6, $8);
    }
    ;

declaration
    : IN IDENT AT expr HASH {
        Node *n  = makeNode(NODE_DECL, $4, NULL, NULL);
        n->name  = strdup($2);
        n->value = 0;   
        $$ = n;
    }
    | FLT IDENT AT expr HASH {
        Node *n  = makeNode(NODE_DECL, $4, NULL, NULL);
        n->name  = strdup($2);
        n->value = 1;  
        $$ = n;
    }
    | BOOL IDENT AT expr HASH {
    Node *n  = makeNode(NODE_DECL, $4, NULL, NULL);
    n->name  = strdup($2);
    n->value = 2;  
    $$ = n;
    }
    | CH IDENT AT STRING_LIT HASH {
        Node *n = makeNode(NODE_DECL, NULL, NULL, NULL);
        n->name  = strdup($2);
        n->value = 3;   
        Node *s = makeNode(NODE_STRING_LIT, NULL, NULL, NULL);
        s->name = strdup($4);
        n->left = s;
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
    | STRJOIN '(' expr DOLLAR expr ')'  {
                    $$ = makeNode(NODE_STR_JOIN, $3, NULL, $5);
                }
                | STRLENF '(' expr ')'  {
                    $$ = makeNode(NODE_STR_LEN, $3, NULL, NULL);
                }
                | TAKEPART '(' expr DOLLAR expr DOLLAR expr ')'  {
                    $$ = makeNode(NODE_STR_PART, $3, $5, $7);
                }  
    | IDENT '[' expr ']' {
            Node *n = makeNode(NODE_ARRAY_ACCESS, $3, NULL, NULL);
            n->name = strdup($1);
            $$ = n;
        }                      
    | func_call  { $$ = $1; }
    | '(' expr ')'  { $$ = $2; }
    ;

%%

int yyerror(char *msg) {
    printf("Syntax Error: %s\n", msg);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: confused.exe input.txt [output.txt]\n");
        return 1;
    }

    
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