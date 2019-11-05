#define PI 3.14159265

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
   float roughness = 1.0;
   float F0 = 3.0;
   float k = 1.0;

   vec3 norm = normalize(normal);

   vec3 L = normalize(lightVec);
   vec3 V = normalize(viewVec);
   vec3 H = normalize(L + V);
    
   float NdotL = dot(L, norm);
   float NdotV = max(dot(norm, V), 0.0);
   float NdotH = clamp(dot(H, norm), 0.0, 1.0);
   float VdotH = max(dot(V, H), 0.0);
    
    //Cooke Torrence illumination
   //Ir = IiaRa + sigma(Iid(NdotL) dwil(sRs + dRd))
    //Iia 
   if (NdotL > 0.0) {
          //fresnel constant
         float F = pow(1.0 - VdotH, 5.0);
         F *= (1.0 - F0);
         F += F0;
         
          //getting Beckmann distribution function
          //D = 1/m^2cos^4alpha
          //deals with roughness
         float msquared = roughness * roughness;
         float r1 = 1.0/(4.0 * msquared * pow(NdotH, 4.0));
         float r2 = (NdotH * NdotH - 1.0) / (msquared * NdotH * NdotH);
         float D = r1 * exp(r2);
         
          //geometrical attenuation factor G
         float g1 = (2.0 * NdotH * NdotV)/VdotH;
         float g2 = (2.0 * NdotH * NdotL)/VdotH;
         float G = min(1.0, min(g1, g2));
         
         float Rs = (F * D * G) / (PI * NdotL * NdotV);

   }
   
   vec4 specular = (DiffuseContribution * NdotL) + (SpecularContribution * NdotL)
    * (k + DiffuseContribution * (1.0 - k));
   
   gl_FragColor = specular;
}
