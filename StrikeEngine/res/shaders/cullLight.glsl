// Based on clustered shading implementation by DaveH355
// https://github.com/DaveH355/clustered-shading

#type COMPUTE
#version 430 core

#define LOCAL_SIZE 128
#define TOTAL_CLUSTERS (16 * 9 * 24)
layout(local_size_x = LOCAL_SIZE, local_size_y = 1, local_size_z = 1) in;

struct PointLight
{
    vec4 position;
    vec4 color;
    float intensity;
    float radius;
};

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

layout(std430, binding = 2) restrict buffer lightSSBO
{
    PointLight pointLight[];
};

uniform mat4 uViewMatrix;

// Sphere-AABB intersection test
bool sphereAABBIntersection(vec3 sphereCenter, float sphereRadius, vec3 aabbMin, vec3 aabbMax)
{
    // Find the closest point on the AABB to the sphere center
    vec3 closestPoint = clamp(sphereCenter, aabbMin, aabbMax);
    
    // Calculate the distance between the sphere center and closest point
    vec3 diff = sphereCenter - closestPoint;
    float distanceSquared = dot(diff, diff);
    
    // Check if the distance is less than or equal to the sphere radius
    return distanceSquared <= (sphereRadius * sphereRadius);
}

void main()
{
    // Get the cluster index from the invocation
    uint clusterIndex = gl_GlobalInvocationID.x;
    
    // Early exit if outside cluster bounds
    if (clusterIndex >= TOTAL_CLUSTERS)
    {
        return;
    }

    // Get cluster AABB bounds in view space
    vec3 clusterMin = clusters[clusterIndex].minPoint.xyz;
    vec3 clusterMax = clusters[clusterIndex].maxPoint.xyz;
    
    // Reset light count for this cluster
    uint lightCount = 0;
    uint totalLightCount = pointLight.length();

    
    // Test each light against this cluster
    for (uint i = 0; i < totalLightCount; i++)
    {
        vec3 lightPosition = (uViewMatrix * pointLight[i].position).xyz;
        float lightRadius = pointLight[i].radius;

        if (sphereAABBIntersection(lightPosition, lightRadius, clusterMin, clusterMax))
        {
            clusters[clusterIndex].lightIndices[lightCount] = i;
            lightCount++;
        }

        if (lightCount >= 100) 
            break; 
    }
    
    clusters[clusterIndex].count = lightCount;
}