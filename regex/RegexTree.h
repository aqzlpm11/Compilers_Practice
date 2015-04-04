#ifndef REGEX_TREE
#define REGEX_TREE

#include <cctype>
#include <cstdio>
#include <cstring>
#include <vector>

class RegexTree{
	public:
	friend class Trans;

	RegexTree();
	~RegexTree();

	enum {
		NOTHING = 0,
		STAR = 256,
		AND,
		OR
	};

	struct Node{
		int type;
		std::vector<Node *> nxt; 

		// for DFA
		bool nullable = false;
		std::vector<int> firstpos;
		std::vector<int> lastpos;


		Node(int tp):type(tp){}
		~Node() {
			for (int i = 0; i < nxt.size(); i++) {
				delete nxt[i];
			}
		}

		void show() const {
			if (0 < type  && type < 256) { 
				printf("%c", type); 
			} else if (type == STAR) {
				printf("(");
				nxt[0]->show();
				printf(")*");
			} else if (type == AND) {
				printf("(");
				nxt[0]->show();
				nxt[1]->show();
				printf(")");
			} else if (type == OR) {
				printf("(");
				nxt[0]->show();
				printf("|");
				nxt[1]->show();
				printf(")");
			} else {
				puts("\nShow ERROR");
			}
		}

	};


	void build(const char *str);

	Node const* getRoot() const { if (root == nullptr) fprintf(stderr, "Warming: RegexTree::root == nullptr\n");
		return root; 
	}

	void show() const {
		if (isError) puts("Can't Show RegexTree: Syntax ERROR");
		else root->show();
	}

	private:
	inline Node *exp(); // exp ->  A | A | A ...
	inline Node *A();   // A   ->  BBBB ...
	inline Node *B();   // B   ->  C* [or]  C
	inline Node *C();   // C   ->  (exp)  [or]  {letter}  [or]  {number}  [or]  {null}

	void match(char ch);
	void peekNext(bool skipSpace = false) ;

	void syntaxError();

	void init(const char *buf);

	char peek = ' ';
	Node *root = nullptr;

	bool isError = false;

	// buffer of the regex
	const char *buffer = nullptr;
	int bufHead = 0;

};

#endif
