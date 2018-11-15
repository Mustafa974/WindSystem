//
//  BeforeProcess.cpp
//  test
//
//  Created by Mustafa on 2018/10/26.
//  Copyright © 2018年 Mustafa. All rights reserved.
//

#include "BeforeProcess.hpp"

//替换字符串
string&   replace_all_distinct(string& str, const string& old_value, const string& new_value, bool& flag){
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

//构造函数，传入csv文件路径、时间间隔、所需列号
BeforeProcess::BeforeProcess(string path1, string path2, int ts){
    src_path = path1;
    dest_path = path2;
    time_slot = ts;
}

//删除时间列中的冒号、斜杠、空格等，用于判断时间间隔
string BeforeProcess::Trim(string str){
    //str.find_first_not_of(" \t\r\n"),在字符串str中从索引0开始，返回首次不匹配"\t\r\n"的位置
    bool flag = false;
    str.erase(0,str.find_first_not_of("\t\r\n"));
    str.erase(str.find_last_not_of("\t\r\n") + 1);
    str = replace_all_distinct(str, "/", "", flag);
    str = replace_all_distinct(str, ":", "", flag);
    return str;
}

//从时间列中删去中文字符并插入AM/PM列
string BeforeProcess::TrimTime(string str, string& time){
    //str.find_first_not_of(" \t\r\n"),在字符串str中从索引0开始，返回首次不匹配"\t\r\n"的位置
    bool flag = false;
    time = "null";
    str.erase(0,str.find_first_not_of("\t\r\n"));
    str.erase(str.find_last_not_of("\t\r\n") + 1);
    str = replace_all_distinct(str, " ", "/", flag);
    str = replace_all_distinct(str, "\311\317\316\347", "", flag);
    if (flag == true){
        time = "am";
    }
    str = replace_all_distinct(str, "\317\302\316\347", "", flag);
    if (flag == true){
        time = "pm";
    }
    return str;
}

//传入风向角度，计算属于哪种风，并以string形式返回
string BeforeProcess::CalWindDir(string wd){
    float _wd = atof(wd.c_str());
    string result;
    if((_wd > 348.75 && _wd <= 360)||(_wd >= 0 && _wd <= 11.25)){
        result = "N";
    }
    else if (_wd > 11.25 && _wd <= 33.75){
        result = "NNE";
    }
    else if (_wd > 33.75 && _wd <= 56.25){
        result = "NE";
    }
    else if (_wd > 56.25 && _wd <= 78.75){
        result = "ENE";
    }
    else if (_wd > 78.75 && _wd <= 101.25){
        result = "E";
    }
    else if (_wd > 101.25 && _wd <= 123.75){
        result = "ESE";
    }
    else if (_wd > 123.75 && _wd <= 146.25){
        result = "SE";
    }
    else if (_wd > 146.25 && _wd <= 168.75){
        result = "SSE";
    }
    else if (_wd > 168.75 && _wd <= 191.25){
        result = "S";
    }
    else if (_wd > 191.25 && _wd <= 213.75){
        result = "SSW";
    }
    else if (_wd > 213.75 && _wd <= 236.25){
        result = "SW";
    }
    else if (_wd > 236.25 && _wd <= 258.75){
        result = "WSW";
    }
    else if (_wd > 258.75 && _wd <= 281.25){
        result = "W";
    }
    else if (_wd > 281.25 && _wd <= 303.75){
        result = "WNW";
    }
    else if (_wd > 303.75 && _wd <= 326.25){
        result = "NW";
    }
    else if (_wd > 326.25 && _wd <= 348.75){
        result = "NNW";
    }
    else{
        result = "ERROR";
    }
    return result;
}

//存储CSV中的数据到私有对象csv_data中
bool BeforeProcess::SaveData(){
    //打开文件
    ifstream fin(src_path);
    if (!fin){
        cout<<"无法打开文件"<<endl;//若无法打开文件，直接返回false
        return false;
    }
    else{
        string line;//存储读取的每一行
        long count = 0;//记录行数
        while (getline(fin, line)){//整行读取，换行符“\n”区分，遇到文件尾标志eof终止读取
            istringstream sin(line); //将整行字符串line读入到字符串流istringstream中
            vector<string> fields; //声明一个字符串向量
            string field;
            string time;
            int col = 0;
            while (getline(sin, field, ',')) //将字符串流sin中的字符读入到field字符串中，以逗号为分隔符
            {
                if(col == 1){
                    field = TrimTime(field, time);//删去字符串中多余的符号
                    fields.push_back(time);
                }
                fields.push_back(field); //将刚刚读取的字符串添加到向量fields中
                col++;
            }
            /*
             for(int i = 0; i < fields.size(); i++){
             cout<<fields[i]<<"\t";
             }
             */
            column = (long)fields.size();//存储CSV列数
            csv_data.push_back(fields);
            count++;
            //cout << endl;
        }
        role = count;//存储s行数
        return true;
    }
}

//打印存储的数据
void BeforeProcess::PrintData(){
    cout<<"打印数据"<<endl;
    for(int i = 0; i < csv_data.size(); i++){
        for(int j = 0; j < csv_data[i].size(); j++){
            cout<<csv_data[i][j]<<"\t";
        }
        cout<<endl;
    }
}

//清空数据
void BeforeProcess::ClearData(){
    for(int i = 0; i < csv_data.size(); i++){
        csv_data[i].clear();
    }
    csv_data.clear();
    cout<<"成功清除数据"<<endl;
}

//设置该CSV的时间间隔
void BeforeProcess::SetInterval(int col){
    vector<vector<string>>::iterator it = csv_data.begin();
    for (int i = 0; i<2; i++)
    {
        it++;//将指针指向第一行有效数据
    }
    vector<string> vec1 = *it;
    it++;
    vector<string> vec2 = *it;
    string _time;
    string str = Trim(vec1[col]);
    long time1 = atof(str.c_str());
    str = Trim(vec2[col]);
    long time2 = atof(str.c_str());
    //cout<<time1<<"\t"<<time2<<endl;
    int time = (int)(time2 - time1)/100;
    interval = time;
}

//获取时间间隔
int BeforeProcess::GetInterval(){
    return interval;
}

//计算time_slot时间间隔的风速平均值并存入txt文件
bool BeforeProcess::SaveResult(int time_col, int avg_col, int dir_col){
    cout<<"开始存储结果"<<endl;
    vector<vector<string>>::iterator it = csv_data.begin();
    int count = 0, index = 0;
    long next_time = -1, cur_time = -1;//用来标记上次的时间和当前时间
    bool time_flag = true;
    
    while (it != csv_data.end()){//每执行一轮，就计算一次time_slot时间间隔内的所需数据
        map<string, int> windMap;
        stringstream ss1, ss2;
        string time, dir;
        int max = 0;
        float sum = 0.00, result = 0.00;
        
        if (count < 2){//将指针指向第一行有效数据
            it++;
            count++;
            continue;
        }
        if(time_flag){
            long tmp = atol(Trim((*it)[time_col]).c_str());
            it++;
            long tmp_next = atol(Trim((*(it))[time_col]).c_str());
            it--;
            cur_time = tmp/10000 - (tmp/1000000)*100;
            next_time = tmp_next/10000 - (tmp_next/1000000)*100;
            cout<<"current:"<<cur_time<<", next:"<<next_time<<endl;
            if (cur_time == next_time){
                cout<<"下一个不是整点，跳过"<<endl;
                it++;
                continue;//如果当前时刻不是整点
            }
            else{
                time_flag = false;
                cout<<"下一个是整点"<<endl;
                it++;
                continue;
            }
        }
        time = (*it)[time_col]+" "+(*it)[1];//标记当前一轮time_slot的开始时间
        sum = 0;
        count = 0;
        for(int i = 0; i < time_slot/interval; i++){
            if (it == csv_data.end()){
                break;//如果当前遍历到数据向量结尾，则跳出循环
            }
            sum += atof((*it)[avg_col].c_str());//累加时间间隔内的总风速
            dir = CalWindDir((*it)[dir_col]);//将风向角度转换为相应字符
            windMap[dir]++;
            it++;
            count++;
        }
        if(count < time_slot/interval){
            break;//如果最后的数据量不足一小时，则直接跳过b无需计算
        }
        result = sum/(float)(count);//计算平均结果
        ss1<<result;//float转string
        for(map<string, int>::iterator it0=windMap.begin(); it0!=windMap.end(); it0++){//获取map中12条数据中所有风向出现频率最高的一条
            //cout<<it->first<<"\t"<<it->second<<endl;
            if(it0->second > max){//it->first得到key，it->second得到value
                max = it0->second;
                dir = it0->first;
            }
        }
        vector<string> vec;
        vec.push_back(time);//插入该条数据的时间
        vec.push_back(ss1.str());//插入平均数结果
        vec.push_back(dir);//插入最高频风向
        ss2<<max;//int转string
        vec.push_back(ss2.str());//插入最高频风向的出现次数
//        for (vector<string>::iterator it = vec.begin(); it != vec.end(); it++){
//            cout<<*it<<"\t";
//        }
//        cout<<endl;
        result_data.push_back(vec);
        //cout<<time<<"\t"<<ss1.str()<<"\t"<<dir<<"\t"<<ss2.str()<<endl;
        index++;//存完一条结果数据后，下标加一
    }
//    PrintResultVec();
    return true;
}

//将结果数据全部存入txt
bool BeforeProcess::SaveToTXT(){
    ofstream outfile;
    outfile.open(dest_path, ios::binary | ios::app | ios::in | ios::out);
    if (!outfile){
        cout<<"无法输出到txt"<<endl;//无法打开文件，返回false
        return false;
    }
    vector<vector<string>>::iterator it = result_data.begin();
    while (it != result_data.end()) {
        vector<string> vec = *it;
        outfile<<"Time："<<vec[0]<<",\t"<<time_slot<<"分钟内 风速平均值："<<vec[1]<<",\t最常见风向："<<vec[2]<<",\t出现次数："<<vec[3]<<endl;//遍历结果vector,并依次输出到txt
        it++;
    }
    outfile.close();
    cout<<"存储到txt成功"<<endl;
    return true;
}

//清空目标txt文件
bool BeforeProcess::ClearTXT(){
    ofstream f(dest_path, ios::trunc);
    if (!f ){
        cout<<"无法打开txt"<<endl;
        return false;
    }
    f.close();
    cout<<"成功清空txt"<<endl;
    return true;
}

//打印结果
void BeforeProcess::PrintResultVec(){
    for(vector<vector<string>>::iterator it = result_data.begin(); it!=result_data.end(); it++){
        for(vector<string>::iterator itt = (*it).begin(); itt!=(*it).end(); itt++){
            cout<<*itt<<"\t";
        }
        cout<<endl;
    }
    cout<<endl;
}
