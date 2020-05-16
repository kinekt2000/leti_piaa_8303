#include <iostream>
#include <fstream>
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


// wrapper over string
// keeps index of string in the text
class TextPart{
    friend class Splitter;

    const std::string _str;
    const size_t _index;
    
    TextPart(const std::string& str, const size_t index):
        _str(str), _index(index){}

public:
    TextPart(const TextPart& other):
        _str(other._str), _index(other._index)
    {}

    const std::string& str() const
    { return _str; }

    const size_t& index() const
    { return _index; }

};

class Splitter
{
public:
    enum Error
    {
        absent,
        some_parts_excluded,
        cannot_be_splitted
    };

private:
    std::vector<TextPart> parts;
    Error _error;


public:
    Splitter(const std::string& text, size_t parts_count, size_t minimal_part_size = 0):
        _error(Error::absent)
    {
        if(log_file.is_open()){
            log_file << "splitter initiated" << std::endl;
        }

        // empty text cannot be splitted
        if(text.empty())
        {
            if(log_file.is_open()){
                log_file << "splitter error" << std::endl;
            }

            _error = Error::cannot_be_splitted;
            return;
        }

        // cannot split, if parts count more than characters in text
        if(text.size() < parts_count)
        {
            // weird error
            if(log_file.is_open()){
                log_file << "splitter error: too many parts" << std::endl;
            }

            _error = Error::cannot_be_splitted;
            return;
        }

        // size of minimal part cannot be smaller than 1
        if(minimal_part_size == 0) minimal_part_size = 1;

        size_t part_length = text.size()/parts_count;
        
        // if minimal part length smaller than minimum,
        // recalculate parts count using minimum
        if(minimal_part_size > part_length)
        {
            parts_count = text.size()/minimal_part_size;

            // if minimal part bigger than text, text cannot be splitted
            if(parts_count == 0)
            {
                if(log_file.is_open()){
                    log_file << "splitter error: minimal part bigger than whole text" << std::endl;
                }

                _error = Error::cannot_be_splitted;
                return;
            }
            
            if(log_file.is_open()){
                log_file << "splitter warning: minimal length not suitable for this parts quantity" << std::endl;
            }

            _error = Error::some_parts_excluded;
            part_length = minimal_part_size;
        }

        // need to restribute a residual to all other parts
        size_t residual = text.size()%part_length;

        // connector size. Size of similiar parts of neighbour tokens
        size_t connector = minimal_part_size/2;
        if(minimal_part_size%2) connector++;

        // there is only 1 part
        // so, text cannot be splitted
        if(parts_count == 1)
        {
            if(log_file.is_open()){
                log_file << "splitter error: there can't be only one part" << std::endl;
            }
            _error = Error::cannot_be_splitted;
            return;
        } 

        if(log_file.is_open()){
            log_file << std::endl;
            log_file << "The whole text divided into " << parts_count << ",\n"
                     << "with a remainder of " << residual << std::endl;
            log_file << "A remainder will be distributed among parts" << std::endl;
            log_file << std::endl << std::endl;
        }

        auto token_begin = text.begin();
        auto token_end = text.begin() + part_length;
        // get all whole tokens of text
        for(size_t i = 0; i < parts_count; i++)
        {
            std::string token;

            // if there is a residual text after division
            if(residual > 0)
            {
                // extra_residual need to round up
                // it didn't work without individual var
                // so weird
                bool extra_residual = (residual%(parts_count - i))?1:0;

                // get part of residual text for this part
                size_t extra = residual/(parts_count - i) + extra_residual;

                if(log_file.is_open()){
                    log_file << "There is " << extra << " characters for " << i+1 << " part" << std::endl;
                }

                token_end += extra;
                residual -= extra;
            }

            // get token which overlaying next part by half of minimal part
            // we should do this to miss needle entries on part borders 
            token.assign(token_begin, token_end+((i != parts_count-1)?connector:0));
            if(log_file.is_open()){
                log_file << "Considering the overlay on the next part, \n"
                         << "We've got part " << i+1 << ":\n" 
                         <<  token << "\n" 
                         << "which starts at index " << token_begin - text.begin() 
                         << " in the main text" << std::endl;
            }

            // create text part with saving its index
            TextPart part(token, token_begin - text.begin());
            parts.emplace_back(part);

            // move token definers
            token_begin = token_end;
            token_end += part_length;
        }
        if(log_file.is_open()){
            log_file << std::endl;
        }
    }

    bool good() const
    {return (_error == Error::absent)?1:0; }

    Error error() const
    { return _error; }

    const auto& getParts() const
    { return parts; }
};


// KMPA to all parts, then add offset of each part
auto distributeText(const std::vector<TextPart>& parts, const std::string& needle){
    std::vector<int> overlaps;
    size_t i = 1;      
    for(auto part: parts){
        if(log_file.is_open()){
            log_file << "Start KMP for " << i << " part:\n"
                     << part.str() << std::endl
                     << "which has offset: " << part.index() << std::endl;
        }

        // get entries of needle into part
        auto private_overlaps = KMPA(part.str(), needle);

        if(log_file.is_open()){
            log_file << "positions of needle in this haystack (with global offset):";
        }

        // add offset of part to each index
        for(auto it = private_overlaps.begin();
            it != private_overlaps.end(); it++)
        {
            if(log_file.is_open()){
                log_file << " " << *it;
            }

            *it += part.index();

            if(log_file.is_open()){
                log_file << "(" << *it << ")";
            }
        }

        if(log_file.is_open()){
            log_file << std::endl;
        }

        // put them into vector with all indexes vector
        overlaps.insert(overlaps.end(), private_overlaps.begin(), private_overlaps.end());
    }
    if(log_file.is_open()){
        log_file << std::endl;
    }

    return overlaps;
}


int main(int argc, char* argv[]){
    if(argc > 1){
        log_file.open(argv[1], std::ios::out | std::ios::trunc);
    }

    // initialize string to text and needle
    std::string text;
    std::string pattern;

    // desired amount of parts
    size_t parts_count;

    // size of needle
    size_t connector_size;

    // get string using getline to read spaces too
    std::getline(std::cin, text);

    // get desired amount of parts
    std::cin >> parts_count;

    // get needle
    std::cin >> pattern;

    // split text, using desired parts amount and needle size too
    Splitter splitter(text, parts_count, pattern.size());

    // if splitter could not split the text, we can check the
    // error in log file
    if(splitter.error() == Splitter::cannot_be_splitted){
        std::cout << "splitter error. check logs" << std::endl;
        return 1;
    }

    // print parts we got
    for(auto& token: splitter.getParts()){
        std::cout << token.str() << "; " << token.index() << std::endl;
    }

    // run KMPA to all parts and get indexes of needle
    auto overlaps = distributeText(splitter.getParts(), pattern);

    for(auto it = overlaps.begin(); it != overlaps.end(); it++){
        if(it != overlaps.begin())
            std::cout << ",";        
        std::cout << *it; 
    }
    if(overlaps.empty()){
        std::cout << "nil";
    }
    std::cout << std::endl;

    return 0;
}























