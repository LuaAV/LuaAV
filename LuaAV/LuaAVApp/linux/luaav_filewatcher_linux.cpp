#include "luaav_filewatcher.h"
#include "luaav_app.h"
#include "types/al_pq.hpp"
#include "lua_utility.h"

#include <sys/inotify.h>
#include <errno.h>
#include <inotifytools/inotifytools.h>
#include <string.h>

#include <string>
#include <set>

using std::string;
using std::set;

namespace luaav {


// used to keep track of modification dates so that
// we don't inadvertently reload files that haven't changed
// on FileWatcher callbacks
class FileInfo {
public:
	
	FileInfo(const char *filename)
	:	filename(filename),
		cb(0),
		udata(0)
	{}
	
	FileInfo(const char *filename, FileWatcher::watcher_cb cb, void *udata=0)
	:	filename(filename),
		cb(cb),
		udata(udata)
	{}

	~FileInfo()
	{}
	
	// relational operators for set<FileInfo>
	bool operator == (const FileInfo &rhs) const {
		return filename == rhs.filename;
	}
	
	bool operator < (const FileInfo &rhs) const {
		return filename < rhs.filename;
	}
	
	static const FileInfo * getInfo(const char *filename) {
		set<FileInfo>::iterator it = FileInfo::cActiveFileInfo.find(FileInfo(filename));
		if(it != FileInfo::cActiveFileInfo.end()) {
			return &(*it);
		}
		else {
			return NULL;
		}
	}

	// data members
	string filename;
	FileWatcher::watcher_cb cb;
	void *udata;
	
	// set of all FileInfo instances
	static set<FileInfo> cActiveFileInfo;
};

set<FileInfo> FileInfo::cActiveFileInfo;

/*
    * IN_ACCESS - File was accessed (read) *
    * IN_ATTRIB - Metadata changed (permissions, timestamps, extended attributes, etc.) *
    * IN_CLOSE_WRITE - File opened for writing was closed *
    * IN_CLOSE_NOWRITE - File not opened for writing was closed *
    * IN_CREATE - File/directory created in watched directory *
    * IN_DELETE - File/directory deleted from watched directory *
    * IN_DELETE_SELF - Watched file/directory was itself deleted
    * IN_MODIFY - File was modified *
    * IN_MOVE_SELF - Watched file/directory was itself moved
    * IN_MOVED_FROM - File moved out of watched directory *
    * IN_MOVED_TO - File moved into watched directory *
    * IN_OPEN - File was opened *


    * IN_DONT_FOLLOW - Don't dereference pathname if it is a symbolic link
    * IN_MASK_ADD - Add (OR) events to watch mask for this pathname if it already exists (instead of replacing mask)
    * IN_ONESHOT - Monitor pathname for one event, then remove from watch list
    * IN_ONLYDIR - Only watch pathname if it is a directory
*/

class FileWatcherImpl {
public:
    FileWatcherImpl();
    ~FileWatcherImpl();

	static bool cPolling;

    void enable();
    void disable();
};


bool FileWatcherImpl::cPolling = false;
static FileWatcherImpl *g_filewatcher = NULL;


#define FILEWATCH_PERIOD    (0.33)

/*
struct inotify_event {
    int             wd;             // watch descriptor
    uint32_t        mask;           // watch mask
    uint32_t        cookie;         // cookie to synchronize two events
    uint32_t        len;            // length (including nulls) of name
    char            name __flexarr; // stub for possible name
};

*/

static int pollfiles(al_sec now, void *ptr, luaav_state *S) {

    struct inotify_event *e = NULL;
    do {
        e = inotifytools_next_event(0);
        if(e) {
			const char *c_fpath = inotifytools_filename_from_wd(e->wd);
			if(c_fpath) {
		        if(e->mask & IN_MOVE_SELF) {
		            const luaav::FileInfo * f_info = luaav::FileInfo::getInfo(c_fpath);
					if(f_info && f_info->cb) {
						f_info->cb(c_fpath, f_info->udata);
					}
					else {
						lua::call(luaav_getstate(luaav_app_state()), "fileopen", c_fpath);
					}
		        }
				else {
					/*const luaav::FileInfo * f_info = luaav::FileInfo::getInfo(c_fpath);
					if(f_info) {
						string filename(f_info->filename);
						FileWatcher::watcher_cb cb = f_info->cb;
						void *udata = f_info->udata;

					//	luaav::FileWatcher::unwatchfile(filename.data());
					//	luaav::FileWatcher::watchfile(filename.data(), cb, udata);
					}
					else {
					//	luaav::FileWatcher::unwatchfile(c_fpath);
					//	luaav::FileWatcher::watchfile(c_fpath);
					}*/
				}
			}
        }
    } while(e);

	if(FileWatcherImpl::cPolling) {
		al::PQHelper queue(luaav_state_mainpq(S));
		queue.send(FILEWATCH_PERIOD, FILEWATCH_PERIOD, pollfiles, ptr, S);
	}

	return 0;
}

/*
    We need to poll the file watching mechanism at some low frequency.  Since
    we already have an internal timing system, we can use it to perform the poll
    instead of rolling another clock internal to the FileWatcher.
*/
FileWatcherImpl :: FileWatcherImpl() {
    if(!inotifytools_initialize()) {
        //ERROR initializing inotifytools
        //int err = inotifytools_error();
        printf("Couldn't initialize inotify.  Are you running Linux 2.6.13 or later, and was the\n option enabled when your kernel was compiled?  If so, \n something mysterious has gone wrong.");
        return;
    }
    enable();
}

FileWatcherImpl :: ~FileWatcherImpl() {
    disable();
    inotifytools_cleanup();
}

void FileWatcherImpl :: enable() {
	if(!cPolling) {
		cPolling = true;
		luaav_state *S = luaav_app_state();
		al::PQHelper queue(luaav_state_mainpq(S));
		queue.send(FILEWATCH_PERIOD, FILEWATCH_PERIOD, pollfiles, (void *)NULL, S);
	}
}

void FileWatcherImpl :: disable() {
	if(cPolling) {
		cPolling = false;
	}
}

void FileWatcher :: watchfile(const char *filename, watcher_cb cb, void *udata) {
    if(!g_filewatcher) g_filewatcher = new FileWatcherImpl();
    if(!inotifytools_watch_file(filename, IN_MODIFY | IN_MOVE_SELF)) {
        if(inotifytools_error() == ENOSPC) {
            printf("Failed to watch %s; upper limit on inotify watches reached!\n", filename);
            printf("Please increase the amount of inotify watches allowed per user via `/proc/sys/fs/inotify/ max_user_watches'.\n");
        }
        else {
            printf("Failed to watch '%s': %s\n", filename, strerror(inotifytools_error()));
        }
        return;
    }

	FileInfo::cActiveFileInfo.insert(FileInfo(filename, cb, udata));
}

void FileWatcher :: unwatchfile(const char *filename) {
    if(!g_filewatcher) g_filewatcher = new FileWatcherImpl();

	set<FileInfo>::iterator it = FileInfo::cActiveFileInfo.find(FileInfo(filename));
	if(it != FileInfo::cActiveFileInfo.end()) FileInfo::cActiveFileInfo.erase(it);

    if(!inotifytools_remove_watch_by_filename(filename)) {
        int err = inotifytools_error();
        printf("Error: unwatchingfile %d\n", err);
        return;
    }
}

bool FileWatcher :: watchingfile(const char *filename) {
    if(!g_filewatcher) g_filewatcher = new FileWatcherImpl();
    return inotifytools_watching_file(filename) == 1;
}

}   //luaav::
