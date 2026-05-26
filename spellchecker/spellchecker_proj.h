#ifndef SPELLCHECKER_H
#define SPELLCHECKER_H

#define TABLE_SIZE 10007
#define WORD_LEN 50
#define ALPHABET_SIZE 26

// Structure for hash table node
typedef struct Node {
    char word[WORD_LEN];
    struct Node* next;
} Node;

typedef struct trienode {
    struct trienode *child[ALPHABET_SIZE];
    int endofword;
} trienode;

// Function declarations
unsigned int hashFunction(const char* word);
Node* insertWord(Node* table[], const char* word);
void insertTrie(trienode *root, const char *word);
int removeWord(Node *table[], const char *word);
int removeTrie(trienode *root, const char *word);
int loadDictionary(Node* table[], const char* filename);
int checkWord(Node* table[], const char* word);
void freeTable(Node* table[]);
void suggest(trienode *root, const char *prefix);
static void help_suggest(trienode *p, char *buffer, int depth);
void toLowerCase(char *str);
void freeTrie(trienode* root);
int containsAllLetters(const char *word, const char *letters);
void suggestByLetters(Node *table[], const char *letters);
void printLargestWord(Node *table[]);
static void printDescendingHelper(trienode *node, char *buffer, int depth);
void printDescending(trienode*root);

#endif
