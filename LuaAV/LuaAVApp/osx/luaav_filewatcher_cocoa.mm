#include "luaav_filewatcher.h"

// Cocoa file watching
#import "UKKQueue.h"
#import "UKFileWatcher.h"
#import <Cocoa/Cocoa.h>

#import <string>
#import <set>

using std::string;
using std::set;

namespace luaav {
	
// Keeps track of modification dates so that
// we don't inadvertently reload files that haven't changed
// on FileWatcher callbacks
class FileInfo {
public:
	
	FileInfo(const char *filename)
	:	filename(filename),
		moddate(0),
		cb(0),
		udata(0)
	{}
	
	FileInfo(const char *filename, NSTimeInterval moddate)
	:	filename(filename),
		moddate(moddate),
		cb(0),
		udata(0)
	{}
	
	FileInfo(const char *filename, NSTimeInterval moddate, FileWatcher::watcher_cb cb, void *udata=0)
	:	filename(filename),
		moddate(moddate),
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

	// utility functions
	static NSTimeInterval getModDate(const char *filename) {
		NSTimeInterval moddate = 0;
		
		NSString *path = [NSString stringWithCString:filename encoding:NSASCIIStringEncoding];
		NSDictionary *attrs = [[NSFileManager defaultManager] fileAttributesAtPath:path traverseLink:YES];
		
		
		if (attrs != nil) {
//			NSNumber *fileSize;
//			if (fileSize = [attrs objectForKey:NSFileSize]) {
//				NSLog(@"File size: %qi\n", [fileSize unsignedLongLongValue]);
//			}
//			NSString *fileOwner;
//			if (fileOwner = [attrs objectForKey:NSFileOwnerAccountName]) {
//				NSLog(@"Owner: %@\n", fileOwner);
//			}
			NSDate *date;
			if (date = [attrs objectForKey:NSFileModificationDate]) {
				moddate = [date timeIntervalSince1970];
			}
		}
		
//		NSDictionary *attrs = [[NSFileManager defaultManager] attributesOfItemAtPath:path error:&err];
//		if(attrs) {
//			NSDate *date = [attrs fileModificationDate];
//			moddate = [date timeIntervalSince1970];
//		}
		
		return moddate;
	}
	
	static bool fileHasChanged(const char *filename) {
		int moddate = 0;
		set<FileInfo>::iterator it = FileInfo::cActiveFileInfo.find(FileInfo(filename));
		if(it != FileInfo::cActiveFileInfo.end()) moddate = it->moddate;
		
		return getModDate(filename) > moddate;
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
	NSTimeInterval moddate;
	FileWatcher::watcher_cb cb;
	void *udata;
	
	// set of all FileInfo instances
	static set<FileInfo> cActiveFileInfo;
};

set<FileInfo> FileInfo::cActiveFileInfo;
	
}	// luaav::


@interface LuaAVFileWatcher : NSObject {
	// nothing
}

@end



static LuaAVFileWatcher *file_watcher = nil;

@implementation LuaAVFileWatcher

- (id)init
{
	// init memory
	self = [super init];

	// initialize filewatcher functionality
	[[UKKQueue sharedFileWatcher] setDelegate:self];	// set self as delegate for notifications
	NSWorkspace *workspace = [NSWorkspace sharedWorkspace];
	NSNotificationCenter *notificationCenter = [workspace notificationCenter];
	NSArray *notifications = [NSArray arrayWithObjects:
		UKFileWatcherRenameNotification,
		UKFileWatcherWriteNotification,
		UKFileWatcherDeleteNotification,
		UKFileWatcherAttributeChangeNotification,
		UKFileWatcherSizeIncreaseNotification,
		UKFileWatcherLinkCountChangeNotification,
		UKFileWatcherAccessRevocationNotification,
		nil];

	for(int i=0; i < [notifications count]; i++) {
		[notificationCenter addObserver:self selector:@selector(autowatchHandler:) name:[notifications objectAtIndex:i] object:nil];
	}
	
	return self;
}

// this gets called because we reigistered with UKFileWatcher
- (void)watcher:(id<UKFileWatcher>)kq receivedNotification:(NSString *)nm forPath:(NSString *)fpath
{
	if(nm == UKFileWatcherAttributeChangeNotification || nm == UKFileWatcherDeleteNotification) {
		const char *c_fpath = [fpath cStringUsingEncoding: NSASCIIStringEncoding];
		if(luaav::FileInfo::fileHasChanged(c_fpath)) {
			const luaav::FileInfo * f_info = luaav::FileInfo::getInfo(c_fpath);
			if(f_info && f_info->cb) {
				f_info->cb(c_fpath, f_info->udata);
			}
		}
		else {
			const luaav::FileInfo * f_info = luaav::FileInfo::getInfo(c_fpath);
			if(f_info) {
				luaav::FileWatcher::watcher_cb cb = f_info->cb;
				void *udata = f_info->udata;
				luaav::FileWatcher::unwatchfile(c_fpath);	// f_info is invalid after this call
				luaav::FileWatcher::watchfile(c_fpath, cb, udata);
			}
		}
	}
}

@end


namespace luaav {

void FileWatcher :: watchfile(const char *filename, watcher_cb cb, void *udata) {
	NSString *f = [NSString stringWithCString:filename encoding:NSASCIIStringEncoding];
	if(! file_watcher) file_watcher = [[LuaAVFileWatcher alloc] init];
	[[UKKQueue sharedFileWatcher] addPathToQueue:f];
	
//	printf("watching: %s cb: %x\n", filename, (void *)cb);
	FileInfo::cActiveFileInfo.insert(FileInfo(filename, FileInfo::getModDate(filename), cb, udata));
	
	// for open recent menu list (when not overriding with LuaAVMenu classes)
	[[NSDocumentController sharedDocumentController] noteNewRecentDocumentURL: [NSURL URLWithString: f]];
}

bool FileWatcher :: watchingfile(const char *filename) {
	NSString *f = [NSString stringWithCString:filename encoding:NSASCIIStringEncoding];
	if(! file_watcher) file_watcher = [[LuaAVFileWatcher alloc] init];
	return [[UKKQueue sharedFileWatcher] watchingFile:f];
}

void FileWatcher :: unwatchfile(const char *filename) {
//	printf("UNwatching: %s\n", filename);
	set<FileInfo>::iterator it = FileInfo::cActiveFileInfo.find(FileInfo(filename));
	if(it != FileInfo::cActiveFileInfo.end()) FileInfo::cActiveFileInfo.erase(it);

	NSString *f = [NSString stringWithCString:filename encoding:NSASCIIStringEncoding];
	if(! file_watcher) file_watcher = [[LuaAVFileWatcher alloc] init];
	[[UKKQueue sharedFileWatcher] removePathFromQueue:f];
}

}	// luaav::
