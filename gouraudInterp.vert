
uniform vec4 lightPos;

varying vec3 normal;
varying vec3 lightVec;
varying vec3 viewVec;


void main()
{   
	vec4 vert = gl_ModelViewMatrix * gl_Vertex;
   // N should be the average of the face normals that the vertex belongs to
   viewVec = vec3(gl_ModelViewProjectionMatrix * gl_Vertex);
   normal = normalize(gl_NormalMatrix * gl_Normal);
//   lightVec = vec3(normalize(lightPos));
   lightVec = vec3(lightPos - vert);

   float dist = length(lightVec);
   
   vec3 N = normal/dist;
   
   float NdotL = max(dot(N, lightVec), 1.0);
   
   gl_Position = ftransform();

}