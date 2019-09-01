//
//  base64.hpp
//  ibeDemo
//
//  Created by 马栋捷 on 2019/8/31.
//  Copyright © 2019 jerry. All rights reserved.
//

#ifndef base64_hpp
#define base64_hpp

#include <stdio.h>
#include <string.h>

#define BASE64_ENCODE_OUT_SIZE(s) ((unsigned int)((((s) + 2) / 3) * 4 + 1))
#define BASE64_DECODE_OUT_SIZE(s) ((unsigned int)(((s) / 4) * 3) + 1)

char* base64_encode( const unsigned char * bindata, int binlength,char * base64  ); //base64编码
int base64_decode( const char * base64, int base64length,unsigned char * bindata ); //base64解码


#endif /* base64_hpp */
