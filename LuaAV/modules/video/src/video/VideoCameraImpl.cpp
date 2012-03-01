#include "VideoCameraImpl.h"

#if defined (__APPLE__) || defined (OSX)
#include "QtKitVideoCameraImpl.h"
#else
#include "ofVideoCameraImpl.h"
#endif

namespace video {

VideoCameraImpl :: VideoCameraImpl(Type type)
:	mType(type)
{}

VideoCameraImpl :: ~VideoCameraImpl()
{}
	
VideoCameraImpl * VideoCameraImpl :: create() {
	VideoCameraImpl *impl = NULL;
#if defined (__APPLE__) || defined (OSX)
	impl = new QtKitVideoCameraImpl();
#else
	impl = new ofVideoCameraImpl();
#endif
	return impl;
}

void VideoCameraImpl :: destroy(VideoCameraImpl *impl) {
	delete impl;
}

#if defined (__APPLE__) || defined (OSX)

VideoCameraImpl * VideoCameraImpl :: create(const char *uid) {
	VideoCameraImpl *impl = NULL;
	impl = new QtKitVideoCameraImpl(uid);
	return impl;
}

void VideoCameraImpl :: list_devices(vector<Device> &devices) {
	QtKitVideoCameraImpl::list_devices(devices);
}
#endif

}	// video::

