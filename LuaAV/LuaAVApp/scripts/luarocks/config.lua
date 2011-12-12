local app = require("app")

module("luarocks.config")
LUAROCKS_PREFIX=[[/usr/local]]
LUA_INCDIR=[[/usr/local/include]]
LUA_LIBDIR=[[/usr/local/lib]]
LUA_BINDIR=[[/usr/local/bin]]
--LUAROCKS_SYSCONFIG=[[/usr/local/etc/luarocks/config.lua]]
LUAROCKS_ROCKS_TREE=[[/usr/local]]
LUAROCKS_UNAME_S=[[Darwin]]
LUAROCKS_UNAME_M=[[i386]]
LUAROCKS_DOWNLOADER=[[wget]]
LUAROCKS_WGET_PATH = app.resourcepath.."/wget"
--LUAROCKS_MD5CHECKER=[[/usr/loca/bin/md5sum]]
LUAROCKS_MD5CHECKER = app.resourcepath.."/md5sum"
