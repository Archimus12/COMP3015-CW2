#version 460

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

out vec3 Colour;
out vec3 LightIntensity;

uniform struct LightInfo {
    vec4 Position; // Light position in eye coords.
    vec3 La; // Ambient light intensity
    vec3 Ld; // Diffuse
    vec3 Ls; // Specular
} lights[3];
uniform struct MaterialInfo {
    vec3 Ka; // Ambient reflectivity
    vec3 Kd; // Diffuse reflectivity
    vec3 Ks; // Specular reflectivity
    float Shininess; // Specular shininess factor
    vec3 Colour; // Specular reflectivity
    bool animated;
    float Time;
} Material;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;

//vertex animation variables
uniform float Frequency = 0.5f;
uniform float Velocity = 1.5f;
uniform float Amplitude = 0.3f;

vec3 phong( int light, vec3 position, vec3 n )
{
    //Calculates ambient light
    vec3 ambient = lights[light].La * Material.Ka;

    vec3 s = normalize(vec3(lights[light].Position) -(position*lights[light].Position.w));

    float sDotN = max(dot(s,n),0.0);

    //Calculates diffuse light
    vec3 diffuse = lights[light].Ld * Material.Kd * sDotN;

    //Calculates specular light
    vec3 spec = vec3(0.0);
    if( sDotN > 0.0 )
    {
        vec3 v = normalize(-position.xyz);
        vec3 r = reflect(-s,n);
        spec = lights[light].Ls * Material.Ks*pow(max(dot(r,v),0.0),Material.Shininess);
    }
    //return vec3(1.0);
    return ambient + lights[light].La * (diffuse + spec);
}
void getCamSpaceValues(out vec3 norm, out vec4 position){
    //Calculates position in camera space
    norm = normalize(NormalMatrix*VertexNormal);
    position = (ModelViewMatrix * vec4(VertexPosition,1.0));
}
void main()
{
    vec4 pos = vec4(VertexPosition,1.0f);

    if (Material.animated){
        //Translate y coordinates
        float u = Frequency * pos.x - Velocity * Material.Time;
        pos.y += Amplitude * sin(u + pos.y);

        //compute normal
        vec3 n = vec3(0.0);
        n.xy = normalize(vec2(cos(u),1.0));
    }
    

    vec3 CamNorm;
    vec4 camPosition;
    getCamSpaceValues(CamNorm,camPosition);
    Colour = Material.Colour;
    //Colour=vec3(0.0);
    //goes through 3 lights
    for (int i=0; i<3;i++){
    //Adds the colours
        Colour+=phong(i,vec3(camPosition),CamNorm);
    }
    gl_Position=MVP*pos;
}

