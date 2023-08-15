#ifndef COORDINATE
#define COORDINATE


struct VectorXY{
    int x, y;  
};


struct VectorRowColumn{
    int row, column;  
};


union Coordinate{
    VectorXY xy;
    VectorRowColumn rowColumn;
};


bool InAreaXY(int x, int y);
bool InAreaXY(VectorXY xy);
bool InAreaRowColumn(VectorRowColumn rowColumn);
VectorXY RowColumnToXY(VectorRowColumn);
VectorRowColumn XYtoRowColunm(VectorXY);


bool InAreaXY(int x, int y) {
    return x >= 0 && y >= 0 && x <= 8 && y <= 8 && x + y >= 4 && x + y <= 12; 
}


bool InAreaXY(VectorXY xy) {
    return InAreaXY(xy.x, xy.y); 
}


bool InAreaRowColumn(VectorRowColumn rowColumn) {
    return InAreaXY(RowColumnToXY(rowColumn));
}


VectorXY RowColumnToXY(VectorRowColumn) {
    
}



#endif