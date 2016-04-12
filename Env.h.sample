#ifndef __okeya__Env_h
#define __okeya__Env_h

#include <string>
#include <unordered_map>

#ifdef __APPLE__
#include "TargetConditionals.h"
#endif

static const std::string GAME_ID = "";
static const std::unordered_map<int, const std::string> SKU = {{120, "com.oke_ya.appname.charge1"}};

#if TARGET_IPHONE_SIMULATOR
// SIMULATOR
static const std::string API_URL = "http://localhost:3000";
#elif COCOS2D_DEBUG
//DEBUG
static const std::string API_URL = "https://";
#else
//RELEASE
static const std::string API_URL = "https://";
#endif


#endif //__okeya__Env_h
