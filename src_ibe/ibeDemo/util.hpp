//
//  util.hpp
//  ibeDemo
//
//  Created by 马栋捷 on 2019/8/30.
//  Copyright © 2019 jerry. All rights reserved.
//

#ifndef util_hpp
#define util_hpp

#include <stdio.h>
#include "iostream"
#include <vector>
#include <map>
using namespace std;

class Utils{
    
public:
    static vector<string> split(string str,string pattern);
    static map<std::string,std::string> parsePostBody(std::string param);
    static string&   replace_all_distinct(string&   str,const   string&   old_value,const   string&   new_value);
};

#endif /* util_hpp */
