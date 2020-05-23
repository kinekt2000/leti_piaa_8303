#include "trie_node.h"
#include "wildcard_search.h"

#include <iostream>
#include <queue>
#include <algorithm>


WildcardSearch::WildcardSearch(bool log_on):
    root(nullptr), logger(log_on)
{
    if(logger) log.open("log", std::ofstream::out | std::ofstream::trunc);
}


WildcardSearch::WildcardSearch():
    root(nullptr), logger(1)
{
    log.open("log", std::ofstream::out | std::ofstream::trunc);
}


WildcardSearch::~WildcardSearch()
{
    delete root;
    log.close();
}


void WildcardSearch::calcSuffixLink(TrieNode* node)
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


void WildcardSearch::turnIntoMachine()
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


void WildcardSearch::insert(const std::string& key)
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


bool WildcardSearch::empty() const
{
    return !root;
}


void WildcardSearch::setPattern(const std::string& key, char joker)
{
    subP_map.clear();
    subP_count = 0;
    pattern_size = key.size();

    std::string sCurrP; // current pattern

    for(auto it = key.begin(); it != key.end(); it++)
    {
        char c = *it;
        size_t index = it - key.begin();
        if(c != joker)
        {
            sCurrP.push_back(c);
            if(it == key.end()-1)
            {
                // save position of this substring
                size_t sub_index = index + 2;
                sub_index -= sCurrP.size();
                subP_map[sCurrP].push_back(sub_index);
                subP_count++;
            }
        }
        else
        {
            if(!sCurrP.empty())
            {
                // we are on the position beyond string 
                // therefore we don't need to add one
                size_t sub_index = index + 1;
                sub_index -= sCurrP.size();
                subP_map[sCurrP].push_back(sub_index);
                subP_count++;
                sCurrP.clear();
            }
        }
    }

    // put all substrings into aho-corasick machine
    for(auto& item: subP_map)
    {
        insert(item.first);
    }

    // add suffix links into trie
    turnIntoMachine();
}


std::vector<TrieNode*> WildcardSearch::getAllNodes()
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


void WildcardSearch::print()
{
    auto nodes = getAllNodes();
    for(auto node: nodes){
        std::cout << node->parent << " " << node << " " 
                  << node->toParent << " " << suffix[node] << std::endl;
    }
}


std::vector<size_t> WildcardSearch::search(const std::string& text)
{
    std::vector<size_t> pattern_entries;
    if(!root) return pattern_entries;

    if(logger) log << "search initiated. Initial state is root" << std::endl;

    // subP_entries[i] - how many substrings of pattern
    // macthed on position i
    std::map<size_t, size_t> subP_entries;

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
                size_t index = (it - text.begin()) + 1 - pSuffixCrawl->getString().size();

                if(logger) log << "state " << pSuffixCrawl->id() << " is end of pattern. "
                               << "pattern found on position " << index << std::endl; 

                index++;

                if(logger) log << "supposed full pattern positions: ";
                bool nowhere = 1;
                for(auto l: subP_map[pSuffixCrawl->getString()])
                {
                    if(l <= index)
                    {
                        nowhere = 0;
                        subP_entries[index - l]++;
                        if(logger) log << index - l << " ";
                    }
                }
                if(logger && nowhere) log << "nowhere";
                if(logger) log << std::endl;
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
    }

    for(auto p: subP_entries)
    {
        if(p.second == subP_count)
        {
            if(p.first + pattern_size <= text.size())
                pattern_entries.push_back(p.first);
        }
    }
    return pattern_entries;
}









