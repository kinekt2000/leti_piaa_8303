#include <map>
#include <string>

class TrieNode
{
    friend class ACSearch;
    friend class WildcardSearch; 

    size_t _id;
    std::map<char, TrieNode*> children;
    TrieNode *parent;
    char toParent;
    bool bEnd;
    std::string _str;
    
    TrieNode(size_t id, TrieNode* parent = nullptr, char toParent = 0):
        _id(id), parent(parent), toParent(toParent), bEnd(0)
    {};

public:
    bool end() const
    {
        return bEnd;
    }

    const std::string& getString() const
    {
        return _str;
    }

    size_t id() const
    {
        return _id;
    }
};
