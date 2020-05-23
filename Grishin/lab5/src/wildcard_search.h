#ifndef WILDCARD_SEARCH
#define WILDCARD_SEARCH

#include <map>
#include <vector>
#include <string>
#include <fstream>

class TrieNode;

class WildcardSearch
{
    // default aho-corasick part
    TrieNode *root;
    size_t size;
    std::map<TrieNode*, TrieNode*> suffix;

    // wildcards addition
    // key - substring of pattern
    // value - vector of positions, where we can find this substring
    size_t pattern_size;
    std::map<std::string, std::vector<size_t>> subP_map;
    size_t subP_count;

    std::ofstream log;
    bool logger; 

    // encapasulated default aho-corasick funcitons
    void calcSuffixLink(TrieNode* node);
    void turnIntoMachine();
    void insert(const std::string& key);

public:
    WildcardSearch(bool log_on = 1);
    WildcardSearch();
    ~WildcardSearch();

    bool empty() const;

    void setPattern(const std::string& key, char joker);    

    // returns empty vector, if trie is empty
    std::vector<TrieNode*> getAllNodes();    

    void print();

    std::vector<size_t> search(const std::string& text);
};

#endif
