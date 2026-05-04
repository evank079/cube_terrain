#version 410 core

layout(location = 0) in vec3  aPos;
layout(location = 1) in vec3  aNormal;
layout(location = 2) in float aDisplacement;//Depricated, not used

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float uMinHeight;
uniform float uMaxHeight;

out float vDisplacement;
out vec3  vWorldPos;
out vec3  vSunDir; //per-cube sun direction
out vec3  vFaceNormal; //outward face normal (for frag)

float hash13(vec3 p) {
    p  = fract(p * vec3(443.8975, 397.2973, 491.1871));
    p += dot(p, p.yzx + 19.19);
    return fract((p.x + p.y) * p.z);
}

vec3 hash33(vec3 p) {
    return vec3(
        hash13(p + 0.0),
        hash13(p + 17.0),
        hash13(p + 31.0)
    );
}

//Value noise smoothly interpolates 3D hash. Adjacent samples are correlated
//so heights vary smoothly across vertices instead of being independent random spikes.
float vnoise(vec3 p) {
    vec3 i = floor(p);
    vec3 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);//smoothstep for cubic interpolation

    float n000 = hash13(i + vec3(0,0,0));
    float n100 = hash13(i + vec3(1,0,0));
    float n010 = hash13(i + vec3(0,1,0));
    float n110 = hash13(i + vec3(1,1,0));
    float n001 = hash13(i + vec3(0,0,1));
    float n101 = hash13(i + vec3(1,0,1));
    float n011 = hash13(i + vec3(0,1,1));
    float n111 = hash13(i + vec3(1,1,1));

    return mix(

        mix(mix(n000, n100, f.x), mix(n010, n110, f.x), f.y),
        mix(mix(n001, n101, f.x), mix(n011, n111, f.x), f.y),
        f.z

    );

}

//higher = fewer mountains, more ocean. Ridged fbm output concentrates near 0.65,
const float kHeightBias = 2.0;

//higher = sun direction stays closer to straight up across cubes. 0.0 = fully random.
const float kSunUpBias  = 0.5;

//base noise frequency || Smaller = larger continents. Higher = finer detail.
const float kNoiseScale = 0.7;

//fBm config. More octaves = more detail (and more cost). Lacunarity > 1 spaces the
//frequencies apart; gain < 1 makes higher octaves quieter.
const int kOctaves  = 4;
const float kLacunarity = 2.8;
const float kGain = 0.55;

//ridged noise: forms sharp ridge-lines along vnoise's iso-surfaces. Output [0,1]
//with peaks where vnoise crosses 0.5; gives mountain-ridge topology.
float ridged(vec3 p) {

    return 1.0 - abs(2.0 * vnoise(p) - 1.0);

}//end ridged

//fractal Brownian motion of ridged noise (aka "ridged multifractal"-ish). Stacks
//octaves so big mountain ridges have smaller ridges riding on them.
float fbm(vec3 p) {

    float sum = 0.0;
    float amp = 1.0;
    float frq = 1.0;
    float ampSum = 0.0;

    for (int i = 0; i < kOctaves; i++) {
        
        sum += ridged(p * frq) * amp;
        ampSum += amp;
        amp *= kGain;
        frq *= kLacunarity;

    }//end for loop

    return sum / ampSum;

}//end fbm

void main() {

    vec3 worldPos = (model * vec4(aPos, 1.0)).xyz;
    float u = fbm(worldPos * kNoiseScale);
    float h = mix(uMinHeight, uMaxHeight, pow(u, kHeightBias));

    vec4 displacedWorld = model * vec4(aPos - h * aNormal, 1.0);

    //per cube sun: hash the cube's world-space anchor to a direction, biased upward.
    vec3 cubeAnchor = model[3].xyz;
    vec3 sunRand = hash33(cubeAnchor) * 2.0 - 1.0;   // [-1, 1] per component
    vSunDir = normalize(sunRand + vec3(0.0, kSunUpBias, 0.0));

    vDisplacement = h;
    vWorldPos = displacedWorld.xyz;
    vFaceNormal = aNormal; // model is pure translation, so no rotation needed
    gl_Position = projection * view * displacedWorld;

}//end main
