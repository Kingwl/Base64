
#include "stdafx.h"
#include "iostream"
#include "string"
#include "sstream"
#include "boost\algorithm\string.hpp"

typedef  unsigned char  byte;

static const char Table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static struct Index{
    Index()
    {
        _Index['='] = 0;
        byte i = 0;
        for (auto &r : Table)
        {
            _Index[r] = i++;
        }
    }

    byte operator[](const char c) const
    {
        return _Index[c];
    }

    byte _Index[0xFF];

}initData;


std::string::size_type Encode(const std::string &str, std::string &dest)
{
    std::stringstream result;
    std::string::size_type len = str.length() - 2; 
    std::string::size_type i = 0;

    for ( ; i < len; i += 3)
    {
        result << Table[((str[i] >> 2) & 0x3F)];
        result << Table[(((str[i] << 4) & 0x30) | ((str[i + 1] >> 4) & 0x0F))];
        result << Table[(((str[i + 1] << 2) & 0x3F) | ((str[i + 2] >> 6) & 0x03))];
        result << Table[(str[i + 2] & 0x3F)];
    }
    switch (len + 2 - i)
    {
    case 0:
        break;
    case 1:
        result << Table[((str[i] >> 2) & 0x3F)];
        result << Table[((str[i] << 4) & 0x30)];
        result << "==";
        break;
    case 2:
        result << Table[((str[i] >> 2) & 0x3F)];
        result << Table[(((str[i] << 4) & 0x30) | ((str[i + 1] >> 4) & 0x0F))];
        result << Table[((str[i + 1] << 2) & 0x3F)];
        result << "=";
        break;
    }

    dest = std::move(result.str());
    //dest = result.str();

    return len * 4 / 3;
}

std::string::size_type Decode(const std::string &str, std::string &dest)
{
    std::string::size_type len = str.length();
    std::string::size_type i = 0;
    if (!len) return 0;
    if (len % 4 != 0) return -1;

    byte in_temp[4];
    byte out_temp[3];

    std::stringstream result;
    for (; i < len; i += 4)
    {
        in_temp[0] = initData[str[i]];
        in_temp[1] = initData[str[i + 1]];
        in_temp[2] = initData[str[i + 2]];
        in_temp[3] = initData[str[i + 3]];

        out_temp[0] = (((in_temp[0] << 2) & 0xFC) | ((in_temp[1] >> 4) & 0x03));
        out_temp[1] = (((in_temp[1] << 4) & 0xF0) | ((in_temp[2] >> 2) & 0x0F));
        out_temp[2] = (((in_temp[2] << 6) & 0xC0) | (in_temp[3] & 0x3F));
        result << out_temp[0];
        if (str[2] == '=')
        {
            continue;
        }
        result << out_temp[1];
        if (str[3] == '=')
        {
            continue;;
        }
        result << out_temp[2];
    }

    dest = std::move(result.str());
    //dest = result.str();

    return len / 4 * 3;
}

int main(int argc, char * argv[])
{
    std::string input = "16xizghcui12ydhusjkdbyyuioyevuibaysdcua3";
    std::string code;

    Encode(input, code);
    std::cout << code << std::endl;

    std::string result;
    Decode(code, result);
    std::cout << result << std::endl;

    boost::trim(result);
    if (input.compare(result))
    {
        std::cout << "ok" << std::endl;
    }

    system("pause");
    return 0;
}

