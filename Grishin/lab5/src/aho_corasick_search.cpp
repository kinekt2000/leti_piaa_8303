#include "aho_corasick_search.h"
#include "trie_node.h"

#include <iostream>
#include <queue>
#include <algorithm>


ACSearch::ACSearch(bool log_on):
    root(nullptr), size(0), logger(log_on)
{
    if(log_on) log.open("log", std::ofstream::out | std::ofstream::trunc);
}


ACSearch::ACSearch():
    root(nullptr), size(0), logger(1)
{
    log.open("log", std::ofstream::out | std::ofstream::trunc);
}


void ACSearch::calcSuffixLink(TrieNode* node)
{
    if(logger)
    {
        log << "suffix-link calculator initiated for node: " << node->id() << std::endl;
    }

    // suffix link of root is root itself
    if(node == root)
    {
        if(logger) log << "suffix[root] = root" << std::endl;

        suffix[node] = root;
        return;
    }

    // suffix links of root children are root
    if(node->parent == root)
    {
        if(logger) log << "this is a child of root. suffix = root" << std::endl;
        suffix[node] =  root;
        return;
    }

    // for computate the suffix link
    // we need the suffix link of node parent
    TrieNode *pCurrBetterNode;
    try
    {
        pCurrBetterNode = suffix.at(node->parent);
    }
    catch (std::out_of_range)
    {
        // trying to calculate prefix, before parent
        throw std::out_of_range("can't take first parent suffix, "
                                "check <calcSuffixLink>, "
                                "it's actually broken");
    }

    // and the character, which that moves us to node
    char cParentChar = node->toParent;

    if(logger) log << "parent suffix " << pCurrBetterNode->id();    
    if(logger) log << "jump through suffix link" << std::endl;
    while(true)
    {
        // try to find the children of parent suffix
        // with the same key as to current node
        if(pCurrBetterNode->children.find(cParentChar) != pCurrBetterNode->children.end())
        {
            if(logger) log << "child found: " << pCurrBetterNode->children[cParentChar]->id()
                           << " id. Set suffix link on it" << std::endl;
            suffix[node] = pCurrBetterNode->children[cParentChar];
            break;
        }

        // otherwise jump through suffix links
        // until reach root
        if(pCurrBetterNode == root)
        {
            if(logger) log << "suffix path led to root, suffix link set on root" << std::endl;
            suffix[node] = root;
            break;
        }

        try
        {
            pCurrBetterNode = suffix.at(pCurrBetterNode);
            if(logger) log << "Next suffix. Current node is "
                           << pCurrBetterNode->id() << std::endl;
        }
        catch(std::out_of_range)
        {
            // trying to calculate prefix of node, which doesn't
            // have full prefix path
            throw std::out_of_range("can't take next parent suffix, "
                                    "check <calcSuffixLink>, "
                                    "it's actually broken");
        }
    }
}


bool ACSearch::empty() const
{
    return !root;
}


void ACSearch::insert(const std::string& key)
{
    if(logger) log << "add key: " << key << std::endl;
    if(empty()){
        if(logger) log << "trie is empty, root created" << std::endl; 
        root = new TrieNode(0);
        size = 1;
    }

    auto pCrawl = root;
    for(auto c: key)
    {
        if(pCrawl->children.count(c) == 0)
        {
            if(logger) log << "create children with character: " << c << std::endl;
            pCrawl->children[c] = new TrieNode(size, pCrawl, c);
            size++;
        }
        else
        {
            if(logger) log << "already have a child with that character, go deeper" << std::endl;
        }
        pCrawl = pCrawl->children[c];
    }
    pCrawl->bEnd = 1;
    pCrawl->_str = key;

    if(logger) log << std::endl;
}


// works bad with nodes IDs
// there is no log outputs, because i don't use this function
bool ACSearch::remove(const std::string &key)
{
    if(empty()) return 0;
    TrieNode *pTail = root;

    for(auto c: key)
    {
        if(pTail->children.count(c))
        {
            pTail = pTail->children[c];
        }
        else
        {
            return 0;
        }
    }

    while(pTail)
    {
        auto pParent = pTail->parent;
        if(pTail->children.size() == 1)
        {
            if(pParent)
                pParent->children.erase(pTail->toParent);
            delete pTail;
            pTail = pParent;
        }
        else return 1;
    }
    root = nullptr;
    return 1;
}


// there is no log outputs, because i don't use this function
bool ACSearch::isIn(const std::string& key) const
{
    if(empty()) return 0;
    auto pCrawl = root;
    for(const auto c: key)
    {
        if(pCrawl->children.count(c) == 0)
        {
            return false;
        }

        try
        {
            pCrawl = pCrawl->children.at(c);
        }
        catch(std::out_of_range)
        {
            // what the hell, it's not possible
            throw std::out_of_range("can't accsess element, but the key "
                                    "is in, check <isIn> function, "
                                    "it's actually broken");
        }
    }
    return(pCrawl->bEnd);
}




// returns empty vector, if trie is empty
// there is no log outputs, because it's just a BFS
std::vector<TrieNode*> ACSearch::getAllNodes()
{
    std::vector<TrieNode*> nodes;
    if(root == nullptr)
        return nodes;

    // find all graph nodes using breadth first search
    std::queue<TrieNode*> q;
    q.push(root);

    while(!q.empty())
    {
        auto pCurrNode = q.front();
        q.pop();
        nodes.push_back(pCurrNode);

        for(auto child: pCurrNode->children)
        {
            // it is a tree, there can't be cycles,
            // so, there is no need to mark nodes
            q.push(child.second);
        }
    }
    return nodes;
}


void ACSearch::turnIntoMachine()
{
    if(logger) log << "Machinizer initiated" << std::endl;
    if(!root) return;

    suffix.clear();

    // find suffix links using breadth first search
    // which needs queue
    std::queue<TrieNode*> q;

    q.push(root);
    while(!q.empty())
    {
        auto pCurrNode = q.front();
        q.pop();

        if(logger) log << "built suffix link for " << pCurrNode->id() << " node. " << std::endl;
        calcSuffixLink(pCurrNode);
        if(logger) log << "Suffix link leads to " << suffix.at(pCurrNode)->id() << " node" 
                       << std::endl << std::endl;

        for(auto child: pCurrNode->children){
            q.push(child.second);
        }
    }
}

bool ACSearch::isMachine() const
{
    return(!suffix.empty());
}

void ACSearch::print()
{
    auto nodes = getAllNodes();
    for(auto node: nodes)
    {
        std::cout << node->parent << " " << node << " " 
                  << node->toParent << " " << suffix[node] << std::endl;
    }
}


void ACSearch::print_longest_links()
{
    auto nodes = getAllNodes();

    std::vector<TrieNode*> max_suffix_path;
    std::vector<TrieNode*> max_dictionary_path;

    // check all nodes to find which have longest path
    for(auto node: nodes)
    {
        auto pSuffixCrawl = node;
        std::vector<TrieNode*> current_suffix_path;
        std::vector<TrieNode*> current_dictionary_path;

        while(pSuffixCrawl != root)
        {
            current_suffix_path.push_back(pSuffixCrawl);
            if(pSuffixCrawl->end()) current_dictionary_path.push_back(pSuffixCrawl);            
            
            TrieNode* pNextNode;
            try
            {
                 pNextNode = suffix.at(pSuffixCrawl);
            }
            catch(std::out_of_range)
            {
                throw std::out_of_range("Can't access suffix link of vertex, "
                                        "error caused by <print_longest_links> function. "
                                        "Probably, the trie wasn't machinized");
            }

            if(pNextNode->end() && current_dictionary_path.empty())
            {
                current_dictionary_path.push_back(pSuffixCrawl);
            }
            pSuffixCrawl = pNextNode;
        }

        if(current_suffix_path.size() > max_suffix_path.size())
            max_suffix_path = current_suffix_path;
        if(current_dictionary_path.size() > max_dictionary_path.size())
            max_dictionary_path = current_dictionary_path;
    }

    std::cout << "longest suffix-link path size is " << max_suffix_path.size() << std::endl;
    std::cout << "first suffix-link path with this size: " << std::endl;
    for(auto node: max_suffix_path)
    {
        std::cout << node->id() << " -";
        if(node->end()) std::cout << "[" << node->getString() << "]";
        else
        {
            std::cout << "(";
            auto pTrieCrawl = node;
            while(pTrieCrawl != root)
            {
                std::cout << pTrieCrawl->toParent;
                pTrieCrawl = pTrieCrawl->parent;
            }
            std::cout << ")";
        }
        std::cout << "- ";
    }
    std::cout << root->id() << std::endl;

    if(max_dictionary_path.empty())
    {
        std::cout << "there is no dictionary links" << std::endl;
        return;
    }

    std::cout << "longest dictionary-link path size is " << max_dictionary_path.size() << std::endl;
    std::cout << "first dictionary-link path with this size: " << std::endl;
    for(auto node: max_dictionary_path)
    {
        std::cout << node->id() << ": ";
        if(node->end()) std::cout << node->getString() << std::endl;
        else std::cout << "\\\\not an end" << std::endl;
    }
    
}

std::map<size_t, std::vector<std::string>> 
ACSearch::search(const std::string& text)
{
    std::map<size_t, std::vector<std::string>> map_pos;
    if(!root) return map_pos;

    if(logger) log << "search initiated. Initial state is root" << std::endl;

    auto pCurrentState = root;

    for(auto it = text.begin(); it != text.end(); it++)
    {
        char c = *it;
        // calculate new state
        if(logger) log << "~~~calculate new state" << std::endl;
        while(true)
        {
            // if state has child with edge 'c', go in
            if(pCurrentState->children.find(c) != pCurrentState->children.end())
            { 
                try
                {
                    pCurrentState = pCurrentState->children.at(c);
                }
                catch(std::out_of_range)
                {
                    // find failed
                    throw std::out_of_range("Wierd thing in <search>, when trying go to the child");
                }
                
                if(logger) log << "current state has child with \'" << c << "\' character. "
                               << "Its id is " << pCurrentState->id() << std::endl;
                break;
            }

            // otherwise we gonna go deeper to the root using suffix
            if(pCurrentState == root) break;

            try
            {
                pCurrentState = suffix.at(pCurrentState);
            }
            catch(std::out_of_range)
            {
                throw std::out_of_range("Can't access suffix link of vertex, "
                                        "check <search> function, "
                                        "it's actually broken");
            }

            if(logger) log << "There is no child with \'" << c << "\' character. "
                           << "Set state as suffix link. New state is " << pCurrentState->id() << std::endl;
        }
        if(logger) log << "~~~new state is " << pCurrentState->id() << std::endl << std::endl;
        

        auto pSuffixCrawl = pCurrentState;
        // check all suffixes of current state

        if(pCurrentState != root && logger)
            log << "new state is not root, we have to check node and its suffix path\n"
                << "there could be ends of patterns" << std::endl;          
        while(pSuffixCrawl != root)
        {
            // when match
            if(pSuffixCrawl->end())
            {
                if(logger) log << "state " << pSuffixCrawl->id() << " is end of pattern. "
                               << "pattern found on position " 
                               << (it - text.begin()) + 1 - pSuffixCrawl->getString().size() << std::endl;
                size_t index = (it - text.begin()) + 1 - pSuffixCrawl->getString().size();
                map_pos[index].push_back(pSuffixCrawl->getString());
            }

            try
            {
                pSuffixCrawl = suffix.at(pSuffixCrawl);
            }
            catch(std::out_of_range)
            {
                throw std::out_of_range("Can't access suffix link of vertex, "
                                        "check <search> function, "
                                        "it's actually broken");
            }
            if(logger) log << "go deeper on the suffix link. Now, state to check is "
                           << pSuffixCrawl->id() << std::endl;
            
        }
        if(logger) log << std::endl;
    }
    return map_pos;
}

