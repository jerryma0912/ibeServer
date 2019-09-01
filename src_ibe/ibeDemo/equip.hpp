//
//  equip.hpp
//  ibeDemo
//
//  Created by 马栋捷 on 2019/8/29.
//  Copyright © 2019 jerry. All rights reserved.
//

#ifndef equip_hpp
#define equip_hpp

#include <stdio.h>
#include <sstream>
#include <iostream>
#include "pbc.h"
#include "pkg.hpp"
#include "base64.hpp"
#include "util.hpp"
using namespace std;

class EquipEntity{

private:
    element_t Did;
public:
    string id;
    element_t Qid;
    PKGEntity *pkg;
    
    EquipEntity(string id,PKGEntity *pkg);
    void MessageEncrypt(EquipEntity e,string message,element_t *U,mpz_t *V);
    string MessageDecrypt(element_t U,mpz_t V);
    
    
    string element2String(element_t e); //下行编码
    string mpz2String(mpz_t m);  //下行编码
    void string2Element(string s,element_t e);//上行编码
    void string2Mpz(string s,mpz_t *m);//上行编码
    
    string getPriKey();
    
    
    void mympz_inp_raw(mpz_t *z, const unsigned char* c, int n);
    void mympz_out_raw(mpz_t x, unsigned char *c, int n);
    int mympz_sizeinbytes(mpz_t x);


};
#endif /* equip_hpp */
