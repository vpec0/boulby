#ifndef ANATREE_H
#define ANATREE_H 1

#include <vector>
#include <string>

#include "TTree.h"

#include "Constants.hh"

namespace AnaTree {

    const int	MAX_NEUTRONS  = 8192;	// just hoping this will never be exceeded
    const int MAX_DEPOSITIONS = 256;	// maximum energy deposits, bunched in 100 ns, to be stored

    const int Ndetectors = 5;

    struct	Event_t;
    TTree*	createTree(const char* tree_name, Event_t& event);
    void	resetEvent(Event_t&);


#define GEN_LIST				\
    DOUBLE(mu_en);				\
    DOUBLE(mu_cosz);				\
    DOUBLE(mu_phi);				\
    DOUBLE(mu_startx);				\
    DOUBLE(mu_starty);				\
    DOUBLE(mu_startz);				\
    DOUBLE(mu_endx);				\
    DOUBLE(mu_endy);				\
    DOUBLE(mu_endz);				\
    DOUBLE(mu_len)				\


#define NEUTRON_LIST				\
    INT(runNo);					\
    INT(eventNo);				\
    INT(n_neutrons);				\
    INT(n_neutrons_total);    			\
    DOUBLE_ARR(n_energy,n_neutrons);		\
    DOUBLE_ARR2(start_xyz, n_neutrons, 3);	\
    DOUBLE_ARR2(end_xyz, n_neutrons, 3);	\
    STRING_ARR(process);			\
    INT_ARR(trackId,n_neutrons);		\
    INT_ARR(parentId,n_neutrons);		\
    INT_ARR(parentPdg,n_neutrons)		\

#define DEPOSITS_LIST				\
    INT(n_tpc);					\
    INT(n_rfr);					\
    INT(n_skin);				\
    INT(n_gdls);				\
    INT(n_wt);					\
						\
    DOUBLE_ARR(Tdep_tpc, n_tpc);		\
    DOUBLE_ARR(Tdep_rfr, n_rfr);		\
    DOUBLE_ARR(Tdep_skin, n_skin);		\
    DOUBLE_ARR(Tdep_gdls, n_gdls);		\
    DOUBLE_ARR(Tdep_wt, n_wt);			\
    					     	\
    DOUBLE_ARR2(Edep_tpc, n_tpc, kNDepositionClasses);		\
    DOUBLE_ARR2(Edep_rfr, n_rfr, kNDepositionClasses);		\
    DOUBLE_ARR2(Edep_skin, n_skin, kNDepositionClasses);	\
    DOUBLE_ARR2(Edep_gdls, n_gdls, kNDepositionClasses);	\
    DOUBLE_ARR2(Edep_wt, n_wt, kNDepositionClasses)		\





#define INT(var) Int_t var
#define DOUBLE(var) Double_t var
#define STRING_ARR(var) std::vector<std::string>* var = 0
#define INT_ARR(var, size) Int_t var[MAX_NEUTRONS]
#define DOUBLE_ARR(var, size) Double_t var[MAX_NEUTRONS]
#define DOUBLE_ARR2(var, size1, size2) Double_t var[MAX_NEUTRONS][size2]

    struct Event_t {
	GEN_LIST;

	NEUTRON_LIST;

#undef DOUBLE_ARR
#undef DOUBLE_ARR2
#define DOUBLE_ARR(var, size) Double_t var[MAX_DEPOSITIONS]
#define DOUBLE_ARR2(var, size1, size2) Double_t var[MAX_DEPOSITIONS][size2]

	DEPOSITS_LIST;
    };

#undef INT
#undef DOUBLE
#undef STRING_ARR
#undef INT_ARR
#undef DOUBLE_ARR
#undef DOUBLE_ARR2




#ifdef ANATREE_CXX
#define INT(var) tree->Branch(#var, &event.var, #var"/I")
#define DOUBLE(var) tree->Branch(#var, &event.var, #var"/D")
#define STRING_ARR(var) tree->Branch(#var, &event.var)
#define INT_ARR(var, size) tree->Branch(#var, event.var, #var"["#size"]/I")
#define DOUBLE_ARR(var, size) tree->Branch(#var, event.var, #var"["#size"]/D")
#define DOUBLE_ARR2(var, size1, size2) tree->Branch(#var, event.var, #var"["#size1"]["#size2"]/D")

    TTree* createTree(const char* tree_name, Event_t& event) {
	TTree* tree = new TTree(tree_name, tree_name);

	GEN_LIST;

	NEUTRON_LIST;

#undef DOUBLE_ARR2
#define DOUBLE_ARR2(var, size1, size2) tree->Branch(#var, event.var, Form(#var"["#size1"][%d]/D", size2))
	DEPOSITS_LIST;

	return tree;
    }

#undef INT
#undef DOUBLE
#undef STRING_ARR
#undef INT_ARR
#undef DOUBLE_ARR
#undef DOUBLE_ARR2

#define INT(var) tree->SetBranchAddress(#var, &event.var)
#define DOUBLE(var) tree->SetBranchAddress(#var, &event.var)
#define STRING_ARR(var) tree->SetBranchAddress(#var, &event.var)
#define INT_ARR(var, size) tree->SetBranchAddress(#var, event.var)
#define DOUBLE_ARR(var, size) tree->SetBranchAddress(#var, event.var)
#define DOUBLE_ARR2(var, size1, size2) tree->SetBranchAddress(#var, event.var)

    TTree* registerTree(TTree* tree, Event_t& event) {
	GEN_LIST;

	NEUTRON_LIST;

	DEPOSITS_LIST;

	return tree;
    }

#undef INT
#undef DOUBLE
#undef STRING_ARR
#undef INT_ARR
#undef DOUBLE_ARR
#undef DOUBLE_ARR2

    void resetEvent(Event_t& evt) {
	if (evt.process)
            delete evt.process;
        evt = {};
        evt.process = new std::vector<std::string>;
    }



#endif // ANATREE_CXX
} // namespace AnaTree
#endif // ANATREE_H
