#include <fstream>
#include <iostream>
#include <string>
#include <vector>

std::ofstream log_file;


// get array of prefix function values of casted image
auto prefix(const std::string& image){

    // log printer
    if(log_file.is_open()){
        log_file << "prefix function initiated for \"" << image 
                 << "\" pattern" << std::endl;
    }

    std::vector<int> pi;
    // fisrt char has 0 prefix-function;
    pi.push_back(0);

    // so, starts from 1 char
    // "j" points to current prefix
    for(size_t i = 1, j = 0; i < image.size(); i++){

        // if current char i and j are not equal
        if(image[i] != image[j]){
            // if j points to 0, there is no common parts
            // prefix function equals zero
            if(j == 0){
                pi.push_back(0);
            }

            // check prefix of previous symbol
            else{
                j = pi[j-1];
                i--;
            }
        }

        // else (chars are equal)
        // prefix function equals to length of image[0...j]
        // which is j+1
        else{
            pi.push_back(j+1);
            j++;
        }
    }

    // log printer
    if(log_file.is_open()){
        log_file << "prefix function is:";
        for(auto i: pi)
            log_file << " " << i;
        log_file << std::endl << std::endl;
    }

    return pi;
}


// conveniently arranges the string with which KMPA works
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
    // find prefix of needle (image, pattern)
    auto pi = prefix(needle);

    // print error into log if there it is
    std::vector<int> overlaps;
    if(needle.size() > haystack.size()){
        std::cout << "haystack bigger than needle" << std::endl;
        return overlaps;
    }

    // print initial state to log
    if(log_file.is_open()){
        log_file << "initial state" << std::endl;
        logger(log_file, haystack, needle, 0, 0);
        log_file << std::endl;
    }

    // go through all characters
    // i points to haystack, j points to needle 
    for(size_t i = 0, j = 0; i < haystack.size(); i++){
        // save i and j at cycle step begin,
        // to correctly show strings in the log         
        auto ti = i;
        auto tj = j;

        // if current haystack character equals
        // to current needle character
        if(haystack[i] == needle[j]){
            j++; // increment j, i increments automatically

            // if j equals needle size
            // we went througn the needle
            // it means there is needle in haystack
            if(j == needle.size()){
                //print appropriate message into log
                if(log_file.is_open())
                    log_file << "pattern carriage equal to pattern size\n" 
                             << "it means, we found pattern in text" << std::endl;

                // we on last character of needle in the haystack now
                // but should save index of first character
                overlaps.push_back(i - j + 1);

                // "move" needle using prefix-function of last character 
                j = pi[j-1];
            }

            // print message, that we gonna try find next needle
            if(log_file.is_open())
                log_file << "characters match, move carriage" << std::endl;
        }

        // else (if current charactes not equal)
        else{
            // if we not on first symbol of needle,
            // "move" needle by prefix-function of previous character
            // it's coincides with its prefix
            if(j != 0){
                if(log_file.is_open())
                    log_file << "characters don't match, set pattern carriage on \n" 
                             << "prefix function of previous symbol: " << pi[j-1] << std::endl;
                j = pi[j-1];
                i--;
            }

            // otherwise just move haystack carriage
            else{
                if(log_file.is_open())
                    log_file << "mismatch on first symbol of pattern,\n" 
                             << "move haystack carriage" << std::endl;
            }
        }

        // print current state of haystack and needle
        if(log_file.is_open()){
            logger(log_file, haystack, needle, ti, tj);
            log_file << std::endl;
        }
    }

    // print message about end of haystack into log
    if(log_file.is_open()){
        log_file << "can't move carriage, end of string" << std::endl;
    }

    // return our indexes,
    // if there were no matches,
    // it returns empty vector
    return overlaps;
}


int main(int argc, char* argv[]){
    // get log file as first argument
    if(argc > 1){
        log_file.open(argv[1], std::ios::out | std::ios::trunc);
    }

    // initialize strings to haystack and needle
    std::string haystack;
    std::string needle;

    // get needle and haystack
    std::cin >> needle;
    std::cin >> haystack;

    // get needle entry indexes
    auto overlaps = KMPA(haystack, needle);

    // for each element of array 
    for(auto it = overlaps.begin(); it != overlaps.end(); it++){
        // print a comma, before index if this is not first element
        if(it != overlaps.begin())
            std::cout << ",";
        // print element
        std::cout << *it; 
    }

    // if entries array is empty
    // print "nil"
    if(overlaps.empty())
        std::cout << "nil";
    std::cout << std::endl;

    log_file.close();

    return 0;
}























