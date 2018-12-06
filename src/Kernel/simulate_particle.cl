typedef struct tag_vect3
{
  float x;
  float y;
  float z;
} vect3;

typedef struct tag_particle
{
  vect3 position;
  float size;
  vect3 target;
  float mass;
  float life;
  float randX;
  float randY;
} particle;

#define GRAVITY -9.81
#define FLOOR_Z -7.0

__kernel void simulate_particle(__global particle * particleArray,
        __global float * particleArrayToOpenGL,
        __global float * metaBuffer,
        __global bool * spaceBuffer)
{
    size_t tid = get_global_id(0);


    if (metaBuffer[tid] > 0.0) {
    		vect3 acc;
    		acc.x = 0.0;
    		acc.y = 0.0;
    		acc.z = 0.0;

    		if (particleArrayToOpenGL[(tid * 4) + 2] <= FLOOR_Z) {
    			particleArrayToOpenGL[(tid * 4) + 2] = FLOOR_Z + 0.1;
    			acc.x = particleArray[tid].randX * 0.0016;
    			acc.y = particleArray[tid].randY * 0.0016;
    			acc.z = 5.0 * 0.0016;
    		} else {
    	    	if (spaceBuffer[0] == true) {
                    acc.x = (particleArray[tid].target.x - particleArrayToOpenGL[(tid * 4) + 0])
                            * 0.016;
                    acc.y = (particleArray[tid].target.y - particleArrayToOpenGL[(tid * 4) + 1])
                            * 0.016;
                    acc.z = (particleArray[tid].target.z - particleArrayToOpenGL[(tid * 4) + 2])
                            * 0.016;

                } else {
    			    acc.z = GRAVITY * 0.0016;
                }

    		}

    		float sx = acc.x * particleArray[tid].mass;
    		float sy = acc.y * particleArray[tid].mass;
    		float sz = acc.z * particleArray[tid].mass;

    		particleArrayToOpenGL[(tid * 4) + 0] += sx;
    		particleArrayToOpenGL[(tid * 4) + 1] += sy;
    		particleArrayToOpenGL[(tid * 4) + 2] += sz;


            if (spaceBuffer[0] == false) {
    		    metaBuffer[tid] -= 0.01;
    		}
    } else {


           if (metaBuffer[tid] < -1.0 && spaceBuffer[0] == false) {
                  particleArrayToOpenGL[(tid * 4) + 0] = particleArray[tid].position.x;
                  particleArrayToOpenGL[(tid * 4) + 1] = particleArray[tid].position.y;
                  particleArrayToOpenGL[(tid * 4) + 2] = particleArray[tid].position.z;
                  particleArrayToOpenGL[(tid * 4) + 3] = particleArray[tid].size;

                  metaBuffer[tid] = particleArray[tid].life + 4;
            } else {
                particleArrayToOpenGL[(tid * 4) + 0] = -50.0;
                particleArrayToOpenGL[(tid * 4) + 1] = -50.0;
                particleArrayToOpenGL[(tid * 4) + 2] = -50.0;

                particleArrayToOpenGL[(tid * 4) + 3] = -1.0f;
                metaBuffer[tid] -= 0.01;
            }
    }


}