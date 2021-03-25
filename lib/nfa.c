#include <ctype.h>
#include "../include/nfa.h"
#include "../include/macro.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXNFA 1024
#define MAXACCEPT 2048 * 10
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
PRIVATE char *expression, *start_expression;
PRIVATE char lexeme;

PRIVATE struct nfa *nstack[32];
PRIVATE struct nfa **nstack_p = &nstack[32];

PRIVATE struct nfa *nfas;
PRIVATE int next_alloc;
PRIVATE int nfa_size;

PRIVATE int *accepts;

PRIVATE enum token advance() {
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
            goto exit;
        }
        expression = *istack_p;
        istack_p++;
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
            //push
            if(--istack_p >= &istack[32]) {
                // TODO
            }
            *--istack_p = expression;
            if(macro_get(istack_p, &expression) != M_OK) {
                //TODO error handle
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
    if(nfas == NULL) {
        if((nfas = calloc(MAXNFA, sizeof(struct nfa))) == NULL) {
            //TODO Error
        }
    }
    if(++nfa_size > MAXNFA) {
        // TODO Error
    }
    struct nfa *toreturn = (nstack_p < &nstack[32]) ? *nstack_p++ : &nfas[next_alloc++];
    toreturn->edge = EPSILON;
    return toreturn;
}

PRIVATE void free_nfa(struct nfa *tofree) {
    memset(tofree, 0, sizeof(struct nfa));
    *--nstack_p = tofree;
    if(nstack_p < nstack) {
        //TODO Error
    }
    tofree->edge = EMPTY;
}

PRIVATE char *save_accept(char *action) {
    static int *accepts_p;
    if(accepts == NULL) {
        if((accepts = calloc(MAXACCEPT, 1)) == NULL) {
            //TODO Error
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
    ENTER("machine");
    // start state
    struct nfa *start, *p;
    p = start = rule();
    while(!match(EOI)) {
        p->next2 = new_nfa();
        p = p->next2;
        p->next = rule();
    }
    LEAVE("machine");
    return start;
}

PRIVATE struct nfa *rule() {
    ENTER("rule");
    struct nfa *start, *end;
    int anchor = 0;
    if(match(BOL)) {
        advance();
        start = new_nfa();
        anchor |= START;
        expr(&start->next, &end);
    } else {
        expr(&start, &end);
    }
    if(match(EOL)) {
        end->next = new_nfa();
        end = end->next;
        advance();
        anchor |= END;
    }
    while(isspace(*expression)) expression++;
    end->accept = save_accept(expression);
    end->anchor = anchor;
    advance();
    D(printf("Accept action: %s\n", end->accept);)
    LEAVE("rule");

    return start;
}

PRIVATE void expr(struct nfa **startp, struct nfa **endp) {
    ENTER("expr");
    concat(startp, endp);
    struct nfa *startp2, *endp2;
    while(match(OR)) {
        advance();
        concat(&startp2, &endp2);
        struct nfa *binder = new_nfa();
        binder->next = *startp;
        binder->next2 = startp2;
        *startp = binder;
        binder = new_nfa();
        (*endp)->next = binder;
        endp2->next = binder;
        *endp = binder;
    }
    LEAVE("expr");
}

PRIVATE void concat(struct nfa **startp, struct nfa **endp) {
    ENTER("concat");
    if(can_concat()) {
        factor(startp, endp);
    }
    while(can_concat()) {
        struct nfa *startp2, *endp2;
        factor(&startp2, &endp2);
        memcpy(*endp, startp2, sizeof(struct nfa));
        free_nfa(startp2);
    }
    LEAVE("concat");
}

PRIVATE int can_concat() {
    switch(current_token) {
        case OR:
        case EOS:
        case EOL:
        case RPAREN:
        case EOI: return 0;
        case PLUS:
        case CLOSURE:
        case OPTIONAL:  return 0; //TODO Error
        case BOL: return 0; // TODO Error
        case RCCL: return 0; //TODO Error
        default: return 1;
    }
}

PRIVATE void factor(struct nfa **startp, struct nfa **endp) {
    ENTER("factor");
    term(startp, endp);
    if(match(CLOSURE) | match(PLUS) | match(OPTIONAL)) {
        struct nfa *newstart = new_nfa(), *newend = new_nfa();
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
    }
    LEAVE("factor");
}

PRIVATE void term(struct nfa **startp, struct nfa **endp) {
    ENTER("term");
    if(match(LPAREN)) {
        advance();
        expr(startp, endp);
        if(!match(RPAREN)) {
            //TODO Error
        }
        advance();
        return;
    }
    *startp = new_nfa();
    *endp = (*startp)->next = new_nfa();
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
            if(match(BOL)) {
                advance();
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
            for(; first <= lexeme; first++) setadd(ccl, first);
        } else {
            first = lexeme;
            setadd(ccl, first);
        }
        advance();
    }
}

PRIVATE void printf_nfa(struct nfa *start) {
    printf("/*--------------------------------------------------*/\n");
    printf("NFA: start state is #%d\n", (int)(start-nfas));
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
            i >= ' ' ? printf("%c", i) : printf("^@%c", i+' ');
        }
    }
    printf("]");
}

int thompson_construction(struct nfa **nfa, struct nfa **start, char *(*inputfun)()) {
    input_fun = inputfun;
    current_token = EOS;
    advance();
    *start = machine();
    *nfa = nfas;
    D(printf_nfa(*start);)
    return next_alloc;
}


