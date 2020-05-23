#include "wildcard_search.h"

#include <iostream>
#include <algorithm>


int main()
{
    std::string text;
    std::getline(std::cin, text);
    std::string pattern;
    std::cin >> pattern;
    char joker;
    std::cin >> joker;


    WildcardSearch w(1);
    w.setPattern(pattern, joker);
    auto pos = w.search(text);

    for(auto p: pos){
        std::cout << p << std::endl;
    }

    return 0;
}

























