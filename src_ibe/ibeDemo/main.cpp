//
//  main.cpp
//  ibeDemo
//
//  Created by 马栋捷 on 2019/8/29.
//  Copyright © 2019 jerry. All rights reserved.
//

#include <iostream>
#include "pbc.h"
#include "pkg.hpp"
#include "equip.hpp"
#include "httpServer.hpp"
#include "util.hpp"
#include <map>
using namespace std;


mg_serve_http_opts HttpServer::s_server_option;
string HttpServer::s_web_dir = "../src_ibe/web";
unordered_map<string, ReqHandler> HttpServer::s_handler_map;
unordered_set<mg_connection *> HttpServer::s_websocket_session_set;
PKGEntity p = PKGEntity("../src_ibe/param/a.param");
map<string,EquipEntity> equipMap;


bool handle_equipReg(string url, string body, mg_connection *c, OnRspCallback rsp_callback){
    cout << "==========handle equipReg==========" << endl;
    try{
        //1 解析参数
        cout << "/1/---- Param Parse " << endl;
        map<string,string> b = Utils::parsePostBody(body);
        map<string,string>::iterator paramEquipId = b.find("EquipId");
        if(paramEquipId == b.end())
            throw "\"No EquipId param error\"";
        //2 查找对应注册的设备
        map<string,EquipEntity>::iterator sendEquip = equipMap.find(paramEquipId->second);
        if (sendEquip != equipMap.end())
            throw "\"This Equipment is registered\"";
        //3 注册设备
        cout<<"/2/---- Get Private Key "<<endl;
        EquipEntity equip = EquipEntity(paramEquipId->second,&p);
        equipMap.insert(pair<string,EquipEntity>(paramEquipId->second,equip));
        string priKey = equip.getPriKey();
        cout<<"- priKey: "<<priKey<<endl;
        //4 返回值
        string result;
        result +="{";
        result +="\"priKey\":";
        result += "\"";
        result += priKey;
        result += "\"";
        result +="}";
        rsp_callback(c, result.c_str());
    }
    catch(exception ex){
        cout<<"/---- ERR: equip register process err ---- "<<endl;
        rsp_callback(c, "\"equip register process err\"");
    }
    catch(const char* info){
        cout<<"/---- ERR: "<<info<<" ---- "<<endl;
        rsp_callback(c, info);
    }
    cout << "==============  END  ==============" << endl<<endl;
    return true;
}

bool handle_msgEncrypt(string url, string body, mg_connection *c, OnRspCallback rsp_callback)
{
    cout << "==========handle msgEncrypt==========" << endl;
    try{
        //1 解析参数
        cout << "/1/---- Param Parse " << endl;
        map<string,string> b = Utils::parsePostBody(body);
        map<string,string>::iterator paramEquipId = b.find("EquipId");
        if(paramEquipId == b.end())
            throw "\"No EquipId param error\"";
        map<string,string>::iterator paramMsg = b.find("Msg");
        if(paramMsg == b.end())
            throw "\"No Msg param error\"";
        map<string,string>::iterator paramRecvEquip = b.find("RecvEquipId");
        if(paramRecvEquip == b.end())
            throw "\"No RecvEquipId param error\"";
        //2 查找对应注册的设备
        map<string,EquipEntity>::iterator sendEquip = equipMap.find(paramEquipId->second);
        if (sendEquip == equipMap.end())
            throw "\"This Equipment is not registered\"";
        map<string,EquipEntity>::iterator recvEquip = equipMap.find(paramRecvEquip->second);
        if (recvEquip == equipMap.end())
            throw "\"Receive Equipment is not registered\"";
        //3 解析信息合法性
        string message = paramMsg->second;
        if(message=="")
            throw "\"Msg is illegal\"";
        //4 加密
        cout << "/2/---- Message Encrypt "<<endl;
        element_t U;
        mpz_t V;
        sendEquip->second.MessageEncrypt(recvEquip->second, message, &U, &V);
        string uString = sendEquip->second.element2String(U);
        string vString = sendEquip->second.mpz2String(V);
        element_printf("- U orign: %B\n", U);
        cout<<"- U base64: "<<uString<<endl;
        gmp_printf ("- V orign: %Zd\n", V);
        cout<<"- V base64: "<<vString<<endl;
        //5 返回加密结果
        string result;
        result +="{";
        result +="\"U\":";
        result += "\"";
        result += uString;
        result += "\",";
        result +="\"V\":";
        result += "\"";
        result += vString;
        result += "\"";
        result +="}";
        
        rsp_callback(c, result);
    }
    catch(exception ex){
        cout<<"/---- ERR: msgEncrypt process err ----"<<endl;
        rsp_callback(c, "\"msgEncrypt process err\"");
    }
    catch(const char* info){
        cout<<"/---- ERR: "<<info<<" ---- "<<endl;
        rsp_callback(c, info);
    }
    
    cout << "==============  END  ==============" << endl<<endl;
    return true;
}

bool handle_msgDecrypt(string url, string body, mg_connection *c, OnRspCallback rsp_callback)
{
    cout << "==========handle msgDecrypt==========" << endl;
    try{
        //1 解析参数
        cout << "/1/---- Param Parse " << endl;
        map<string,string> b = Utils::parsePostBody(body);
        map<string,string>::iterator paramEquipId = b.find("EquipId");
        if(paramEquipId == b.end())
            throw "\"No EquipId param error\"";
        map<string,string>::iterator paramU = b.find("U");
        if(paramU == b.end())
            throw "\"No U param error\"";
        map<string,string>::iterator paramV = b.find("V");
        if(paramV == b.end())
            throw "\"No V param error\"";
        //2 查找对应注册的设备
        map<string,EquipEntity>::iterator equip = equipMap.find(paramEquipId->second);
        if (equip == equipMap.end())
            throw "\"This Equipment is not registered\"";
        //3 解析内容
        cout << "/2/---- U/V parse "<<endl;
        element_t U;
        element_init_G1(U, equip->second.pkg->pair);
        equip->second.string2Element(paramU->second, U);
        mpz_t V;
        mpz_init(V);
        equip->second.string2Mpz(paramV->second, &V);
        element_printf("- U parseOrign:\n %B\n", U);
        gmp_printf ("- V parseOrign:\n %Zd\n", V);
        //4 解密信息
        cout << "/3/---- Message Decrypt "<<endl;
        string info = equip->second.MessageDecrypt(U, V);
        //5 输出
        string result;
        result +="\"";
        result += info;
        result += "\"";
        
        rsp_callback(c, result);
    }
    catch(exception ex){
        cout<<"/---- ERR: msgDecrypt process err ----"<<endl;
        rsp_callback(c, "\"msgDecrypt process err\"");
    }
    catch(const char* info){
        cout<<"/---- ERR: "<<info<<" ---- "<<endl;
        rsp_callback(c, info);
    }
    cout << "==============  END  ==============" << endl<<endl;
    return true;
}

bool handle_equipLogout(string url, string body, mg_connection *c, OnRspCallback rsp_callback){
    cout << "==========handle equipLogout==========" << endl;
    try{
        //1 解析参数
        cout << "/1/---- Param Parse " << endl;
        map<string,string> b = Utils::parsePostBody(body);
        map<string,string>::iterator paramEquipId = b.find("EquipId");
        if(paramEquipId == b.end())
            throw "\"No EquipId param error\"";
        //2 查找对应注册的设备
        map<string,EquipEntity>::iterator sendEquip = equipMap.find(paramEquipId->second);
        if (sendEquip == equipMap.end())
            throw "\"This Equipment is not registered\"";
        //3 撤销设备
        cout<<"/2/---- Log out Equipment "<<endl;
        equipMap.erase(sendEquip);
        //4 返回值
        string result;
        result +="\"";
        result += "logout Successful";
        result +="\"";
        rsp_callback(c, result.c_str());
    }
    catch(exception ex){
        cout<<"/---- ERR: equip Logout process err ---- "<<endl;
        rsp_callback(c, "\"equip Logout process err\"");
    }
    catch(const char* info){
        cout<<"/---- ERR: "<<info<<" ---- "<<endl;
        rsp_callback(c, info);
    }
    cout << "==============  END  ==============" << endl<<endl;
    return true;
}

void ibe_test(){
    EquipEntity a = EquipEntity("aa",&p);
    EquipEntity b = EquipEntity("bb",&p);
    
    element_t U;
    mpz_t V;
    a.MessageEncrypt(b,"hello world 12345!@#$%^{}:,./;'大家好", &U, &V);
    element_printf("U: %B\n", U);
    gmp_printf ("V: %Zd\n", V);
    string message = b.MessageDecrypt(U, V);
}

int main(int argc, const char * argv[]) {
    //ibe_test();
    auto http_server = shared_ptr<HttpServer>(new HttpServer);
    http_server->Init("7999");
    // add handler
    http_server->AddHandler("/api/msgEncrypt", handle_msgEncrypt);
    http_server->AddHandler("/api/msgDecrypt", handle_msgDecrypt);
    http_server->AddHandler("/api/equipReg", handle_equipReg);
    http_server->AddHandler("/api/equipLogout", handle_equipLogout);
    http_server->Start();
}
