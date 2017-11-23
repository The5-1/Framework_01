#version 330

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform vec3 col;
uniform vec3 cameraPosition;
uniform vec3 cameraDirection;
uniform vec3 lightPos;


layout(location = 0) in  vec3 vPosition; 
layout(location = 1) in  vec3 vNormal; 


//out
out vec3 lightColor;
out vec3 objectColor;



out vec3 normalW;
out vec3 lightW;
out vec3 viewW;
out vec3 posW;


void main()
{
	//Colors
	lightColor = vec3(1, 0.5, 0.5);
	objectColor = vec3(0.5, 0.5, 1);
	
	
	/*
	//viewPos: Position von der Camera zum Vertex
	viewPosition = viewMatrix * modelMatrix * vec4(vPosition + vec3(2.0) , 1);
	
	//mat4 modelViewMatrix = modelMatrix * viewMatrix;
	mat3 normalMatrix = transpose(inverse(mat3(viewMatrix * modelMatrix)));
	
	//Normale
	normal.xyz = (viewMatrix*vec4(normalMatrix*vNormal,0.0)).xyz;
	
    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(vPosition, 1);
	
	//Fragment Position in die World Matrix setzten für Fragment shader
	fragPos = vec3(modelMatrix * vec4(vPosition, 1.0));
	*/
}