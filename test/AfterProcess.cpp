//
//  AfterProcess.cpp
//  test
//
//  Created by Mustafa on 2018/10/29.
//  Copyright © 2018年 Mustafa. All rights reserved.
//

#include "AfterProcess.hpp"
#include "BeforeProcess.hpp"
#include <math.h>
#include <DirectoryService/DirectoryService.h>
#include <map>
#define PI acos(-1)
typedef pair<double, int> PAIR;
typedef pair<string, int> PAIR2;
struct CmpByValue {
    bool operator()(const PAIR& lhs, const PAIR& rhs) {
        return lhs.second > rhs.second;
    }
};
struct CmpByValue2 {
    bool operator()(const PAIR2& lhs, const PAIR2& rhs) {
        return lhs.second > rhs.second;
    }
};

//构造函数，传入csv文件路径、目标txt文件路径
AfterProcess::AfterProcess(vector<double> xy){
    dest_path = ".\\result.txt";
    vector<double>::iterator it = xy.begin();
    while (it != xy.end()) {
        xyz_domain.push_back(*it);//将传入的设置好的x,y12存储下来
        it ++;
    }
    xyz_domain.push_back(2.0);//存储z0=2m
    //把预设的高度范围存储下来（有重复值，但为了方便后续更改，先暂时这么设置）
    z_vec.push_back(0.5);
    z_vec.push_back(2.5);
    z_vec.push_back(5.0);
    z_vec.push_back(10.0);
    z_vec.push_back(0.5);
    z_vec.push_back(10.0);
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

//计算tan角度,结果存入wind_angle向量中，计算角度时，将风速小于0.2的点的角度设置为-9999
void AfterProcess::SaveAngle(){
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
//                if (sqrt(wind_speed_X[i][j]*wind_speed_X[i][j]+wind_speed_Y[i][j]*wind_speed_Y[i][j]) < 0.2){
//
//                    angle = -9999.0;
//                }
                if (wind_speed_X[i][j] != -9999 && wind_speed_Y[i][j] != -9999 && wind_speed[i][j] > 0.2){
                    angle = atan2(wind_speed_Y[i][j], wind_speed_X[i][j])/PI*180;
                    if (angle < 0){
//                        cout<<"不正常角度"<<angle<<"\ti:"<<i<<",j:"<<j<<endl;
                        angle += 360;
                    }
//                    cout<<"XY都有数据，Y方向为: "<<wind_speed_Y[i][j]<<",\t\tX方向为: "<<wind_speed_X[i][j]<<",\t\t正切值为: "<<wind_speed_Y[i][j]/wind_speed_X[i][j]<<",\t\t角度为: "<<angle<<endl;
                }
                vec_angle.push_back(angle);
            }
            wind_angle.push_back(vec_angle);
        }
    }
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

// 根据z的值求得z的下标，如果传入的type参数为0，则表示当前为z的下界，若为1则求上界
int AfterProcess::GetZIndex(double value, int type){
    int z_index = -1;
    for(int i = 0; i < z_axis.size(); i++){
        if (z_axis[i] < value && value < z_axis[i+1]){
            if (type == 0){
                z_index = i+1;
            }
            else if (type == 1){
                z_index = i;
            }
        }
        else if (z_axis[i] == value){
            z_index = i;
        }
    }
    return z_index;
}

//从传入的vector中找到全部风速小于0.2的静风并返回个数
int AfterProcess::CalcuCalmWind(vector<double> vec){
    int count = 0;
    for (int i = 0; i < vec.size(); i++){
        if (vec[i] < 0.2){
            count++;
        }
    }
    return count;
}

//获取指定区域内指定高度的对应数值列表
vector<double> AfterProcess::GetDomainValue(vector<vector<double>> vec,string str, int z){
    cout<<str<<"区域，获取数值中..."<<endl;
    // xyz的上下界下标
    int x_left_index = 0, x_right_index = 0, y_left_index = 0, y_right_index = 0, z_down_index = 0, z_up_index = 0;
    vector<int> XYZIndex = GetXYZIndex(str);
    vector<double> result;
    
    x_left_index = XYZIndex[0];
    x_right_index = XYZIndex[1];
    y_left_index = XYZIndex[2];
    y_right_index = XYZIndex[3];
    if (z == 1){
        z_down_index = GetZIndex(z_vec[0], 0);
        z_up_index = GetZIndex(z_vec[1], 1);
    }
    else if (z == 2){
        z_down_index = GetZIndex(z_vec[2], 0);
        z_up_index = GetZIndex(z_vec[3], 1);
    }
    else if (z == 3){
        z_down_index = GetZIndex(z_vec[4], 0);
        z_up_index = GetZIndex(z_vec[5], 1);
    }
    cout<<"xLeftIndex:"<<x_left_index<<endl
    <<"xRightIndex:"<<x_right_index<<endl
    <<"yLeftIndex:"<<y_left_index<<endl
    <<"yRightIndex:"<<y_right_index<<endl
    <<"zDownIndex:"<<z_down_index<<endl
    <<"zUpIndex:"<<z_up_index<<endl;
    
    
    // 统计计算风速
    for(int i = 0; i < vec.size(); i++){
        // 如果当前层数小于z的下限则直接跳过
        if (i < z_down_index){
            continue;
        }
        // 如果当前层数大于z的上限，直接结束风速计算操作
        else if (i > z_up_index){
            break;
        }
        // 当前层数在所需的范围内
        for(int j = 0; j < vec[i].size(); j++){
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
                // 当前行数大于x的上限，结束本层操作
                else if (col > x_right_index){
                    continue;
                }
                // 处于区域的层数、行数、列数中，记录风速
                else{
                    // 若风速为-9999，跳过
                    if (vec[i][j] == -9999){
                        continue;
                    }
                    //将所需风速存入vector中
                    result.push_back(vec[i][j]);
                }
            }
        }
//        cout<<endl<<"第"<<i+1<<"层风速数据个数："<<wind_speed[i].size()<<endl;
    }
    return result;
}

//根据传入的参数，计算对应空间区域的平均风速，str表示要计算的区域（a,1,2等）,z表示要计算的z区域, 函数返回的vector中只有两项，第一项为风速个数，第二项为风速总和，第三项为静风个数
vector<double> AfterProcess::CalculateAvgWindSpeed(string str, int z){
    // 获取对应区域的风速列表
    vector<double> temp = GetDomainValue(wind_speed, str, z);
    double wind_count = temp.size(), wind_sum = 0.0;
    //计算风速总和
    for (int i = 0; i < wind_count; i++){
        wind_sum += temp[i];
    }
    //获取风速列表中的静风数量
    double calmWind = CalcuCalmWind(temp);
    // 如果求区域2或区域3的平均风速，需要扣除区域1/2的平均风速
    if (str == "2"){
        temp = CalculateAvgWindSpeed("1", z);
        wind_count -= temp[0];
        wind_sum -= temp[1];
        calmWind -= temp[2];
    }
    else if (str == "3"){
        temp = CalculateAvgWindSpeed("2", z);
        wind_count -= temp[0];
        wind_sum -= temp[1];
        calmWind -= temp[2];
    }
    
    cout<<"风速总数量为"<<wind_count<<endl;
    cout<<str<<"区域的平均风速为"<<wind_sum/wind_count<<endl<<endl;
    //结果vector，存入所需数据并返回
    vector<double> result;
    result.push_back(wind_count);//存储风速个数
    result.push_back(wind_sum);//存储风速数值总和
    result.push_back(calmWind);//存储静风个数
    return result;
}

vector<PAIR> AfterProcess::GetFrequency(vector<double> temp){
    cout<<"获取风速/角度频率中..."<<endl;
    map<double, int> wind_map; //用于存储风速和频率的map
    for (int i = 0 ;i < temp.size(); i++){
        wind_map[temp[i]]++;
    }
    //把map中元素转存到vector<<double, int>>中
    vector<PAIR> wind_map_vec;
    for(map<double, int>::iterator it = wind_map.begin(); it != wind_map.end(); it++){
        wind_map_vec.push_back(make_pair(it->first, it->second));
    }
    //将风速按照出现频次排序并存入vector中
    sort(wind_map_vec.begin(), wind_map_vec.end(), CmpByValue());
//    for (vector<PAIR>::iterator it = wind_map_vec.begin(); it < wind_map_vec.end(); it++) {
//        cout<<"风速："<<it->first<<"\t频数："<<it->second<<endl;
//    }
    return wind_map_vec;
}

vector<PAIR2> AfterProcess::GetAngleFrequency(vector<double> temp){
    cout<<"获取风速/角度频率中..."<<endl;
    map<string, int> wind_map; //用于存储风速和频率的map
    string angle;
    for (int i = 0 ;i < temp.size(); i++){
        if (temp[i] != -9999){
            angle = CalWindDir(temp[i]);
            wind_map[angle]++;
        }
    }
    //把map中元素转存到vector<<double, int>>中
    vector<PAIR2> wind_map_vec;
    for(map<string, int>::iterator it = wind_map.begin(); it != wind_map.end(); it++){
        wind_map_vec.push_back(make_pair(it->first, it->second));
    }
    //将风速按照出现频次排序并存入vector中
    sort(wind_map_vec.begin(), wind_map_vec.end(), CmpByValue2());
//    for (vector<PAIR2>::iterator it = wind_map_vec.begin(); it < wind_map_vec.end(); it++) {
//            cout<<"风向角："<<it->first<<"\t频数："<<it->second<<endl;
//    }
    return wind_map_vec;
}

//计算V1：z代表要求的高度域，有1、2、3三种
double AfterProcess::CalculateV1(int z){
    cout<<"计算V1中..."<<endl;
    vector<double> vec = GetDomainValue(wind_speed, "3", z), wind_value;
    cout<<"成功获取3区域风速值"<<endl;
    for (int i = 0; i < vec.size(); i++){
        if (vec[i]<0.2){
            continue;//如果风速小于0.2，直接跳过
        }
        //四舍五入，保存两位小数
        double temp = floor(vec[i] * 100.000f + 0.5) / 100.000f;
        wind_value.push_back(temp);
    }
    //计算1区域中z范围高度内的风速/风向角度的频率
    vector<PAIR> wind_map = GetFrequency(wind_value);
    //找到频率最高的风速并返回
    vector<PAIR>::iterator it = wind_map.begin();
    cout<<"V1的风速为："<<it->first<<"，出现次数为"<<it->second<<endl<<endl;
    return it->first;
}

//计算目标区域中的Rv
vector<double> AfterProcess::CalculateAvgRv(string str, int z){
    cout<<"计算"<<str<<"区域Rv中..."<<endl;
    vector<double> vec_wind;
    double sum = 0.0, V1 = CalculateV1(z), avg = 0.0;
    
    //不用扣除，直接计算，返回均值结果
    if (str == "0" || str == "a" || str == "1"){
        //获取对应区域对应高度的风速列表
        vec_wind = GetDomainValue(wind_speed, str, z);
        //计算RV风速s总和
        for (int i = 0; i < vec_wind.size(); i++){
            sum += vec_wind[i]/V1;
        }
    }
    avg = sum/vec_wind.size();
    cout<<str<<"区域Rv均值为："<<avg<<endl<<endl;
    //返回RV均值
    vec_wind.clear();
    vec_wind.push_back(V1);
    vec_wind.push_back(avg);
    return vec_wind;
}

//计算θ1，需要用3区域扣除2区域进行计算，返回计算结果θ1
double AfterProcess::Calculateθ1(int z){
    cout<<"计算θ1中..."<<endl;
    vector<double> vec_angle; //存储所需区域的风向角度
    double x_left_left_index = 0.0, x_left_right_index = 0.0, x_right_left_index = 0.0, x_right_right_index = 0.0, y_left_left_index = 0.0, y_left_right_index = 0.0, y_right_left_index = 0.0, y_right_right_index = 0.0, z_up_index = 0.0, z_down_index = 0.0;
    //设置xy的下标范围
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
    //设置z的下标范围
    if (z == 1){
        z_down_index = GetZIndex(z_vec[0], 0);
        z_up_index = GetZIndex(z_vec[1], 1);
    }
    else if (z == 2){
        z_down_index = GetZIndex(z_vec[2], 0);
        z_up_index = GetZIndex(z_vec[3], 1);
    }
    else if (z == 3){
        z_down_index = GetZIndex(z_vec[4], 0);
        z_up_index = GetZIndex(z_vec[5], 1);
    }
    
    //存储目标范围内的角度值，i为层数
    for(int i = 0; i < wind_angle.size(); i++){
        if (i < z_down_index){
            continue;
        }
        else if (i > z_up_index){
            break;
        }
        //在目标z范围内，j为该层内的z下标，row为行数，col为列数
        else if (i > z_down_index && i < z_up_index){
            for (int j = 0; j < wind_angle[i].size(); j++){
                int row = j/x_axis.size();
                if (row < y_left_left_index){
                    continue;
                }
                else if (row > y_right_right_index){
                    break;
                }
                //在目标y范围内
                else if ((row >= y_left_left_index && row < y_left_right_index)||(row>y_right_left_index && row <= y_right_right_index)){
                    int col = j-(j/x_axis.size())*(int)x_axis.size();
                    if (col < x_left_left_index){
                        continue;
                    }
                    else if (col > x_right_right_index){
                        continue;
                    }
                    //在目标x范围内
                    else{
                        if (wind_angle[i][j] == -9999){
                            continue;
                        }
                        vec_angle.push_back(wind_angle[i][j]);
                    }
                }
                //在目标y范围内
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
                        if (wind_angle[i][j] == -9999){
                            continue;
                        }
                        vec_angle.push_back(wind_angle[i][j]);
                    }
                }
            }
        }
    }
    
    cout<<"vec-angle的大小为"<<vec_angle.size()<<endl;
    
    vector<PAIR> map = GetFrequency(vec_angle);
    vector<PAIR>::iterator it = map.begin();
    cout<<"θ1角度为："<<it->first<<"，出现次数为"<<it->second<<endl<<endl;
    return it->first;
}

//计算目标区域中的θ2比例
vector<double> AfterProcess::CalculateθRatio(string str, int z){
    cout<<"计算"<<str<<"区域θ比值中..."<<endl;
    int count = 0; //计算θ2角度的个数
    vector<double> vec_angle, vec_result; //存储所需区域的风向角度
    double theta = 0.0;
    theta = Calculateθ1(z);
    //不用扣除，直接计算
    if (str == "0" || str == "a" || str == "1"){
        vec_angle = GetDomainValue(wind_angle, str, z);
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
    //计算结果、打印并返回
    double result = (double)count/vec_angle.size();
    vec_result.push_back(theta);
    vec_result.push_back(result);
    cout<<str<<"区域的标准角度为："<<theta<<",\t占比为："<<result<<endl<<endl;
    return vec_result;
}

//计算前三个高频风向角度
vector<PAIR2> AfterProcess::CalculateWindAngle(string str, int z, int count){
    cout<<"计算"<<str<<"区域高频风向角中..."<<endl;
    vector<double> vec_angle;
    vector<string> vec_result;
    vec_angle = GetDomainValue(wind_angle, str, z);//存储所需区域的风向角度
    int _count = 0;
    for (int i = 0; i < vec_angle.size(); i++){
        if(vec_angle[i] == -9999) {
            _count++;
        }
    }
    vector<PAIR2> map = GetAngleFrequency(vec_angle);//获取风向角度的map
    map.push_back(make_pair("DATA", vec_angle.size()-_count));
    return map;
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
    
//传入风向角度，计算属于哪种风，并以string形式返回
string AfterProcess::CalWindDir(double _wd){
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

//将结果数据全部存入txt
bool AfterProcess::SaveToTXT(vector<vector<string>> _vector){
    ClearTXT();
    ofstream outfile;
    outfile.open(dest_path, ios::binary | ios::app | ios::in | ios::out);
    if (!outfile){
        cout<<"无法输出到txt"<<endl;//无法打开文件，返回false
        return false;
    }
    vector<string> str;
    int i = 0;
    str.push_back("高度0.5~2.5m：\t");
    str.push_back("高度5.0~10.0m：\t");
    str.push_back("高度0.5~10.0m：\t");
    vector<vector<string>>::iterator it = _vector.begin();
    while (it != _vector.end()) {
        vector<string> vec = *it;
        //存储过程
        outfile<<str[i]<<"平均风速："<<vec[0]<<"\t静风比例："<<vec[1]<<"\tV1："<<vec[2]<<"\tRv均值："<<vec[3]<<"\tθ1："<<vec[4]<<"\tθ2比例："<<vec[5]<<"\t第一高频风向角："<<vec[6]<<"\t第一高频风向频率："<<vec[7]<<"\t第二高频风向角："<<vec[8]<<"\t第二高频风向频率："<<vec[9]<<"\t第三高频风向角："<<vec[10]<<"\t第三高频风向频率："<<vec[11]<<"\n";
        it++;
        i++;
    }
    outfile.close();
    cout<<"存储到txt成功"<<endl;
    return true;
}
    
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
