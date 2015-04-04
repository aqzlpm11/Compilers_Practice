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
		if (r->type == NOTHING) {
			delete r;
			break;
		}

		Node *now = new Node(AND);
		now->nxt.push_back(l); 
		now->nxt.push_back(r); 
		l = now;
	}

	return l;
}

RegexTree::Node *RegexTree::B(){
	RegexTree::Node *l = C();
	if (l->type != NOTHING && peek == '*') {
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
	} else {
		l = new Node(NOTHING);
	}
	return l;
}

RegexTree::Node *RegexTree::exp() {
	RegexTree::Node *l = A();
	while (l && peek == '|') {
		match('|');

		auto r = A();
		if (r->type == NOTHING) {
			fprintf(stderr, "ERROR: except charactor, find NOTHING\n");
			syntaxError();
		}

		RegexTree::Node *now = new RegexTree::Node(OR);
		now->nxt.push_back(l);
		now->nxt.push_back(r);

		l = now;
	}
	return l;
}

void RegexTree::match(char ch) {
	if (peek != ch) {
		fprintf(stderr, "ERROR: expect %c, find '%c'\n", ch, peek);
		syntaxError();
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
	init(s);

	peekNext(true);

	delete root;
	root = exp();

	buffer = nullptr; // buffer s may invaild after build
}

void RegexTree::syntaxError() {
	isError = true;

	fprintf(stderr, "%s\n", buffer);
	for (int i = 0; i < bufHead-1; i++) {
		fprintf(stderr, " ");
	}
	fprintf(stderr, "^\n");
}

void RegexTree::init(const char *buf) {
	buffer = buf;
	bufHead = 0;
	isError = false;
}

