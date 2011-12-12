#include "al_OpenCLEngine.hpp"


namespace al {
namespace cl {

OpenCLEngine :: ~OpenCLEngine() {
	while(! mDeviceQueues.empty()) {
		map<cl_device_id, OpenCLCommandQueue *>::iterator it = mDeviceQueues.begin();
		OpenCLCommandQueue *queue = it->second;
		mDeviceQueues.erase(it);
		delete queue;
	}
	
	while(! mPrograms.empty()) {
		map<string, OpenCLProgram *>::iterator it = mPrograms.begin();
		OpenCLProgram *program = it->second;
		mPrograms.erase(it);
		delete program;
	}
	
	mContext.destroy();
}

const vector<OpenCLPlatform> & OpenCLEngine :: get_platforms() {
	if(mPlatforms.size() <= 0) {
		OpenCLPlatform::get_platforms(mPlatforms);
	}
	return mPlatforms;
}

OpenCLPlatform * OpenCLEngine :: get_platform(cl_platform_id id) {
	if(mPlatforms.size() <= 0) {
		OpenCLPlatform::get_platforms(mPlatforms);
	}
	for(int i=0; i < mPlatforms.size(); i++) {
		if(mPlatforms[i].get_platform() == id) {
			return &(mPlatforms[i]);
		}
	}
	return NULL;
}

const OpenCLDevice * OpenCLEngine :: get_device(cl_device_id id) {
	if(mPlatforms.size() <= 0) {
		OpenCLPlatform::get_platforms(mPlatforms);
	}
	if(mPlatforms.size() <= 0) {
		return NULL;
	}
	
	const vector<OpenCLDevice> &devices =  mPlatforms[0].get_devices();
	for(int i=0; i < devices.size(); i++) {
		if(devices[i].get_device() == id) {
			return &(devices[i]);
		}
	}
	return NULL;
}

OpenCLDevice * OpenCLEngine :: default_device() {
	if(! mContext.get_context()) {
		if(! create_context()) {
			return NULL;
		}
	}
	
	return mPlatforms.front().default_device();
}

OpenCLDevice * OpenCLEngine :: cpu_device() {
	if(! mContext.get_context()) {
		if(! create_context()) {
			return NULL;
		}
	}
	
	return mPlatforms.front().cpu_device();
}

OpenCLDevice * OpenCLEngine :: gpu_device() {
	if(! mContext.get_context()) {
		if(! create_context()) {
			return NULL;
		}
	}
	
	return mPlatforms.front().gpu_device();
}

void OpenCLEngine :: compile_source(const char *name, const char *source) {
	if(! mContext.get_context()) {
		if(! create_context()) {
			return;
		}
	}
	
	map<string, OpenCLProgram *>::iterator it = mPrograms.find(string(name));
	if(it != mPrograms.end()) {
		OpenCLProgram *old_program = it->second;
		mPrograms.erase(it);
		delete old_program;
	}

	OpenCLProgram *program = new OpenCLProgram();
	program->create(mContext, source);
	if(program->get_program() != 0) {
		program->build(mPlatforms.front().get_devices());
		mPrograms.insert(std::pair<string, OpenCLProgram *>(string(name), program));
	}
	else {
		delete program;
	}
}

OpenCLKernel * OpenCLEngine :: get_kernel(const char *name, const char *kernel_name) {
	map<string, OpenCLProgram *>::iterator it = mPrograms.find(string(name));
	if(it == mPrograms.end()) {
		return NULL;
	}
	
	OpenCLProgram *program = it->second;
	if(kernel_name) {
		return OpenCLKernel::create(*program, kernel_name);
	}
	else {
		vector<OpenCLKernel *> kers;
		OpenCLKernel::create(*program, kers);
		if(kers.size() > 0) {
			OpenCLKernel *kernel = kers.front();
			for(unsigned int i=1; i < kers.size(); i++) {
				delete kers[i];
			}
			return kernel;
		}
		else {
			return NULL;
		}
	}
}

OpenCLEvent OpenCLEngine :: enqueue_kernel(
	const OpenCLDevice &dev, 
	const OpenCLKernel *ker, 
	cl_uint ndim, 
	size_t *global, 
	size_t *local
)
{
	OpenCLCommandQueue *queue = queue_for_device(dev);
	if(queue) {
		return queue->enqueue_kernel(*ker, ndim, global, local);
	}
	else {
		return OpenCLEvent();
	}
}

OpenCLEvent OpenCLEngine :: enqueue_read(
	const OpenCLDevice &dev, 
	OpenCLMemoryBuffer *mem, 
	bool block, 
	size_t offset, 
	size_t size, 
	void *ptr
)
{
	OpenCLCommandQueue *queue = queue_for_device(dev);
	if(queue) {
		return queue->enqueue_read(*mem, block, offset, size, ptr);
	}
	else {
		return OpenCLEvent();
	}
}

OpenCLEvent OpenCLEngine :: enqueue_read(
	const OpenCLDevice &dev,
	OpenCLMemoryBuffer *mem, 
	bool block, 
	size_t offset, 
	AlloArray *array
) {
	OpenCLCommandQueue *queue = queue_for_device(dev);
	if(queue) {
		return queue->enqueue_read(*mem, block, offset, array);
	}
	else {
		return OpenCLEvent();
	}
}

OpenCLMemoryBuffer * OpenCLEngine :: create_memory_buffer(cl_mem_flags usage, size_t size, void *ptr) {
	if(! mContext.get_context()) {
		if(! create_context()) {
			return NULL;
		}
	}
	
	OpenCLMemoryBuffer *mem = new OpenCLMemoryBuffer();
	mem->create(mContext, usage, size, ptr);
	return mem;
}

OpenCLMemoryBuffer * OpenCLEngine :: create_memory_buffer(cl_mem_flags usage, AlloArray *array) {
	if(! mContext.get_context()) {
		if(! create_context()) {
			return NULL;
		}
	}
	
	OpenCLMemoryBuffer *mem = new OpenCLMemoryBuffer();
	mem->create(mContext, usage, array);
	return mem;
}

bool OpenCLEngine :: create_context() {
	if(mPlatforms.size() <= 0) {
		OpenCLPlatform::get_platforms(mPlatforms);
	}
	
	if(mPlatforms.size() <= 0) {
		return false;
	}
	
	OpenCLPlatform &platform = mPlatforms.front();
	const vector<OpenCLDevice> &devices =  platform.get_devices();
	mContext.create(devices);
	
	return mContext.get_context() != 0;
}

OpenCLCommandQueue * OpenCLEngine :: queue_for_device(const OpenCLDevice &dev) {
	if(! mContext.get_context()) {
		if(! create_context()) {
			return NULL;
		}
	}
	

	map<cl_device_id, OpenCLCommandQueue *>::iterator it =	mDeviceQueues.find(dev.get_device());
	OpenCLCommandQueue *queue = NULL;
	if(it == mDeviceQueues.end()) {
		queue = new OpenCLCommandQueue();
		queue->create(mContext, dev);
		if(queue->get_command_queue() == 0) {
			delete queue;
			return NULL;
		}
		
		mDeviceQueues.insert(
			std::pair<cl_device_id, OpenCLCommandQueue *>(
				dev.get_device(),
				queue
			)
		);
	}
	else {
		queue = it->second;
	}
	
	return queue;
}

}	// cl::
}	// al::
