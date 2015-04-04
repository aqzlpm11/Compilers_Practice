#include "RegexTree.h"

RegexTree::RegexTree(){}

RegexTree::~RegexTree(){
	delete root;
	root = nullptr;
}

RegexTree::Node *RegexTree::A() {
	Node *l = B();

	while (true) {
		Node *r = B();
		if (r == nullptr) break; 

		Node *now = new Node(AND);
		now->nxt.push_back(l);
		now->nxt.push_back(r);

		l = now;
	}

	return l;
}

RegexTree::Node *RegexTree::B(){
	RegexTree::Node *l = C();
	if (l != nullptr && peek == '*') {
		match('*');

		RegexTree::Node *now = new Node(STAR);
		now->nxt.push_back(l);

		l = now;
	}
	return l;
}

RegexTree::Node *RegexTree::C(){
	RegexTree::Node *l = nullptr;
	if (peek == '(') {
		match('(');
		l = exp();
		match(')');
	} else if (isalnum(peek)) {
		l = new RegexTree::Node(peek);

		match(peek);
	}

	return l;
}

RegexTree::Node *RegexTree::exp() {
	RegexTree::Node *l = A();
	while (l && peek == '|') {
		match('|');

		RegexTree::Node *now = new RegexTree::Node(OR);
		now->nxt.push_back(l);
		now->nxt.push_back(A());

		l = now;
	}
	return l;
}

void RegexTree::match(char ch) {
	if (peek != ch) {
		printf("expect %c, find '%c'\n", ch, peek);
		return; 
	} 
	peekNext(); 
}

void RegexTree::peekNext(bool skipSpace /*= false*/) {
	peek = buffer[bufHead++];

	if (skipSpace) {
		while (peek && isspace(peek)) {
			peek = buffer[++bufHead];
		}
	}

	if (peek == '\0') {
		bufHead--;
	}
}

void RegexTree::build(const char *s) {
	buffer = s;
	bufHead = 0;

	peekNext(true);

	delete root;
	root = exp();

	buffer = nullptr; // buffer s may invaild after build
}

