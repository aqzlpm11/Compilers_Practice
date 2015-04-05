#include "RegexTree.h"
#include "Trans.h"
#include "DFA.h"

int main() {
	RegexTree tree;

	tree.build("(a|b)*abb");
	//tree.build("a*|b*");
	//tree.build("a|b");

	DFA a;
	Trans::createDFAByTree(tree, a);

	puts("");
	tree.show();

	puts("");

	return 0;
}
