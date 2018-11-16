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

using namespace std;


int main(int argc, const char * argv[]) {
//    BeforeProcess manager("/Users/mustafa/Desktop/城规研究室/风计算/beforeProcess.csv","/Users/mustafa/Desktop/城规研究室/风计算/bpResult.txt", 60);
////    manager.ClearData();
//    manager.SaveData();
//    cout<<"存储数据结束"<<endl;
////    manager.PrintData();
//    manager.SetInterval(2);
//    cout<<"设置时间间隔结束"<<endl;
//    manager.SaveResult(2, 3, 5);
//    manager.ClearTXT();
//    manager.SaveToTXT();
    
    double x1 = 184.0, y1 = 120.0, x2 = 257.2, y2 = 193.2;
    vector<double> vec;
    vec.push_back(x1);
    vec.push_back(y1);
    vec.push_back(x2);
    vec.push_back(y2);
    AfterProcess managerA("/Users/mustafa/Desktop/城规研究室/后处理任务2/Wind.CSV", "/Users/mustafa/Desktop/城规研究室/后处理任务2/Wind.CSV/apResult.txt",vec);
    managerA.SaveData();
    managerA.PrintXSpeed();
    managerA.PrintYSpeed();
    managerA.PrintZSpeed();
    managerA.SaveNumOf9999();
    managerA.PrintNumOf9999();
    managerA.SaveXYZ();
    managerA.PrintXYZDomain();
    managerA.CalvulateAvgWindSpeed("0");
    managerA.CalvulateAvgWindSpeed("a");
    managerA.CalvulateAvgWindSpeed("1");
    managerA.CalvulateAvgWindSpeed("2");
    managerA.CalvulateAvgWindSpeed("3");
//    managerA.PrintWindSpeed();
//    managerA.SaveToTXT();
    
    return 0;
}
