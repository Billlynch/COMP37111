#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 squareVertices;
// Position of the center of the particule and size of the square
layout(location = 1) in vec4 xyzs;
// the colour in r g b a for the particle
layout(location = 2) in vec4 color;

// Output data
out vec4 particlecolor;

// Values that stay constant for the whole mesh.
uniform vec3 CameraRight_worldspace;
uniform vec3 CameraUp_worldspace;
  /* Model-View-Projection matrix, but without the Model
   * (the position is in BillboardPos; the orientation depends on the camera)
   */
uniform mat4 VP;

void main()
{
    // because we encoded it this way.
	float particleSize = xyzs.w;

	if (particleSize != -1.0) {
	    vec3 particleCenter_wordspace = xyzs.xyz;

    	vec3 vertexPosition_worldspace =
    		particleCenter_wordspace
    		+ CameraRight_worldspace * squareVertices.x * particleSize
    		+ CameraUp_worldspace * squareVertices.y * particleSize;

    	// Output position of the vertex
    	// set the 4th dimension to 1 which basically means ignore
    	gl_Position = VP * vec4(vertexPosition_worldspace, 1.0f);

    	particlecolor = color;
	}

}

