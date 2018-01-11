#version 400

// Vertex buffer
in vec3 vertex;
in vec3 normal;
in vec3 color;

// Uniform (global) buffer
uniform mat4 world_mat;
uniform mat4 view_mat;
uniform mat4 normal_mat;
uniform float timer;

// Attributes forwarded to the geometry shader
out vec4 particle_color;
out float particle_id;

// Simulation parameters (constants)
uniform vec3 up_vec = vec3(0.0, 1.0, 0.0); // Up direction
float accel = 1.2; // An acceleration applied to the particles coming from some attraction force
float speed = 0.5; // Control the speed of the motion
float grav = 0.2; // gravity

// Define some useful constants
const float pi = 3.1415926536;
const float pi_over_two = 1.5707963268;
const float two_pi = 2.0*pi;


void main()
{
    // Define particle id
    particle_id = color.r; // Derived from the particle color. We use the id to keep track of particles
                                   

	float circtime = timer - 4.0 * floor(timer / 4);
    float t = circtime - 1; // Our time parameter

	vec4 position = world_mat * vec4(vertex, 1.0);
	vec4 norm = normal_mat * vec4(normal, 1.0);


    // First, work in local model coordinates (do not apply any transformation)
	if (circtime > 1)
	{
		position.x += norm.x*t*speed;
		position.y += norm.y*t*speed;
		position.z += norm.z*t*speed;
	}

	// Define output position but do not apply the projection matrix yet
    gl_Position = view_mat * position;
    
    // Define amount of blending depending on the cyclic time
    float alpha = 1.0 - circtime/2;
    particle_color = vec4(color, alpha);
}
