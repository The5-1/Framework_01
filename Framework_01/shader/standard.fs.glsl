#version 330

layout(location = 0)  out vec4 out0; // color 

//Fragt nach color vom Vertex Shader
in vec3 color;
in vec3 normal;
in vec4 viewPosition;
in vec4 viewCameraPosition;

uniform vec3 cameraPosition;
uniform vec3 cameraDirection;


void main() 
{ 

	//Normale sollte Länge 1 haben, normalize = auf Länge 1 setzten
	vec3 normalN = normalize(normal);
	
	//Aus einem Vec4 einen Vec3 machen, ein vec4 bestehen aus x y z w
	vec3 viewPos3 = viewPosition.xyz / viewPosition.w;
	vec3 viewCam3 = viewCameraPosition.xyz / viewCameraPosition.w;
	vec3 vertexToCam = viewCam3 - viewPos3;
	
	//Dot Product
	float dotNV = dot(normalN, vertexToCam) / 10;
	vec3 colorNV = vec3( pow(dotNV, 3) );
	

	
	
	
	//Endresultat des Fragment Shaders
	out0 = vec4(colorNV, 1);
}
