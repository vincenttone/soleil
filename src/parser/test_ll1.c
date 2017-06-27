#include <stdio.h>
#include "sol_ll1.h"

enum _Sign {
    S = 1,
    E,
    Op,
    Lc,
    Rc,
    Plus,
    Mul,
    id,
    end,
    Limit,
};

int main()
{
    enum _Sign c[Limit-1] = {S, E, Op, Lc, Rc, Plus, Mul, id, end};
    SolLL1Parser *p = solLL1Parser_new();
    SolLL1ParserSign *sS = solLL1Parser_reg_nonterminal(p, &c[0]);
    SolLL1ParserSign *sE = solLL1Parser_reg_nonterminal(p, &c[1]);
    SolLL1ParserSign *sOp = solLL1Parser_reg_nonterminal(p, &c[2]);
    SolLL1ParserSign *sLc = solLL1Parser_reg_nonterminal(p, &c[3]);
    SolLL1ParserSign *sRc = solLL1Parser_reg_nonterminal(p, &c[4]);
    SolLL1ParserSign *sPlus = solLL1Parser_reg_nonterminal(p, &c[5]);
    SolLL1ParserSign *sMul = solLL1Parser_reg_nonterminal(p, &c[6]);
    SolLL1ParserSign *sid = solLL1Parser_reg_terminal(p, &c[7]);
    SolLL1ParserSign *send = solLL1Parser_reg_terminal(p, &c[8]);
    
    SolLL1ParserForm *f1 = solLL1ParserForm_new();
    solLL1ParserForm_add_sign(f1, sS);
    solLL1ParserForm_add_sign(f1, sE);
    solLL1ParserForm_add_sign(f1, send);
    
    SolLL1ParserForm *f2 = solLL1ParserForm_new();
        solLL1ParserForm_add_sign(f2, sE);
    solLL1ParserForm_add_sign(f2, sid);

    SolLL1ParserForm *f3 = solLL1ParserForm_new();
    solLL1ParserForm_add_sign(f3, sE);
    solLL1ParserForm_add_sign(f3, sLc);
    solLL1ParserForm_add_sign(f3, sE);
    solLL1ParserForm_add_sign(f3, sOp);
    solLL1ParserForm_add_sign(f3, sE);
    solLL1ParserForm_add_sign(f3, sRc);

    SolLL1ParserForm *f4 = solLL1ParserForm_new();
    solLL1ParserForm_add_sign(f4, sOp);
    solLL1ParserForm_add_sign(f4, sPlus);

    SolLL1ParserForm *f5 = solLL1ParserForm_new();
    solLL1ParserForm_add_sign(f5, sOp);
    solLL1ParserForm_add_sign(f5, sMul);

    solLL1Parser_reg_form(p, f1);
    solLL1Parser_reg_form(p, f2);
    solLL1Parser_reg_form(p, f3);
    solLL1Parser_reg_form(p, f4);
    solLL1Parser_reg_form(p, f5);
    solLL1Parser_free(p);

    return 0;
}
