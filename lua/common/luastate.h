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
  lu_byte gcstate;                // 标记gc当前处于哪个阶段
  lu_byte currentwhite;           // 当前gc是哪种白色，10与01中的一种，在gc的atomic阶段结束时，会从一种切换为另一种
  struct GCObject* allgc;         // 所有新建的gc对象，都要放入到这个链表中，放入的方式是直接放在链表的头部
  struct GCObject** sweepgc;      // 用于记录当前sweep进度
  struct GCObject* gray;          // gc对象，首次从白色被标记为灰色的时候，会被放入这个列表，放入这个列表的gc对象，是准备被propagate的对象
  struct GCObject* grayagain;     // 从黑色变回灰色时，会放入这个链表中，作用是避免table反复在黑色和灰色之间来回切换重复扫描
  lu_mem totalbytes;              // 记录开辟内存字节大小的变量之一，真实的大小是totalbytes+GCdebt
  l_mem GCdebt;                   // 可以为负数，当GCdebt>0时才能触发gc流程
  lu_mem GCmemtrav;               // 每次进行gc操作时，所遍历的对象字节大小之和，单位是byte，当其值大于单步执行的内存上限时，gc终止
  lu_mem GCestimate;              // 在sweep阶段结束时，会被重新计算，本质是totalbytes+GCdebt，它的作用是，在本轮gc结束时，将自身扩充两倍大小，
                                  // 然后让真实大小减去扩充后的自己得到差debt，然后totalbytes会等于扩充后的自己，而GCdebt则会被负数debt赋值，就是是说下一次执行gc流程，要在有|debt|个bytes内存被开辟后，才会开始。目的是避免gc太过频繁
  int GCstepmul;                  // 一个和GC单次处理多少字节相关的参数
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