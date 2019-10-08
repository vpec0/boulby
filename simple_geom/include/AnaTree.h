#ifndef ANATREE_H
#define ANATREE_H 1

#include "TTree.h"

const int MAX_NEUTRONS = 200;


#define VAR_LIST INT(n_neutrons);\
    DOUBLE_ARR(n_energy,n_neutrons)\


#define INT(var) Int_t var
#define DOUBLE_ARR(var, size) Double_t var[MAX_NEUTRONS]
struct Event_t {
    VAR_LIST;
};
#undef INT
#undef DOUBLE_ARR


#ifdef ANATREE_CXX
#define INT(var) tree->Branch(#var, &event.var, #var"/I");
#define DOUBLE_ARR(var, size) tree->Branch(#var, event.var, #var"["#size"]/D");

TTree* createTree(const char* tree_name, Event_t& event) {
    TTree* tree = new TTree(tree_name, tree_name);

    VAR_LIST;

    return tree;
}

#undef INT
#undef DOUBLE_ARR
#endif // ANATREE_CXX

#endif // ANATREE_H
