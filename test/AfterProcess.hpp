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
    AfterProcess(string src_path, string dest_path, vector<double> xy);//构造函数，传入csv、目标txt文件路径
    bool SaveData();//存储CSV中的数据到结果集中
    
    void SaveXYZ();//存储各区域xyz取值范围以及xyz极值
    void SaveNumOf9999();
    vector<double> CalvulateAvgWindSpeed(string);
    
    void PrintXYZDomain();
    void PrintXSpeed();
    void PrintYSpeed();
    void PrintZSpeed();
    void PrintNumOf9999();
    void PrintWindSpeed();
//    void PrintData();//打印结果集
//    bool SaveToTXT();//将结果存入txt
    
private:
    string src_path;//CSV文件路径
    string dest_path;//目标txt路径
    vector<double> x_axis, y_axis, z_axis, num_of_9999, xyz_domain;//存储风速二维数组，xx方向的坐标，y方向的坐标，z方向每层的高度，每层-9999的个数，存储设置的区域xyz范围,顺序为x1,y1,x2,y2,z0,za,z1,z2,x3,y3,z3
    vector<vector<double>> wind_speed;//存储每层的全部风速
    double xMin, xMax, xMid, yMin, yMax, yMid, zMin, zMax;//存储xyz轴的极值以及中心点
    bool ClearTXT();//清空目标txt文件
    string& replace_all_distinct(string& str, const string& old_value, const string& new_value, bool& flag);
    string Trim(string str);//删除时间列中的冒号、斜杠、空格等
};

#endif /* AfterProcess_hpp */
