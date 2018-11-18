//
//  AfterProcess.hpp
//  test
//
//  Created by Mustafa on 2018/10/29.
//  Copyright © 2018年 Mustafa. All rights reserved.
//

#ifndef AfterProcess_hpp
#define AfterProcess_hpp

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include "MathFormula.hpp"

using namespace std;

//构造后处理的类
class AfterProcess{
    
public:
    AfterProcess(vector<double> xy);//构造函数，传入z设置好的x1,y1,x2,y2
    bool SaveData(string src_path);//存储CSV中的数据到结果集中
    void SaveXYData(string src_path_X, string src_path_Y, int col);//存储XY方向的速度分量
    void SaveXYZ();//存储各区域xyz取值范围以及xyz极值
    void SaveNumOf9999();//存储数据中没吃呢个-9999的个数
    vector<double> CalculateAvgWindSpeed(string);
    void CalculateAngle();
    void CalculateRv(string);
    void CalculateθRatio(string);
    
    void PrintXYZDomain();
    void PrintXSpeed();
    void PrintYSpeed();
    void PrintZSpeed();
    void PrintNumOf9999();
    void PrintWindSpeed();
    void PrintAngle();
    void PrintRv();
//    void PrintData();//打印结果集
//    bool SaveToTXT();//将结果存入txt
    
private:
    vector<double> x_axis, y_axis, z_axis, num_of_9999, xyz_domain;//存储风速二维数组，xx方向的坐标，y方向的坐标，z方向每层的高度，每层-9999的个数，存储设置的区域xyz范围,顺序为x1,y1,x2,y2,z0,za,z1,z2,x3,y3,z3
    vector<vector<double>> wind_speed;//存储每层的全部风速
    vector<vector<double>> wind_speed_X;//存储X轴每层的全部风速
    vector<vector<double>> wind_speed_Y;//存储Y轴每层的全部风速
    vector<vector<double>> wind_angle;//存储每层的风向角度
    vector<vector<double>> wind_Rv;//存储每层的风速与V1的比值
    double xMin, xMax, xMid, yMin, yMax, yMid, zMin, zMax;//存储xyz轴的极值以及中心点
    bool ClearTXT();//清空目标txt文件
    vector<int> GetXYZIndex(string);//获取某区域的xyz下标上下限，顺序为xL,xR,yL,yR,zD,zU
    double CalculateV1Andθ(vector<vector<double>>);
    string& replace_all_distinct(string& str, const string& old_value, const string& new_value, bool& flag);
    string Trim(string str);//删除时间列中的冒号、斜杠、空格等
};

#endif /* AfterProcess_hpp */
