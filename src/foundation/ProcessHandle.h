#ifndef LUA_POCO_PROCESSHANDLE_H
#define LUA_POCO_PROCESSHANDLE_H

#include "LuaPoco.h"
#include "Userdata.h"
#include "Poco/Process.h"

namespace LuaPoco
{

class ProcessHandleUserdata : public Userdata
{
public:
	ProcessHandleUserdata(const Poco::ProcessHandle& ph);
	virtual ~ProcessHandleUserdata();
	virtual UserdataType getType();
	// register metatable for this class
	static bool registerProcessHandle(lua_State* L);
private:
	ProcessHandleUserdata();
	// no constructor function due to Poco::Process::launch()
	// metamethod infrastructure
	static int metamethod__gc(lua_State* L);
	static int metamethod__tostring(lua_State* L);
	
	// userdata methods
	static int id(lua_State* L);
	static int wait(lua_State* L);
	static int kill(lua_State* L);
	
	Poco::ProcessHandle mProcessHandle;
};

} // LuaPoco

#endif