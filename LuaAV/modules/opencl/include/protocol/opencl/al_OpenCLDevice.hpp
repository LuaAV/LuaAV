#ifndef AL_OPENCL_DEVICE_H
#define AL_OPENCL_DEVICE_H 1

#include "al_OpenCLInternal.hpp"
#include <vector>
#include <string>

using std::vector;
using std::string;

namespace al {
namespace cl {

class OpenCLPlatform;

class OpenCLDevice {
protected:
	OpenCLDevice(cl_device_id device=0)
	:	mDevice(device),
		mVendorId(0),
		mDeviceType(CL_DEVICE_TYPE_DEFAULT),
		mProfile(UNKNOWN_PROFILE),
		mAvailable(CL_FALSE),
		mExecCapability(CL_EXEC_KERNEL),
		mQueueProperties(CL_QUEUE_PROFILING_ENABLE),
		mCompilerAvailable(CL_FALSE),
		mEndianLittle(CL_TRUE),
		mErrorCorrectionSupport(CL_FALSE),
		mMaxComputeUnits(0),
		mMaxConstantArgs(0),
		mMaxConstantBufferSize(0),
		mMaxMemAllocSize(0),
		mMaxParameterSize(0),
		mMaxReadImageArgs(0),
		mMaxSamplers(0),
		mMaxWorkGroupSize(0),
		mMaxWorkItemDimensions(0),
		mMaxWriteImageArgs(0),
		mDeviceAddressBits(0),
		mGlobalMemCacheSize(0),
		mGlobalMemCacheType(CL_NONE),
		mGlobalMemCachelineSize(0),
		mGlobalMemSize(0),
		mLocalMemSize(0),
		mLocalMemType(CL_LOCAL),
		mProfilingTimerResolution(0),
		mMaxClockFrequency(0)
	{
		mMaxWorkItemSizes[0] = 0;
		mMaxWorkItemSizes[1] = 0;
		mMaxWorkItemSizes[2] = 0;
	}

public:

	~OpenCLDevice() {}
	
	bool has_extension(const char *ext);

	static void get_devices(const OpenCLPlatform &plat, vector<OpenCLDevice> &devs);

	cl_device_id get_device() const {return mDevice;}
	string get_platform() const {return mPlatform;}
	string get_name() const {return mName;}
	string get_vendor() const {return mVendor;}
	cl_uint get_vendor_id() const {return mVendorId;}
	string get_version() const {return mVersion;}
	string get_driver_version() const {return mDriverVersion;}
	string get_extensions() const {return mExtensions;}
	cl_device_type get_device_type() const {return mDeviceType;}
	Profile get_profile() const {return mProfile;}
	cl_bool get_available() const {return mAvailable;}
	cl_device_exec_capabilities get_exec_capabilities() const {return mExecCapability;}
	cl_command_queue_properties get_queue_properties() const {return mQueueProperties;}
	cl_bool get_compiler_available() const {return mCompilerAvailable;}
	cl_bool get_endian_little() const {return mEndianLittle;}
	cl_bool get_error_correction_support() const {return mErrorCorrectionSupport;}
	cl_uint get_max_compute_units() const {return mMaxComputeUnits;}
	cl_uint get_max_constant_args() const {return mMaxConstantArgs;}
	cl_ulong get_max_constant_buffer_size() const {return mMaxConstantBufferSize;}
	cl_ulong get_max_mem_alloc_size() const {return mMaxMemAllocSize;}
	size_t get_max_parameter_size() const {return mMaxParameterSize;}
	cl_uint get_max_read_image_args() const {return mMaxReadImageArgs;}
	cl_uint get_max_sampleres() const {return mMaxSamplers;}
	size_t get_max_work_group_size() const {return mMaxWorkGroupSize;}
	cl_uint get_max_work_item_dimensions() const {return mMaxWorkItemDimensions;}
	const size_t * get_max_work_item_sizes() const {return mMaxWorkItemSizes;}
	cl_uint get_max_write_image_args() const {return mMaxWriteImageArgs;}
	cl_uint get_device_address_bits() const {return mDeviceAddressBits;}
	cl_ulong get_global_mem_cache_size() const {return mGlobalMemCacheSize;}
	cl_device_mem_cache_type get_global_mem_cache_type() const {return mGlobalMemCacheType;}
	cl_uint get_global_mem_cacheline_size() const {return mGlobalMemCachelineSize;}
	cl_ulong get_global_mem_size() const {return mGlobalMemSize;}
	cl_ulong get_local_mem_size() const {return mLocalMemSize;}
	cl_device_local_mem_type get_local_mem_type() const {return mLocalMemType;}
	size_t get_profiling_timer_resolution() const {return mProfilingTimerResolution;}
	cl_uint get_max_clock_frequency() const {return mMaxClockFrequency;}
	
protected:

	void set_device(cl_device_id v) {mDevice = v;}
	void set_platform(const string &v) {mPlatform = v;}
	void set_name(const string &v) {mName = v;}
	void set_vendor(const string &v) {mVendor = v;}
	void set_vendor_id(cl_uint v) {mVendorId = v;}
	void set_version(const string &v) {mVersion = v;}
	void set_driver_version(const string &v) {mDriverVersion = v;}
	void set_extensions(const string &v) {mExtensions = v;}
	void set_device_type(cl_device_type v) {mDeviceType = v;}
	void set_profile(Profile v) {mProfile = v;}
	void set_available(cl_bool v) {mAvailable = v;}
	void set_exec_capabilities(cl_device_exec_capabilities v) {mExecCapability = v;}
	void set_queue_properties(cl_command_queue_properties v) {mQueueProperties = v;}
	void set_compiler_available(cl_bool v) {mCompilerAvailable = v;}
	void set_endian_little(cl_bool v) {mEndianLittle = v;}
	void set_error_correction_support(cl_bool v) {mErrorCorrectionSupport = v;}
	void set_max_compute_units(cl_uint v) {mMaxComputeUnits = v;}
	void set_max_constant_args(cl_uint v) {mMaxConstantArgs = v;}
	void set_max_constant_buffer_size(cl_ulong v) {mMaxConstantBufferSize = v;}
	void set_max_mem_alloc_size(cl_ulong v) {mMaxMemAllocSize = v;}
	void set_max_parameter_size(size_t v) {mMaxParameterSize = v;}
	void set_max_read_image_args(cl_uint v) {mMaxReadImageArgs = v;}
	void set_max_samplers(cl_uint v) {mMaxSamplers = v;}
	void set_max_work_group_size(size_t v) {mMaxWorkGroupSize = v;}
	void set_max_work_item_dimensions(cl_uint v) {mMaxWorkItemDimensions = v;}
	void set_max_work_item_sizes(size_t *v) {
		mMaxWorkItemSizes[0] = v[0];
		mMaxWorkItemSizes[1] = v[1];
		mMaxWorkItemSizes[2] = v[2];
	}
	void set_max_write_image_args(cl_uint v) {mMaxWriteImageArgs = v;}
	void set_device_address_bits(cl_uint v) {mDeviceAddressBits = v;}
	void set_global_mem_cache_size(cl_ulong v) {mGlobalMemCacheSize = v;}
	void set_global_mem_cache_type(cl_device_mem_cache_type v) {mGlobalMemCacheType = v;}
	void set_global_mem_cacheline_size(cl_uint v) {mGlobalMemCachelineSize = v;}
	void set_global_mem_size(cl_ulong v) {mGlobalMemSize = v;}
	void set_local_mem_size(cl_ulong v) {mLocalMemSize = v;}
	void set_local_mem_type(cl_device_local_mem_type v) {mLocalMemType = v;}
	void set_profiling_timer_resolution(size_t v) {mProfilingTimerResolution = v;}
	void set_max_clock_frequency(cl_uint v) {mMaxClockFrequency = v;}
	
protected:
	cl_device_id mDevice;
	string mPlatform;
	string mName;
	string mVendor;
	cl_uint mVendorId;
	string mVersion;		// <space><major_version.minor_version><space><vendor-specific information>
	string mDriverVersion;	// major_number.minor_number
	string mExtensions;
	cl_device_type mDeviceType;
	Profile mProfile;
	cl_bool mAvailable;
	
	//	capabilities
	cl_device_exec_capabilities mExecCapability;
	cl_command_queue_properties mQueueProperties;
	
	// key properties
	cl_bool mCompilerAvailable;
	cl_bool mEndianLittle;
	
	// misc. properties
	cl_bool mErrorCorrectionSupport;
	
	// compute environment constants
	cl_uint mMaxComputeUnits;
	cl_uint mMaxConstantArgs;
	cl_ulong mMaxConstantBufferSize;
	cl_ulong mMaxMemAllocSize;
	size_t mMaxParameterSize;
	cl_uint mMaxReadImageArgs;
	cl_uint mMaxSamplers;
	size_t mMaxWorkGroupSize;
	cl_uint mMaxWorkItemDimensions;
	size_t mMaxWorkItemSizes[3];
	cl_uint mMaxWriteImageArgs;
	
	
	// numerics
	/*
	CL_DEVICE_SINGLE_FP_CONFIG:
	CL_DEVICE_DOUBLE_FP_CONFIG:
	CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR:
	CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT:
	CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT:
	CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG:
	CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT:
	CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE:
	*/
	
	// alignment
	/*
	CL_DEVICE_MEM_BASE_ADDR_ALIGN:
	CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE:
	*/
	
	// memory
	cl_uint mDeviceAddressBits;
	cl_ulong mGlobalMemCacheSize;
	cl_device_mem_cache_type mGlobalMemCacheType;
	cl_uint mGlobalMemCachelineSize;
	cl_ulong mGlobalMemSize;
	cl_ulong mLocalMemSize;
	cl_device_local_mem_type mLocalMemType;
	
	
	// images
/*	CL_DEVICE_IMAGE_SUPPORT:
	CL_DEVICE_IMAGE2D_MAX_HEIGHT:
	CL_DEVICE_IMAGE2D_MAX_WIDTH:
	CL_DEVICE_IMAGE3D_MAX_DEPTH:
	CL_DEVICE_IMAGE3D_MAX_HEIGHT:
	CL_DEVICE_IMAGE3D_MAX_WIDTH:
*/	
	
	// timing
	size_t mProfilingTimerResolution;
	cl_uint mMaxClockFrequency;
};

}	// cl::
}	// al::

#endif // AL_OPENCL_DEVICE_H
