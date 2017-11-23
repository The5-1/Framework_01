#version 330

//layout holt sich information von der Grafikkarte
layout(location = 0) in  vec3 vPosition; 
layout(location = 1) in  vec3 vNormal; 

//Information an der Fragment Shader ausgeben
out vec3 color;
out vec3 normal;
out vec4 viewPosition;
out vec4 viewCameraPosition;


//Uniform: Holt sich information aus dem Programm
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform vec3 col;
uniform vec3 cameraPosition;
uniform vec3 cameraDirection;




void main() {
	
	
	viewCameraPosition = vec4(cameraPosition, 1);
	//viewCameraPosition = viewMatrix * vec4(cameraPosition, 1);

	//Normale für ViewSpace berechnen
	mat3 normalMatrix = transpose(inverse(mat3(viewMatrix * modelMatrix)));
	normal = normalMatrix * vNormal.xyz;
	
	//color definieren
	//color = cameraDirection;
	
	//Farbe bestimmen
	//color = col;
	
	//Die Farbe abhängig der Position machen
	//color = vPosition;
	
	//Farbe abhängig der Normale
	//color = vNormal;
	
	//Farbe abhängig der Kamera Position
	//color = cameraPosition;
	
	//Farbe abhängig des KAmera Winkels
	//color = cameraDirection;
	
	//viewPosition beschreiben (position vom Vertex vom Auge der Camera
	viewPosition = viewMatrix * modelMatrix * vec4(vPosition + vec3(2.0) , 1);
	
	//													  Dieser Teil der Gleichung beschreibt die Position/Skalierung des Models
	//gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(vPosition * 2 + vec3(0, 5, 0), 1);
	gl_Position = projMatrix * viewPosition;
}

//Dot Product: Schauen die Verktoren in die selbe Richtung, gibt reelle Zahl aus; +: selbe richtung, -: unterschiedliche richtung, 0:senkrecht
//Cross Product: gibt einen Vektor aus der senkrecht auf beiden Vektoren liegt

 