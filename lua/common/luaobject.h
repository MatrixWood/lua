#ifndef luaobject_h
#define luaobject_h 

#include "lua.h"

typedef struct lua_State lua_State;

typedef LUA_INTEGER lua_Integer;
typedef LUA_NUMBER lua_Number;
typedef unsigned char lu_byte;
typedef int (*lua_CFunction)(lua_State* L);
typedef void* (*lua_Alloc)(void* ud, void* ptr, size_t osize, size_t nsize);

// lua number type 
#define LUA_NUMINT (LUA_TNUMBER | (0 << 4))
#define LUA_NUMFLT (LUA_TNUMBER | (1 << 4))

// lua function type 
#define LUA_TLCL (LUA_TFUNCTION | (0 << 4)) /* Lua closure */
#define LUA_TLCF (LUA_TFUNCTION | (1 << 4)) /* light C function */
#define LUA_TCCL (LUA_TFUNCTION | (2 << 4)) /* C closure */

// string type 
#define LUA_LNGSTR (LUA_TSTRING | (0 << 4))
#define LUA_SHRSTR (LUA_TSTRING | (1 << 4))

// GCObject
#define CommonHeader struct GCObject* next; lu_byte tt_; lu_byte marked
#define LUA_GCSTEPMUL 200

struct GCObject {
    CommonHeader;
};

typedef union lua_Value {
    struct GCObject* gc;
    void* p;  // light userdata类型变量（由使用者自行释放），和full userdata区分（受GC机制管控）
    int b;  // 布尔类型值
    lua_Integer i; // 整形变量，只在LUA_NUMINT时赋值
    lua_Number n; // 浮点型变量，只在LUA_NUMFLT时赋值，所以为浮点型
    lua_CFunction f; // light c functions类型变量
} Value;

typedef struct lua_TValue {
    Value value_; // 表示值
    int tt_; // 表示类型
} TValue;

typedef struct TString {
    CommonHeader;
} TString;

#endif