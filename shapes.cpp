#include"shapes.h"
#include<cmath>
//Returns a vector with x nd y coordinates of points that make up a circle
std::vector<std::pair<int, int>> circle(int r){
    std::vector<std::pair<int, int>> positions{};
    double x = r, y = r;
    while(int(x-r) != int(y) + 1){
        if(int(sqrt((x-r) * (x-r) +  y * y)) == r){
            //right side
            positions.push_back({x,r-y});
            positions.push_back({x, y+r});
            positions.push_back({y+r, x});
            positions.push_back({y+r, r*2-x});
            //left side
            positions.push_back({r-y, x});
            positions.push_back({2*r-x,y+r});
            positions.push_back({r-y, r*2-x});
            positions.push_back({2*r-x, r-y});
 
            x++;
        }
        else y--;
    }
    return positions;
}
// ISSUE "drawing" on the same pixel multiple times! (apparent when drawing in a blend mode)
std::vector<std::pair<int, int>> filledCircle(int r){ 
    std::vector<std::pair<int, int>> positions{};
    double x = r, y = r;
    while(int(x-r) != int(y) + 1){
        if(int(sqrt((x-r) * (x-r) +  y * y)) == r){
            for(int i = r-y; i < y+r; i++){
                positions.push_back({x, i});
                positions.push_back({2*r-x, i});
            }
            for(int i = r*2-x; i < x; i++){
                positions.push_back({y + r, i});
                positions.push_back({r-y, i});
            }
            x++;
        }
        else y--;
    }
    return positions;
}