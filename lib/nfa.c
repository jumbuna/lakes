#include <ctype.h>
#include "../include/nfa.h"
#include "../include/macro.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#define match(t) (t == current_token)

#ifdef DEBUG
int level = 0;
#define ENTER(x) (printf("%*sENTER: %s `%c` %10s\n", level++*4, "", x, lexeme, expression))
#define LEAVE(x) (printf("%*sLEAVE: %s `%c` %10s\n", --level*4, "", x, lexeme, expression))
#else
#define ENTER(x)
#define LEAVE(x)
#endif

extern int esc();

PRIVATE int last_error;

#define ERROR(x) (last_error = x)

#define NORETURN -2

#define RETURN_ON_ERROR_WITH_VALUE(...)   \
    if(last_error) return (__VA_ARGS__);\
    else

#define RETURN_ON_ERROR_VOID()  \
    if(last_error) return;\
    else

enum token {
    LITERAL,
    CLOSURE,
    OPTIONAL,
    PLUS,
    ANY,
    LBRACE,
    RBRACE,
    LPAREN,
    RPAREN,
    LCCL,
    RCCL,
    DASH,
    EOI,
    EOS,
    BOL,
    EOL,
    OR
};

// Generated by tkgen
PRIVATE enum token token_map[128] = {
    LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL, 
    LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL, 
    LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL, 
    LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL, 
    LITERAL,   LITERAL,   LITERAL,   LITERAL,       EOL,   LITERAL,   LITERAL,   LITERAL, 
     LPAREN,    RPAREN,   CLOSURE,      PLUS,   LITERAL,      DASH,       ANY,   LITERAL, 
    LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL, 
    LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL,  OPTIONAL, 
    LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL, 
    LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL, 
    LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL, 
    LITERAL,   LITERAL,   LITERAL,      LCCL,   LITERAL,      RCCL,       BOL,   LITERAL, 
    LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL, 
    LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL, 
    LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL,   LITERAL, 
    LITERAL,   LITERAL,   LITERAL,   LITERAL,        OR,   LITERAL,   LITERAL,   LITERAL
};

PRIVATE enum token current_token;

PRIVATE struct nfa *nstack[32];
PRIVATE struct nfa **nstack_p = &nstack[32];

char *mstack[32];
char **mstack_p = &mstack[32];

char *pre_macro;

PRIVATE struct nfa *nfas;
PRIVATE int next_alloc;
PRIVATE int nfa_size;

PRIVATE int *accepts;

PRIVATE enum token advance() {
    RETURN_ON_ERROR_WITH_VALUE(-1);

    static int in_quote;
    static char *istack[32];
    static char **istack_p = &istack[32]; 
    int is_escape;
    if(current_token == EOI) {
        goto exit;
    }
    if(current_token == EOS) {
        do {
            if(!(expression = (*input_fun)())) {
                current_token = EOI; 
                goto exit;
            }
            while(isspace(*expression)) expression++;
        } while( !*expression );
        D(printf("expression: %s\n", expression);)
        start_expression = expression;
    }
    while(*expression == '\0') {
        if(istack_p >= &istack[32]) {
            current_token = EOS;
            pre_macro = NULL;
            goto exit;
        }
        expression = *istack_p;
        istack_p++;
        ++mstack_p;
    }
    if(*expression == '"') {
        in_quote = ~in_quote;
        expression++;
        if(!*expression) {
            current_token = EOS;
            goto exit;
        }
    }
    if(!in_quote) {
        while(*expression == '{') {
            if(!pre_macro) {
                pre_macro = expression;
            }
            //push
            if(--istack_p >= &istack[32]) {
                // TODO
                ERROR(N_NESTTOODEEP);
                RETURN_ON_ERROR_WITH_VALUE(-1);
            }
            *--mstack_p = expression+1;
            *--istack_p = expression;
            if(macro_get(istack_p, &expression) != M_OK) {
                //TODO error handle
                ERROR(N_NOMACRO);
                RETURN_ON_ERROR_WITH_VALUE(-1);
            }
        }
    }
    is_escape = (*expression == '\\');
    if(!in_quote) {
        if(isspace(*expression)) {
            current_token = EOS;
            goto exit;
        }
        lexeme = esc(&expression);
    } else {
        if(is_escape && expression[1] == '"') {
            lexeme = '"';
            expression += 2;
        } else {
            lexeme = *expression++;
        }
    }
    current_token = (is_escape || in_quote) ? LITERAL : token_map[lexeme];
exit:
    return current_token;
}

PRIVATE struct nfa *machine(), *rule(), *new_nfa();
PRIVATE void expr(), concat(), factor(), term(), free_nfa(),  fill_ccl(), printf_nfa(), print_ccl();
PRIVATE int can_concat();
PRIVATE char *save_accept();


PRIVATE struct nfa *new_nfa() {
    RETURN_ON_ERROR_WITH_VALUE(NULL);
    if(nfas == NULL) {
        if((nfas = calloc(MAXNFA, sizeof(struct nfa))) == NULL) {
            // TODO Error
            ERROR(N_NFASNOMEM);
            RETURN_ON_ERROR_WITH_VALUE(NULL);
        }
    }
    if(++nfa_size > MAXNFA) {
        // TODO Error
        ERROR(N_TOOMANYNFAS);
        RETURN_ON_ERROR_WITH_VALUE(NULL);
    }
    struct nfa *toreturn = (nstack_p < &nstack[32]) ? *nstack_p++ : &nfas[next_alloc++];
    toreturn->edge = EPSILON;
    return toreturn;
}

PRIVATE void free_nfa(struct nfa *tofree) {
    RETURN_ON_ERROR_VOID();
    memset(tofree, 0, sizeof(struct nfa));
    *--nstack_p = tofree;
    if(nstack_p < nstack) {
        //TODO Error
        ERROR(N_COMPLEXEXPR);
        RETURN_ON_ERROR_VOID();
    }
    tofree->edge = EMPTY;
}

PRIVATE char *save_accept(char *action) {
    RETURN_ON_ERROR_WITH_VALUE(NULL);
    static int *accepts_p;
    if(accepts == NULL) {
        if((accepts = calloc(MAXACCEPT, 1)) == NULL) {
            //TODO Error
            RETURN_ON_ERROR_WITH_VALUE(NULL);
        }
        accepts_p = accepts;
    }
    if(!*action) {
        return NULL;
    }
    if(*action == '|') {
        return (char*) (accepts_p +1);
    }
    *accepts_p++ = Lineno;
    char *p = (char *) accepts_p;
    p = stpcpy(p, action);
    // restore alignment
    p  =  p + (sizeof(int) - ((long)p % sizeof(int)));
    int *temp = accepts_p;
    accepts_p = (int*) p;
    return (char*) temp;
}

PRIVATE struct nfa *machine() {
    RETURN_ON_ERROR_WITH_VALUE(NULL);
    ENTER("machine");
    // start state
    struct nfa *start, *p;
    p = start = new_nfa();
    RETURN_ON_ERROR_WITH_VALUE(NULL);
    p->next = rule();
    while(!match(EOI)) {
        RETURN_ON_ERROR_WITH_VALUE(NULL);
        p->next2 = new_nfa();
        p = p->next2;
        p->next = rule();
    }
    LEAVE("machine");
    return start;
}

PRIVATE struct nfa *rule() {
    RETURN_ON_ERROR_WITH_VALUE(NULL);
    ENTER("rule");
    struct nfa *start, *end;
    int anchor = 0;
    if(match(BOL)) {
        advance();
        start = new_nfa();
        anchor |= START;
        RETURN_ON_ERROR_WITH_VALUE(NULL);
        start->edge = '\n';
        expr(&start->next, &end);
    } else {
        expr(&start, &end);
    }
    if(match(EOL)) {
        RETURN_ON_ERROR_WITH_VALUE(NULL);
        end->next = new_nfa();
        RETURN_ON_ERROR_WITH_VALUE(NULL);
        end->edge = '\n';
        end = end->next;
        advance();
        anchor |= END;
    }
    while(isspace(*expression)) expression++;
    end->accept = save_accept(expression);
    RETURN_ON_ERROR_WITH_VALUE(NULL);
    end->anchor = anchor;
    advance();
    D(printf("Accept action: %s\n", end->accept);)
    LEAVE("rule");
    return start;
}

PRIVATE void expr(struct nfa **startp, struct nfa **endp) {
    RETURN_ON_ERROR_VOID();
    ENTER("expr");
    concat(startp, endp);
    struct nfa *startp2, *endp2;
    while(match(OR)) {
        advance();
        concat(&startp2, &endp2);
        RETURN_ON_ERROR_VOID();
        struct nfa *binder = new_nfa();
        RETURN_ON_ERROR_VOID();
        binder->next = *startp;
        binder->next2 = startp2;
        *startp = binder;
        binder = new_nfa();
        RETURN_ON_ERROR_VOID();
        (*endp)->next = binder;
        endp2->next = binder;
        *endp = binder;
    }
    LEAVE("expr");
}

PRIVATE void concat(struct nfa **startp, struct nfa **endp) {
    RETURN_ON_ERROR_VOID();
    ENTER("concat");
    if(can_concat()) {
        RETURN_ON_ERROR_VOID();
        factor(startp, endp);
        RETURN_ON_ERROR_VOID();
    }
    while(can_concat()) {
        RETURN_ON_ERROR_VOID();
        struct nfa *startp2, *endp2;
        factor(&startp2, &endp2);
        RETURN_ON_ERROR_VOID();
        memcpy(*endp, startp2, sizeof(struct nfa));
        (*endp) = endp2;
        free_nfa(startp2);
        RETURN_ON_ERROR_VOID();
    }
    LEAVE("concat");
}

PRIVATE int can_concat() {
    RETURN_ON_ERROR_WITH_VALUE(0);
    switch(current_token) {
        case OR:
        case EOS:
        case EOL:
        case RPAREN:
        case EOI: return 0;
        case PLUS:
        case CLOSURE:
        case OPTIONAL: ERROR(N_BADCLOSURE); return 0; //TODO Error
        case BOL: ERROR(N_BADBOL); return 0; // TODO Error
        case RCCL: ERROR(N_NOCCL); return 0; //TODO Error
        default: return 1;
    }
}

PRIVATE void factor(struct nfa **startp, struct nfa **endp) {
    RETURN_ON_ERROR_VOID();
    ENTER("factor");
    term(startp, endp);
    RETURN_ON_ERROR_VOID();
    if(match(CLOSURE) | match(PLUS) | match(OPTIONAL)) {
        struct nfa *newstart = new_nfa(), *newend = new_nfa();
        RETURN_ON_ERROR_VOID();
        newstart->next = *startp;
        (*endp)->next = newend;
        if(match(CLOSURE) | match(PLUS)) {
            (*endp)->next2 = *startp;
        }
        if(match(CLOSURE) | match(OPTIONAL)) {
            newstart->next2 = newend;
        }
        *startp = newstart;
        *endp = newend;
        advance();
        RETURN_ON_ERROR_VOID();
    }
    LEAVE("factor");
}

PRIVATE void term(struct nfa **startp, struct nfa **endp) {
    ENTER("term");
    if(match(LPAREN)) {
        advance();
        expr(startp, endp);
        RETURN_ON_ERROR_VOID();
        if(!match(RPAREN)) {
            //TODO Error
            ERROR(N_MISSNGPAREN);
            RETURN_ON_ERROR_VOID();
        }
        advance();
        return;
    }
    *startp = new_nfa();
    RETURN_ON_ERROR_VOID();
    *endp = (*startp)->next = new_nfa();
    RETURN_ON_ERROR_VOID();
    if(!match(ANY) & !match(LCCL)) {
        (*startp)->edge = lexeme;
    } else {
        (*startp)->edge = CCL;
        (*startp)->ccl = setcreate();
        if(match(ANY)) {
            // all but \n
            setadd((*startp)->ccl, '\n');
            setcomplement((*startp)->ccl);
        } else {
            advance();
            RETURN_ON_ERROR_VOID();
            if(match(BOL)) {
                advance();
                RETURN_ON_ERROR_VOID();
                setcomplement((*startp)->ccl);
                //[^]
                if(match(RCCL)) {
                    setadd((*startp)->ccl, ' ');
                    setadd((*startp)->ccl, '\n');
                    setadd((*startp)->ccl, '\f');
                    setadd((*startp)->ccl, '\t');
                    goto exit;
                } else {
                    //[^...]
                    goto fill;
                }
            }
            else if(match(RCCL)) {
                //[]
                setadd((*startp)->ccl, ' ');
                setadd((*startp)->ccl, '\n');
                setadd((*startp)->ccl, '\f');
                setadd((*startp)->ccl, '\t');
            } else {
                //[...]
                fill: 
                    fill_ccl((*startp)->ccl);
                    if(!match(RCCL)) {
                        //TODO Error
                        ERROR(N_OPENCCL);
                        RETURN_ON_ERROR_VOID();
                    }
            }
        }
    }
exit:
    advance();
    LEAVE("term");
}

PRIVATE void fill_ccl(struct set *ccl) {
    int first;
    while(!match(RCCL) && !match(EOS)) {
        if(match(DASH)) {
            advance();
            RETURN_ON_ERROR_VOID();
            for(; first <= lexeme; first++) setadd(ccl, first);
        } else {
            first = lexeme;
            setadd(ccl, first);
        }
        advance();
        RETURN_ON_ERROR_VOID();
    }
}

PRIVATE void printf_nfa(struct nfa *start) {
    printf("/*--------------------------------------------------*/\n");
    if(!next_alloc) printf("NO NFA\n");
    for(int i = 0; i < next_alloc; i++) {
        printf("STATE %d", i);
        if(nfas[i].next) {
            printf(" to %d ", (int)(nfas[i].next-nfas));
            if(nfas[i].next2) {
                printf("%d ", (int)(nfas[i].next2-nfas));
            }
            printf("on ");
            switch(nfas[i].edge) {
                case EPSILON: printf("Epsilon");break;
                case CCL: print_ccl(nfas[i].ccl);break;
                default: printf("%c", nfas[i].edge );
            }
        } else {
            printf(" [TERMINATING] ");
        }
        if(&nfas[i] == start) {
            printf(" (START STATE) ");
        }
        if(nfas[i].accept) {
            printf(" <%s> ", nfas[i].accept);
        }
        printf("\n");
    }
    printf("/*--------------------------------------------------*/\n");
}

PRIVATE void print_ccl(struct set *s) {
    printf("[");
    for(int i = 0; i < 128; i++) {
        if(settest(s, i)) {
            i >= ' ' ? printf("%c", i) : printf("^%c", i+'@');
        }
    }
    printf("]");
}

int thompson_construction(struct nfa **nfa, struct nfa **start, int *size, char *(*inputfun)()) {
    input_fun = inputfun;
    current_token = EOS;
    advance();
    *start = machine();
    *size = next_alloc;
    *nfa = nfas;
    D(printf_nfa(*start);)
    if(last_error){
        if(nfas) free(nfas);
        if(accepts) free(accepts);
    }
    RETURN_ON_ERROR_WITH_VALUE(last_error)
    return last_error;
}

char *nerror2str[] = {
    "OK",
    "Too many nfas",
    "Expression missing closing parenthesis `)`",
    "Macro nesting too deep. Max nesting 32",
    "No memory for nfas",
    "No memory for accept action strings",
    "Expression Too complex",
    "Closure not bound to any expression",
    "`^` not allowed here",
    "No opening `[` to match this `]`",
    "No closing `[`",
    "Macro not found"
};

void print_nerror() {
    if(last_error == N_OK) return;
    char *temp;
    char *to_use = pre_macro ? pre_macro : expression;
    fprintf(stderr, "%s%s:%d:%d: error: %s\n", Ifilename[0] == '/' ? "" : (temp = getcwd(0,0)), Ifilename, Lineno, (int) (to_use-start_expression), nerror2str[last_error]);
    free(temp);
    fprintf(stderr, "%s\n", start_expression);
    temp = start_expression;
    while(temp < to_use) {
        putc('~', stderr);
        temp++;
    }
    putc('^', stderr);
    putc('\n', stderr);
    if(mstack_p < &mstack[32]) {
        char **tempp = mstack_p;
        if(last_error == N_NOMACRO) {
            temp = strchr(*tempp, '}');
            assert(temp);
            *temp = 0;
            fputs("\nMacroname: ", stderr);
            fputs(*tempp, stderr);
            fputs("\n\n", stderr);
            ++tempp;
        }
        fputs("Macro backtrace:\n", stderr);
        while(tempp != &mstack[32]) {
            temp = strchr(*tempp, '}');
            *temp = 0;
            fputs(*tempp, stderr);
            fputc('\n', stderr);
            tempp++;
        }
    }
    fflush(stderr);
}