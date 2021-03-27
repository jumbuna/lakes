char *date = "(0?[1-9]|[1-2][0-9]|3[0-1])/(0?[1-9]|1[1-2])/([0-9][0-9][0-9][0-9]|[0-9][0-9]) return date;";

struct nfa *nfa_array;

char *test_expression() {
    static int firstrun = 1;
    if(firstrun) {
        firstrun = 0;
        return date;
    }
    return 0;
}