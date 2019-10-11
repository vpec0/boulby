#ifndef ANATREE_H
#define ANATREE_H 1

#include "TTree.h"

const int MAX_NEUTRONS = 200;

struct Event_t;
TTree* createTree(const char* tree_name, Event_t& event);
void resetEvent(Event_t&);

#define VAR_LIST				\
    INT(runNo);					\
    INT(eventNo);				\
    INT(n_neutrons);				\
    DOUBLE_ARR(n_energy,n_neutrons);		\
    DOUBLE_ARR2(n_start_xyz, n_neutrons, 3);	\
    DOUBLE_ARR2(n_end_xyz, n_neutrons, 3)	\


#define INT(var) Int_t var
#define DOUBLE_ARR(var, size) Double_t var[MAX_NEUTRONS]
#define DOUBLE_ARR2(var, size1, size2) Double_t var[MAX_NEUTRONS][size2]
struct Event_t {
    VAR_LIST;
};
#undef INT
#undef DOUBLE_ARR




#ifdef ANATREE_CXX
#define INT(var) tree->Branch(#var, &event.var, #var"/I");
#define DOUBLE_ARR(var, size) tree->Branch(#var, event.var, #var"["#size"]/D");
#define DOUBLE_ARR2(var, size1, size2) tree->Branch(#var, event.var, #var"["#size1"]["#size2"]/D");

TTree* createTree(const char* tree_name, Event_t& event) {
    TTree* tree = new TTree(tree_name, tree_name);

    VAR_LIST;

    return tree;
}

#undef INT
#undef DOUBLE_ARR

void resetEvent(Event_t& evt) {
    evt = {};
}


#endif // ANATREE_CXX

#endif // ANATREE_H
