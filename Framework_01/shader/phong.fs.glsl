#version 330

layout(location = 0)  out vec4 out0; // color 

in vec3 lightColor;
in vec3 objectColor;
in vec3 normal;
in vec4 viewPosition;
in vec4 viewCameraPosition;
in vec3 fragPos;

uniform vec3 cameraPosition;
uniform vec3 cameraDirection;
uniform vec3 lightPos;

//Material Properties
struct Material 
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};


uniform Material material;


void main()
{
	//Ambient Lighting
	//OLD
	//float ambientStrength = 1;
	//vec3 ambient = ambientStrength * lightColor;
	//NEW
	vec3 ambient = lightColor * material.ambient;
	
	//Diffuse Lighting
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = lightColor * (diff * material.diffuse);
	
	//Specular Lighting
	//float specularStrenghth = 1;
	vec3 viewDir = normalize(viewPosition.xyz - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = lightColor * (spec * material.specular);
	
	
	//End result
	vec3 result = ambient + diffuse + specular;
    out0 = vec4(result, 1);
} 





//Dot Product: Schauen die Verktoren in die selbe Richtung, gibt reelle Zahl aus; +: selbe richtung, -: unterschiedliche richtung, 0:senkrecht
//Cross Product: gibt einen Vektor aus der senkrecht auf beiden Vektoren liegt
//Phong Shader:
//Ambient lighting:
//Jede Fläche ist immer beleuchtet, egal wie schwach.
//Diffuse lighting:
//Flächen die vom Licht angestrahlt werden (Dot Product: <=0: wird nicht angestrahlt, >0: wird angestrahlt)
//Specular lighting:
//Der hell leuchtende Punkt von Reflektionen
//
//
//1. Normale von Lichtequelle und VertexPosition
//2. DotProduct von Camera Position und der Normale
//3. 

 