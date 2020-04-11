#include <iostream>
#include <algorithm>
#include <vector>
#include <ctime>

#include "atr.h"

struct Point
{
    int x;
    int y;
};


struct SquareData
{
	Point pos;
	int w;

    friend std::ostream& operator<<(std::ostream& os, const SquareData& sq){
        os << sq.pos.x << " " << sq.pos.y << " " << sq.w << std::endl;
    }
};


void setColor(int matrixColor){
    switch(matrixColor % 9){
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
	int **iBestConfiguration;

	const int iSize;

	int **iArray;
    int iColors = 0;
    int res;

    SquareData findPotentialSquare() const{
        int x = -1;
        int y = -1;
        for(int i = 0; i < iSize; i++){
            bool b = 0;
            for(int j = 0; j < iSize; j++){
                if(iArray[i][j] == 0){
                    y = i;
                    x = j;
                    b = 1;
                    break;
                }
            }
            if(b) break;
        }

        if(x == -1 && y == -1){
            return SquareData{Point{0, 0}, 0};
        }

        int s;
        for(s = 0; s <= iSize - std::max(x, y); s++){
            bool b = 0;
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

        int s;
        for(s = x; s < iSize; s++){
            if(iArray[y][s] != color)
                break;
        }
        s -= x;
        return SquareData{Point{x, y}, s};
    }


    void backtrack(int iCurSize){
        if(iCurSize >= res)
            return;

        auto emptySquare = findPotentialSquare();

        if(emptySquare.w == iSize){
            emptySquare.w /= 3;
            emptySquare.w *= 2;
        }

        if(emptySquare.w == 0){
            res = std::min(res, iCurSize);
            setConfiguration();
        }

        else{
            int w = emptySquare.w;

            if(emptySquare.pos.x == 0 && emptySquare.pos.y == 0){
                int half_w = iSize/2;
                SquareData topLeft{Point{0, 0}, half_w + iSize%2};
                setSquare(topLeft.pos.x, topLeft.pos.y, topLeft.w);
                setSquare(topLeft.w, 0, half_w);
                setSquare(0, topLeft.w, half_w);

//                half_w = topLeft.w / 2;
//                SquareData lowRight{Point{iSize - (topLeft.w/2+topLeft.w%2),
//                                          iSize - (topLeft.w/2+topLeft.w%2)},
//                                          half_w + topLeft.w%2};
//                setSquare(lowRight.pos.x, lowRight.pos.y, lowRight.w);
//                setSquare(lowRight.pos.x - half_w, lowRight.pos.y + topLeft.w%2, half_w);
//                setSquare(lowRight.pos.x + topLeft.w%2, lowRight.pos.y - half_w, half_w);
                backtrack(3);
            }
            else
                while(w > 0){
                    setSquare(emptySquare.pos.x, emptySquare.pos.y, w);
                    backtrack(iCurSize+1);
    
                    delSquare(emptySquare.pos.x, emptySquare.pos.y);
                    w--;
                }
        }
    }


    void setConfiguration(){
        for(int i = 0; i < iSize; i++){
            for(int j = 0; j < iSize; j++){
                iBestConfiguration[i][j] = iArray[i][j];
            }
        }
    }


    void updateMatrix(){
        for(int i = 0; i < iSize; i++){
            for(int j = 0; j < iSize; j++){
                iArray[i][j] = iBestConfiguration[i][j];
            }
        }
    }


    int checkOptimalSolution(){
        if (iSize % 2 == 0)
        {
            div2();
            return 1;
        }
        else if (iSize % 3 == 0)
        {
            div3();
            return 1;
        }
        else if (iSize % 5 == 0)
        {
            div5();
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
        iBestConfiguration = new int*[iSize];
		iArray = new int*[iSize];
		for(int i = 0; i < iSize; i++){
            iBestConfiguration[i] = new int[iSize];
			iArray[i] = new int[iSize];
			for(int j = 0; j < iSize; j++){
                iBestConfiguration[i][j] = 0;
				iArray[i][j] = 0;
			}
		}
	}


	~Square(){
		for(int i = 0; i < iSize; i++){
            delete [] iBestConfiguration[i];
			delete [] iArray[i];
		}
        delete [] iBestConfiguration;
		delete [] iArray;
	}


	int getSize() const {
		return iSize;
	}


    float proceed(){
        if(!checkOptimalSolution()){
            res = iSize*iSize;
            auto t = clock();
            backtrack(0);
            t = clock() - t;

            iColors = res;
            updateMatrix();
            return (float)t/CLOCKS_PER_SEC;
        }
        return 0;
    }


    void setSquare(int x, int y, int w){
        
        for(int i = y; i < y + w && i < iSize; i++){
            for(int j = x; j < x + w && j < iSize; j++){
                iArray[i][j] = iColors+1;
            }
        }
        iColors++;
    }


    void delSquare(int x, int y){
        int color = iArray[y][x];
        for(int i = y; i < iSize; i++){
            for(int j = x; j < iSize; j++){
                if(iArray[i][j] == color)
                    iArray[i][j] = 0;
                else break;
            }
        }

        for(int i = 0; i < iSize; i++){
            for(int j = 0; j < iSize; j++){
                if(iArray[i][j] > color)
                    iArray[i][j]--;
            }
        }

        iColors--;
    }

    
    void printConfiguration(std::ostream &os){
        for(int y = 0; y < iSize; y++){
			for(int x = 0; x < iSize; x++){
                setColor(iArray[y][x]);
				os << "  ";
			}
            resetcolor();
			os << '\n';
		}
        resetcolor();


        os << iColors << std::endl;
        for(int i = 1; i <= iColors; i++){
            auto sq = findSquare(i);
            os << sq.pos.x+1 << " " << sq.pos.y+1 << " " << sq.w << std::endl;
        }
    }

};


int main(){
    int size;
    std::cin >> size;
    
    Square square(size);

    auto t = square.proceed();

    square.printConfiguration(std::cout);
    
    std::cout << "time: " << t << std::endl;
    
	return 0;
}
