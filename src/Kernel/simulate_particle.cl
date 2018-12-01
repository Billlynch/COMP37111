typedef struct tag_vect3
{
  double x;
  double y;
  double z;
} vect3;

typedef struct tag_particle
{
  vect3 position;
  double s;
  vect3 target;
  double mass;
  vect3 speed;
  double life;
  int n_bounces;
  unsigned char r;
  unsigned char g;
  unsigned char b;
  unsigned char a;
} particle;

#define GRAVITY -9.81
#define FLOOR_Z -7.0

__kernel void simulate_particle(__global particle * particleArray, __global float * metaBuffer)
{
    size_t tid = get_global_id(0);

    if (metaBuffer[1] == 0) {
		particleArray[tid].life -= metaBuffer[0];
	}

	if (particleArray[tid].life > 0.0) {
		vect3 acc;
		acc.x = 0.0;
		acc.y = 0.0;
		acc.z = 0.0;
		double speed_multiplier = 5.5;

		if (particleArray[tid].position.z <= FLOOR_Z) {
			particleArray[tid].position.z = FLOOR_Z + 0.1;
			acc.z = 5.0 / particleArray[tid].n_bounces;
			particleArray[tid].n_bounces++;
			speed_multiplier = 50.0;
		} else {
			acc.z = GRAVITY;
		}

		particleArray[tid].speed.x = acc.x * particleArray[tid].mass * metaBuffer[0] * speed_multiplier;
		particleArray[tid].speed.y = acc.y * particleArray[tid].mass * metaBuffer[0] * speed_multiplier;
		particleArray[tid].speed.z = acc.z * particleArray[tid].mass * metaBuffer[0] * speed_multiplier;

		particleArray[tid].position.x = particleArray[tid].speed.x * metaBuffer[0];
		particleArray[tid].position.y = particleArray[tid].speed.y * metaBuffer[0];
		particleArray[tid].position.z = particleArray[tid].speed.z * metaBuffer[0];


	} else {
		particleArray[tid].camDist = -1.0;
	}

}