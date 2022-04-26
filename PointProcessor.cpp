#include "PointProcessor.h"

#include <iostream>
//
//void PointProcessor::get_point(int &x, int &y, double xx, double yy) {
//  x = (int)(xx * 10) + 1800;
//  y = (int)(yy * -10) + 800;
////  std::cout << x << " " << y << '\n';
//}

void PointProcessor::get_point(int &x, int &y, double xx, double yy) {
  x = (int)((xx + 100) * 10 + 800);
  y = (int)(yy * (-10) + 800);
}


