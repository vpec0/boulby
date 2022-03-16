namespace GenMuonEvent {

#define VAR_LIST				\
    INT(entry);					\
    INT(pdg);					\
    DOUBLE(energy);				\
    DOUBLE(posX);				\
    DOUBLE(posY);				\
    DOUBLE(posZ);				\
    DOUBLE(cosX);				\
    DOUBLE(cosY);				\
    DOUBLE(cosZ);				\


#define INT(var) Int_t var
#define DOUBLE(var) Double_t var

    struct Event_t {
	VAR_LIST;
    };

#undef INT
#undef DOUBLE




#define INT(var) tree->Branch(#var, &event.var, #var"/I")
#define DOUBLE(var) tree->Branch(#var, &event.var, #var"/D")

    TTree* createTree(const char* tree_name, Event_t& event) {
	TTree* tree = new TTree(tree_name, tree_name);

	VAR_LIST;

	return tree;
    }



#undef INT
#undef DOUBLE

#define INT(var) tree->SetBranchAddress(#var, &event.var)
#define DOUBLE(var) tree->SetBranchAddress(#var, &event.var)

    TTree* registerTree(TTree* tree, Event_t& event) {

	VAR_LIST;

	return tree;
    }
}
