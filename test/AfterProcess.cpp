//
//  AfterProcess.cpp
//  test
//
//  Created by Mustafa on 2018/10/29.
//  Copyright © 2018年 Mustafa. All rights reserved.
//

#include "AfterProcess.hpp"


//构造函数，传入csv文件路径、目标txt文件路径
AfterProcess::AfterProcess(string path1, string path2, vector<float> xy){
    src_path = path1;
    dest_path = path2;
    vector<float>::iterator it = xy.begin();
    while (it != xy.end()) {
        xyz_domain.push_back(*it);//将传入的设置好的x,y12存储下来
        it ++;
    }
    xyz_domain.push_back(2.0);//存储z0=2m
}

//存储CSV中的数据到私有对象csv_data中
bool AfterProcess::SaveData(){
    //打开文件
    ifstream fin(src_path);
    if (!fin){
        cout<<"无法打开文件"<<endl;//若无法打开文件，直接返回false
        return false;
    }
    else{
        string line;//存储读取的每一行
        long count = 0;//记录行数
        bool flag = true;//用来控制跳过第一行的标记
        bool end = false;//用来标记是否读完文件
        bool save_xy = true;//用来标记是否需要存储xy值
        vector<double> vec_speed, vec_z;//记录每层的风速二维表格，每进入新的一层就清空一次
        while (getline(fin, line)){//整行读取，换行符“\n”区分，遇到文件尾标志eof终止读取
            if (flag){
                //cout<<"第一行，直接跳过"<<endl;
                flag = false;//将标记设为false，表示不是第一行
                continue;
            }
            if ((line != "\n")&&(line != "")&&(line != "\r")) {//读取每层风速数值信息
                end = false;
                istringstream sin(line); //将整行字符串line读入到字符串流istringstream中
                string field;
                int num = 0;//记录当前列数
                //将字符串流sin中的字符读入到field字符串中，以逗号为分隔符，读取一行数据中的各列
                while (getline(sin, field, ',')){
                    if (count == 0){//第一行，记录Z的层数和高度
                        field = Trim(field);
                        //cout<<"Z行，"<<field<<endl;
                        vec_z.push_back(atof(field.c_str()));//每一层都存储一下层数和高度，存入结果集中，每层结束后清空
                        if (num == 1){//只存取第一行第二列中每层的高度
                            z_speed.push_back(atof(field.c_str()));
                        }
                    }
                    else if (count == 2 && save_xy){//第三行，记录x轴风速
                        //cout<<"第三行"<<endl;
                        if (num < 2){//第三行的前两列直接跳过
                            //cout<<"第三行前两列，跳过"<<endl;
                            num++;
                            continue;
                        }
                        if (atof(field.c_str()) == 0){//读到一行的末尾，直接跳出循环
                            //cout<<"到行末，读到EOF 0字符"<<endl;
                            break;
                        }
                        x_speed.push_back(atof(field.c_str()));//x轴风速存入对应数列
                    }
                    else if (count > 2){//正式数据行，需要记录y轴风速和风速矩阵
                        //cout<<"正式行"<<endl;
                        if (atof(field.c_str()) == 0){//读到一行的末尾，直接跳出循环
                            //cout<<"到行末，读到EOF 0字符"<<endl;
                            break;
                        }
                        if (num == 0){//正式行的第一列跳过
                            //cout<<"正式行第一列，跳过"<<endl;
                            num++;
                            continue;
                        }
                        else if (num == 1 && save_xy){//正式行的第二列，记录y轴风速存入对应数列，只有第一层需要存储，因为每层的xy值都相同
                            //cout<<"正式行第二列，存y轴"<<endl;
                            y_speed.push_back(atof(field.c_str()));
                        }
                        else{//正式行的正式列，全部存入总风速数列
                            //cout<<"正式行正式列"<<endl;
                            double speed =atof(field.c_str());
                            vec_speed.push_back(speed);//存储每层的全部风速数据
                        }
                    }
                    num++;
                }
                count++;//记录每一层数据块的行数
                //cout<<"当前行数："<<count+1<<endl;
            }
            else{//遇到空行，表示换层，清空vectors
                //cout<<"遇到空行"<<endl;
                if (end == true){//如果连续多行空行则一直跳过，直到读到文件末尾
                    continue;
                }
                end = true;
//                vector<string> vec;
//                stringstream ss1;
//                ss1<<vec_z[0];
//                vec.push_back(ss1.str());
//                stringstream ss2;
//                ss2<<vec_z[1];
//                vec.push_back(ss2.str());
//
//                MathFormula mf1(x_speed);
//                stringstream ss3;
//                ss3<<mf1.Max();
//                vec.push_back(ss3.str());
//                stringstream ss4;
//                ss4<<mf1.Min();
//                vec.push_back(ss4.str());
//                stringstream ss5;
//                ss5<<mf1.AvgGap();
//                vec.push_back(ss5.str());
//
//                MathFormula mf2(y_speed);
//                stringstream ss6;
//                ss6<<mf2.Max();
//                vec.push_back(ss6.str());
//                stringstream ss7;
//                ss7<<mf2.Min();
//                vec.push_back(ss7.str());
//                stringstream ss8;
//                ss8<<mf2.AvgGap();
//                vec.push_back(ss8.str());
//
//                MathFormula mf3(vec_speed);
//                stringstream ss9;
//                ss9<<wind_speed.size();
//                vec.push_back(ss9.str());
//                stringstream ss10;
//                ss10<<mf3.Mean();
//                vec.push_back(ss10.str());
//                stringstream ss11;
//                ss11<<mf3.Variance();
//                vec.push_back(ss11.str());
//                stringstream ss12;
//                ss12<<mf3.StandardDeviation();
//                vec.push_back(ss12.str());
                
//                result_data.push_back(vec);
                wind_speed.push_back(vec_speed);
                
                count = 0;
                wind_speed.clear();
                vec_z.clear();
                save_xy = false;
                continue;
            }
        }
        return true;
    }
}

//打印存储的数据
//void AfterProcess::PrintData(){
//    cout<<"打印数据"<<endl;
//    for(int i = 0; i < result_data.size(); i++){
//        for(int j = 0; j < result_data[i].size(); j++){
//            cout<<result_data[i][j]<<"\t";
//        }
//        cout<<endl;
//    }
//}
//
////清空数据
//void AfterProcess::ClearData(){
//    for(int i = 0; i < result_data.size(); i++){
//        result_data[i].clear();
//    }
//    result_data.clear();
//    cout<<"成功清除数据"<<endl;
//}
//
////将结果数据全部存入txt
//bool AfterProcess::SaveToTXT(){
//    ClearTXT();
//    ofstream outfile;
//    outfile.open(dest_path, ios::binary | ios::app | ios::in | ios::out);
//    if (!outfile){
//        cout<<"无法输出到txt"<<endl;//无法打开文件，返回false
//        return false;
//    }
//    vector<vector<string>>::iterator it = result_data.begin();
//    while (it != result_data.end()) {
//        vector<string> vec = *it;
//        //存储过程
//        outfile<<"Z="<<vec[0]<<", 高度="<<vec[1]<<", XMax="<<vec[2]<<", XMin="<<vec[3]<<", X平均间距="<<vec[4]<<", YMax="<<vec[5]<<", YMin="<<vec[6]<<", Y平均间距="<<vec[7]<<", 有效数据量="<<vec[8]<<"个, 平均风速="<<vec[9]<<", 方差="<<vec[10]<<", 标准差="<<vec[11]<<endl;
//        it++;
//    }
//    outfile.close();
//    cout<<"存储到txt成功"<<endl;
//    return true;
//}
//
////清空目标txt文件
//bool AfterProcess::ClearTXT(){
//    ofstream f(dest_path, ios::trunc);
//    if (!f ){
//        cout<<"无法打开txt"<<endl;
//        return false;
//    }
//    f.close();
//    cout<<"成功清空txt"<<endl;
//    return true;
//}

//替换字符串
string& AfterProcess::replace_all_distinct(string& str, const string& old_value, const string& new_value, bool& flag){
    flag = false;
    for(string::size_type pos(0); pos!=string::npos; pos+=new_value.length()){
        if((pos=str.find(old_value,pos))!=string::npos){
            str.replace(pos,old_value.length(),new_value);
            flag = true;
        }
        else break;
    }
    return str;
}

//删除时间列中的冒号、斜杠、空格等
string AfterProcess::Trim(string str){
    //str.find_first_not_of(" \t\r\n"),在字符串str中从索引0开始，返回首次不匹配"\t\r\n"的位置
    bool flag = false;
    str.erase(0,str.find_first_not_of("\t\r\n"));
    str.erase(str.find_last_not_of("\t\r\n") + 1);
    str = replace_all_distinct(str, " ", "", flag);
    str = replace_all_distinct(str, "*", "", flag);
    str = replace_all_distinct(str, "=", "", flag);
    str = replace_all_distinct(str, "Z", "", flag);
    return str;
}
