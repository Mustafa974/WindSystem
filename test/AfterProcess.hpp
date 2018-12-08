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
typedef pair<double, int> PAIR;
typedef pair<string, int> PAIR2;

using namespace std;

//构造后处理的类
class AfterProcess{
    
public:
    AfterProcess(vector<double> xy);//构造函数，传入z设置好的x1,y1,x2,y2
    bool SaveData(string src_path);//存储CSV中的数据到结果集中
    void SaveXYData(string src_path_X, string src_path_Y, int col);//存储XY方向的速度分量
    void SaveXYZ();//存储各区域xyz取值范围以及xyz极值
    void SaveNumOf9999();//存储数据中没吃呢个-9999的个数
    void SaveAngle();//计算XY风向角，并存储在私有成员变量里
    vector<double> CalculateAvgWindSpeed(string str, int z);//计算指定区域和指定高度内的平均风速，并返回一个vector，存储风速总个数、风速总和、静风数量
    vector<double> CalculateAvgRv(string, int);
    vector<double> CalculateθRatio(string, int);
    vector<PAIR2> CalculateWindAngle(string, int, int);
    
    void PrintXYZDomain();
    void PrintXSpeed();
    void PrintYSpeed();
    void PrintZSpeed();
    void PrintNumOf9999();
    void PrintWindSpeed();
    void PrintAngle();
    bool SaveToTXT(vector<vector<string>>);//将结果存入txt
    
private:
    string dest_path;
    vector<double> x_axis, y_axis, z_axis, num_of_9999, xyz_domain, z_vec;//存储风速二维数组，xx方向的坐标，y方向的坐标，z方向每层的高度，每层-9999的个数，存储设置的区域xyz范围,顺序为x1,y1,x2,y2,z0,za,z1,z2,x3,y3,z3;z_vec存储用户预设的三个区域的z上下限高度
    vector<vector<double>> wind_speed;//存储每层的全部风速
    vector<vector<double>> wind_speed_X;//存储X轴每层的全部风速
    vector<vector<double>> wind_speed_Y;//存储Y轴每层的全部风速
    vector<vector<double>> wind_angle;//存储每层的风向角度
    double xMin, xMax, xMid, yMin, yMax, yMid, zMin, zMax;//存储xyz轴的极值以及中心点
    bool ClearTXT();//清空目标txt文件
    vector<int> GetXYZIndex(string);//获取某区域的xyz下标上下限，顺序为xL,xR,yL,yR,zD,zU
    vector<double> GetDomainValue(vector<vector<double>>,string, int);//获取指定区域内指定高度的对应数值列表
    int GetZIndex(double z, int type);
    vector<PAIR> GetFrequency(vector<double>);//计算指定区域和指定高度的风速0.1m/s间隔输出，并返回该Map
    vector<PAIR2> GetAngleFrequency(vector<double>);//计算风向角度频率并返回map
    int CalcuCalmWind(vector<double>);
    double CalculateV1(int z);//计算整个空间指定高度内的最高频风速，精确到小数点后两位
    double Calculateθ1(int z);//计算3区域减去2区域内的最高频风速θ1
    string CalWindDir(double _wd);
    string& replace_all_distinct(string& str, const string& old_value, const string& new_value, bool& flag);
    string Trim(string str);//删除时间列中的冒号、斜杠、空格等
};

#endif /* AfterProcess_hpp */
