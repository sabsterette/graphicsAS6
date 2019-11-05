varying vec3 normal;
varying vec3 lightVec;
varying vec3 viewVec;

uniform vec4 AmbientContribution;
uniform vec4 DiffuseContribution;
uniform vec4 SpecularContribution;
uniform float exponent;
uniform int illuminationMode;

void main()
{
	vec3 norm = normalize(normal);

	vec3 L = normalize(lightVec);
	vec3 V = normalize(viewVec);
	vec3 H = normalize(L + V);
	// k used for cook torrance
	float k = 1.0;
 	
	float NdotL = dot(L, norm);
	float NdotV = max(dot(norm, V), 0.0);
	float NdotH = clamp(dot(H, norm), 0.0, 1.0);
	float VdotH = max(dot(V, H), 0.0);

	//ambient
	vec4 ambient = AmbientContribution;
    
	// diffuse lighting
	vec4 diffuse =  DiffuseContribution * NdotL + 0.5;
   
	// specular 
	vec3 reflectionVec = normalize(reflect(L, norm));
	float RdotV = max(dot(-V, reflectionVec), 0.0);
	vec4 temp1 =  { 1.0, 1.0, 1.0, 1.0 };
	vec4 specular = pow(RdotV, exponent * gl_FrontMaterial.shininess) * SpecularContribution;
	specular = clamp(specular, 0, 1);
	vec4 result = ambient + diffuse + specular;

	gl_FragColor = result;
}