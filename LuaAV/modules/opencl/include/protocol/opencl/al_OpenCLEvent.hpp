#ifndef AL_OPENCL_EVENT_H
#define AL_OPENCL_EVENT_H 1

#include "al_OpenCLInternal.hpp"
#include <vector>
#include <string>

using std::vector;
using std::string;

namespace al {
namespace cl {

class OpenCLEvent {
public:	
	OpenCLEvent(cl_event event=0)
	:	mEvent(event)
	{}

	~OpenCLEvent() {}

	cl_event get_event() const {return mEvent;}

protected:
	cl_event mEvent;
};

}	// cl::
}	// al::


#endif // AL_OPENCL_EVENT_H
