#include "RegularExpression.h"

namespace
{
	
Poco::RegularExpression::Options parseRegexOptions(const std::string& options)
{
	Poco::RegularExpression::Options opts = 0;
	
	if (options.find("RE_CASELESS") != string::npos)
		opts |= Poco::RegularExpression::RE_CASELESS;
	if (options.find("RE_MULTILINE") != string::npos)
		opts |= Poco::RegularExpression::RE_MULTILINE;
	if (options.find("RE_DOTALL") != string::npos)
		opts |= Poco::RegularExpression::RE_DOTALL;
	if (options.find("RE_EXTENDED") != string::npos)
		opts |= Poco::RegularExpression::RE_EXTENDED;
	if (options.find("RE_ANCHORED") != string::npos)
		opts |= Poco::RegularExpression::RE_ANCHORED;
	if (options.find("RE_EXTRA") != string::npos)
		opts |= Poco::RegularExpression::RE_EXTRA;
	if (options.find("RE_NOTBOL") != string::npos)
		opts |= Poco::RegularExpression::RE_NOTBOL;
	if (options.find("RE_NOTEOL") != string::npos)
		opts |= Poco::RegularExpression::RE_NOTEOL;
	if (options.find("RE_UNGREEDY") != string::npos)
		opts |= Poco::RegularExpression::RE_UNGREEDY;
	if (options.find("RE_NOTEMPTY") != string::npos)
		opts |= Poco::RegularExpression::RE_NOTEMPTY;
	if (options.find("RE_UTF8") != string::npos)
		opts |= Poco::RegularExpression::RE_UTF8;
	if (options.find("RE_NO_AUTO_CAPTURE") != string::npos)
		opts |= Poco::RegularExpression::RE_NO_AUTO_CAPTURE;
	if (options.find("RE_NO_UTF8_CHECK") != string::npos)
		opts |= Poco::RegularExpression::RE_NO_UTF8_CHECK;
	if (options.find("RE_FIRSTLINE") != string::npos)
		opts |= Poco::RegularExpression::RE_FIRSTLINE;
	if (options.find("RE_DUPNAMES") != string::npos)
		opts |= Poco::RegularExpression::RE_DUPNAMES;
	if (options.find("RE_NEWLINE_CR") != string::npos)
		opts |= Poco::RegularExpression::RE_NEWLINE_CR;
	if (options.find("RE_NEWLINE_LF") != string::npos)
		opts |= Poco::RegularExpression::RE_NEWLINE_LF;
	if (options.find("RE_NEWLINE_CRLF") != string::npos)
		opts |= Poco::RegularExpression::RE_NEWLINE_CRLF;
	if (options.find("RE_NEWLINE_ANY") != string::npos)
		opts |= Poco::RegularExpression::RE_NEWLINE_ANY;
	if (options.find("RE_NEWLINE_ANYCRLF") != string::npos)
		opts |= Poco::RegularExpression::RE_NEWLINE_ANYCRLF;
	if (options.find("RE_GLOBAL") != string::npos)
		opts |= Poco::RegularExpression::RE_GLOBAL;
	if (options.find("RE_NO_VARS") != string::npos)
		opts |= Poco::RegularExpression::RE_NO_VARS;

	return opts;
}

}

namespace LuaPoco
{
RegularExpressionUserdata::RegularExpressionUserdata(const std::string& pattern, int options, bool study)
{
	mRegularExpression(pattern, options, study);
}

RegularExpressionUserdata::~RegularExpressionUserdata()
{
}

UserdataType RegularExpressionUserdata::getType()
{
	return Userdata_RegularExpression;
}

// register metatable for this class
bool RegularExpressionUserdata::registerRegularExpression(lua_State* L)
{
	if (!lua_istable(L, -1))
		return false;
	
	// constructor: poco.File()
	lua_pushcfunction(L, RegularExpression);
	lua_setfield(L, -2, "RegularExpression");
	lua_pushcfunction(L, RegexMatch);
	lua_setfield(L, -2, "RegexMatch");
	
	// create metatable for Poco::File
	luaL_newmetatable(L, "Poco.RegularExpression.metatable");
	// indexing and gc
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, metamethod__gc);
	lua_setfield(L, -2, "__gc");
	lua_pushcfunction(L, metamethod__tostring);
	lua_setfield(L, -2, "__tostring");
	// methods
	lua_pushcfunction(L, extract);
	lua_setfield(L, -2, "extract");
	lua_pushcfunction(L, match);
	lua_setfield(L, -2, "match");
	lua_pushcfunction(L, extractOffsets);
	lua_setfield(L, -2, "extractOffsets");
	lua_pushcfunction(L, extractCaptures);
	lua_setfield(L, -2, "extractCaptures");
	lua_pushcfunction(L, substitute);
	lua_setfield(L, -2, "substitute");
	lua_pushcfunction(L, metamethod__eq);
	lua_setfield(L, -2, "__eq");
	lua_pop(L, 1);
	
	return true;
}

int RegularExpressionUserdata::RegexMatch(lua_State* L)
{
	
}

// constructor function
// pattern, options, study
int RegularExpressionUserdata::RegularExpression(lua_State* L)
{
	int rv = 0;
	const char* pattern = luaL_checkstring(L, 1);
	const char* optionsStr = luaL_checkstring(L, 2);
	Poco::RegularExpression::Options options = parseRegexOptions(optionsStr);
	bool study = true;
	if (lua_gettop(L) > 2 && lua_isboolean(L, 3))
		study = lua_toboolean(L, 3);
	
	void* ud = lua_newuserdata(L, sizeof RegularExpressionUserdata);
	try
	{
		RegularExpressionUserdata *reud = new (ud) RegularExpressionUserdata(pattern, options, study);
		luaL_getmetatable(L, "Poco.File.metatable");
		lua_setmetatable(L, -2);
		rv = 1;
	}
	catch (const Poco::Exception& e)
	{
		lua_pushnil(L);
		lua_pushstring(L, e.what());
		rv = 2;
	}
	catch (...)
	{
		lua_pushnil(L);
		lua_pushstring(L, "unknown error");
		rv = 2;
	}
	
	return rv;
}

// metamethod infrastructure
int RegularExpressionUserdata::metamethod__gc(lua_State* L)
{
	RegularExpressionUserdata* reud = reinterpret_cast<RegularExpressionUserdata*>(
		luaL_checkudata(L, 1, "Poco.RegularExpression.metatable"));
	reud->~RegularExpressionUserdata();
}

int RegularExpressionUserdata::metamethod__tostring(lua_State* L)
{
	RegularExpressionUserdata* reud = reinterpret_cast<RegularExpressionUserdata*>(
		luaL_checkudata(L, 1, "Poco.RegularExpression.metatable"));
	
	lua_pushfstring(L, "Poco.RegularExpression (%p)", reinterpret_cast<void*>(fud));
	return 1;
}

// userdata methods
int RegularExpressionUserdata::extract(lua_State* L)
{
	int rv = 0;
	RegularExpressionUserdata* reud = reinterpret_cast<RegularExpressionUserdata*>(
		luaL_checkudata(L, 1, "Poco.RegularExpression.metatable"));
	
	const char* subject = luaL_checkstring(L, 2);
	Poco::RegularExpression::Options options = 0;
	int startPosition = 0;
	
	int top = lua_gettop(L);
	if (top > 3)
	{
		const char* optionsStr = luaL_checkstring(L, 3);
		options = parseRegexOptions(optionsStr);
	}
	if (top > 4)
	{
		startPosition = luaL_checkint(L, 4) - 1;
		startPosition = startPosition < 0 ? 0 : startPosition;
	}
	
	try
	{
		std::string match;
		int matchCount = mRegularExpression.extract(subject, startPosition, match, options);
		lua_pushnumber(L, matchCount);
		lua_pushlstring(L, match.c_str(), match.size());
		rv = 2;
	}
	catch (const Poco::Exception& e)
	{
		lua_pushnil(L);
		lua_pushstring(L, e.what());
		rv = 2;
	}
	catch (...)
	{
		lua_pushnil(L);
		lua_pushstring(L, "unknown error");
		rv = 2;
	}
	
	return rv;
}

int RegularExpressionUserdata::match(lua_State* L)
{
	int rv = 0;
	RegularExpressionUserdata* reud = reinterpret_cast<RegularExpressionUserdata*>(
		luaL_checkudata(L, 1, "Poco.RegularExpression.metatable"));
	
	const char* subject = luaL_checkstring(L, 2);
	Poco::RegularExpression::Options options = 0;
	int startPosition = 0;
	
	int top = lua_gettop(L);
	if (top > 2)
	{
		const char* optionsStr = luaL_checkstring(L, 3);
		options = parseRegexOptions(optionsStr);
	}
	if (top > 3)
	{
		startPosition = luaL_checkint(L, 4) - 1;
		startPosition = startPosition < 0 ? 0 : startPosition;
	}
	
	try
	{
		Poco::RegularExpression::Match match;
		int matchCount = mRegularExpression.extract(subject, startPosition, match, options);
		lua_pushnumber(L, matchCount);
		if (matchCount > 0)
		{
			// return a 1-based position for start and end
			lua_pushnumber(L, match.offset + 1);
			lua_pushnumber(L, match.offset + 1 + match.length);
		}
		else
		{
			lua_pushnil(L);
			lua_pushnil(L);
		}
		rv = 3;
	}
	catch (const Poco::Exception& e)
	{
		lua_pushnil(L);
		lua_pushstring(L, e.what());
		rv = 2;
	}
	catch (...)
	{
		lua_pushnil(L);
		lua_pushstring(L, "unknown error");
		rv = 2;
	}
	
	return rv;
}

int RegularExpressionUserdata::extractOffsets(lua_State* L)
{
	int rv = 0;
	RegularExpressionUserdata* reud = reinterpret_cast<RegularExpressionUserdata*>(
		luaL_checkudata(L, 1, "Poco.RegularExpression.metatable"));
	
	size_t subjectSize = 0;
	const char* subject = luaL_checklstring(L, 2, &subjectSize);
	
	Poco::RegularExpression::Options options = 0;
	int startPosition = 0;
	
	luaL_checktype(L, 3, LUA_TTABLE);
	
	int top = lua_gettop(L);
	if (top > 3)
	{
		const char* optionsStr = luaL_checkstring(L, 4);
		options = parseRegexOptions(optionsStr);
	}
	
	if (top > 4)
	{
		startPosition = luaL_checkint(L, 5) - 1;
		startPosition = startPosition < 0 ? 0 : startPosition;
	}
	
	try
	{
		Poco::RegularExpression::MatchVec matches;
		int matchCount = mRegularExpression.match(subject, startPosition, matches, options);
		lua_pushnumber(L, matchCount);
		for (size_t i = 0; matchCount > 0 && i < matches.size(); ++i)
		{
			// we're going to trust that PCRE doesn't return match values out of bounds
			// push start and end positions of matches into table
			lua_pushnumber(L, matches[i].offset + 1);
			lua_rawseti(L, 3, i * 2 + 1);
			lua_pushnumber(L, matches[i], offset + 1 + matches[i].length);
			lua_rawseti(L, 3, i * 2 + 2);
		}
		rv = 1;
	}
	catch (const Poco::Exception& e)
	{
		lua_pushnil(L);
		lua_pushstring(L, e.what());
		rv = 2;
	}
	catch (...)
	{
		lua_pushnil(L);
		lua_pushstring(L, "unknown error");
		rv = 2;
	}
	
	return rv;
}

int RegularExpressionUserdata::extractCaptures(lua_State* L)
{
	int rv = 0;
	RegularExpressionUserdata* reud = reinterpret_cast<RegularExpressionUserdata*>(
		luaL_checkudata(L, 1, "Poco.RegularExpression.metatable"));
	
	size_t subjectSize = 0;
	const char* subject = luaL_checklstring(L, 2, &subjectSize);
	
	Poco::RegularExpression::Options options = 0;
	int startPosition = 0;
	
	luaL_checktype(L, 3, LUA_TTABLE);
	
	int top = lua_gettop(L);
	if (top > 3)
	{
		const char* optionsStr = luaL_checkstring(L, 4);
		options = parseRegexOptions(optionsStr);
	}
	
	if (top > 4)
	{
		startPosition = luaL_checkint(L, 5) - 1;
		startPosition = startPosition < 0 ? 0 : startPosition;
	}
	
	try
	{
		Poco::RegularExpression::MatchVec matches;
		int matchCount = mRegularExpression.match(subject, startPosition, matches, options);
		lua_pushnumber(L, matchCount);
		for (size_t i = 0; matchCount > 0 && i < matches.size(); ++i)
		{
			lua_pushlstring(L, subject + matches[i].offset, matches[i].length);
			// overwrite values from 1 to matchCount
			lua_rawseti(L, 3, i + 1);
		}
		rv = 1;
	}
	catch (const Poco::Exception& e)
	{
		lua_pushnil(L);
		lua_pushstring(L, e.what());
		rv = 2;
	}
	catch (...)
	{
		lua_pushnil(L);
		lua_pushstring(L, "unknown error");
		rv = 2;
	}
	
	return rv;
}

int RegularExpressionUserdata::substitute(lua_State* L)
{
	
}

int RegularExpressionUserdata::metamethod__eq(lua_State* L)
{
	
}

} // LuaPoco
