#ifndef AL_OPENCL_CONTEXT_H
#define AL_OPENCL_CONTEXT_H 1

#include "al_OpenCLInternal.hpp"
#include "al_OpenCLDevice.hpp"
#include <vector>
#include <list>
#include <string>

using std::vector;
using std::list;
using std::string;

namespace al {
namespace cl {


class OpenCLContext {
public:
	OpenCLContext(cl_context context=0)
	:	mContext(context)
	{}
	
	~OpenCLContext() {
		destroy();
	}
	
	cl_context get_context() const {return mContext;}
	void create(const OpenCLDevice &dev);
	void create(const vector<OpenCLDevice> &devs);
	void destroy();
	
	void attach_resource(OpenCLResource<OpenCLContext> *resource);
	void detach_resource(OpenCLResource<OpenCLContext> *resource);

protected:
	cl_context mContext;
	vector<OpenCLDevice> mDevices;
	list<OpenCLResource<OpenCLContext> *> mResources;
};

}	// cl::
}	// al::


#endif // AL_OPENCL_CONTEXT_H
