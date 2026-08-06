#version 460 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aColor;
layout(location = 3) in float aBranchDepth;
layout(location = 4) in mat4 aInstanceTransform;

uniform mat4 uTransform;      // camera projection * view
uniform vec3 uCameraPosition;
uniform float uLodNearDist;   // below this: full recursion depth
uniform float uLodFarDist;    // above this: trunk + first branch level only
uniform float uMaxDepth;      // matches kFernMaxDepth on the CPU side

out vec3 vColor;

void main()
{
    vec3 instancePos = aInstanceTransform[3].xyz;
    float dist = distance(instancePos, uCameraPosition);

    // Recursion depth doubles as the LOD axis: a far fern only draws its
    // trunk + first branch level, a near one gets full recursive detail.
    // No mesh swapping needed - every depth already lives in one buffer.
    float allowedDepth = dist > uLodFarDist
        ? 1.0
        : (dist > uLodNearDist ? max(uMaxDepth - 1.0, 0.0) : uMaxDepth);

    if (aBranchDepth > allowedDepth + 0.5) {
        // Pushed outside the clip volume - both endpoints of a culled
        // segment land here identically, so the whole line disappears.
        gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
    } else {
        gl_Position = uTransform * aInstanceTransform * vec4(aPosition, 1.0);
    }

    vColor = aColor;
}
