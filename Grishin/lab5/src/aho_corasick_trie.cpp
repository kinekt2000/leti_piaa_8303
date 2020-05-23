#include "aho_corasick_search.h"
#include "trie_node.h"

#include <iostream>
#include <queue>
#include <algorithm>


ACSearch::ACSearch():
    root(nullptr)
{}


void ACSearch::calcSuffixLink(TrieNode* node)
{

    // suffix link of root is root itself
    if(node == root)
    {
        suffix[node] = root;
        return;
    }

    // suffix links of root children are root
    if(node->parent == root)
    {
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

    
    while(true)
    {
        // try to find the children of parent suffix
        // with the same key as to current node
        if(pCurrBetterNode->children.find(cParentChar) != pCurrBetterNode->children.end())
        {
            suffix[node] = pCurrBetterNode->children[cParentChar];
            break;
        }

        // otherwise jump through suffix links
        // until reach root
        if(pCurrBetterNode == root)
        {
            suffix[node] = root;
            break;
        }

        try
        {
            pCurrBetterNode = suffix.at(pCurrBetterNode);
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
    if(empty())
        root = new TrieNode();

    auto pCrawl = root;
    for(auto c: key)
    {
        if(pCrawl->children.count(c) == 0)
        {
            pCrawl->children[c] = new TrieNode(pCrawl, c);
        }
        pCrawl = pCrawl->children[c];
    }
    pCrawl->bEnd = 1;
    pCrawl->_str = key;
}

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
    if(!root) return;

    // find suffix links using breadth first search
    // which needs queue
    std::queue<TrieNode*> q;

    q.push(root);
    while(!q.empty())
    {
        auto pCurrNode = q.front();
        q.pop();

        calcSuffixLink(pCurrNode);

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
    for(auto node: nodes){
        std::cout << node->parent << " " << node << " " 
                  << node->toParent << " " << suffix[node] << std::endl;
    }
}

std::map<size_t, std::vector<std::string>> 
ACSearch::search(const std::string& text)
{
    std::map<size_t, std::vector<std::string>> map_pos;
    if(!root) return map_pos;

    auto pCurrentState = root;

    for(auto it = text.begin(); it != text.end(); it++)
    {
        char c = *it;
        // calculate new state
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
        }
        

        auto pSuffixCrawl = pCurrentState;
        // check all suffixes of current state             
        while(pSuffixCrawl != root)
        {
            // when match
            if(pSuffixCrawl->end())
            {
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
        }
    }
    return map_pos;
}

