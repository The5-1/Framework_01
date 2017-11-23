#version 330 
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;


out vec3 ourColor;
out vec2 TexCoord;
out vec2 changedTexCoord;

void main()
{
	mat4 modelMatrixT = mat4(	5.0, 0.0, 0.0, 0.0, 
								0.0, 5.0, 0.0, 0.0, 
								0.0, 0.0, 5.0, 0.0, 
								0.0, 0.0, 0.0, 1.0);
								
	//modelMatrixT = mat4(mat3(5.0));
	
    gl_Position = projMatrix * viewMatrix * modelMatrixT * vec4(aPos, 1.0);
    ourColor = aColor;
    TexCoord = aTexCoord;
	changedTexCoord = TexCoord + vec2(1, 0.5);
}