#version 410 core

in  float vDisplacement;
in  vec3  vWorldPos;
in  vec3  vSunDir;
in  vec3  vFaceNormal;
out vec4  FragColor;

uniform float uMinHeight;
uniform float uMaxHeight;

vec3 sampleGradient(float t) {

    const vec3 c0 = vec3(0.0, 0.0, 0.9); 
    const vec3 c1 = vec3(1.0, 1.0, 0.7); 
    const vec3 c2 = vec3(0.0, 0.55, 0.0);
    const vec3 c3 = vec3(0.75, 0.75, 0.75);

    t = clamp(t, 0.0, 1.0);
    float s = t * 3.0;
    if (s < 1.0) return mix(c0, c1, s);
    if (s < 2.0) return mix(c1, c2, s - 1.0);
    return mix(c2, c3, s - 2.0);

}//end sampleGradient

void main() {

    float t = (vDisplacement - uMinHeight) / (uMaxHeight - uMinHeight);
    vec3  baseColor = sampleGradient(t);

    //flat shaded normal from screenspace derivatives of world position
    vec3 N = normalize(cross(dFdx(vWorldPos), dFdy(vWorldPos)));

    //we want the normals pointing inward
    if (dot(N, vFaceNormal) > 0.0) N = -N;

    vec3 L = normalize(vSunDir);

    //low ambient + high diffuse ratio so slope-induced shading (i.e. terrain bumps)
    //dominates the per-pixel brightness and the geometry actually reads as 3D.
    float ambient = 0.2;
    float diffuse = 0.8 * max(dot(N, L), 0.0);

    FragColor = vec4(baseColor * (ambient + diffuse), 1.0);

}//end main
