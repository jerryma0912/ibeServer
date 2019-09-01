//
//  equip.cpp
//  ibeDemo
//
//  Created by 马栋捷 on 2019/8/29.
//  Copyright © 2019 jerry. All rights reserved.
//

#include "equip.hpp"
EquipEntity::EquipEntity(string id,PKGEntity *pkg){
    //1.设置参数
    this->id = id;
    this->pkg = pkg;
    
    //2.获取公钥哈希和私钥
    element_init_G1(this->Qid, this->pkg->pair);
    element_init_G1(this->Did, this->pkg->pair);
    this->pkg->PKGExtractPriKey(id,&this->Qid,&this->Did);
    cout<<"[EQUIP *"<<(this->id)<<"*] get PriKey and PubKey"<<endl;
}

void EquipEntity::MessageEncrypt(EquipEntity e,string message,element_t *U,mpz_t *V){
    //解析message string->ascII码->mpz_t
    mpz_t M;
    mpz_init(M);
    unsigned char* str = (unsigned char *)message.c_str();
    mympz_inp_raw(&M,str,message.length());
    //定义局部参数
    element_t r,gID,gIDr;
    pairing_pp_t pp;
    mpz_t z;
    //初始化参数
    element_init_G1(*U, this->pkg->pair);
    mpz_init(*V);
    element_init_Zr(r, this->pkg->pair);
    element_init_GT(gID, this->pkg->pair);
    element_init_GT(gIDr, this->pkg->pair);
    mpz_init(z);
    pairing_pp_init(pp, this->pkg->Ppub, this->pkg->pair);
    //加密过程
    cout<<"[EQUIP *"<<(this->id)<<"* Send Message to *"<<e.id<<"*]"<<endl;
    cout<<"- msgs orign: "<<message<<endl;
    gmp_printf("- msgs encode_mpz: %Zd\n",M);
    element_random(r);
    element_mul_zn(*U, this->pkg->P, r);
    
    pairing_pp_apply(gID, e.Qid, pp); //gID=e(Qid,Ppub),用接收方公钥加密
    element_mul_zn(gIDr, gID, r); //gIDr=gID^r
    element_to_mpz(z,gIDr); //H(gIDr)
    mpz_xor(*V,M,z); //V=M xor z
    //清空局部参数
    element_clear(r);
    pairing_pp_clear(pp);
    element_clear(gID);
    element_clear(gIDr);
    mpz_clear(z);
    mpz_clear(M);
}

string EquipEntity::MessageDecrypt(element_t U,mpz_t V){
    //定义局部参数
    pairing_pp_t pp;
    element_t tem1;
    mpz_t z,M;
    //初始化参数
    pairing_pp_init(pp, U, this->pkg->pair);
    element_init_GT(tem1, this->pkg->pair);
    mpz_init(z);
    mpz_init(M);
    //解密过程
    cout<<"[EQUIP *"<<(this->id)<<"* Decrypt msgs]"<<endl;
    pairing_pp_apply(tem1,this->Did, pp);
    element_to_mpz(z,tem1);
    mpz_xor(M,V,z);
    //解析内容
    int len = mympz_sizeinbytes(M);
    unsigned char* str = (unsigned char *)malloc(len+1);
    str[len] = '\0';
    mympz_out_raw(M,str,len);
    string s = (char*)str;
    gmp_printf("- encode_mpz: %Zd\n",M);
    cout<<"- orign: "<<s<<endl;
    free(str);
    //清空局部参数
    pairing_pp_clear(pp);
    element_clear(tem1);
    mpz_clear(z);
    mpz_clear(M);
    
    return s;
}

string EquipEntity::element2String(element_t e){
    //1 element -> byte数组
    unsigned char* c = (unsigned char *)malloc(1024);
    int len = element_to_bytes(c,e);    //真实的字节长度！
    c[len+1]='\0';
    //cout<<" << U byte[] Len (-'\\0')："<<len<<" >>"<<endl;
    //2 byte数组 --base64编码--> string
    unsigned int encodeLen = (BASE64_ENCODE_OUT_SIZE(len));
    char* encode_out = (char*)malloc(encodeLen);
    base64_encode(c, len, encode_out);  //末尾已经添加\0
    //cout<<" << U base64 Len (-'\\0'): "<<encodeLen-1<<" >>"<<endl;
    //3 返回string
    string result = encode_out;
    //4 释放空间
    free(encode_out);
    free(c);
    
    return result;
}

void EquipEntity::string2Element(string s,element_t e){
    //1 html解码 -> string
    Utils::replace_all_distinct(s, "%2B", "+");
    Utils::replace_all_distinct(s, "%3D", "=");
    //cout<<" << U base64 Len (-'\\0'): "<<s.length()<<" >>"<<endl;
    //2 string --base64解码-->  byte数组
    unsigned int decodeLen = BASE64_DECODE_OUT_SIZE(s.length());    //最大的长度，因为可能有=填充
    unsigned char* decode_out = (unsigned char*)malloc(decodeLen);
    int decoderealLen = base64_decode(s.c_str(),s.length(), decode_out);    //末尾已经添加'\0'
    //cout<<" << U byte[] Len (-'\\0')："<<decoderealLen<<" >>"<<endl;
    //3 byte数组 -> element
    element_from_bytes(e,decode_out);
}

string EquipEntity::mpz2String(mpz_t M){
    //1 mpz -> byte数组
    int len = mympz_sizeinbytes(M);
    unsigned char* str = (unsigned char *)malloc(len+1);
    str[len] = '\0';
    mympz_out_raw(M,str,len);
    //cout<<" << V byte[] Len (-'\\0')："<<len<<" >>"<<endl;
    //2 byte数组 --base64编码--> string
    unsigned int encodeLen = (BASE64_ENCODE_OUT_SIZE(len));
    char* encode_out = (char*)malloc(encodeLen);
    base64_encode(str, len, encode_out);  //末尾已经添加\0
    //cout<<" << V base64 Len (-'\\0'): "<<encodeLen-1<<" >>"<<endl;
    //3 返回string
    string result = encode_out;
    //4 释放空间
    free(encode_out);
    free(str);
    
    return result;
    
}

void EquipEntity::string2Mpz(string s,mpz_t *m){
    //1 html解码 -> string
    Utils::replace_all_distinct(s, "%2B", "+");
    Utils::replace_all_distinct(s, "%3D", "=");
    //cout<<" << V base64 Len (-'\\0'): "<<s.length()<<" >>"<<endl;
    //2 string --base64解码-->  byte数组
    unsigned int decodeLen = BASE64_DECODE_OUT_SIZE(s.length());
    unsigned char* decode_out = (unsigned char*)malloc(decodeLen);
    int decoderealLen = base64_decode(s.c_str(), s.length(),decode_out);
    //cout<<" << V byte[] Len (-'\\0')："<<decoderealLen<<" >>"<<endl;
    //3 byte数组 -> mpz
    mympz_inp_raw(m,decode_out,strlen((char*)decode_out));
}

string EquipEntity::getPriKey(){

    return element2String(Did);
}



void EquipEntity::mympz_inp_raw(mpz_t *z, const unsigned char* c, int n){
    int i = 0;
    mpz_set_ui(*z, 0);
    while(i < n) {
        mpz_mul_2exp(*z, *z, 8);
        mpz_add_ui(*z, *z, c[i]);
        i++;
    }
}

void EquipEntity::mympz_out_raw(mpz_t x, unsigned char *c, int n){
    unsigned long l;
    mpz_t z;
    int i = n - 1;
    
    mpz_init_set(z, x);
    
    while(i >= 0) {
        l = mpz_get_ui(z);
        c[i] = (unsigned char) l;
        mpz_tdiv_q_2exp(z, z, 8);
        i--;
    }
    mpz_clear(z);
}

int EquipEntity::mympz_sizeinbytes(mpz_t x){
    int i, n;
    n = mpz_sizeinbase(x, 2);
    i = n >> 3;
    if (n % 8) i++;
    
    return i;
}

