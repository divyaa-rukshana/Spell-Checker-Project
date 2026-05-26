#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "spellchecker_proj.h"
// Hash function
unsigned int hashFunction(const char* word) {
    unsigned int hash = 0;
    while (*word) {
        hash = (hash * 31 + tolower(*word)) % TABLE_SIZE;
        word++;
    }
    return hash;
}
// Insert a word into hash table
Node* insertWord(Node* table[], const char* word) {
    unsigned int index = hashFunction(word);
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (!newNode) {
        printf("Memory allocation failed for word: %s\n", word);
        return table[index];
    }
    strcpy(newNode->word, word);
    newNode->next = table[index];
    table[index] = newNode;
    return table[index];
}
void insertTrie(trienode *root, const char *word) {
    trienode *curr = root;
    for (int i = 0; word[i]; i++) {
        int idx = tolower(word[i]) - 'a';
        if (idx < 0 || idx >= ALPHABET_SIZE) continue;
        if (!curr->child[idx]) {
            curr->child[idx] = (trienode*)calloc(1, sizeof(trienode));
        }
        curr = curr->child[idx];
    }
    curr->endofword = 1;
}
// Remove a word from the hash table
int removeWord(Node *table[], const char *word) {
    unsigned int index = hashFunction(word);
    Node *curr = table[index];
    Node *prev = NULL;
    while (curr != NULL) {
        if (strcasecmp(curr->word, word) == 0) {
            if (prev == NULL) {
                table[index] = curr->next;
            } else {
                prev->next = curr->next;
            }
            free(curr);
            printf("\"%s\" removed from dictionary.\n", word);
            return 1; 
        }
        prev = curr;
        curr = curr->next;
    }
    printf("\"%s\" not found in dictionary.\n", word);
    return 0; 
}
// Removes a word from the trie
int removeTrie(trienode *root, const char *word) {
    trienode *curr = root;
    for (int i = 0; word[i]; i++) {
        int idx = tolower(word[i]) - 'a';
        if (idx < 0 || idx >= ALPHABET_SIZE || !curr->child[idx]) {
            return 0; 
        }
        curr = curr->child[idx];
    }
    if (curr->endofword) {
        curr->endofword = 0;
        return 1; 
    }
    return 0; 
}
// Load dictionary file into hash table
int loadDictionary(Node* table[], const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open %s\n", filename);
        return 0;
    }
    char word[WORD_LEN];
    int count = 0;
    while (fscanf(file, "%s", word) != EOF) {
        insertWord(table, word);
        count++;
    }
    fclose(file);
    printf("Loaded %d words from %s successfully.\n", count, filename);
    return count;
}
// Check if a word exists
int checkWord(Node* table[], const char* word) {
    unsigned int index = hashFunction(word);
    Node* curr = table[index];
    while (curr != NULL) {
        if (strcasecmp(curr->word, word) == 0)
            return 1;
        curr = curr->next;
    }
    return 0;
}
// Free all allocated memory
void freeTable(Node* table[]) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        Node* curr = table[i];
        while (curr != NULL) {
            Node* temp = curr;
            curr = curr->next;
            free(temp);
        }
        table[i] = NULL;
    }
}

void freeTrie(trienode* root) {
    if (root == NULL)
        return;

    for (int i = 0; i < 26; i++) {
        freeTrie(root->child[i]);
    }

    free(root); // free this node
}


void suggest(trienode *root, const char *prefix)
{
    printf("Suggestions for '%s': \n",prefix);

    trienode *curr = root;
    for(int i=0; prefix[i]; i++)
    {
        int j = tolower(prefix[i]) - 'a';
        if(!curr->child[j])
        {
            printf("There are no suggestions for '%s'\n",prefix);
            return;
        }
        curr = curr->child[j];
    }
    char buffer[100];
    strcpy(buffer, prefix);
    help_suggest(curr,buffer,strlen(prefix));
}
static void help_suggest(trienode *p, char *buffer, int depth)
{
    if(p->endofword)
    {
        buffer[depth] = '\0';
        printf("--> %s\n",buffer);
    }
    for(int i=0;i<ALPHABET_SIZE;i++)
    {
        if(p->child[i])
        {
            buffer[depth] = 'a' + i;
            help_suggest(p->child[i],buffer, depth+1);
        }
    }
}
void toLowerCase(char *str)
{
    for (int i = 0; str[i]; i++)
        str[i] = tolower((unsigned char)str[i]);
}

// Suggest by Letters (Any Order)

int containsAllLetters(const char *word, const char *letters) {
    int freqWord[26] = {0};
    int freqInput[26] = {0};

    // Count frequency of letters in both word and input
    for (int i = 0; word[i]; i++) {
        if (isalpha((unsigned char)word[i]))
            freqWord[tolower(word[i]) - 'a']++;
    }
    for (int i = 0; letters[i]; i++) {
        if (isalpha((unsigned char)letters[i]))
            freqInput[tolower(letters[i]) - 'a']++;
    }

    // Check if all input letters appear in the word
    for (int i = 0; i < 26; i++) {
        if (freqInput[i] > 0 && freqWord[i] == 0)
            return 0;
    }
    return 1;
}

void suggestByLetters(Node *table[], const char *letters) {
    printf("Words containing all letters of '%s':\n", letters);
    int found = 0;

    for (int i = 0; i < TABLE_SIZE; i++) {
        Node *curr = table[i];
        while (curr != NULL) {
            if (containsAllLetters(curr->word, letters)) {
                printf("--> %s\n", curr->word);
                found = 1;
            }
            curr = curr->next;
        }
    }

    if (!found)
        printf("No words found containing all letters of '%s'\n", letters);
}

// Print all words in the trie in reverse alphabetical order
void printDescending(trienode *root)
{
    char buffer[100];
    printf("\nDictionary (Descending Order):\n");
    printDescendingHelper(root, buffer, 0);
}
static void printDescendingHelper(trienode *node, char *buffer, int depth)
{
    if (node == NULL)
        return;
    
    if (node->endofword)
    {
        buffer[depth] = '\0';
        printf("%s\n", buffer);
    }
   
    for (int i = ALPHABET_SIZE - 1; i >= 0; i--)
    {
        if (node->child[i])
        {
            buffer[depth] = 'a' + i;
            printDescendingHelper(node->child[i], buffer, depth + 1);
        }
    }
}

void printLargestWord(Node *table[]) {
    char largestWord[WORD_LEN] = "";
    int maxLen = 0;

    for (int i = 0; i < TABLE_SIZE; i++) {
        Node *curr = table[i];
        while (curr != NULL) {
            int len = strlen(curr->word);
            if (len > maxLen) {
                maxLen = len;
                strcpy(largestWord, curr->word);
            }
            curr = curr->next;
        }
    }
    if (maxLen > 0)
        printf("Largest word in dictionary: %s\n", largestWord);
    else
        printf("No words in dictionary.\n");
}