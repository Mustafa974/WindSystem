//
//  MathFormula.cpp
//  test
//
//  Created by Mustafa on 2018/10/29.
//  Copyright © 2018年 Mustafa. All rights reserved.
//

#include "MathFormula.hpp"

//构造函数，存储数组到vector中，初始化各个值
MathFormula::MathFormula(vector<double> _vec){
    for(int i = 0; i < _vec.size(); i++){
        vec.push_back(_vec[i]);
    }
    average = -1.00;
    variance = -1.00;
    standard_deviation = -1.00;
}

//求最大值
double MathFormula::Max(){
    double max = 0.0;
    for (vector<double>::iterator it = vec.begin(); it != vec.end(); it++) {
        if ((*it) > max){
            max = *it;
        }
    }
    return max;
}

//求最小值
double MathFormula::Min(){
    double min = 0.0;
    for (vector<double>::iterator it = vec.begin(); it != vec.end(); it++) {
        if ((*it) < min){
            min = *it;
        }
    }
    return min;
}

//求平均间距
double MathFormula::AvgGap(){
    vector<double> gap;
    for (int i = 0; i < vec.size()-1; i++) {
        gap.push_back(vec[i+1]-vec[i]);
    }
    MathFormula mf(gap);
    return mf.Mean();
}

//求数组均值
double MathFormula::Mean(){
    double sum = 0.0;
    for(int i = 0; i < vec.size(); i++){
        sum += vec[i];//求和
    }
    average = sum/vec.size();//为私有成员赋值
    return average;
}

//求方差
double MathFormula::Variance(){
    if(average == -1.00){
        average = Mean();
    }
    double sum = 0.00;
    for (vector<double>::iterator it = vec.begin(); it != vec.end(); ++it){
        double tmp = *it - average;
        sum += tmp * tmp;
    }
    variance = sum/(double)vec.size();
    return variance;
}

//求标准差
double MathFormula::StandardDeviation(){
    if(variance == -1.00){
        variance = Variance();
    }
    standard_deviation = sqrt(variance);
    return standard_deviation;
}
