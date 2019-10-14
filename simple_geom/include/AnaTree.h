#ifndef ANATREE_H
#define ANATREE_H 1

#include <vector>
#include <string>

#include "TTree.h"

const int MAX_NEUTRONS = 400;

struct Event_t;
TTree* createTree(const char* tree_name, Event_t& event);
void resetEvent(Event_t&);

#define VAR_LIST				\
    INT(runNo);					\
    INT(eventNo);				\
    INT(n_neutrons);				\
    DOUBLE_ARR(n_energy,n_neutrons);		\
    DOUBLE_ARR2(start_xyz, n_neutrons, 3);	\
    DOUBLE_ARR2(end_xyz, n_neutrons, 3);	\
    STRING_ARR(process);			\
    INT_ARR(trackId,n_neutrons);		\
    INT_ARR(parentId,n_neutrons);		\
    INT_ARR(parentPdg,n_neutrons)		\


#define INT(var) Int_t var
#define STRING_ARR(var) std::vector<std::string> var
#define INT_ARR(var, size) Int_t var[MAX_NEUTRONS]
#define DOUBLE_ARR(var, size) Double_t var[MAX_NEUTRONS]
#define DOUBLE_ARR2(var, size1, size2) Double_t var[MAX_NEUTRONS][size2]
struct Event_t {
    VAR_LIST;
};
#undef INT
#undef STRING_ARR
#undef INT_ARR
#undef DOUBLE_ARR
#undef DOUBLE_ARR2




#ifdef ANATREE_CXX
#define INT(var) tree->Branch(#var, &event.var, #var"/I")
#define STRING_ARR(var) tree->Branch(#var, &event.var)
#define INT_ARR(var, size) tree->Branch(#var, &event.var, #var"["#size"]/I")
#define DOUBLE_ARR(var, size) tree->Branch(#var, event.var, #var"["#size"]/D")
#define DOUBLE_ARR2(var, size1, size2) tree->Branch(#var, event.var, #var"["#size1"]["#size2"]/D")

TTree* createTree(const char* tree_name, Event_t& event) {
    TTree* tree = new TTree(tree_name, tree_name);

    VAR_LIST;

    return tree;
}

#undef INT
#undef STRING_ARR
#undef INT_ARR
#undef DOUBLE_ARR
#undef DOUBLE_ARR2

void resetEvent(Event_t& evt) {
    evt = {};
}


#endif // ANATREE_CXX

#endif // ANATREE_H
