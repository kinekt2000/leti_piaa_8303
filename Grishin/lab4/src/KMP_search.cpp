#include <fstream>
#include <iostream>
#include <string>
#include <vector>

std::ofstream log_file;

auto prefix(const std::string& image){
    if(log_file.is_open()){
        log_file << "prefix function initiated for \"" << image 
                 << "\" pattern" << std::endl;
    }

    std::vector<int> pi;
    pi.push_back(0);
    for(size_t i = 1, j = 0; i < image.size(); i++){
        if(image[i] != image[j]){
            if(j == 0){
                pi.push_back(0);
            }
            else{
                j = pi[j-1];
                i--;
            }
        }
        else{
            pi.push_back(j+1);
            j++;
        }
    }

    if(log_file.is_open()){
        log_file << "prefix function is:";
        for(auto i: pi)
            log_file << " " << i;
        log_file << std::endl << std::endl;
    }

    return pi;
}


void logger(std::ofstream &log,
            const std::string& haystack,
            const std::string& needle,
            size_t i, size_t j)
{
    std::string border;
    border.assign(haystack.size(), '-');
    log << border << std::endl;

    std::string car_top;
    std::string car_bot;
    car_top.assign(i, ' ');
    car_bot = car_top + "^";
    car_top += "V";
    log << car_top << std::endl;
    
    log << haystack << std::endl;

    std::string needle_str;
    needle_str.assign(i-j, ' ');
    needle_str += needle;
    log << needle_str << std::endl;
    log << car_bot << std::endl;
    log << border << std::endl;
}


auto KMPA(const std::string& haystack, const std::string& needle){
    auto pi = prefix(needle);
    std::vector<int> overlaps;
    if(needle.size() > haystack.size()){
        std::cout << "haystack bigger than needle" << std::endl;
        return overlaps;
    }

    if(log_file.is_open()){
        log_file << "initial state" << std::endl;
        logger(log_file, haystack, needle, 0, 0);
        log_file << std::endl;
    }

    for(size_t i = 0, j = 0; i < haystack.size(); i++){
        auto ti = i;
        auto tj = j;

        if(haystack[i] == needle[j]){
            j++;
            if(j == needle.size()){
                if(log_file.is_open())
                    log_file << "pattern carriage equal to pattern size\n" 
                             << "it means, we found pattern in text" << std::endl;

                overlaps.push_back(i - j + 1);
                j = pi[j-1];
            }
            if(log_file.is_open())
                log_file << "characters match, move carriage" << std::endl;
        }
        else{  
            if(j != 0){
                if(log_file.is_open())
                    log_file << "characters don't match, set pattern carriage on \n" 
                             << "prefix function of previous symbol: " << pi[j-1] << std::endl;
                j = pi[j-1];
                i--;
            }
            else{
                if(log_file.is_open())
                    log_file << "mismatch on first symbol of pattern,\n" 
                             << "move haystack carriage" << std::endl;
            }
        }

        if(log_file.is_open()){
            logger(log_file, haystack, needle, ti, tj);
            log_file << std::endl;
        }
    }
    if(log_file.is_open()){
        log_file << "can't move carriage, end of string" << std::endl;
    }
    return overlaps;
}


int main(int argc, char* argv[]){
    if(argc > 1){
        log_file.open(argv[1], std::ios::out | std::ios::trunc);
    }

    std::string haystack;
    std::string needle;

    std::cin >> needle;
    std::cin >> haystack;

    auto overlaps = KMPA(haystack, needle);

    for(auto it = overlaps.begin(); it != overlaps.end(); it++){
        if(it != overlaps.begin())
            std::cout << ",";        
        std::cout << *it; 
    }
    if(overlaps.empty())
        std::cout << "nil";
    std::cout << std::endl;

    log_file.close();

    return 0;
}























