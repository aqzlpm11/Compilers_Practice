#include "RegexTree.h"
#include "Trans.h"
#include "DFA.h"

int main() {
	RegexTree tree;

	tree.build("(a|b)*abb");
	//tree.build("a*|b*");
	//tree.build("a");

	DFA a;
	Trans::createDFAByTree(tree, a);

	puts("");
	tree.show();
	puts("");
	a.show();


	char str[] = "abbabb";

	for (int haha = 0; str[haha]; haha++) {
		int ed = a.match(str+haha);
		for (int i = 0; i < ed; i++) {
			printf("%c", *(str+i+haha));
		}puts("");
	}

	return 0;
}
