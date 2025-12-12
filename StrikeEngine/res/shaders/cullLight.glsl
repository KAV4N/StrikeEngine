// Thanks to DaveH355 for https://github.com/DaveH355/clustered-shading tutorial
#type COMPUTE
#version 430 core

#define LOCAL_SIZE 128
layout(local_size_x = LOCAL_SIZE, local_size_y = 1, local_size_z = 1) in;

struct PointLight {
    vec4 position;
    vec4 color; // rgb [0,1], w unused
    float intensity;
    float radius;
    float fallOff;
};

struct Cluster {
    vec4 minPoint;
    vec4 maxPoint;
    uint count;
    uint lightIndices[100];
};

layout(std430, binding = 1) restrict buffer clusterSSBO {
    Cluster clusters[];
};

layout(std430, binding = 2) restrict buffer lightSSBO {
    PointLight pointLight[];
};

uniform mat4 viewMatrix;

bool testSphereAABB(uint i, Cluster c);

void main() {
    uint lightCount = pointLight.length();
    uint index = gl_WorkGroupID.x * LOCAL_SIZE + gl_LocalInvocationID.x;
    Cluster cluster = clusters[index];

    cluster.count = 0;

    for (uint i = 0; i < lightCount; ++i) {
        if (testSphereAABB(i, cluster) && cluster.count < 100) {
            cluster.lightIndices[cluster.count] = i;
            cluster.count++;
        }
    }
    clusters[index] = cluster;
}

bool sphereAABBIntersection(vec3 center, float radius, vec3 aabbMin, vec3 aabbMax) {
    vec3 closestPoint = clamp(center, aabbMin, aabbMax);
    float distanceSquared = dot(closestPoint - center, closestPoint - center);
    return distanceSquared <= radius * radius;
}

bool testSphereAABB(uint i, Cluster cluster) {
    vec3 center = vec3(viewMatrix * pointLight[i].position);
    float radius = pointLight[i].radius;
    vec3 aabbMin = cluster.minPoint.xyz;
    vec3 aabbMax = cluster.maxPoint.xyz;
    return sphereAABBIntersection(center, radius, aabbMin, aabbMax);
}