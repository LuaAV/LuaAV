#ifndef AL_OPENCL_PLATFORM_H
#define AL_OPENCL_PLATFORM_H 1

#include "al_OpenCLInternal.hpp"
#include "al_OpenCLExtensions.hpp"
#include "al_OpenCLDevice.hpp"
#include <vector>
#include <string>

using std::vector;
using std::string;

namespace al {
namespace cl {

class OpenCLPlatform {
protected:
	OpenCLPlatform(cl_platform_id platform=0)
	:	mPlatform(platform),
		mProfile(UNKNOWN_PROFILE)
	{
		load_extension_functions();
	}

public:
	~OpenCLPlatform() {}

	static void get_platforms(vector<OpenCLPlatform> &plats);


	cl_platform_id get_platform() const {return mPlatform;}
	string get_vendor() const {return mVendor;}
	string get_name() const {return mName;}
	string get_version() const {return mVersion;}
	Profile get_profile() const {return mProfile;}
	string get_extensions() const {return mExtensions;}
	
	const vector<OpenCLDevice> & get_devices();

	OpenCLDevice * default_device();
	OpenCLDevice * cpu_device();
	OpenCLDevice * gpu_device();

protected:

	void set_platform(cl_platform_id platform) {
		mPlatform = platform;
		mDevices.clear();
	}
	
	void set_vendor(const string &vendor) {mVendor.assign(vendor);}
	void set_name(const string &name) {mName.assign(name);}
	void set_version(const string &version) {mVersion.assign(version);}
	void set_profile(Profile profile) {mProfile = profile;}
	void set_extension(const string &extensions) {mExtensions.assign(extensions);}


protected:
	
	cl_platform_id mPlatform;
	string mVendor;
	string mName;
	string mVersion;	// <space><major_version.minor_version><space><platform-specific information>
	Profile mProfile;
	string mExtensions;
	
	vector<OpenCLDevice> mDevices;
	
};

}	// cl::
}	// al::

#endif // AL_OPENCL_PLATFORM_H
