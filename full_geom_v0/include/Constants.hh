#ifndef CONSTANTS_HH
#define CONSTANTS_HH

enum EDepositionClass {
    kXe = 0,
    kEm,
    kMu,
    kOther,
    kNDepositionClasses,
};


const char* DepositionClassNames[] = {
    "Xe",
    "Em",
    "Mu",
    "Other"
};
#endif
