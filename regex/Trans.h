#ifndef TRANS_H
#define TRANS_H

#include "DFA.h"
#include "RegexTree.h"

class Trans{
	private:
		Trans(){}

	public:
	static void createDFAByTree(const RegexTree &tree, DFA &dfa_OUT);
};

#endif
