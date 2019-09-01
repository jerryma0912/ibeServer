//
//  util.cpp
//  ibeDemo
//
//  Created by 马栋捷 on 2019/8/30.
//  Copyright © 2019 jerry. All rights reserved.
//

#include "util.hpp"

vector<string> Utils::split(string str,string pattern){
    
    string::size_type pos;
    vector<string> result;
    str+=pattern;//扩展字符串以方便操作
    int size=str.size();
    
    for(int i=0; i<size; i++)
    {
        pos=str.find(pattern,i);
        if(pos<size)
        {
            string s=str.substr(i,pos-i);
            result.push_back(s);
            i=pos+pattern.size()-1;
        }
    }
    return result;
}

map<string,string> Utils::parsePostBody(string param){
    
    map<string,string> bodyParam;
    
    vector<string> v = Utils::split(param,"&");
    for(vector<string>::iterator it=v.begin();it!=v.end();it++){    //解析参数
        string str = (string)*it;
        int lo=str.find_first_of("=");
        bodyParam.insert(pair<string,string>(str.substr(0,lo),str.substr(lo+1,str.length()-1)));
    }
    
    for(map<string,string>::iterator a=bodyParam.begin();a!=bodyParam.end();a++){
        cout <<a->first<<" : "<<a->second<<endl;
    }
    
    return bodyParam;
}

string& Utils::replace_all_distinct(string& str, const string& old_value, const string& new_value)
{
    for(string::size_type   pos(0);   pos!=string::npos;   pos+=new_value.length()){
        
        if((pos=str.find(old_value,pos))!=string::npos){
             str.replace(pos,old_value.length(),new_value);
        }
        else{
            break;
        }
    }
    return   str;
}
