//
//  AfterProcess.cpp
//  test
//
//  Created by Mustafa on 2018/10/29.
//  Copyright © 2018年 Mustafa. All rights reserved.
//

#include "AfterProcess.hpp"
#include <math.h>
#include <map>
#define PI acos(-1)
typedef pair<double, int> PAIR;
struct CmpByValue {
    bool operator()(const PAIR& lhs, const PAIR& rhs) {
        return lhs.second > rhs.second;
    }
};

//构造函数，传入csv文件路径、目标txt文件路径
AfterProcess::AfterProcess(vector<double> xy){
    vector<double>::iterator it = xy.begin();
    while (it != xy.end()) {
        xyz_domain.push_back(*it);//将传入的设置好的x,y12存储下来
        it ++;
    }
    xyz_domain.push_back(2.0);//存储z0=2m
}

//存储CSV中的数据到私有对象csv_data中
bool AfterProcess::SaveData(string src_path){
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
        bool blank_row = false;//用来标记是否遇到空白行
        vector<double> vec_speed;//记录每层的风速二维表格，每进入新的一层就清空一次
        while (getline(fin, line)){//整行读取，换行符“\n”区分，遇到文件尾标志eof终止读取
            if (flag){
//                cout<<"第一行，直接跳过"<<endl;
                flag = false;//将标记设为false，表示不是第一行
                continue;
            }
            //如果当前行为空行，直接跳过
            if ((line == "\n")||(line == "")||(line == "\r")){
//                cout<<"遇到直接空行，跳过"<<endl;
                wind_speed.push_back(vec_speed);
                count = 0;
                vec_speed.clear();
                save_xy = false;
                blank_row = false;
                continue;
            }
            else if ((line != "\n")&&(line != "")&&(line != "\r")) {//读取每层风速数值信息
                end = false;//每层终止标志设为false
                istringstream sin(line); //将整行字符串line读入到字符串流istringstream中
                string field;
                int num = 0;//记录当前列数
                //将字符串流sin中的字符读入到field字符串中，以逗号为分隔符，读取一行数据中的各列
                while (getline(sin, field, ',')){
                    if (num == 0 && field == "\0"){
//                        cout<<"第一列为空"<<endl;
                        blank_row = true;
                        num++;
                        continue;
                    }
                    else if (num == 1 && blank_row){
//                        cout<<"第一列为空，第二列‘"<<field<<"'"<<endl;
                        if (field == "\0"){
//                            cout<<"遇到空行"<<endl;
                            wind_speed.push_back(vec_speed);
                            
                            count = -1;
                            vec_speed.clear();
                            save_xy = false;
                            blank_row = false;
                            break; //遇到全是空字符的空行，直接读取下一行
                        }
                        else{
//                            cout<<"第一列为空，第二列不为空"<<endl;
                            blank_row = false;
                        }
                    }
                    if (count == 0){//第一行，记录Z的层数和高度
                        field = Trim(field);
//                        cout<<"Z行，"<<field<<"."<<endl;
                        if (num == 1){//只存取第一行第二列中每层的高度
                            z_axis.push_back(atof(field.c_str()));
                        }
                    }
                    else if (count == 2 && save_xy){//第三行，记录x轴风速
//                        cout<<"x轴行"<<endl;
                        if (num < 2){//第三行的前两列直接跳过
                            //cout<<"第三行前两列，跳过"<<endl;
                            num++;
                            continue;
                        }
                        if (atof(field.c_str()) == 0){//读到一行的末尾，直接跳出循环
                            //cout<<"到行末，读到EOF 0字符"<<endl;
                            break;
                        }
                        x_axis.push_back(atof(field.c_str()));//x轴风速存入对应数列
                    }
                    else if (count > 2){//正式数据行，需要记录y轴风速和风速矩阵
//                        cout<<"正式行"<<endl;
//                        if (atof(field.c_str()) == 0){//读到一行的末尾，直接跳出循环
//                            //cout<<"到行末，读到EOF 0字符"<<endl;
//                            break;
//                        }
                        if (num == 0){//正式行的第一列跳过
                            //cout<<"正式行第一列，跳过"<<endl;
                            num++;
                            continue;
                        }
                        else if (num == 1 && save_xy){//正式行的第二列，记录y轴风速存入对应数列，只有第一层需要存储，因为每层的xy值都相同
//                            cout<<"正式行第二列，存y轴"<<endl;
                            y_axis.push_back(atof(field.c_str()));
                        }
                        else if (num > 1){//正式行的正式列，全部存入总风速数列
//                            cout<<"正式行正式列"<<endl;
                            double speed =atof(field.c_str());
//                            cout<<count-2<<"行，"<<num-1<<"列风速数据:"<<speed<<endl;
                            vec_speed.push_back(speed);//存储每层的全部风速数据
                        }
                    }
                    num++;
                }
                count++;//记录每一层数据块的行数
//                cout<<"当前行数："<<count+1<<endl;
            }
        }
        wind_speed.push_back(vec_speed);//最后一次存储的数据存入vector中
        return true;
    }
}

//存储CSV中的X、Y轴风速
void AfterProcess::SaveXYData(string src_path_X, string src_path_Y, int col){
    cout<<"存储XY风向数据中..."<<endl;
    ifstream finX(src_path_X);
    if (!finX){
        cout<<"无法打开X文件"<<endl;//若无法打开文件，直接返回false
    }
    else{
        string line;//存储读取的每一行
        long count = 0;//记录行数
        bool flag = true;//用来控制跳过第一行的标记
        bool end = false;//用来标记是否读完文件
        bool blank_row = false;//用来标记是否遇到空白行
        vector<double> vec_speed;//记录每层的风速二维表格，每进入新的一层就清空一次
        while (getline(finX, line)){//整行读取，换行符“\n”区分，遇到文件尾标志eof终止读取
            if (flag){
                flag = false;//将标记设为false，表示不是第一行
                continue;
            }
            if ((line == "\n")||(line == "")||(line == "\r")){
//                cout<<"X文件遇到直接空行，跳过"<<endl;
                wind_speed_X.push_back(vec_speed);
                count = 0;
                vec_speed.clear();
                blank_row = false;
                continue;
            }
            else if ((line != "\n")&&(line != "")&&(line != "\r")) {//读取每层风速数值信息
                end = false;//每层终止标志设为false
                istringstream sin(line); //将整行字符串line读入到字符串流istringstream中
                string field;
                int num = 0;//记录当前列数
                while (getline(sin, field, ',')){
                    if (num == 0 && field == "\0"){
                        blank_row = true;
                        num++;
                        continue;
                    }
                    else if (num == 1 && blank_row){
                        if (field == "\0"){
                            wind_speed_X.push_back(vec_speed);
                            count = -1;
                            vec_speed.clear();
                            blank_row = false;
                            break; //遇到全是空字符的空行，直接读取下一行
                        }
                        else{
                            blank_row = false;
                        }
                    }
                    if (count > 2){//正式数据行，需要记录y轴风速和风速矩阵
                        if (num == 0){//正式行的第一列跳过
                            num++;
                            continue;
                        }
                        if (atof(field.c_str()) == 0 && num >= col){//读到一行的末尾，直接跳出循环
//                            cout<<count+1<<" 行 "<<num+1<<" 列，"<<"到行末，读到EOF 0字符"<<endl;
                            break;
                        }
                        else if (num > 1){//正式行的正式列，全部存入总风速数列
                            double speed =atof(field.c_str());
                            vec_speed.push_back(speed);//存储每层的全部风速数据
                        }
                    }
                    num++;
                }
                count++;//记录每一层数据块的行数
            }
        }
        wind_speed_X.push_back(vec_speed);//最后一次存储的数据存入vector中
    }
    
    ifstream finY(src_path_Y);
    if (!finY){
        cout<<"无法打开Y文件"<<endl;//若无法打开文件，直接返回false
    }
    else{
        string line;//存储读取的每一行
        long count = 0;//记录行数
        bool flag = true;//用来控制跳过第一行的标记
        bool end = false;//用来标记是否读完文件
        bool blank_row = false;//用来标记是否遇到空白行
        vector<double> vec_speed;//记录每层的风速二维表格，每进入新的一层就清空一次
        while (getline(finY, line)){//整行读取，换行符“\n”区分，遇到文件尾标志eof终止读取
            if (flag){
                flag = false;//将标记设为false，表示不是第一行
                continue;
            }
            if ((line == "\n")||(line == "")||(line == "\r")){
//                cout<<"Y文件遇到直接空行，跳过"<<endl;
                wind_speed_Y.push_back(vec_speed);
                count = 0;
                vec_speed.clear();
                blank_row = false;
                continue;
            }
            else if ((line != "\n")&&(line != "")&&(line != "\r")) {//读取每层风速数值信息
                end = false;//每层终止标志设为false
                istringstream sin(line); //将整行字符串line读入到字符串流istringstream中
                string field;
                int num = 0;//记录当前列数
                while (getline(sin, field, ',')){
                    if (num == 0 && field == "\0"){
                        blank_row = true;
                        num++;
                        continue;
                    }
                    else if (num == 1 && blank_row){
                        if (field == "\0"){
                            wind_speed_Y.push_back(vec_speed);
                            count = -1;
                            vec_speed.clear();
                            blank_row = false;
                            break; //遇到全是空字符的空行，直接读取下一行
                        }
                        else{
                            blank_row = false;
                        }
                    }
                    if (count > 2){//正式数据行，需要记录y轴风速和风速矩阵
                        if (num == 0){//正式行的第一列跳过
                            num++;
                            continue;
                        }
                        if (atof(field.c_str()) == 0 && num >= col){//读到一行的末尾，直接跳出循环
//                            cout<<count+1<<" 行 "<<num+1<<" 列，"<<"到行末，读到EOF 0字符"<<endl;
                            break;
                        }
                        else if (num > 1){//正式行的正式列，全部存入总风速数列
                            double speed =atof(field.c_str());
                            vec_speed.push_back(speed);//存储每层的全部风速数据
                        }
                    }
                    num++;
                }
                count++;//记录每一层数据块的行数
            }
        }
        wind_speed_Y.push_back(vec_speed);//最后一次存储的数据存入vector中
    }
}

//根据存储的风速数据计算每一层-9999的数据量
void AfterProcess::SaveNumOf9999(){
    cout<<"计算无效风速数量中..."<<endl;
    double count = 0.0;
    for(int i = 0; i < wind_speed.size(); i++){
        for(int j = 0; j < wind_speed[i].size(); j++){
            if (wind_speed[i][j] == -9999){
                count++;
            }
        }
        num_of_9999.push_back(count);
        count = 0.0;
    }
    cout<<endl;
}

//根据现有数据，存储xyz的极值并计算za,z1,z2,z3等数据
void AfterProcess::SaveXYZ(){
    cout<<"计算区域范围中..."<<endl;
    double min = 0.0, max = 0.0, za = 0.0, z1 = 0.0, z2 = 0.0;
    //存储x轴坐标极值和平均值
    for(int i = 0; i < x_axis.size(); i++){
        if (i == 0){
            min = max = x_axis[i];
        }
        else{
            if (x_axis[i] < min){
                min = x_axis[i];
            }
            if (x_axis[i] > max){
                max = x_axis[i];
            }
        }
    }
    xMin = min; xMax = max; xMid = (double)(max + min)/2;
    //存储y轴坐标极值和平均值
    for(int i = 0; i < y_axis.size(); i++){
        if (i == 0){
            min = max = y_axis[i];
        }
        else{
            if (y_axis[i] < min){
                min = y_axis[i];
            }
            if (y_axis[i] > max){
                max = y_axis[i];
            }
        }
    }
    yMin = min; yMax = max; yMid = (double)(max + min)/2;
    //存储z轴坐标极值(z3)
    for(int i = 0; i < z_axis.size(); i++){
        if (i == 0){
            min = max = z_axis[i];
        }
        else{
            if (z_axis[i] < min){
                min = z_axis[i];
            }
            if (z_axis[i] > max){
                max = z_axis[i];
            }
        }
    }
    zMin = min; zMax = max;
    //存储za,z1,z2
    for(int i = 0; i < num_of_9999.size(); i++){
        if (num_of_9999[i] != 0){
            continue;
        }
        cout<<"最高建筑高度为第"<<i<<"层"<<endl;
        z1 = z_axis[i-1];
        za = z1/2;
        z2 = 1.5 * z1;
        if (z2 > zMax){
            z2 = zMax;
        }
        break;
    }
    xyz_domain.push_back(za);
    xyz_domain.push_back(z1);
    xyz_domain.push_back(z2);
    xyz_domain.push_back(xMax-xMin);//x3
    xyz_domain.push_back(yMax-yMin);//y3
    xyz_domain.push_back(zMax);//z3
    cout<<"xMin:"<<xMin<<"\t\txMax:"<<xMax<<"\t\txMid:"<<xMid<<endl;
    cout<<"yMin:"<<yMin<<"\tyMax:"<<yMax<<"\tyMid:"<<yMid<<endl;
    cout<<"zMin:"<<zMin<<"\t\tzMax:"<<zMax<<endl<<endl;
}

//获取某区域的xyz下标上下限
vector<int> AfterProcess::GetXYZIndex(string str){
    cout<<"获取"<<str<<"区域xyz下标中..."<<endl;
    
    int x_domain = 0, y_domain = 0, z_domain = 0;
    int x_left_index = 0, x_right_index = 0, y_left_index = 0, y_right_index = 0, z_down_index = 0, z_up_index = 0;
    double valuel = 0.0, valuer = 0.0;
    vector<int> result;

    // 区域取x1,y1,z0
    if (str == "0"){
        x_domain = 0;
        y_domain = 1;
        z_domain = 4;
    }
    // 区域取x1,y1,za
    else if (str == "a"){
        x_domain = 0;
        y_domain = 1;
        z_domain = 5;
    }
    // 区域取x1,y1,z1
    else if (str == "1"){
        x_domain = 0;
        y_domain = 1;
        z_domain = 6;
    }
    // 区域取x2,y2,z2 - x1,y1,z1
    else if (str == "2"){
        x_domain = 2;
        y_domain = 3;
        z_domain = 7;
    }
    // 区域取x3,y3,z3 - x2,y2,z2
    else if (str == "3"){
        x_domain = 8;
        y_domain = 9;
        z_domain = 10;
    }
    
    // 求得x的左右下标界限
    valuel = xMid - xyz_domain[x_domain]/2; valuer = xMid + xyz_domain[x_domain]/2;
//    cout<<"x_left_value:"<<valuel<<"\t\tx_right_value:"<<valuer<<endl;
    for(int i = 0; i < x_axis.size(); i++){
        if (x_axis[i] < valuel && valuel < x_axis[i+1]){
            x_left_index = i+1;
        }
        else if (x_axis[i] == valuel){
            x_left_index = i;
        }
        if (x_axis[i] < valuer && valuer < x_axis[i+1]){
            x_right_index = i;
        }
        else if (x_axis[i] == valuer){
            x_right_index = i;
        }
    }
    // 求得y的左右下标界限
    valuel = yMid + xyz_domain[y_domain]/2; valuer = yMid - xyz_domain[y_domain]/2;
//    cout<<"y_left_value:"<<valuel<<"\ty_right_value:"<<valuer<<endl;
    for(int i = 0; i < y_axis.size(); i++){
        if (y_axis[i] == valuel){
            y_left_index = i;
        }
        else if (y_axis[i+1] < valuel && valuel < y_axis[i]){
            y_left_index = i+1;
        }
        else{
            if (y_axis[i] == valuer){
                y_right_index = i;
            }
            else if (y_axis[i+1] < valuer && valuer < y_axis[i]){
                y_right_index = i;
            }
        }
    }
    // 求得z的下标上界
    z_down_index = 0;
    for(int i = 0; i < z_axis.size(); i++){
        if (z_axis[i] < xyz_domain[z_domain] && xyz_domain[z_domain] < z_axis[i+1]){
            valuel = xyz_domain[z_domain] - z_axis[i];
            valuer = z_axis[i+1] - xyz_domain[z_domain];
            if (valuel < valuer){
                z_up_index = i;
            }
            else{
                z_up_index = i+1;
            }
        }
        else if (z_axis[i] == xyz_domain[z_domain]){
            z_up_index = i;
        }
    }
//    cout<<"z_down_value:"<<0<<"\t\t\tz_up_value:"<<xyz_domain[z_domain]<<endl;
    
    //打印调试信息
//    cout<<"xyz index scope:\t xLeft: "<<x_left_index<<"\txRight: "<<x_right_index<<"\t\tyLeft： "<<y_left_index<<"\tyRight: "<<y_right_index<<"\tzDown: "<<z_down_index<<"\tzUp: "<<z_up_index<<endl<<endl;
    
    result.push_back(x_left_index);
    result.push_back(x_right_index);
    result.push_back(y_left_index);
    result.push_back(y_right_index);
    result.push_back(z_down_index);
    result.push_back(z_up_index);
    return result;
}

//根据传入的参数，计算对应空间区域的平均风速
vector<double> AfterProcess::CalculateAvgWindSpeed(string str){
    cout<<str<<"区域，计算平均风速中..."<<endl;
    int x_left_index = 0, x_right_index = 0, y_left_index = 0, y_right_index = 0, z_down_index = 0, z_up_index = 0;
    vector<int> XYZIndex = GetXYZIndex(str);
    vector<double> vec_wind;
    map<double, int> wind_map;
    
    x_left_index = XYZIndex[0];
    x_right_index = XYZIndex[1];
    y_left_index = XYZIndex[2];
    y_right_index = XYZIndex[3];
    z_down_index = XYZIndex[4];
    z_up_index = XYZIndex[5];
    
    // 统计计算风速
    for(int i = 0; i < wind_speed.size(); i++){
        // 如果当前层数小于z的下限则直接跳过
        if (i < z_down_index){
            continue;
        }
        // 如果当前层数大于z的上限，直接结束风速计算操作
        else if (i > z_up_index){
            break;
        }
        // 当前层数在所需的范围内
        for(int j = 0; j < wind_speed[i].size(); j++){
            // 记录当前行数
            int row = j/x_axis.size();
            // 当前行数小于y的下限，跳过
            if (row < y_left_index){
                continue;
            }
            // 当前行数大于y的上限，结束本层操作
            else if (row > y_right_index){
                break;
            }
            // 当前行数在所需的范围内
            else{
                // 记录当前列数
                int col = j-(j/x_axis.size())*(int)x_axis.size();
                // 当前列数小于x的下限，跳过
                if (col < x_left_index){
                    continue;
                }
                // 当前行数大于y的上限，结束本层操作
                else if (col > x_right_index){
                    continue;
                }
                // 处于区域的层数、行数、列数中，记录风速
                else{
                    // 若风速为-9999，跳过
                    if (wind_speed[i][j] == -9999){
                        continue;
                    }
                    wind_map[wind_speed[i][j]]++;
                    vec_wind.push_back(wind_speed[i][j]);
                }
            }
        }
//        cout<<endl<<"第"<<i+1<<"层风速数据个数："<<wind_speed[i].size()<<endl;
    }
    
    // 计算风速数据个数和风速之和
    vector<double> temp;
    double wind_count = vec_wind.size(), wind_sum = 0.0;
    for (int i = 0; i < vec_wind.size(); i++){
        wind_sum += vec_wind[i];
    }
    
    // 如果为区域a，则进行风速0.1m/s间隔输出
    if (str == "a"){
        double last = wind_map.begin()->first;
        int count = 0;
        for (map<double, int>::iterator it  = wind_map.begin(); it != wind_map.end(); it++){
            if (it->first <= last+0.1){
                count += it->second;
            }
            else{
                cout<<"风速区间起始点："<<last<<"风速数量："<<count<<endl;
                last = it->first;
                count = it->second;
            }
        }
    }
    
    // 如果求区域2或区域3的平均风速，需要扣除区域1/2的平均风速
    if (str == "2"){
        temp = CalculateAvgWindSpeed("1");
        wind_count -= temp[0];
        wind_sum -= temp[1];
    }
    else if (str == "3"){
        temp = CalculateAvgWindSpeed("2");
        wind_count -= temp[0];
        wind_sum -= temp[1];
    }
    
    cout<<"风速总数量为"<<wind_count<<endl;
    cout<<str<<"区域的平均风速为"<<wind_sum/wind_count<<endl<<endl;
    
    vector<double> result;
    result.push_back(wind_count);
    result.push_back(wind_sum);
    return result;
}

//计算tan角度
void AfterProcess::CalculateAngle(){
    cout<<"计算XY风速角度中..."<<endl;
//    cout<<"size of X wind is: "<<wind_speed_X.size()<<endl;
//    cout<<"size of Y wind is: "<<wind_speed_Y.size()<<endl;
    if (wind_speed_X.size() != wind_speed_Y.size()){
        cout<<"二者数据数目不同，无法计算"<<endl;
    }
    else {
        vector<double> vec_angle;
        for (int i = 0; i < wind_speed_X.size(); i++){
            vec_angle.clear();
            for (int j = 0; j < wind_speed_X[i].size(); j++){
                double angle = -9999.0;
                if (wind_speed_X[i][j] != -9999 && wind_speed_Y[i][j] != -9999){
//                    if (wind_speed_X[i][j] == 0 && wind_speed_Y[i][j] == 0){
//                        vec_angle.push_back(angle);
//                        continue;
//                    }
                    angle = atan2(wind_speed_Y[i][j], wind_speed_X[i][j])/PI*180;
                    cout<<"XY都有数据，Y方向为: "<<wind_speed_Y[i][j]<<",\t\tX方向为: "<<wind_speed_X[i][j]<<",\t\t正切值为: "<<wind_speed_Y[i][j]/wind_speed_X[i][j]<<",\t\t角度为: "<<angle<<endl;
                }
                vec_angle.push_back(angle);
            }
            wind_angle.push_back(vec_angle);
        }
    }
}

//计算V1：外围3区域z=2最高频风速/风向角度
double AfterProcess::CalculateV1Andθ(vector<vector<double>> vec){
    cout<<"计算V1/θ中..."<<endl;
    double x_left_left_index = 0.0, x_left_right_index = 0.0, x_right_left_index = 0.0, x_right_right_index = 0.0, y_left_left_index = 0.0, y_left_right_index = 0.0, y_right_left_index = 0.0, y_right_right_index = 0.0, z_index = 0.0;
    map<double, int> wind_map;
    vector<int> indices = GetXYZIndex("3");
    x_left_left_index = indices[0];
    x_right_right_index = indices[1];
    y_left_left_index = indices[2];
    y_right_right_index = indices[3];
    indices = GetXYZIndex("2");
    x_left_right_index = indices[0];
    x_right_left_index = indices[1];
    y_left_right_index = indices[2];
    y_right_left_index = indices[3];
    indices = GetXYZIndex("0");
    z_index = indices[5];
    
    //存储指定区域内风速频率
    for (int i = 0; i < vec[z_index].size(); i++){
        // 记录当前行数
        int row = i/x_axis.size();
        // 当前行数小于y的下限，跳过
        if (row < y_left_left_index){
            continue;
        }
        // 当前行数大于y的上限，结束本层操作
        else if (row > y_right_right_index){
            break;
        }
        // 当前行数在外边框区域，直接计算
        else if ((row >= y_left_left_index && row < y_left_right_index)||(row>y_right_left_index && row <= y_right_right_index)){
            // 记录当前列数
            int col = i-(i/x_axis.size())*(int)x_axis.size();
            // 当前列数小于x的下限，跳过
            if (col < x_left_left_index){
                continue;
            }
            // 当前列数大于x的上限，结束本层操作
            else if (col > x_right_right_index){
                continue;
            }
            // 处于区域的层数、行数、列数中，记录风速
            else{
//                cout<<row<<" 行 "<<col<<" 列，存储风速 "<<wind_speed[z_index][i]<<endl;
                // 若风速为-9999，跳过
                if (vec[z_index][i] == -9999){
                    continue;
                }
                double ws = vec[z_index][i];
                wind_map[ws]++;
            }
        }
        // 当前行数在内边框区域，扣除中间x区域
        else{
            // 记录当前列数
            int col = i-(i/x_axis.size())*(int)x_axis.size();
            // 当前列数小于x的下限，跳过
            if (col < x_left_left_index){
                continue;
            }
            // 当前列数大于x的上限，结束本层操作
            else if (col > x_right_right_index){
                continue;
            }
            // 当前
            else if (col >= x_left_right_index && col <= x_right_left_index){
//                cout<<row<<" 行 "<<col<<" 列，扣除中间区域2数据"<<endl;
                continue;
            }
            // 处于区域的层数、行数、列数中，记录风速
            else{
//                cout<<row<<" 行 "<<col<<" 列，存储风速 "<<wind_speed[z_index][i]<<endl;
                // 若风速为-9999，跳过
                if (vec[z_index][i] == -9999){
                    continue;
                }
                double ws = vec[z_index][i];
                wind_map[ws]++;
            }
        }
    }
    
    //把map中元素转存到vector中
    vector<PAIR> wind_map_vec;
    for(map<double, int>::iterator it = wind_map.begin(); it != wind_map.end(); it++){
        wind_map_vec.push_back(make_pair(it->first, it->second));
    }
    //将y风速按照出现频次排序并存入vector中
    sort(wind_map_vec.begin(), wind_map_vec.end(), CmpByValue());
    //打印存储下来的风速map
//    for (vector<PAIR>::iterator it = wind_map_vec.begin(); it < wind_map_vec.end(); it++) {
//        cout<<"风速："<<it->first<<"\t频数："<<it->second<<endl;
//    }
    //找到频率最高的风速并返回
    double result = wind_map_vec.begin()->first;
    cout<<"V1的风速/θ角度为："<<result<<endl<<endl;
    return result;
}

//计算目标区域中的Rv
void AfterProcess::CalculateRv(string str){
    cout<<"计算"<<str<<"区域Rv中..."<<endl;
    vector<double> vec_wind;
    double x_left_left_index = 0.0, x_left_right_index = 0.0, x_right_left_index = 0.0, x_right_right_index = 0.0, y_left_left_index = 0.0, y_left_right_index = 0.0, y_right_left_index = 0.0, y_right_right_index = 0.0, z_index = 0.0, z_down_index = 0.0, V1 = 0.0;
    V1 = CalculateV1Andθ(wind_speed);
    
    //不用扣除，直接计算
    if (str == "0" || str == "a" || str == "1"){
        vector<int> indices = GetXYZIndex(str);
        x_left_left_index = indices[0];
        x_right_right_index = indices[1];
        y_left_left_index = indices[2];
        y_right_right_index = indices[3];
        z_index = indices[5];
        
        for(int i = 0; i < wind_speed.size(); i++){
            if (i > z_index){
                break;
            }
            for(int j = 0; j < wind_speed[i].size(); j++){
                int row = j/x_axis.size();
                if (row < y_left_left_index){
                    continue;
                }
                else if (row > y_right_right_index){
                    break;
                }
                else{
                    int col = j-(j/x_axis.size())*(int)x_axis.size();
                    if (col < x_left_left_index){
                        continue;
                    }
                    else if (col > x_right_right_index){
                        continue;
                    }
                    else{
                        if (wind_speed[i][j] == -9999){
                            vec_wind.push_back(-9999);
                            continue;
                        }
                        vec_wind.push_back(wind_speed[i][j]/V1);
                    }
                }
            }
            wind_Rv.push_back(vec_wind);
            vec_wind.clear();
        }
    }
    
    //扣除中间的区域再计算
    else if (str == "2"){
        vector<int> indices = GetXYZIndex(str);
        x_left_left_index = indices[0];
        x_right_right_index = indices[1];
        y_left_left_index = indices[2];
        y_right_right_index = indices[3];
        z_index = indices[5];
        indices = GetXYZIndex("1");
        x_left_right_index = indices[0];
        x_right_left_index = indices[1];
        y_left_right_index = indices[2];
        y_right_left_index = indices[3];
        z_down_index = indices[5];
        
        for(int i = 0; i < wind_speed.size(); i++){
            if (i > z_index){
                break;
            }
            else if (i > z_down_index && i < z_index){
                for(int j = 0; j < wind_speed[i].size(); j++){
                    int row = j/x_axis.size();
                    if (row < y_left_left_index){
                        continue;
                    }
                    else if (row > y_right_right_index){
                        break;
                    }
                    else{
                        int col = j-(j/x_axis.size())*(int)x_axis.size();
                        if (col < x_left_left_index){
                            continue;
                        }
                        else if (col > x_right_right_index){
                            continue;
                        }
                        else{
                            if (wind_speed[i][j] == -9999){
                                vec_wind.push_back(-9999);
                                continue;
                            }
                            vec_wind.push_back(wind_speed[i][j]/V1);
                        }
                    }
                }
                wind_Rv.push_back(vec_wind);
                vec_wind.clear();
            }
            // 当前层数在1区域内，需要扣除中间部分
            else {
                for (int j = 0; j < wind_speed[i].size(); j++){
                    int row = j/x_axis.size();
                    if (row < y_left_left_index){
                        continue;
                    }
                    else if (row > y_right_right_index){
                        break;
                    }
                    else if ((row >= y_left_left_index && row < y_left_right_index)||(row>y_right_left_index && row <= y_right_right_index)){
                        int col = j-(j/x_axis.size())*(int)x_axis.size();
                        if (col < x_left_left_index){
                            continue;
                        }
                        else if (col > x_right_right_index){
                            continue;
                        }
                        else{
                            if (wind_speed[i][j] == -9999){
                                vec_wind.push_back(-9999);
                                continue;
                            }
                            vec_wind.push_back(wind_speed[i][j]/V1);
                        }
                    }
                    else{
                        int col = i-(i/x_axis.size())*(int)x_axis.size();
                        if (col < x_left_left_index){
                            continue;
                        }
                        else if (col > x_right_right_index){
                            continue;
                        }
                        else if (col >= x_left_right_index && col <= x_right_left_index){
                            continue;
                        }
                        else{
                            if (wind_speed[i][j] == -9999){
                                vec_wind.push_back(-9999);
                                continue;
                            }
                            vec_wind.push_back(wind_speed[i][j]/V1);
                        }
                    }
                }
                wind_Rv.push_back(vec_wind);
                vec_wind.clear();
            }
        }
    }
    cout<<endl;
}
    
//计算目标区域中的θ2
void AfterProcess::CalculateθRatio(string str){
    cout<<"计算"<<str<<"区域θ比值中..."<<endl;
    int count = 0;
    vector<double> vec_angle;
    double x_left_left_index = 0.0, x_left_right_index = 0.0, x_right_left_index = 0.0, x_right_right_index = 0.0, y_left_left_index = 0.0, y_left_right_index = 0.0, y_right_left_index = 0.0, y_right_right_index = 0.0, z_index = 0.0, z_down_index = 0.0, theta = 0.0;
    theta = CalculateV1Andθ(wind_angle);
    
    //不用扣除，直接计算
    if (str == "0" || str == "a" || str == "1"){
        vector<int> indices = GetXYZIndex(str);
        x_left_left_index = indices[0];
        x_right_right_index = indices[1];
        y_left_left_index = indices[2];
        y_right_right_index = indices[3];
        z_index = indices[5];
        
        for(int i = 0; i < wind_angle.size(); i++){
            if (i > z_index){
                break;
            }
            for(int j = 0; j < wind_angle[i].size(); j++){
                int row = j/x_axis.size();
                if (row < y_left_left_index){
                    continue;
                }
                else if (row > y_right_right_index){
                    break;
                }
                else{
                    int col = j-(j/x_axis.size())*(int)x_axis.size();
                    if (col < x_left_left_index){
                        continue;
                    }
                    else if (col > x_right_right_index){
                        continue;
                    }
                    else{
                        if (wind_angle[i][j] == -9999){
                            continue;
                        }
                        vec_angle.push_back(wind_angle[i][j]);
                    }
                }
            }
        }
    }
    
    //扣除中间的区域再计算
    else if (str == "2"){
        vector<int> indices = GetXYZIndex(str);
        x_left_left_index = indices[0];
        x_right_right_index = indices[1];
        y_left_left_index = indices[2];
        y_right_right_index = indices[3];
        z_index = indices[5];
        indices = GetXYZIndex("1");
        x_left_right_index = indices[0];
        x_right_left_index = indices[1];
        y_left_right_index = indices[2];
        y_right_left_index = indices[3];
        z_down_index = indices[5];
        
        for(int i = 0; i < wind_angle.size(); i++){
            if (i > z_index){
                break;
            }
            else if (i > z_down_index && i < z_index){
                for(int j = 0; j < wind_angle[i].size(); j++){
                    int row = j/x_axis.size();
                    if (row < y_left_left_index){
                        continue;
                    }
                    else if (row > y_right_right_index){
                        break;
                    }
                    else{
                        int col = j-(j/x_axis.size())*(int)x_axis.size();
                        if (col < x_left_left_index){
                            continue;
                        }
                        else if (col > x_right_right_index){
                            continue;
                        }
                        else{
                            if (wind_angle[i][j] == -9999){
                                continue;
                            }
                            vec_angle.push_back(wind_angle[i][j]);
                        }
                    }
                }
            }
            // 当前层数在1区域内，需要扣除中间部分
            else {
                for (int j = 0; j < wind_angle[i].size(); j++){
                    int row = j/x_axis.size();
                    if (row < y_left_left_index){
                        continue;
                    }
                    else if (row > y_right_right_index){
                        break;
                    }
                    else if ((row >= y_left_left_index && row < y_left_right_index)||(row>y_right_left_index && row <= y_right_right_index)){
                        int col = j-(j/x_axis.size())*(int)x_axis.size();
                        if (col < x_left_left_index){
                            continue;
                        }
                        else if (col > x_right_right_index){
                            continue;
                        }
                        else{
                            if (wind_angle[i][j] == -9999){
                                continue;
                            }
                            vec_angle.push_back(wind_angle[i][j]);
                        }
                    }
                    else{
                        int col = i-(i/x_axis.size())*(int)x_axis.size();
                        if (col < x_left_left_index){
                            continue;
                        }
                        else if (col > x_right_right_index){
                            continue;
                        }
                        else if (col >= x_left_right_index && col <= x_right_left_index){
                            continue;
                        }
                        else{
                            if (wind_angle[z_index][i] == -9999){
                                continue;
                            }
                            vec_angle.push_back(wind_angle[i][j]);
                        }
                    }
                }
            }
        }
    }
    
    //计算角度上下限
    double min = 0.0, max = 0.0;
    if (theta >= 30 && theta <= 330){
        min = theta - 30;
        max = theta + 30;
    }
    else if (0 <= theta && theta < 30){
        min = theta - 30 + 360;
        max = theta + 30;
    }
    else if (330 < theta && theta <= 360){
        min = theta - 30;
        max = theta + 30 - 360;
    }
    
    //计算百分比
    if (max > min){
        for (int i = 0; i < vec_angle.size(); i++){
            if (vec_angle[i] < max && vec_angle[i] > min){
                count++;
            }
        }
    }
    else {
        for (int i = 0; i < vec_angle.size(); i++){
            if ((vec_angle[i] < max && vec_angle[i] > 0)||(vec_angle[i] < 360 && vec_angle[i] > min)){
                count++;
            }
        }
    }
    
    double result = (double)count/vec_angle.size();
    cout<<str<<"区域的标准角度为："<<theta<<",\t占比为："<<result<<endl<<endl;
}

void AfterProcess::PrintXYZDomain(){
    cout<<"打印XYZ域数据"<<endl;
//    for(int i = 0; i < xyz_domain.size(); i++)
//        cout<<xyz_domain[i]<<"\t";
    cout<<"x1:\t"<<xyz_domain[0]<<endl;
    cout<<"y1:\t"<<xyz_domain[1]<<endl;
    cout<<"x2:\t"<<xyz_domain[2]<<endl;
    cout<<"y2:\t"<<xyz_domain[3]<<endl;
    cout<<"z0:\t"<<xyz_domain[4]<<endl;
    cout<<"za:\t"<<xyz_domain[5]<<endl;
    cout<<"z1:\t"<<xyz_domain[6]<<endl;
    cout<<"z2:\t"<<xyz_domain[7]<<endl;
    cout<<"x3:\t"<<xyz_domain[8]<<endl;
    cout<<"y3:\t"<<xyz_domain[9]<<endl;
    cout<<"z3:\t"<<xyz_domain[10]<<endl<<endl;
//    cout<<endl<<endl;
}

void AfterProcess::PrintXSpeed(){
    cout<<"打印X数据"<<endl;
    for(int i = 0; i < x_axis.size(); i++)
        cout<<x_axis[i]<<"\t";
    cout<<endl<<endl;
}

void AfterProcess::PrintYSpeed(){
    cout<<"打印Y数据"<<endl;
    for(int i = 0; i < y_axis.size(); i++)
        cout<<y_axis[i]<<"\t";
    cout<<endl<<endl;
}

void AfterProcess::PrintZSpeed(){
    cout<<"打印Z数据"<<endl;
    for(int i = 0; i < z_axis.size(); i++)
        cout<<z_axis[i]<<"\t";
    cout<<endl<<endl;
}

void AfterProcess::PrintNumOf9999(){
    cout<<"打印9999数据个数"<<endl;
    for(int i = 0; i < num_of_9999.size(); i++)
        cout<<num_of_9999[i]<<"\t";
    cout<<endl<<endl;
}

void AfterProcess::PrintWindSpeed(){
    cout<<"打印风速数据中..."<<endl;
    for(int i = 0; i < wind_speed.size(); i++){
        for(int j = 0; j < wind_speed[i].size(); j++){
            cout<<wind_speed[i][j]<<"\t";
        }
        cout<<endl<<"第"<<i+1<<"层风速打印结束"<<endl;
    }
    cout<<endl;
}

void AfterProcess::PrintAngle(){
    cout<<"打印风速角度中..."<<endl;
    cout<<"风速角度列表共"<<wind_angle.size()<<"行"<<endl;
    for(int i = 0; i < wind_angle.size(); i++){
//        for(int j = 0; j < wind_angle[i].size(); j++){
//            cout<<wind_angle[i][j]<<"\t";
//        }
        cout<<endl<<"第"<<i+1<<"层风速角度打印结束, 数据量为"<<wind_angle[i].size()<<endl;
    }
    cout<<endl;
}

void AfterProcess::PrintRv(){
    cout<<"打印风速比例中..."<<endl;
    cout<<"风速比例列表共"<<wind_Rv.size()<<"行"<<endl;
    for(int i = 0; i < wind_Rv.size(); i++){
        for(int j = 0; j < wind_Rv[i].size(); j++){
            cout<<wind_Rv[i][j]<<"\t";
        }
        cout<<endl<<"第"<<i+1<<"层风速角度打印结束, 数据量为"<<wind_Rv[i].size()<<endl;
    }
    cout<<endl;
}

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

/*
 //将结果数据全部存入txt
 bool AfterProcess::SaveToTXT(){
 ClearTXT();
 ofstream outfile;
 outfile.open(dest_path, ios::binary | ios::app | ios::in | ios::out);
 if (!outfile){
 cout<<"无法输出到txt"<<endl;//无法打开文件，返回false
 return false;
 }
 vector<vector<string>>::iterator it = result_data.begin();
 while (it != result_data.end()) {
 vector<string> vec = *it;
 //存储过程
 outfile<<"Z="<<vec[0]<<", 高度="<<vec[1]<<", XMax="<<vec[2]<<", XMin="<<vec[3]<<", X平均间距="<<vec[4]<<", YMax="<<vec[5]<<", YMin="<<vec[6]<<", Y平均间距="<<vec[7]<<", 有效数据量="<<vec[8]<<"个, 平均风速="<<vec[9]<<", 方差="<<vec[10]<<", 标准差="<<vec[11]<<endl;
 it++;
 }
 outfile.close();
 cout<<"存储到txt成功"<<endl;
 return true;
 }
 
 //清空目标txt文件
 bool AfterProcess::ClearTXT(){
 ofstream f(dest_path, ios::trunc);
 if (!f ){
 cout<<"无法打开txt"<<endl;
 return false;
 }
 f.close();
 cout<<"成功清空txt"<<endl;
 return true;
 }
 */
