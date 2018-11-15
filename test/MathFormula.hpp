//
//  MathFormula.hpp
//  test
//
//  Created by Mustafa on 2018/10/29.
//  Copyright © 2018年 Mustafa. All rights reserved.
//

#ifndef MathFormula_hpp
#define MathFormula_hpp

#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

//构造进行数学计算的类
class MathFormula{
    
public:
    MathFormula(vector<double> vec);//构造函数，存储数据列表
    double Max();//求最大值
    double Min();//求最小值
    double AvgGap();//求平均间距
    double Mean();//求均值
    double Variance();//求方差
    double StandardDeviation();//求标准差
    
private:
    vector<double> vec;
    double average;
    double variance;
    double standard_deviation;
    
};


#endif /* MathFormula_hpp */
