
#include "ParticleSystem.h"

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.hpp>
#endif

inline void checkErr(cl_int err, const char *name) {
    if (err != CL_SUCCESS) {
        std::cerr << "Error: " << name << ": " << err << std::endl;
        exit(EXIT_FAILURE);
    }
}


int main(int argc, char* argv[])
{
    std::string file = "/home/bill/Documents/git/COMP37111/src/Models/teapot.obj";

    cl_int err;
    std::vector< cl::Platform > platformList;
    cl::Platform::get(&platformList);

    checkErr(platformList.size()!=0 ? CL_SUCCESS : -1, "cl::Platform::get");
    std::cerr << "Platform count is: " << platformList.size() << std::endl;


    cl_context_properties cprops[3] =
            {CL_CONTEXT_PLATFORM, (cl_context_properties)(platformList[0])(), 0};

    cl::Context context(
            CL_DEVICE_TYPE_GPU,
            cprops,
            NULL,
            NULL,
            &err);
    checkErr(err, "Conext::Context()");







    auto *ps = new ParticleSystem();
    ps->runParticleSystem(file);

    delete ps;
	return 0;
}
