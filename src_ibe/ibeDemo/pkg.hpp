//
//  pkg.hpp
//  ibeDemo
//
//  Created by 马栋捷 on 2019/8/29.
//  Copyright © 2019 jerry. All rights reserved.
//

#ifndef pkg_hpp
#define pkg_hpp

#include <stdio.h>
#include "pbc.h"
using namespace std;

class PKGEntity{
    
private:
    element_t s;    //私钥
public:
    pairing_t pair; //椭圆曲线参数
    element_t P;    //椭圆曲线上找一个P点
    element_t Ppub; //公钥

    PKGEntity(string file); //初始化PKG的参数
    void PKGExtractPriKey(string id,element_t* Qid,element_t* Did);   //根据id获取私钥
};

#endif /* pkg_hpp */
