#version 330

// CSci-4611 Assignment 5:  Art Render

// TODO: You need to calculate per-fragment shading here using the Phong shading
// model.

//k_aI_a + k_dI_d(n dot l) + k_sI_s(h dot n)^s
//n = normal
//l = vector from vertex to light source
// h = halfway vector

in vec3 Position;
in vec3 Normal;

out vec4 color;

uniform vec3 lightPosition;
uniform vec4 Ia, Id, Is;

uniform vec4 ka, kd, ks;
uniform float s;


void main() {
    vec3 finalColor = vec3(0.0, 0.0, 0.0);
	vec3 n = normalize(Normal);
    vec3 l = normalize(lightPosition - Position);
    vec3 e = normalize(-Position);
    vec3 h = normalize(l + e);

    vec4 ambient = ka * Ia;
    vec4 diffuse = kd * Id * clamp(dot(l, n), 0, 1);
    vec4 specular = ks * Is * pow(max(dot(n, h), 0), s);

	color = ambient + diffuse + specular;
}
