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

    uint tileIndex = clusterIndex.x + (clusterIndex.y * uGridSize.x) +
                     (clusterIndex.z * uGridSize.x * uGridSize.y);

    vec2 tileSize = uScreenDimensions / vec2(uGridSize.xy);

    // tile in screen-space
    vec2 minTile_screenspace = vec2(clusterIndex.xy) * tileSize;
    vec2 maxTile_screenspace = vec2(clusterIndex.xy + 1u) * tileSize;

    // convert tile to view space sitting on the near plane
    vec3 minTile = screenToView(minTile_screenspace);
    vec3 maxTile = screenToView(maxTile_screenspace);

    float planeNear = uZNear * pow(uZFar / uZNear, float(clusterIndex.z) / float(uGridSize.z));
    float planeFar = uZNear * pow(uZFar / uZNear, float(clusterIndex.z + 1u) / float(uGridSize.z));

    // the line goes from the eye position in view space (0, 0, 0)
    // through the min/max points of a tile to intersect with a given cluster's near-far planes
    vec3 minPointNear =
        lineIntersectionWithZPlane(vec3(0.0), minTile, planeNear);
    vec3 minPointFar =
        lineIntersectionWithZPlane(vec3(0.0), minTile, planeFar);
    vec3 maxPointNear =
        lineIntersectionWithZPlane(vec3(0.0), maxTile, planeNear);
    vec3 maxPointFar =
        lineIntersectionWithZPlane(vec3(0.0), maxTile, planeFar);

    clusters[tileIndex].minPoint = vec4(min(minPointNear, minPointFar), 0.0);
    clusters[tileIndex].maxPoint = vec4(max(maxPointNear, maxPointFar), 0.0);
    clusters[tileIndex].count = 0;
}

// Returns the intersection point of an infinite line and a
// plane perpendicular to the Z-axis
vec3 lineIntersectionWithZPlane(vec3 startPoint, vec3 endPoint, float zDistance)
{
    vec3 direction = endPoint - startPoint;
    vec3 normal = vec3(0.0, 0.0, -1.0); // plane normal

    float t = (zDistance - dot(normal, startPoint)) / dot(normal, direction);
    return startPoint + t * direction;
}

vec3 screenToView(vec2 screenCoord)
{
    // normalize screenCoord to [-1, 1]
    // depth is set to near plane in NDC (-1 in OpenGL)
    vec4 ndc = vec4(
        screenCoord / vec2(uScreenDimensions) * 2.0 - 1.0,
        -1.0,
        1.0
    );

    vec4 viewCoord = uInverseProjection * ndc;
    viewCoord /= viewCoord.w;
    return viewCoord.xyz;
}
