#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXWORD			(100)
#define MAXSTACK		(100)	/* maximum depth of val stack */

size_t read(int fd, void *buf, size_t count);

char buff[MAXSTACK];			/* buffer for ungetch */
int  bufp = 0;					/* next free position in buff */

struct tnode {					/* the tree node */
	char* word;					/* points to the text */
	int count;					/* number of occurrences */
	struct tnode* left;			/* left child */
	struct tnode* right;		/* right child */
};

/* getchar: simple buffered version */
int _getchar()
{
	static char buf[BUFSIZ];
	static char* bufp = buf;
	static int n = 0;

	if (n == 0) {		/* buffer is empty */
		n = read(0, buf, sizeof(buf));
		bufp = buf;
	}
	return (--n >= 0) && (*bufp != '@') ? (unsigned char)*bufp++ : EOF;
}

/* getch: get a (possibly pushed back) character */
int _getch()
{
	return (bufp > 0) ? buff[--bufp] : _getchar();
}

/* ungetch: push character back on input */
void _ungetch(int c)
{
	if (bufp >= MAXSTACK)
		printf("ungetch: too many characters\n");
	else
		buff[bufp++] = c;
}

/* getword: get next word or character form input */
int getword(char* word, int lim)
{
	int c;
	char* w = word;

	while (isspace(c = _getch()))
		;
	if (c != EOF) {
		*w++ = c;
	}
	if (!isalpha(c)) {
		*w = '\0';
		return c;
	}
	for (; --lim > 0; w++) {
		if (!isalnum(*w = _getch())) {
			_ungetch(*w);
			break;
		}
	}
	*w = '\0';
	return word[0];
}

/* talloc: make a tnode */
struct tnode* talloc(void)
{
	return (struct tnode*)malloc(sizeof(struct tnode));
}

/* salloc: make a duplicate of s */
char* salloc(char* s)
{
	char* p;

	p = (char*)malloc(strlen(s) + 1);	/* +1 fpr '\0' */
	if (p != NULL)
		strcpy(p, s);
	return p;
}

/* addtree: add a node with w, at or below p */
struct tnode* addtree(struct tnode* p, char* w)
{
	int cond;

	if (p == NULL) {		/* a new word has arrived */
		p = talloc();		/* make a new node */
		p->word = salloc(w);
		p->count = 1;
		p->left = p->right = NULL;
	} else if ((cond = strcmp(w, p->word)) == 0)
		p->count++;			/* repeated word */
	else if (cond < 0)		/* less than into left subtree */
		p->left = addtree(p->left, w);
	else
		p->right = addtree(p->right, w);
	return p;
}

/* treeprint: in-order print of tree p */
void treeprint(struct tnode* p)
{
	if (p != NULL) {
		treeprint(p->left);
		printf("%4d %s\n", p->count, p->word);
		treeprint(p->right);
	}
}

/* word frequency count */
int main()
{
	struct tnode* root;
	char word[MAXWORD];

	root = NULL;
	printf("[CTRL+Z|@][ENTER]\n");
	while (getword(word, MAXWORD) != EOF)
		if (isalpha(word[0]))
			root = addtree(root, word);

	treeprint(root);
	return 0;
}
