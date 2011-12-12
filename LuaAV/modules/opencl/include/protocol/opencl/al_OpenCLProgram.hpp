#ifndef AL_OPENCL_PROGRAM_H
#define AL_OPENCL_PROGRAM_H 1

#include "al_OpenCLInternal.hpp"
#include "al_OpenCLDevice.hpp"
#include "al_OpenCLContext.hpp"
#include <vector>
#include <string>

using std::vector;
using std::list;
using std::string;

namespace al {
namespace cl {

class OpenCLProgram : public OpenCLResource<OpenCLContext> {
public:
	OpenCLProgram(cl_program program=0)
	:	mProgram(program)
	{}
	
	virtual ~OpenCLProgram() {}
	
	cl_program get_program() const {return mProgram;}
	void create(OpenCLContext &ctx, const char *source);
	void build(const OpenCLDevice &dev);
	void build(const vector<OpenCLDevice> &devs);
	void destroy();
	
	void attach_resource(OpenCLResource<OpenCLProgram> *resource);
	void detach_resource(OpenCLResource<OpenCLProgram> *resource);

protected:
	cl_program mProgram;
	list<OpenCLResource<OpenCLProgram> *> mResources;
};

}	// cl::
}	// al::


#endif // AL_OPENCL_PROGRAM_H
