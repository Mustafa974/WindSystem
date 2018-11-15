//
//  BeforeProcess.hpp
//  test
//
//  Created by Mustafa on 2018/10/26.
//  Copyright © 2018年 Mustafa. All rights reserved.
//

#ifndef BeforeProcess_hpp
#define BeforeProcess_hpp

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

using namespace std;


//构造前处理的类
class BeforeProcess{
    
public:
    BeforeProcess(string src_path, string dest_path, int ts);//构造函数，传入csv文件路径、时间间隔
    bool SaveData();//存储CSV中的数据到私有对象csv_data中
    void PrintData();//打印存储的数据
    void ClearData();//清空数据
    void SetInterval(int col);//设置该CSV的时间间隔，参数为时间所在列号
    int GetInterval();//获取时间间隔
    bool SaveResult(int time_col, int avg_col, int dir_col);//计算time_slot时间间隔的某列数据平均值并存入txt文件，参数分别为时间所在列号、目标列号
    bool SaveToTXT();//将结果数据全部存入txt
    bool ClearTXT();//清空目标txt文件
    
private:
    string src_path;//CSV文件路径
    string dest_path;//目标txt路径
    int time_slot;//设定要获取的时间间隔
    long column;//CSV列数
    long role;//CSV行数
    int interval;//CSV中的时间间隔
    vector<vector<string>> csv_data;//读取CSV获得的数据
    vector<vector<string>> result_data;//存储要打印到txt的数据
    string Trim(string str);//私有函数，删除时间列中的冒号、斜杠、空格等，用于判断时间间隔
    string TrimTime(string str, string& time);//从时间列中删去中文字符并插入AM/PM列
    string CalWindDir(string wd);
    void PrintResultVec();//打印结果
};

#endif /* BeforeProcess_hpp */
