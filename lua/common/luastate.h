#ifndef luastate_h
#define luastate_h

#include "luaobject.h"

#define LUA_EXTRASPACE sizeof(void *)
#define G(L) ((L)->l_G)

#define STEPMULADJ 200
#define GCSTEPMUL 200 
#define GCSTEPSIZE 1024  //1kb
#define GCPAUSE 100

typedef TValue *StkId;

struct CallInfo {
  StkId func;                     // 被调用函数在栈中的位置
  StkId top;                      // 被调用函数的栈顶位置
  int nresult;                    // 有多少个返回值
  int callstatus;                 // 调用状态
  struct CallInfo* next;          // 下一个调用
  struct CallInfo* previous;      // 上一个调用
};

typedef struct lua_State {
  CommonHeader;       // gc header, all gcobject should have the commonheader
  StkId stack;      // 栈
  StkId stack_last; // 从这里开始，栈不能被使用
  StkId top;        // 栈顶，调动函数时动态改变
  int stack_size;   // 栈的整体大小
  // 保护模式中要用到的结构，当抛出异常时，跳出逻辑
  struct lua_longjmp *errorjmp; // 保护模式中，当抛出异常时，跳出逻辑
  int status;                   // lua_State的状态
  struct lua_State *previous; // 上一个
  struct CallInfo base_ci;  // 虚拟机线程类型实例生命周期保持一致的基础函数调用信息
  struct CallInfo *ci;      // 当前被调用函数信息
  int nci;
  struct global_State *l_G; // global_State指针
  ptrdiff_t errorfunc;      // 错误函数位于栈的哪个位置
  int ncalls;               //进行了多少次调用
  struct GCObject* gclist;
} lua_State;

typedef struct global_State {
  struct lua_State* mainthread;   // 我们的lua_State其实是lua thread，某种程度上来说，它也是协程
  lua_Alloc frealloc;             // 一个可以自定义的内存分配函数
  void* ud;                       // 当我们自定义内存分配器时，可能要用到这个结构，但是我们用官方默认的版本
                                  // 因此它始终是NULL
  lua_CFunction panic;            // 当调用LUA_THROW接口时，如果当前不处于保护模式，那么会直接调用panic函数
                                  // panic函数通常是输出一些关键日志
  //gc fields
  lu_byte gcstate;
  lu_byte currentwhite;
  struct GCObject* allgc;         // gc root set
  struct GCObject** sweepgc;
  struct GCObject* gray;
  struct GCObject* grayagain;
  lu_mem totalbytes;
  l_mem GCdebt;                   // GCdebt will be negative
  lu_mem GCmemtrav;               // per gc step traverse memory bytes 
  lu_mem GCestimate;              // after finish a gc cycle,it records total memory bytes (totalbytes + GCdebt)
  int GCstepmul;
} global_State;

// GCUnion
union GCUnion {
    struct GCObject gc;
    lua_State th;
};

struct lua_State *lua_newstate(lua_Alloc alloc, void *ud);
void lua_close(struct lua_State *L);

void setivalue(StkId target, int integer);
void setfvalue(StkId target, lua_CFunction f);
void setfltvalue(StkId target, float number);
void setbvalue(StkId target, bool b);
void setnilvalue(StkId target);
void setpvalue(StkId target, void *p);

void setobj(StkId target, StkId value);

void increase_top(struct lua_State *L);
void lua_pushcfunction(struct lua_State *L, lua_CFunction f);
void lua_pushinteger(struct lua_State *L, int integer);
void lua_pushnumber(struct lua_State *L, float number);
void lua_pushboolean(struct lua_State *L, bool b);
void lua_pushnil(struct lua_State *L);
void lua_pushlightuserdata(struct lua_State *L, void *p);

lua_Integer lua_tointegerx(struct lua_State *L, int idx, int *isnum);
lua_Number lua_tonumberx(struct lua_State *L, int idx, int *isnum);
bool lua_toboolean(struct lua_State *L, int idx);
int lua_isnil(struct lua_State *L, int idx);

void lua_settop(struct lua_State *L, int idx);
int lua_gettop(struct lua_State *L);
void lua_pop(struct lua_State *L);
TValue* index2addr(struct lua_State* L, int idx);

#endif