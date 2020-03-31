#include <iostream>
#include <algorithm>
#include <vector>
#include <ctime>
#include <fstream>
#include <string>

#include "atr.h"


struct Point
{
    int x;
    int y;
};


struct SquareData
{
	Point pos;
	int w;      // square edge size

    friend std::ostream& operator<<(std::ostream& os, const SquareData& sq){
        os << sq.pos.x << " " << sq.pos.y << " " << sq.w << std::endl;
    }
};


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
	std::vector<SquareData> bestConfiguration;

	const int iSize;

	int **iArray;       // Matrix container
    int iColors = 0;    // Squares on matrix
    int res;            // Flag for backtracking out
    long long int ops = 0;  // backtracking calls

    std::ofstream log;      // file stream for log
    bool logger = 1;        // is logging on?

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


    // partly same as findPossibleSquare
    SquareData findSquare(int color){
        int x = -1;
        int y = -1;
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


    void backtrack(int iCurSize){
        log << std::endl << "Enter backtracking function: " << std::endl;
        ops++;
        if(iCurSize >= res){        // if current quantity of squares more than best config, return
            log << "Current squares quantity cause function out" << std::endl << std::endl;
            return;
        }

        // find free square
        auto emptySquare = findPotentialSquare();

        // if square if whole field size found, decrease it size by 1
        if(emptySquare.w == iSize)
            emptySquare.w--;

        // if structure of founded square is empty, check for best result and save it
        if(emptySquare.w == 0){
            res = std::min(res, iCurSize);
            setConfiguration();
            log << "\nNew best conf set: " << std::endl;
            for(auto sq: bestConfiguration){
                log << sq;
            }
            log << std::endl;
        }

        else{
            int w = emptySquare.w;      // save edge size of square
            log << "Position for square found: " << std::endl;

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
                if (w > 0) log << "Decrease square in pos: " <<
                           emptySquare.pos.x << " " << emptySquare.pos.y << std::endl;
            }
        }
        log << "function out\n" << std::endl;
    }


    void setConfiguration(){
        bestConfiguration.clear();
        // find all squares in matrix and save it to best config
        for(int i = 1; i <= iColors; i++){
            auto square = findSquare(i);
            if(square.w != 0)
                bestConfiguration.emplace_back(square);
        }
    }


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


	~Square(){
        // close log file
        log.close();

        // free memory from matrix
		for(int i = 0; i < iSize; i++){
			delete [] iArray[i];
		}
		delete iArray;
	}


	int getSize() const {
		return iSize;
	}


    float proceed(bool logger = 1){
        // set logger flag
        this->logger = logger;

        // initialize fictional best config size
        res = iSize*iSize;

        // save time of program before function
        auto t = clock();

        backtrack(0);

        // updayte time after function
        t = clock() - t;

        //set best config into matrix
        updateMatrix();

        // turn logger off
        logger = 0;
        log << std::endl;

        return (float)t/CLOCKS_PER_SEC;
    }


    void setSquare(int x, int y, int w){
        
        for(int i = y; i < y + w && i < iSize; i++){
            for(int j = x; j < x + w && j < iSize; j++){
                iArray[i][j] = iColors+1;
            }
        }
        iColors++;
    }

    
    void setSquareVector(const std::vector<SquareData> &vec){
        for(auto square: vec){
            setSquare(square.pos.x, square.pos.y, square.w);
        }
    }


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

        iColors--;
    }


    const std::vector<SquareData>&
    configuration(){
        return bestConfiguration;
    }


    long long int getBacktrackCallsCount() const {
        return ops;
    }


    friend std::ostream& operator<<(std::ostream& os, const Square& sq){
        for(int y = 0; y < sq.iSize; y++){
			for(int x = 0; x < sq.iSize; x++){
                setColor(sq.iArray[y][x]);
				os << "  ";
			}
            resetcolor();
			os << '\n';
		}
        resetcolor();
    }
};


int main(){

    std::string type;
    std::cout << "Enter input type:" << std::endl;

    std::cin >> type;
    while(type != "file" && type != "manual" && type != "quit"){
        std::cout << "Unknown command. Enter:\n"
                  << "<manual> to read directly\n"
                  << "<file> to specify file with data\n"
                  << "<quit> to quit programm" << std::endl;
        std::cin >> type;
    }


    int size;
    std::vector<SquareData> enterData;
    int squaresCount;
    

    if(type == "quit")
        return 0;
    
    if(type == "manual"){
        std::cin >> size;
        std::cin >> squaresCount;
        for(int i = 1; i <= squaresCount; i++){
            int x, y, w;
            std::cin >> x >> y >> w;
            x--;
            y--;

            if(x+w-1 >= size || y+w-1 >= size){
                std::cout << "Square " << i << " out of bounds" << std::endl;
                return 0;
            }

            enterData.emplace_back(SquareData{Point{x, y}, w});
        }
    }

    if(type == "file"){
        std::cout << "File name: ";
        std::string fname;
        std::cin >> fname;
        std::ifstream fs(fname.c_str());
        if(!fs.is_open()){
            std::cout << "Unable to open" << std::endl;
            return 0;
        }
        
        fs >> size;
        fs >> squaresCount;
        for(int i = 1; i <= squaresCount; i++){
            int x, y, w;
            fs >> x >> y >> w;
            x--;
            y--;

            if(x+w-1 >= size || y+w-1 >= size || x-1 < 0 || y-1 < 0){
                std::cout << "Square " << i << " out of bounds" << std::endl;
                return 0;
            }

            enterData.emplace_back(SquareData{Point{x, y}, w});
        }
    }

    Square square(size);

    if(!enterData.empty()){
        square.setSquareVector(enterData);
    }

    float t = square.proceed();

	std::cout << square;

    std::cout << square.configuration().size() << std::endl;
    for(auto piece: square.configuration()){
        std::cout << piece.pos.x + 1 << " " 
                  << piece.pos.y + 1 << " "
                  << piece.w << std::endl;
    }

    std::cout << "Calculated in " << t  << " seconds" << std::endl
              << "Backtracking function calls: " << square.getBacktrackCallsCount() << std::endl;
    
	return 0;
}
