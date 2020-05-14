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
            const std::string& shifted,
            const std::string& base,
            size_t i, size_t j)
{
    std::string border;
    border.assign(shifted.size()*2-1, '-');
    log << border << std::endl;

    std::string car_top;
    std::string car_bot;
    car_top.assign(i, ' ');
    car_bot = car_top + "^";
    car_top += "V";
    log << car_top << std::endl;
    
    std::string haystack = shifted+shifted;
    haystack.pop_back();
    log << haystack << std::endl;

    std::string needle_str;
    needle_str.assign(i-j, ' ');
    needle_str += base;
    log << needle_str << std::endl;
    log << car_bot << std::endl;
    log << border << std::endl;
}


// moves base, until it becomes shifted
auto cyclicShift(const std::string& base, const std::string& shifted){
    std::vector<int> overlaps;    
    auto size = base.size();
    if(size != shifted.size()){
        std::cout << "shifted string and base string have different sizes" << std::endl;
        return overlaps;
    }

    if(log_file.is_open()){
        log_file << "Imagine the haystack like concatenation of shifted string with itself, \n"
                 << "and the base string as the needle" << std::endl << std::endl;
    }

    auto pi = prefix(base);  

    if(log_file.is_open()){
        log_file << "initial state" << std::endl;
        logger(log_file, shifted, base, 0, 0);
        log_file << std::endl;
    }

    for(size_t i = 0, j = 0; i < size*2-1; i++){
        auto ti = i;
        auto tj = j;

        if(shifted[i%size] == base[j]){
            j++;
            if(j == size){
                if(log_file.is_open()){
                    log_file << "charachers match,\n"
                             << "pattern carriage equal to pattern size\n" 
                             << "it means, we found the shift of base string" << std::endl;
                    logger(log_file, shifted, base, i, j-1);
                }

                overlaps.push_back(i - j + 1);
                return overlaps;
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
                    log_file << "mismatch on frist symbol of base string \n" 
                             << "move shifted carriage" << std::endl;
            }
        }
        if(log_file.is_open()){
            logger(log_file, shifted, base, ti, tj);
            log_file << std::endl;
        }
    }
    if(log_file.is_open()){
        log_file << "can't move carriage, there is no base in shifted string" << std::endl;
    }
    return overlaps;
}


int main(int argc, char* argv[]){
    if(argc > 1){
        log_file.open(argv[1], std::ios::out | std::ios::trunc);
    }

    std::string shifted;
    std::string base;

    std::cin >> shifted;
    std::cin >> base;

    auto overlaps = cyclicShift(base, shifted);

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























