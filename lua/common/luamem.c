#include "luamem.h"
#include "luado.h"

void* luaM_realloc(struct lua_State* L, void* ptr, size_t osize, size_t nsize) {
    struct global_State* g = G(L);
    int oldsize = ptr ? osize : 0;

    void* ret = (*g->frealloc)(g->ud, ptr, oldsize, nsize);
    if (ret == NULL) {
        luaD_throw(L, LUA_ERRMEM);
    }

    g->GCdebt = g->GCdebt - oldsize + nsize;
    return ret;
}