#include <iostream> // to use standart stream of input and output
#include <algorithm>// to use std::min function
#include <vector>   // to use vector container
#include <ctime>    // for calculating time of backtracking work
#include <fstream>  // to use file streams (log and file reading)
#include <string>   // to use std::string

// lib for terminal graphics
#include "atr.h"


//storage of coordinate
struct Point
{
    int x;
    int y;
};


//defining square parameters. Its position and size
struct SquareData
{
	Point pos;  // coordinates (begin from (0;0))
	int w;      // square edge size

    friend std::ostream& operator<<(std::ostream& os, const SquareData& sq){
        os << sq.pos.x << " " << sq.pos.y << " " << sq.w << std::endl;
    }
};


// transfer an integer to one of the terminal colors (using deduction ring)
void setColor(int matrixColor){     // change tarminal parameters
    switch(matrixColor % 9){        // sets background color
    case 0:
        set_display_atr(B_BLACK);   
        break;

    case 1:
        set_display_atr(B_RED);
        break;

    case 2:
        set_display_atr(B_GREEN);
        break;

    case 3:
        set_display_atr(B_YELLOW);
        break;

    case 4:
        set_display_atr(B_BLUE);
        break;

    case 5:
        set_display_atr(B_MAGENTA);
        break;

    case 6:
        set_display_atr(B_CYAN);
        break;

    case 7:
        set_display_atr(B_WHITE);
        break;

    case 8:
        resetcolor();
    }
}


class Square
{
private:
    // vector which contains SquareData's of 
    // best configuration of splitting
	std::vector<SquareData> bestConfiguration;

	const int iSize;    // size of square side

	int **iArray;       // Matrix for storring colored squares
    int iColors = 0;    // How many color used to split matrix (also this is a quanity of squares on field)
    int res;            // Flag for backtracking out
    long long int ops = 0;  // backtracking calls

    std::ofstream log;      // file stream for log
    bool logger = 1;        // is logging on?

    // find first free cell from left to right and from top to bot
    // calculate maximum size of square, which can be placed from that cell 
    SquareData findPotentialSquare() const{
        int x = -1;
        int y = -1;
        for(int i = 0; i < iSize; i++){         // find left upper free cell
            bool b = 0;
            for(int j = 0; j < iSize; j++){
                if(iArray[i][j] == 0){
                    y = i;
                    x = j;
                    b = 1;
                    break;                      // remember it and break
                }
            }
            if(b) break;    // breaker of included cycle
        }

        if(x == -1 && y == -1){         // if free cell didn't found return empty structure
            return SquareData{Point{0, 0}, 0};
        }

        // find size of maximal free square from free cell
        int s;
        for(s = 0; s <= iSize - std::max(x, y); s++){       // extend possible border
            bool b = 0;
            //check borders, if they are contain non-zero elements break
            for(int i = y; i < y+s; i++){   
                if(iArray[i][x+s-1] != 0){
                    b = 1;
                    break;
                }
            }
            for(int i = x; i < x+s; i++){
                if(iArray[y+s-1][i] != 0){
                    b = 1;
                    break;
                }
            }
            if(b) break;
        }
        s--;
        return SquareData{Point{x, y}, s};
    }


    // find first cell from left to right and top to bot
    // with required cell, and calculate size of square from found cell
    SquareData findSquare(int color){
        int x = -1;
        int y = -1;

        // find first cell
        for(int i = 0; i < iSize; i++){
            bool b = 0;
            for(int j = 0; j < iSize; j++){
                if(iArray[i][j] == color){
                    y = i;
                    x = j;
                    b = 1;
                    break;
                }
            }
            if(b) break;
        }

        // if cell wasn't found? return
        if(x == -1 && y == -1)
            return SquareData{Point{0, 0}, 0};

        // instead of extending borders, just go along one side
        int s;
        for(s = x; s < iSize; s++){
            if(iArray[y][s] != color)
                break;
        }
        s -= x;
        return SquareData{Point{x, y}, s};
    }


    // backtracking funbction itself
    // iCurSize keeps quantity of placed squares
    void backtrack(int iCurSize){
        if(logger) log << std::endl << "Enter backtracking function: " << std::endl;
        ops++;

        // res - quantity of squares in best configuration
        // if current quantity of squares more then res,
        // no point continuing placing squares, return
        if(iCurSize >= res){        // if current quantity of squares more than best config, return
            if(logger) log << "Current squares quantity cause function out" << std::endl << std::endl;
            return;
        }

        // find free square
        auto emptySquare = findPotentialSquare();

        // if square if whole field size found, decrease it size by 1
        if(emptySquare.w == iSize)
            emptySquare.w--;

        // if structure of founded square is empty, check for best result and save it
        if(emptySquare.w == 0){
            // get minimum between current square quantity on field
            // and quantity of squares in best configuration;
            res = std::min(res, iCurSize);

            // remember all squares on field as SquareInfo structures;
            setConfiguration();
            if(logger){
                log << "\nNew best conf set: " << std::endl;
                for(auto sq: bestConfiguration)
                    log << sq;
                log << std::endl;
            }
        }

        else{
            int w = emptySquare.w;      // save edge size of square
            if(logger) log << "Position for square found: " << std::endl;

            // cycle decreasing edge size by one every lapse
            while(w > 0){
                // set found square with size w int matrix
                setSquare(emptySquare.pos.x, emptySquare.pos.y, w);

                if(logger){
                    log << "square added: ";
                    log << emptySquare.pos.x << " " << emptySquare.pos.y << " " << w << std::endl;
                }

                // call backtracking function to other part of field
                backtrack(iCurSize+1);      

                if(logger){
                    log << "Square deleted: ";
                    log << emptySquare.pos.x << " " << emptySquare.pos.y << " " << w << std::endl;
                }

                // remove square from field to set in the same pos square with less edge
                delSquare(emptySquare.pos.x, emptySquare.pos.y);
                // decrement size of square edge to put
                w--;
                if (logger && w > 0) log << "Decrease square in pos: "
                                         << emptySquare.pos.x << " "
                                         << emptySquare.pos.y << std::endl;
            }
        }
        if(logger) log << "function out\n" << std::endl;
    }


    // remember field as dynamic array of
    // squares definers (called SquareData)
    void setConfiguration(){
        bestConfiguration.clear();
        // find all squares in matrix and save it to best config
        // is square has number, which he gets, when added on table
        // using this numbers we can find all squares on field
        for(int i = 1; i <= iColors; i++){
            auto square = findSquare(i);
            if(square.w != 0)
                // bestConfiguration is a part of main working class
                bestConfiguration.emplace_back(square);
        }
    }


    // put all squares from bestConfiguration on field
    void updateMatrix(){
        int c = 1;
        // colorize matrix using best config
        for(auto data: bestConfiguration){
            for(int i = data.pos.y; i < data.pos.y + data.w; i++){
                for(int j = data.pos.x; j < data.pos.x + data.w; j++){
                    iArray[i][j] = c;
                }
            }
            c++;
        }
    }


    // checks for prime divisibility
    int checkOptimalSolution(){
        // check dividing of iSize. And set best config if it dividing
        if (iSize % 2 == 0)
        {
            div2();
            setConfiguration();
            return 1;
        }
        else if (iSize % 3 == 0)
        {
            div3();
            setConfiguration();
            return 1;
        }
        else if (iSize % 5 == 0)
        {
            div5();
            setConfiguration();
            return 1;
        }
        return 0;
    }


    // function of the same nature
    // if square size can be divided by 2, 3 or 5
    // we just can fill it manually and keeping proportions
    void div2(){
        setSquare(0, 0, iSize/2);
        setSquare(iSize/2, 0, iSize/2);
        setSquare(0, iSize/2, iSize/2);
        setSquare(iSize/2, iSize/2, iSize/2);
    }

    void div3(){
        setSquare(0, 0, iSize/3*2);
        setSquare(iSize/3*2, 0, iSize/3);
        setSquare(iSize/3*2, iSize/3, iSize/3);
        setSquare(0, iSize/3*2, iSize/3);
        setSquare(iSize/3, iSize/3*2, iSize/3);
        setSquare(iSize/3*2, iSize/3*2, iSize/3);
    }

    void div5(){
        setSquare(0, 0, iSize/5*3);
        setSquare(iSize/5*3, 0, iSize/5*2);
        setSquare(iSize/5*3, iSize/5*2, iSize/5*2);
        setSquare(0, iSize/5*3, iSize/5*2);
        setSquare(iSize/5*2, iSize/5*3, iSize/5);
        setSquare(iSize/5*2, iSize/5*4, iSize/5);
        setSquare(iSize/5*3, iSize/5*4, iSize/5);
        setSquare(iSize/5*4, iSize/5*4, iSize/5);
    }



public:
    // Constructor
	Square(int iSize):
		iSize(iSize)
	{
        // create or clear log file
        log.open("log", std::ios::out | std::ios::trunc);

        // create matrix
		iArray = new int*[iSize];
		for(int i = 0; i < iSize; i++){
			iArray[i] = new int[iSize];
			for(int j = 0; j < iSize; j++){
				iArray[i][j] = 0;
			}
		}
	}


    // Destructor
	~Square(){
        // close log file
        log.close();

        // free memory from matrix
		for(int i = 0; i < iSize; i++){
			delete [] iArray[i];
		}
		delete iArray;
	}


	int getSize() const {   // returns size of field
		return iSize;
	}


    // intialize and run backtracking function
    // returns time of working
    float proceed(bool logger = 1){
        // set logger flag
        this->logger = logger;

        // initialize fictional best config size
        // maximal quantity of squares is quantity of cells
        // therefore, the fictious best result is equal
        // to the area
        res = iSize*iSize;

        // save time of program before function
        auto t = clock();

        // run backtracking function
        backtrack(0);

        // update time after function
        t = clock() - t;

        //set best config into matrix
        updateMatrix();

        // turn logger off
        logger = 0;
        log << std::endl;

        return (float)t/CLOCKS_PER_SEC;
    }


    // fills square defined as top-left point is (x;y) and width w
    // if in this area already was any square (there is cell, which not zero)
    // finish the programm and says about conflict
    void setSquare(int x, int y, int w){
        
        for(int i = y; i < y + w && i < iSize; i++){
            for(int j = x; j < x + w && j < iSize; j++){
                if(iArray[i][j] != 0){  // if there is painted cell, can't place square
                    if(logger){
                        auto occupying = findSquare(iArray[i][j]);
                        auto to_insert = SquareData{x, y, w};
                        log << "Conflict of squares " << std::endl;
                        log << "Occupying square: " << occupying.pos.x+1 << " "
                            << occupying.pos.y+1 << " "
                            << occupying.w << std::endl;
                        log << "Square to set: " << x+1 << " " << y+1
                            << " " << w << std::endl;
                    }

                    std::cout << "Conflicting of squares, hard shutdown" << std::endl;

                    // close log file
                    log.close();
                    // free memory from matrix
	                for(int i = 0; i < iSize; i++){
		                delete [] iArray[i];
	                }
	                delete iArray;

                    std::exit(0);
                }

                // Покрасить клетку в цвет выше максимального
                iArray[i][j] = iColors+1;
            }
        }
        // увеличить максимальный цвет на 1
        iColors++;
    }

    
    // gets vector of SquareData, and then
    // foreach data in vector, set appropriate square on field 
    void setSquareVector(const std::vector<SquareData> &vec){
        for(auto square: vec){
            setSquare(square.pos.x, square.pos.y, square.w);
        }
    }


    // set all cells with color of cell(x, y) as zero, keep color of that cell
    // for each square which has color bigger then removed, decrease coloros on
    // its cells. Decrease colors quantity  
    void delSquare(int x, int y){
        // save color of input cell
        int color = iArray[y][x];

        // make all cells with saved color as zero
        for(int i = y; i < iSize; i++){
            for(int j = x; j < iSize; j++){
                if(iArray[i][j] == color)
                    iArray[i][j] = 0;
                else break;
            }
        }

        // decrease all colors more than saved
        for(int i = 0; i < iSize; i++){
            for(int j = 0; j < iSize; j++){
                if(iArray[i][j] > color)
                    iArray[i][j]--;
            }
        }
        
        //Decrease colors quantity
        iColors--;
    }


    // Returns refer to best configuration
    // so that it can be printed 
    const std::vector<SquareData>&
    configuration(){
        return bestConfiguration;
    }


    // at begining of backtrack function ops counter increases
    // so we can get quantity of backtracking calls
    long long int getBacktrackCallsCount() const {
        return ops;
    }


    // operator of matrix displaying.
    // gets color of cell, then converts it into
    // some color of terminal, then prints two spaces
    // (two characters are square, like pixel)
    friend std::ostream& operator<<(std::ostream& os, const Square& sq){
        for(int y = 0; y < sq.iSize; y++){
			for(int x = 0; x < sq.iSize; x++){
                // dye terminal
                setColor(sq.iArray[y][x]);
                // print pixel
				os << "  ";
			}
            // set standart terminal colors
            resetcolor();
            // get next line
			os << '\n';
		}
        // set standart terminal colors
        resetcolor();
    }
};


int main(){

    // get type of input. Manual: put all data using terminal
    //                    File: print name of input file, which lies near programm executor
    std::string type;
    std::cout << "Enter input type:" << std::endl;


    // Ask user to enter a function until the correct one is entered
    // if instead of type, user enters "quit", programm closes 
    std::cin >> type;
    while(type != "file" && type != "manual" && type != "quit"){
        std::cout << "Unknown command. Enter:\n"
                  << "<manual> to read directly\n"
                  << "<file> to specify file with data\n"
                  << "<quit> to quit programm" << std::endl;
        std::cin >> type;
    }


    // initialize vector of predisposed squares
    int size;
    std::vector<SquareData> enterData;
    int squaresCount;
    

    // if "quit" entered, end the programm
    if(type == "quit")
        return 0;
    
    // if "manual" entered.
    // Program gets size of field and
    // quantity of prediposed squares.
    // Then user should enter squares itself
    if(type == "manual"){
        std::cin >> size;
        std::cin >> squaresCount;
        for(int i = 1; i <= squaresCount; i++){
            int x, y, w;
            std::cin >> x >> y >> w;
            x--;
            y--;

            // if read square goes beyond the field,
            // end programm with appropriate message
            if(x+w-1 >= size || y+w-1 >= size || x < 0 || y < 0){
                std::cout << "Square " << i << " out of bounds" << std::endl;
                return 0;
            }

            // save SquareData to input vector
            enterData.emplace_back(SquareData{Point{x, y}, w});
        }
    }

    // if "file" entered
    // programm asks name of file with input.
    // Then it tries to open this. If it can't, then end programm.
    // Else read size of square from first string,
    // Afterwards read quantity of prediposed squares (call it squaresCount)
    // And then 'squareCount' times read next line, and save SquareData of prediposed squares
    if(type == "file"){

        // get file name
        std::cout << "File name: ";
        std::string fname;
        std::cin >> fname;

        // create file descriptor and open file
        std::ifstream fs(fname.c_str());

        // if file is not openned
        // end programm with appropriate message in terminal
        if(!fs.is_open()){
            std::cout << "Unable to open" << std::endl;
            return 0;
        }
        
        // read size of field
        fs >> size;
        // read quantity of prediposed squares
        fs >> squaresCount;
        // read SquareData of prediposed squares in other lines
        for(int i = 1; i <= squaresCount; i++){
            int x, y, w;
            fs >> x >> y >> w;
            x--;
            y--;

            // if read square goes beyond the field,
            // end programm with appropriate message
            if(x+w-1 >= size || y+w-1 >= size || x < 0 || y < 0){
                std::cout << "Square " << i << " out of bounds" << std::endl;
                return 0;
            }

            // insert read Square into input vector
            enterData.emplace_back(SquareData{Point{x, y}, w});
        }
    }

    // intialize field
    Square square(size);

    // set read squares to field
    if(!enterData.empty()){
        square.setSquareVector(enterData);
    }

    // proceed field. Split into smaller squares
    float t = square.proceed();

    // Display the resulting matrix
	std::cout << square;

    // Print all squares in field
    std::cout << square.configuration().size() << std::endl;
    for(auto piece: square.configuration()){
        std::cout << piece.pos.x + 1 << " " 
                  << piece.pos.y + 1 << " "
                  << piece.w << std::endl;
    }

    // Print data about working time, and
    // quantity of calls backtracking function 
    std::cout << "Calculated in " << t  << " seconds" << std::endl
              << "Backtracking function calls: " << square.getBacktrackCallsCount() << std::endl;
    
	return 0;
}
