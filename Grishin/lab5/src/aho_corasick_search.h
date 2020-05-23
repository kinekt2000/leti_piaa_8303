#ifndef AHO_CORASICK_TRIE
#define AHO_CORASICK_TRIE

#include <map>
#include <vector>
#include <string>
#include <fstream>

class TrieNode;

class ACSearch
{
    TrieNode *root;
    size_t size;
    std::map<TrieNode*, TrieNode*> suffix;

    std::ofstream log;
    bool logger;    
    
    void calcSuffixLink(TrieNode* node);

public:
    ACSearch(bool log_on = 1);
    ACSearch();

    bool empty() const;

    void insert(const std::string& key);

    bool remove(const std::string &key);

    bool isIn(const std::string& key) const;

    // returns empty vector, if trie is empty
    std::vector<TrieNode*> getAllNodes();

    // sets suffix links;
    void turnIntoMachine();

    // is suffix links map set
    bool isMachine() const;

    void print();

    void print_longest_links();

    std::map<size_t, std::vector<std::string>>
    search(const std::string& text);
};

#endif

