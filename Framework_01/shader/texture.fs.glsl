#version 330
out vec4 FragColor;
  
in vec3 ourColor;
in vec2 TexCoord;
in vec2 changedTexCoord;

uniform sampler2D ourTexture;

void main()
{
	
    FragColor = texture(ourTexture, changedTexCoord);
	//FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}