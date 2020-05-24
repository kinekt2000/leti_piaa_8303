#include "aho_corasick_search.h"

#include <iostream>
#include <algorithm>


int main()
{
    ACSearch t(1);
    
    std::string text;
    std::getline(std::cin, text);

    size_t n;
    std::cin >> n;
    std::vector<std::string> string_lib;
    for(size_t i = 0; i < n; i++)
    {
        std::string str;
        std::cin >> str;
        t.insert(str);
        string_lib.push_back(str);
    }


    t.turnIntoMachine();

    auto map_pos = t.search(text);

    for(auto index_item: map_pos)
    {
        std::sort(index_item.second.begin(), index_item.second.end(),
                  [&string_lib](const std::string& a, const std::string& b)
                  {
                      return std::find(string_lib.begin(), string_lib.end(), a) <
                             std::find(string_lib.begin(), string_lib.end(), b);
                  });
        for(auto string_item: index_item.second)
        {
            size_t string_number = std::find(string_lib.begin(), string_lib.end(), string_item)
                                 - string_lib.begin();
            std::cout << index_item.first+1 << " " << string_number+1 << std::endl;
        }
    }

    t.print_longest_links();

    return 0;
}

























