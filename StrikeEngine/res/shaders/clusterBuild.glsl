// Based on clustered shading implementation by DaveH355
// https://github.com/DaveH355/clustered-shading

#type COMPUTE
#version 430 core
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

struct Cluster
{
    vec4 minPoint;
    vec4 maxPoint;
    uint count;
    uint lightIndices[100];
};

layout(std430, binding = 1) restrict buffer clusterSSBO
{
    Cluster clusters[];
};

/* Updated uniforms */
uniform float uZNear;
uniform float uZFar;
uniform mat4  uInverseProjection;
uniform uvec3 uGridSize;
uniform uvec2 uScreenDimensions;

vec3 screenToView(vec2 screenCoord);
vec3 lineIntersectionWithZPlane(vec3 startPoint, vec3 endPoint, float zDistance);

/*
 context: glViewport is referred to as the "screen"
 clusters are built based on a 2d screen-space grid and depth slices.
 Later when shading, it is easy to figure what cluster a fragment is in based on
 gl_FragCoord.xy and the fragment's z depth from camera
*/
void main()
{
    uvec3 clusterIndex = gl_GlobalInvocationID;
    if (clusterIndex.x >= uGridSize.x || 
        clusterIndex.y >= uGridSize.y || 
        clusterIndex.z >= uGridSize.z)
    {
        return;
    }

    uint clusterLinearIndex =
        clusterIndex.x +
        clusterIndex.y * uGridSize.x +
        clusterIndex.z * uGridSize.x * uGridSize.y;

    vec2 clusterSize = uScreenDimensions / vec2(uGridSize.xy);

    // cluster bounds in screen-space
    vec2 minCluster_screenspace = vec2(clusterIndex.xy) * clusterSize;
    vec2 maxCluster_screenspace = vec2(clusterIndex.xy + 1u) * clusterSize;

    // convert cluster bounds to view space on the near plane
    vec3 minCluster = screenToView(minCluster_screenspace);
    vec3 maxCluster = screenToView(maxCluster_screenspace);

    float planeNear =
        uZNear * pow(uZFar / uZNear, float(clusterIndex.z) / float(uGridSize.z));
    float planeFar  =
        uZNear * pow(uZFar / uZNear, float(clusterIndex.z + 1u) / float(uGridSize.z));

    // intersect cluster frustum rays with near/far planes
    vec3 minPointNear =
        lineIntersectionWithZPlane(vec3(0.0), minCluster, planeNear);
    vec3 minPointFar =
        lineIntersectionWithZPlane(vec3(0.0), minCluster, planeFar);
    vec3 maxPointNear =
        lineIntersectionWithZPlane(vec3(0.0), maxCluster, planeNear);
    vec3 maxPointFar =
        lineIntersectionWithZPlane(vec3(0.0), maxCluster, planeFar);

    clusters[clusterLinearIndex].minPoint =
        vec4(min(minPointNear, minPointFar), 0.0);
    clusters[clusterLinearIndex].maxPoint =
        vec4(max(maxPointNear, maxPointFar), 0.0);
    clusters[clusterLinearIndex].count = 0;
}

// Returns the intersection point of an infinite line and a
// plane perpendicular to the Z-axis
vec3 lineIntersectionWithZPlane(vec3 startPoint, vec3 endPoint, float zDistance)
{
    vec3 direction = endPoint - startPoint;
    vec3 normal = vec3(0.0, 0.0, -1.0);

    float t = (zDistance - dot(normal, startPoint)) / dot(normal, direction);
    return startPoint + t * direction;
}

vec3 screenToView(vec2 screenCoord)
{
    vec4 ndc = vec4(
        screenCoord / vec2(uScreenDimensions) * 2.0 - 1.0,
        -1.0,
        1.0
    );

    vec4 viewCoord = uInverseProjection * ndc;
    viewCoord /= viewCoord.w;
    return viewCoord.xyz;
}
