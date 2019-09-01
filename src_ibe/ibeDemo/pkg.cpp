//
//  pkg.cpp
//  ibeDemo
//
//  Created by 马栋捷 on 2019/8/29.
//  Copyright © 2019 jerry. All rights reserved.
//


#include <iostream>
#include <sstream>
#include <fstream>
#include "pkg.hpp"
#include "pbc.h"

using namespace std;

PKGEntity::PKGEntity(string file){
    //1. 选择椭圆参数
    ifstream infile;
    infile.open(file.data());   //将文件流对象与文件连接起来
    assert(infile.is_open());   //若失败,则输出错误消息,并终止程序运行
    ostringstream buf;
    char ch;
    while(buf && infile.get(ch))
        buf.put(ch);
    string param = buf.str();
    infile.close();             //关闭文件输入流
    
    //2. 生成参数
    pairing_init_set_buf(this->pair, param.c_str(), param.length());
    
    //3. Setup
    element_init_G1(P, this->pair);
    element_init_G1(Ppub, this->pair);
    element_init_Zr(s, this->pair);
    printf("[PKG] SETUP Init\n");
    element_random(this->P);
    element_random(this->s);
    element_mul_zn(this->Ppub, this->P, this->s);   //Ppub=sP
}

void PKGEntity::PKGExtractPriKey(string id,element_t* Qid,element_t* Did){
    
    printf("[PKG] extract prikey\n");
    element_from_hash(*Qid, (void *)id.data(), id.length());    //Qid=H(id)
    element_mul_zn(*Did, *Qid, this->s);
}
