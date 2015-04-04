#include "RegexTree.h"

int main() {
	RegexTree tree;

	tree.build("(a|b)*abb");

	tree.getRoot()->show();
	puts("");

	return 0;
}
