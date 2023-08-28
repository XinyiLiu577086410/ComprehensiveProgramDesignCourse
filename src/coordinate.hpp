#ifndef COORDINATE
#define COORDINATE
#include <iostream>

/*
      RANGES:
ROW          COLUMN
1           [1,2,...,5]       
2           [1,2,...,6]
3           [1,2,...,7]
4           [1,2,...,8]
5           [1,2,...,9]
6           [1,2,...,8]
7           [1,2,...,7]
8           [1,2,...,6]
9           [1,2,...,5]

COLUMN      ROW
1           [1,2,...,9]
2           [1,2,...,9]
3           [1,2,...,9]
4           [1,2,...,9]
5           [1,2,...,9]
6           [2,3,...,8]
7           [3,4,5,6,7]
8           [ 4, 5, 6 ]
9           [    5    ]

X(Y)         Y(X)
0           [4,5,...,8]
1           [3,4,...,8]
2           [2,3,...,8]
3           [1,2,...,8]
4           [0,1,...,8]
5           [0,1,...,7]
6           [0,1,...,6]
7           [0,1,...,5]
8           [0,1,...,4]

*/
int infXY[] = {4, 3, 2, 1, 0, 0, 0, 0, 0};
int supXY[] = {8, 8, 8, 8, 8, 7, 6, 5, 4};
int infRow[] = {0, 1, 1, 1, 1, 1, 1, 1, 1, 1};
int supRow[] = {0, 5, 6, 7, 8, 9, 8, 7, 6, 5};

struct VectorXY{
    int x, y;  
};


struct VectorRowColumn{
    int row, col;  
};


// union Coordinate{
//     VectorXY xy;
//     VectorRowColumn rowCol;
// };


bool InAreaXY(int x, int y);
bool InAreaXY(VectorXY xy);
bool InAreaRowColumn(VectorRowColumn rowCol);
VectorXY RowColumnToXY(VectorRowColumn);
VectorRowColumn XYtoRowColunm(VectorXY);


bool InAreaXY(int x, int y) {
    return x >= 0 && y >= 0 && x <= 8 && y <= 8 && x + y >= 4 && x + y <= 12; 
}


bool InAreaXY(VectorXY xy) {
    return InAreaXY(xy.x, xy.y); 
}


bool InAreaRowColumn(VectorRowColumn rowCol) {
    return InAreaXY(RowColumnToXY(rowCol));
}

// x + y = row + 3
// upper: y + 1 = col
// under: col = 9 - x


VectorXY RowColumnToXY(VectorRowColumn rowCol) {
    VectorXY xy;
    if(rowCol.row <= 5) {
        xy.y = rowCol.col - 1;
        xy.x = rowCol.row + 3 - xy.y;
    }
    else
    {
        xy.x = 9 - rowCol.col;
        xy.y = rowCol.row + 3 - xy.x;
    }
    return xy;
}


VectorRowColumn XYtoRowColunm(VectorXY xy){
    VectorRowColumn rowCol;
    rowCol.row = xy.x + xy.y - 3; 
    if(rowCol.row <= 5) {
        rowCol.col = xy.y + 1;
    }
    else
    {
        rowCol.col = 9 - xy.x;
    }
    return rowCol;
};



#endif