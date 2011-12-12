/*
** LuaSQL, ODBC driver
** Authors: Pedro Rabinovitch, Roberto Ierusalimschy, Diego Nehab,
** Tomas Guisasola
** See Copyright Notice in license.html
** $Id: ls_odbc.c,v 1.37 2007/08/22 18:37:06 tomas Exp $
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(_WIN32)
#include <windows.h>
#include <sqlext.h>
#elif defined(INFORMIX)
#include "infxcli.h"
#elif defined(UNIXODBC)
#include "sql.h"
#include "sqltypes.h"
#include "sqlext.h"
#endif

#include "lua.h"
#include "lauxlib.h"
#if ! defined (LUA_VERSION_NUM) || LUA_VERSION_NUM < 501
#include "compat-5.1.h"
#endif


#include "luasql.h"

#define LUASQL_ENVIRONMENT_ODBC "ODBC environment"
#define LUASQL_CONNECTION_ODBC "ODBC connection"
#define LUASQL_CURSOR_ODBC "ODBC cursor"


typedef struct {
	short      closed;
	int        conn_counter;
	SQLHENV    henv;               /* environment handle */
} env_data;


typedef struct {
	short      closed;
	int        cur_counter;
	int        env;                /* reference to environment */
	SQLHDBC    hdbc;               /* database connection handle */
} conn_data;


typedef struct {
	short      closed;
	int        conn;               /* reference to connection */
	int        numcols;            /* number of columns */
	int        coltypes, colnames; /* reference to column information tables */
	SQLHSTMT   hstmt;              /* statement handle */
} cur_data;


/* we are lazy */
#define hENV SQL_HANDLE_ENV
#define hSTMT SQL_HANDLE_STMT
#define hDBC SQL_HANDLE_DBC
#define error(a) ((a) != SQL_SUCCESS && (a) != SQL_SUCCESS_WITH_INFO) 


LUASQL_API int luaopen_luasql_odbc (lua_State *L);

/*
** Check for valid environment.
*/
static env_data *getenvironment (lua_State *L) {
	env_data *env = (env_data *)luaL_checkudata (L, 1, LUASQL_ENVIRONMENT_ODBC);
	luaL_argcheck (L, env != NULL, 1, LUASQL_PREFIX"environment expected");
	luaL_argcheck (L, !env->closed, 1, LUASQL_PREFIX"environment is closed");
	return env;
}


/*
** Check for valid connection.
*/
static conn_data *getconnection (lua_State *L) {
	conn_data *conn = (conn_data *)luaL_checkudata (L, 1, LUASQL_CONNECTION_ODBC);
	luaL_argcheck (L, conn != NULL, 1, LUASQL_PREFIX"connection expected");
	luaL_argcheck (L, !conn->closed, 1, LUASQL_PREFIX"connection is closed");
	return conn;
}


/*
** Check for valid cursor.
*/
static cur_data *getcursor (lua_State *L) {
	cur_data *cursor = (cur_data *)luaL_checkudata (L, 1, LUASQL_CURSOR_ODBC);
	luaL_argcheck (L, cursor != NULL, 1, LUASQL_PREFIX"cursor expected");
	luaL_argcheck (L, !cursor->closed, 1, LUASQL_PREFIX"cursor is closed");
	return cursor;
}


/*
** Pushes true and returns 1
*/
static int pass(lua_State *L) {
    lua_pushboolean (L, 1);
    return 1;
}


/*
** Fails with error message from ODBC
** Inputs: 
**   type: type of handle used in operation
**   handle: handle used in operation
*/
static int fail(lua_State *L,  const SQLSMALLINT type, const SQLHANDLE handle) {
    SQLCHAR State[6];
    SQLINTEGER NativeError;
    SQLSMALLINT MsgSize, i;
    SQLRETURN ret;
    char Msg[SQL_MAX_MESSAGE_LENGTH];
    luaL_Buffer b;
    lua_pushnil(L);

    luaL_buffinit(L, &b);
    i = 1;
    while (1) {
        ret = SQLGetDiagRec(type, handle, i, State, &NativeError, Msg, 
                sizeof(Msg), &MsgSize);
        if (ret == SQL_NO_DATA) break;
        luaL_addlstring(&b, Msg, MsgSize);
        luaL_putchar(&b, '\n');
        i++;
    } 
    luaL_pushresult(&b);
    return 2;
}

/*
** Returns the name of an equivalent lua type for a SQL type.
*/
static const char *sqltypetolua (const SQLSMALLINT type) {
    switch (type) {
        case SQL_UNKNOWN_TYPE: case SQL_CHAR: case SQL_VARCHAR: 
        case SQL_TYPE_DATE: case SQL_TYPE_TIME: case SQL_TYPE_TIMESTAMP: 
        case SQL_DATE: case SQL_INTERVAL: case SQL_TIMESTAMP: 
        case SQL_LONGVARCHAR:
            return "string";
        case SQL_BIGINT: case SQL_TINYINT: case SQL_NUMERIC: 
        case SQL_DECIMAL: case SQL_INTEGER: case SQL_SMALLINT: 
        case SQL_FLOAT: case SQL_REAL: case SQL_DOUBLE:
            return "number";
        case SQL_BINARY: case SQL_VARBINARY: case SQL_LONGVARBINARY:
            return "binary";	/* !!!!!! nao seria string? */
        case SQL_BIT:
            return "boolean";
        default:
            assert(0);
            return NULL;
    }
}


/*
** Retrieves data from the i_th column in the current row
** Input
**   types: index in stack of column types table
**   hstmt: statement handle
**   i: column number
** Returns:
**   0 if successfull, non-zero otherwise;
*/
static int push_column(lua_State *L, int coltypes, const SQLHSTMT hstmt, 
        SQLUSMALLINT i) {
    const char *tname;
    char type;
    /* get column type from types table */
	lua_rawgeti (L, LUA_REGISTRYINDEX, coltypes);
	lua_rawgeti (L, -1, i);	/* typename of the column */
    tname = lua_tostring(L, -1);
    if (!tname)
		return luasql_faildirect(L, LUASQL_PREFIX"Invalid type in table.");
    type = tname[1];
    lua_pop(L, 2);	/* pops type name and coltypes table */

    /* deal with data according to type */
    switch (type) {
        /* nUmber */
        case 'u': { 
			double num;
			SQLINTEGER got;
			SQLRETURN rc = SQLGetData(hstmt, i, SQL_C_DOUBLE, &num, 0, &got);
			if (error(rc))
				return fail(L, hSTMT, hstmt);
			if (got == SQL_NULL_DATA)
				lua_pushnil(L);
			else
				lua_pushnumber(L, num);
			return 0;
		}
                  /* bOol */
        case 'o': { 
			char b;
			SQLINTEGER got;
			SQLRETURN rc = SQLGetData(hstmt, i, SQL_C_BIT, &b, 0, &got);
			if (error(rc))
				return fail(L, hSTMT, hstmt);
			if (got == SQL_NULL_DATA)
				lua_pushnil(L);
			else
				lua_pushboolean(L, b);
			return 0;
		}
        /* sTring */
        case 't': 
        /* bInary */
        case 'i': { 
			SQLSMALLINT stype = (type == 't') ? SQL_C_CHAR : SQL_C_BINARY;
			SQLINTEGER got;
			char *buffer;
			luaL_Buffer b;
			SQLRETURN rc;
			luaL_buffinit(L, &b);
			buffer = luaL_prepbuffer(&b);
			rc = SQLGetData(hstmt, i, stype, buffer, LUAL_BUFFERSIZE, &got);
			if (got == SQL_NULL_DATA) {
				lua_pushnil(L);
				return 0;
			}
			/* concat intermediary chunks */
			while (rc == SQL_SUCCESS_WITH_INFO) {
				if (got >= LUAL_BUFFERSIZE || got == SQL_NO_TOTAL) {
					got = LUAL_BUFFERSIZE;
					/* get rid of null termination in string block */
					if (stype == SQL_C_CHAR) got--;
				}
				luaL_addsize(&b, got);
				buffer = luaL_prepbuffer(&b);
				rc = SQLGetData(hstmt, i, stype, buffer, 
					LUAL_BUFFERSIZE, &got);
			}
			/* concat last chunk */
			if (rc == SQL_SUCCESS) {
				if (got >= LUAL_BUFFERSIZE || got == SQL_NO_TOTAL) {
					got = LUAL_BUFFERSIZE;
					/* get rid of null termination in string block */
					if (stype == SQL_C_CHAR) got--;
				}
				luaL_addsize(&b, got);
			}
			if (rc == SQL_ERROR) return fail(L, hSTMT, hstmt);
			/* return everything we got */
			luaL_pushresult(&b);
			return 0;
		}
    }
    return 0;
}

/*
** Get another row of the given cursor.
*/
static int cur_fetch (lua_State *L) {
    cur_data *cur = (cur_data *) getcursor (L);
    SQLHSTMT hstmt = cur->hstmt;
    int ret; 
    SQLRETURN rc = SQLFetch(cur->hstmt); 
    if (rc == SQL_NO_DATA) {
        lua_pushnil(L);
        return 1;
    } else if (error(rc)) return fail(L, hSTMT, hstmt);

	if (lua_istable (L, 2)) {
		SQLUSMALLINT i;
		const char *opts = luaL_optstring (L, 3, "n");
		int num = strchr (opts, 'n') != NULL;
		int alpha = strchr (opts, 'a') != NULL;
		for (i = 1; i <= cur->numcols; i++) {
			ret = push_column (L, cur->coltypes, hstmt, i);
			if (ret)
				return ret;
			if (alpha) {
				lua_rawgeti (L, LUA_REGISTRYINDEX, cur->colnames);
				lua_rawgeti (L, -1, i); /* gets column name */
				lua_pushvalue (L, -3); /* duplicates column value */
				lua_rawset (L, 2); /* table[name] = value */
				lua_pop (L, 1);	/* pops colnames table */
			}
			if (num)
				lua_rawseti (L, 2, i);
			else
				lua_pop (L, 1); /* pops value */
		}
		lua_pushvalue (L, 2);
		return 1;	/* return table */
	}
	else {
		SQLUSMALLINT i;
		luaL_checkstack (L, cur->numcols, LUASQL_PREFIX"too many columns");
		for (i = 1; i <= cur->numcols; i++) {
			ret = push_column (L, cur->coltypes, hstmt, i);
			if (ret)
				return ret;
		}
		return cur->numcols;
	}
}

/*
** Closes a cursor.
*/
static int cur_close (lua_State *L) {
	conn_data *conn;
	cur_data *cur = (cur_data *) luaL_checkudata (L, 1, LUASQL_CURSOR_ODBC);
	SQLRETURN ret;
	luaL_argcheck (L, cur != NULL, 1, LUASQL_PREFIX"cursor expected");
	if (cur->closed) {
		lua_pushboolean (L, 0);
		return 1;
	}

	/* Nullify structure fields. */
	cur->closed = 1;
	ret = SQLCloseCursor(cur->hstmt);
    if (error(ret))
		return fail(L, hSTMT, cur->hstmt);
	ret = SQLFreeHandle(hSTMT, cur->hstmt);
	if (error(ret))
		return fail(L, hSTMT, cur->hstmt);
	/* Decrement cursor counter on connection object */
	lua_rawgeti (L, LUA_REGISTRYINDEX, cur->conn);
	conn = lua_touserdata (L, -1);
	conn->cur_counter--;
	luaL_unref (L, LUA_REGISTRYINDEX, cur->conn);
	luaL_unref (L, LUA_REGISTRYINDEX, cur->colnames);
	luaL_unref (L, LUA_REGISTRYINDEX, cur->coltypes);
    return pass(L);
}


/*
** Returns the table with column names.
*/
static int cur_colnames (lua_State *L) {
	cur_data *cur = (cur_data *) getcursor (L);
	lua_rawgeti (L, LUA_REGISTRYINDEX, cur->colnames);
	return 1;
}


/*
** Returns the table with column types.
*/
static int cur_coltypes (lua_State *L) {
	cur_data *cur = (cur_data *) getcursor (L);
	lua_rawgeti (L, LUA_REGISTRYINDEX, cur->coltypes);
	return 1;
}


/*
** Creates two tables with the names and the types of the columns.
*/
static void create_colinfo (lua_State *L, cur_data *cur) {
	SQLCHAR buffer[256];
	SQLSMALLINT namelen, datatype, i;
	SQLRETURN ret;
	int types, names;

	lua_newtable(L);
	types = lua_gettop (L);
	lua_newtable(L);
	names = lua_gettop (L);
	for (i = 1; i <= cur->numcols; i++) {
		ret = SQLDescribeCol(cur->hstmt, i, buffer, sizeof(buffer), 
                &namelen, &datatype, NULL, NULL, NULL);
		/*if (ret == SQL_ERROR) return fail(L, hSTMT, cur->hstmt);*/
		lua_pushstring (L, buffer);
		lua_rawseti (L, names, i);
		lua_pushstring(L, sqltypetolua(datatype));
		lua_rawseti (L, types, i);
	}
	cur->colnames = luaL_ref (L, LUA_REGISTRYINDEX);
	cur->coltypes = luaL_ref (L, LUA_REGISTRYINDEX);
}


/*
** Creates a cursor table and leave it on the top of the stack.
*/
static int create_cursor (lua_State *L, int o, conn_data *conn, 
        const SQLHSTMT hstmt, const SQLSMALLINT numcols) {
    cur_data *cur = (cur_data *) lua_newuserdata(L, sizeof(cur_data));
	luasql_setmeta (L, LUASQL_CURSOR_ODBC);

	conn->cur_counter++;
    /* fill in structure */
	cur->closed = 0;
	cur->conn = LUA_NOREF;
    cur->numcols = numcols;
	cur->colnames = LUA_NOREF;
	cur->coltypes = LUA_NOREF;
    cur->hstmt = hstmt;
	lua_pushvalue (L, o);
    cur->conn = luaL_ref (L, LUA_REGISTRYINDEX);

	/* make and store column information table */
	create_colinfo (L, cur);

    return 1;
}


/*
** Closes a connection.
*/
static int conn_close (lua_State *L) {
	SQLRETURN ret;
	env_data *env;
    conn_data *conn = (conn_data *)luaL_checkudata(L,1,LUASQL_CONNECTION_ODBC);
	luaL_argcheck (L, conn != NULL, 1, LUASQL_PREFIX"connection expected");
	if (conn->closed) {
		lua_pushboolean (L, 0);
		return 1;
	}
	if (conn->cur_counter > 0)
		return luaL_error (L, LUASQL_PREFIX"there are open cursors");

	/* Decrement connection counter on environment object */
	lua_rawgeti (L, LUA_REGISTRYINDEX, conn->env);
	env = lua_touserdata (L, -1);
	env->conn_counter--;
	/* Nullify structure fields. */
	conn->closed = 1;
	luaL_unref (L, LUA_REGISTRYINDEX, conn->env);
	ret = SQLDisconnect(conn->hdbc);
	if (error(ret))
		return fail(L, hDBC, conn->hdbc);
	ret = SQLFreeHandle(hDBC, conn->hdbc);
	if (error(ret))
		return fail(L, hDBC, conn->hdbc);
    return pass(L);
}


/*
** Executes a SQL statement.
** Returns
**   cursor object: if there are results or
**   row count: number of rows affected by statement if no results
*/
static int conn_execute (lua_State *L) {
	conn_data *conn = (conn_data *) getconnection (L);
	const char *statement = luaL_checkstring(L, 2);
	SQLHDBC hdbc = conn->hdbc;
	SQLHSTMT hstmt;
	SQLSMALLINT numcols;
	SQLRETURN ret;
	ret = SQLAllocHandle(hSTMT, hdbc, &hstmt);
	if (error(ret))
		return fail(L, hDBC, hdbc);

	ret = SQLPrepare(hstmt, (char *) statement, SQL_NTS);
	if (error(ret)) {
		ret = fail(L, hSTMT, hstmt);
		SQLFreeHandle(hSTMT, hstmt);
		return ret;
	}

	/* execute the statement */
	ret = SQLExecute (hstmt);
	if (error(ret)) {
		ret = fail(L, hSTMT, hstmt);
		SQLFreeHandle(hSTMT, hstmt);
		return ret;
	}

	/* determine the number of results */
	ret = SQLNumResultCols (hstmt, &numcols);
	if (error(ret)) {
		ret = fail(L, hSTMT, hstmt);
		SQLFreeHandle(hSTMT, hstmt);
		return ret;
	}

	if (numcols > 0)
    	/* if there is a results table (e.g., SELECT) */
		return create_cursor (L, 1, conn, hstmt, numcols);
	else {
		/* if action has no results (e.g., UPDATE) */
		SQLINTEGER numrows;
		ret = SQLRowCount(hstmt, &numrows);
		if (error(ret)) {
			ret = fail(L, hSTMT, hstmt);
			SQLFreeHandle(hSTMT, hstmt);
			return ret;
		}
		lua_pushnumber(L, numrows);
		SQLFreeHandle(hSTMT, hstmt);
		return 1;
	}
}

/*
** Rolls back a transaction. 
*/
static int conn_commit (lua_State *L) {
	conn_data *conn = (conn_data *) getconnection (L);
	SQLRETURN ret = SQLEndTran(hDBC, conn->hdbc, SQL_COMMIT);
	if (error(ret))
		return fail(L, hSTMT, conn->hdbc);
	else
		return pass(L);
}

/*
** Rollback the current transaction. 
*/
static int conn_rollback (lua_State *L) {
	conn_data *conn = (conn_data *) getconnection (L);
	SQLRETURN ret = SQLEndTran(hDBC, conn->hdbc, SQL_ROLLBACK);
	if (error(ret))
		return fail(L, hSTMT, conn->hdbc);
	else
		return pass(L);
}

/*
** Sets the auto commit mode
*/
static int conn_setautocommit (lua_State *L) {
	conn_data *conn = (conn_data *) getconnection (L);
	SQLRETURN ret;
	if (lua_toboolean (L, 2)) {
		ret = SQLSetConnectAttr(conn->hdbc, SQL_ATTR_AUTOCOMMIT,
			(SQLPOINTER) SQL_AUTOCOMMIT_ON, 0);
	} else {
		ret = SQLSetConnectAttr(conn->hdbc, SQL_ATTR_AUTOCOMMIT,
			(SQLPOINTER) SQL_AUTOCOMMIT_OFF, 0);
	}
	if (error(ret))
		return fail(L, hSTMT, conn->hdbc);
	else
		return pass(L);
}


/*
** Create a new Connection object and push it on top of the stack.
*/
static int create_connection (lua_State *L, int o, env_data *env, SQLHDBC hdbc) {
	conn_data *conn = (conn_data *) lua_newuserdata(L, sizeof(conn_data));
	/* set auto commit mode */
	SQLRETURN ret = SQLSetConnectAttr(hdbc, SQL_ATTR_AUTOCOMMIT, 
		(SQLPOINTER) SQL_AUTOCOMMIT_ON, 0);
	if (error(ret))
		return fail(L, hDBC, hdbc);

	luasql_setmeta (L, LUASQL_CONNECTION_ODBC);

	/* fill in structure */
	conn->closed = 0;
	conn->cur_counter = 0;
	conn->env = LUA_NOREF;
	conn->hdbc = hdbc;
	lua_pushvalue (L, o);
	conn->env = luaL_ref (L, LUA_REGISTRYINDEX);
	env->conn_counter++;
	return 1;
}


/*
** Creates and returns a connection object
** Lua Input: source [, user [, pass]]
**   source: data source
**   user, pass: data source authentication information
** Lua Returns:
**   connection object if successfull
**   nil and error message otherwise.
*/
static int env_connect (lua_State *L) {
	env_data *env = (env_data *) getenvironment (L);
	const char *sourcename = luaL_checkstring (L, 2);
	const char *username = luaL_optstring (L, 3, NULL);
	const char *password = luaL_optstring (L, 4, NULL);
	SQLHDBC hdbc;
	SQLRETURN ret;

	/* tries to allocate connection handle */
	ret = SQLAllocHandle (hDBC, env->henv, &hdbc);
	if (error(ret))
		return luasql_faildirect (L, LUASQL_PREFIX"connection allocation error.");

	/* tries to connect handle */
	ret = SQLConnect (hdbc, (char *) sourcename, SQL_NTS, 
		(char *) username, SQL_NTS, (char *) password, SQL_NTS);
	if (error(ret)) {
		ret = fail(L, hDBC, hdbc);
		SQLFreeHandle(hDBC, hdbc);
		return ret;
	}

	/* success, return connection object */
	return create_connection (L, 1, env, hdbc);
}

/*
** Closes an environment object
*/
static int env_close (lua_State *L) {
	SQLRETURN ret;
	env_data *env = (env_data *)luaL_checkudata(L, 1, LUASQL_ENVIRONMENT_ODBC);
	luaL_argcheck (L, env != NULL, 1, LUASQL_PREFIX"environment expected");
	if (env->closed) {
		lua_pushboolean (L, 0);
		return 1;
	}
	if (env->conn_counter > 0)
		return luaL_error (L, LUASQL_PREFIX"there are open connections");

	env->closed = 1;
	ret = SQLFreeHandle (hENV, env->henv);
	if (error(ret)) {
		int ret = fail(L, hENV, env->henv);
		env->henv = NULL;
		return ret;
	}
	return pass(L);
}

/*
** Create metatables for each class of object.
*/
static void create_metatables (lua_State *L) {
	struct luaL_reg environment_methods[] = {
		{"close", env_close},
		{"connect", env_connect},
		{NULL, NULL},
	};
	struct luaL_reg connection_methods[] = {
		{"close", conn_close},
		{"execute", conn_execute},
		{"commit", conn_commit},
		{"rollback", conn_rollback},
		{"setautocommit", conn_setautocommit},
		{NULL, NULL},
	};
	struct luaL_reg cursor_methods[] = {
		{"close", cur_close},
		{"fetch", cur_fetch},
		{"getcoltypes", cur_coltypes},
		{"getcolnames", cur_colnames},
		{NULL, NULL},
	};
	luasql_createmeta (L, LUASQL_ENVIRONMENT_ODBC, environment_methods);
	luasql_createmeta (L, LUASQL_CONNECTION_ODBC, connection_methods);
	luasql_createmeta (L, LUASQL_CURSOR_ODBC, cursor_methods);
	lua_pop (L, 3);
}


/*
** Creates an Environment and returns it.
*/
static int create_environment (lua_State *L) {
	env_data *env;
	SQLHENV henv;
	SQLRETURN ret = SQLAllocHandle(hENV, SQL_NULL_HANDLE, &henv);
	if (error(ret))
		return luasql_faildirect(L,LUASQL_PREFIX"error creating environment.");

	ret = SQLSetEnvAttr (henv, SQL_ATTR_ODBC_VERSION, 
		(void*)SQL_OV_ODBC3, 0);
	if (error(ret)) {
		ret = luasql_faildirect (L, LUASQL_PREFIX"error setting SQL version.");
		SQLFreeHandle (hENV, henv);
		return ret;
  }

	env = (env_data *)lua_newuserdata (L, sizeof (env_data));
	luasql_setmeta (L, LUASQL_ENVIRONMENT_ODBC);
	/* fill in structure */
	env->closed = 0;
	env->conn_counter = 0;
	env->henv = henv;
	return 1;
}


/*
** Creates the metatables for the objects and registers the
** driver open method.
*/
LUASQL_API int luaopen_luasql_odbc (lua_State *L) {
	struct luaL_reg driver[] = {
		{"odbc", create_environment},
		{NULL, NULL},
	};
	create_metatables (L);
	luaL_openlib (L, LUASQL_TABLENAME, driver, 0);
	luasql_set_info (L);
	return 1;
} 
