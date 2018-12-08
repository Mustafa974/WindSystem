//
//  main.cpp
//  test
//
//  Created by Mustafa on 2018/10/25.
//  Copyright © 2018年 Mustafa. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "BeforeProcess.hpp"
#include "AfterProcess.hpp"
typedef pair<string, int> PAIR2;

using namespace std;


int main(int argc, const char * argv[]) {
//    BeforeProcess manager("/Users/mustafa/Desktop/城规研究室/风计算/beforeProcess.csv","bpResult.txt", 60);
//    manager.ClearData();
//    manager.SaveData();
//    cout<<"存储数据结束"<<endl;
//    manager.PrintData();
//    manager.SetInterval(2);
//    cout<<"设置时间间隔结束"<<endl;
//    manager.SaveResult(2, 3, 5);
//    manager.ClearTXT();
//    manager.SaveToTXT();
    
    double x1 = 194.0, y1 = 112.0, x2 = 271.6, y2 = 198.6;
    vector<double> vec;
    vector<string> result;//结果列表
    vector<vector<string>> print;//用于打印到txt的列表
    vec.push_back(x1);
    vec.push_back(y1);
    vec.push_back(x2);
    vec.push_back(y2);
    
    AfterProcess managerA(vec);
    managerA.SaveData("/Users/mustafa/Desktop/城规研究室/后处理任务2/big_csv/6A3_0.csv");
    managerA.PrintXSpeed();
    managerA.PrintYSpeed();
    managerA.PrintZSpeed();
    
    managerA.SaveNumOf9999();
    managerA.PrintNumOf9999();
    
    managerA.SaveXYZ();
    managerA.PrintXYZDomain();
    
    managerA.SaveXYData("/Users/mustafa/Desktop/城规研究室/后处理任务2/big_csv/6A3_X.csv", "/Users/mustafa/Desktop/城规研究室/后处理任务2/big_csv/6A3_Y.csv", 240);
    managerA.SaveAngle();
    
    //存储结果
    for (int i = 1; i < 4; i++){
        result.clear();
        vec = managerA.CalculateAvgWindSpeed("1", i);
        //将每一高度区域的风速个数、风速总和、静风个数存入result
        stringstream ss1, ss2;
        ss1<<vec[1]/vec[0];
        ss2<<vec[2]/vec[1];
        result.push_back(ss1.str());
        result.push_back(ss2.str());
        //将该区域的V1和RV均值存入结果列表
        vec.clear();
        vec = managerA.CalculateAvgRv("1", i);
        for (int j = 0; j < vec.size(); j++){
            stringstream ss;
            ss<<vec[j];
            result.push_back(ss.str());
        }
        vec.clear();
        vec = managerA.CalculateθRatio("1", i);
        for (int j = 0; j < vec.size(); j++){
            stringstream ss;
            ss<<vec[j];
            result.push_back(ss.str());
        }
        vector<PAIR2> map = managerA.CalculateWindAngle("1", i, 3);
        int sum = 0;
        double frequency = 0.0;
        for (int j = 0; j < map.size(); j++){
            if (map[j].first == "DATA"){
                sum = map[j].second;
            }
            cout<<map[j].first<<"\t"<<map[j].second<<endl;
        }
        for (int j = 0; j < 3; j++){
            stringstream ss;
            result.push_back(map[j].first);
            frequency = (double)map[j].second/(double)sum;
            ss<<frequency;
            result.push_back(ss.str());
        }
        cout<<endl;
        print.push_back(result);
    }
    //打印结果到txt
    managerA.SaveToTXT(print);
    
    /*
     result:
     0：风速个数
     1：风速总和
     2：静风个数
     3：V1值
     4：Rv均值
     5：θ1值
     6：θ2比例
     7：第一高频风向角
     8：第一高频风向频率
     9：第二高频风向角
     10：第二高频风向频率
     11：第三高频风向角
     12：第三高频风向频率
     */
    
    return 0;
}
