/// pipeostream
// An ostream interface for pipe userdata.
// @module pipeostream
#include "PipeOStream.h"
#include "Pipe.h"
#include "Poco/Exception.h"
#include <cstring>

int luaopen_poco_pipeostream(lua_State* L)
{
    return LuaPoco::loadConstructor(L, LuaPoco::PipeOStreamUserdata::PipeOStream);
}

namespace LuaPoco
{

PipeOStreamUserdata::PipeOStreamUserdata(const Poco::Pipe& p) 
    : mPipeOutputStream(p)
    , mPipeReference(0)
{
}

PipeOStreamUserdata::~PipeOStreamUserdata()
{
}

std::ostream& PipeOStreamUserdata::ostream()
{
    return mPipeOutputStream;
}

// register metatable for this class
bool PipeOStreamUserdata::registerPipeOStream(lua_State* L)
{
    luaL_newmetatable(L, "Poco.PipeOStream.metatable");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, metamethod__gc);
    lua_setfield(L, -2, "__gc");
    lua_pushcfunction(L, metamethod__tostring);
    lua_setfield(L, -2, "__tostring");
    
    lua_pushstring(L, "Poco.PipeOStream.metatable");
    lua_setfield(L, -2, "poco.userdata");
    
    // ostream methods
    lua_pushcfunction(L, write);
    lua_setfield(L, -2, "write");
    lua_pushcfunction(L, seek);
    lua_setfield(L, -2, "seek");
    lua_pushcfunction(L, flush);
    lua_setfield(L, -2, "flush");
    
    lua_pop(L, 1);
    return true;
}

/// Constructs a new pipeostream userdata.
// pipeostream holds a reference to a pipe userdata to prevent the pipe from
// being garbage collected while the pipeostream is still trying to use it.
// @param pipe userdata.
// @return userdata or nil. (error)
// @return error message.
// @function new
// @see ostream
// @see pipe
int PipeOStreamUserdata::PipeOStream(lua_State* L)
{
    PipeUserdata* pud = checkPrivateUserdata<PipeUserdata>(L, 1);
    
    void* ud = lua_newuserdata(L, sizeof(PipeOStreamUserdata));
    luaL_getmetatable(L, "Poco.PipeOStream.metatable");
    lua_setmetatable(L, -2);
    
    PipeOStreamUserdata* posud = new(ud) PipeOStreamUserdata(pud->mPipe);
    setPrivateUserdata(L, -1, posud);
    // store a reference to the PipeUserdata to prevent it from being
    // garbage collected while the PipeOutputStream is using it.
    lua_pushvalue(L, 1);
    posud->mPipeReference = luaL_ref(L, LUA_REGISTRYINDEX);
    
    return 1;
}

// metamethod infrastructure
int PipeOStreamUserdata::metamethod__tostring(lua_State* L)
{
    PipeOStreamUserdata* pud = checkPrivateUserdata<PipeOStreamUserdata>(L, 1);
    lua_pushfstring(L, "Poco.PipeOStream (%p)", static_cast<void*>(pud));
    
    return 1;
}

} // LuaPoco
