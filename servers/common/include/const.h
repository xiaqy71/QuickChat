#ifndef CONST_H
#define CONST_H

class ConfigMgr;
extern ConfigMgr gCfgMgr;

enum ErrorCodes
{
    Success    = 0,
    Error_Json = 1001,
    RPCFailed  = 1002,
};

#endif