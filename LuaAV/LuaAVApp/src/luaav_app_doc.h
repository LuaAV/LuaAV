#error "luaav_app_doc.h is a dummy file for documentation"


/*! Application building and manipulation functions
	The app module provides functions and classes for building platform-specific 
	applications with a platform-independent interface.  app contains classes for 
	windowing, building menus, etc. as well as functions for opening and saving files, 
	cipboard interaction, and access to paths on the file system needed by an 
	application such as user home directory, the path of the application etc.
	
	@module app
*/
int luaopen_app(lua_State * L);

/*! Opens an open file panel
	Opens a modal open file panel with multiple selection.  An optional starting 
	directory can be specified.
	
	@param [dir] Starting directory to set panel to
	@ret filename or table of filenames in case of multiple selection
*/
int openpanel(lua_State *L);

/*! Opens an save file panel
	Opens a modal save file panel.  An optional starting 
	directory can be specified.
	
	@param [dir] Starting directory to set panel to
	@ret filename
*/
int savepanel(lua_State *L);

/*! Opens a url in the default web browser
	Opens a url in the default web browser.

	@param url Url to open
*/
int openurl(lua_State *L);

/*! Paste and copy text from the clipboard
	Paste and copy text from the clipboard.  If an 
	argument is specified, the argument will be copied 
	to the clipboard, otherwise the contents of the clipboard 
	will be returned, which can be used in a paste operation.
	
	@param [text] Text to copy to the clipboard
	@ret text to paste from the clipboard
*/
int clipboard(lua_State *L);


/*! (LINUX ONLY) Fork a command
	(LINUX ONLY) Forks a command using the bash shell.  This command 
	is useful for executing commands in a non-blocking manner such as 
	opening a file in a text editor.
	
	@param [cmd] The command string to execute
*/
int fork(lua_State *L);


/*
app_apppath;
app_resourcepath;
app_homepath;
app_configpath;
app_logpath;
app_separator;
app_platform;
*/