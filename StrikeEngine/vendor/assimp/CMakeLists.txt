cmake_minimum_required(VERSION 3.10)
project(assimp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Define options
option(ASSIMP_BUILD_TESTS "Build Assimp tests" OFF)
option(ASSIMP_BUILD_ASSIMP_TOOLS "Build Assimp tools" OFF)

# Add definitions
add_definitions(
    -DASSIMP_BUILD_NO_OWN_ZLIB
    -DASSIMP_BUILD_NO_X_IMPORTER
    -DASSIMP_BUILD_NO_3DS_IMPORTER
    -DASSIMP_BUILD_NO_MD3_IMPORTER
    -DASSIMP_BUILD_NO_MDL_IMPORTER
    -DASSIMP_BUILD_NO_MD2_IMPORTER
    -DASSIMP_BUILD_NO_ASE_IMPORTER
    -DASSIMP_BUILD_NO_AMF_IMPORTER
    -DASSIMP_BUILD_NO_HMP_IMPORTER
    -DASSIMP_BUILD_NO_SMD_IMPORTER
    -DASSIMP_BUILD_NO_MDC_IMPORTER
    -DASSIMP_BUILD_NO_MD5_IMPORTER
    -DASSIMP_BUILD_NO_STL_IMPORTER
    -DASSIMP_BUILD_NO_LWO_IMPORTER
    -DASSIMP_BUILD_NO_DXF_IMPORTER
    -DASSIMP_BUILD_NO_NFF_IMPORTER
    -DASSIMP_BUILD_NO_RAW_IMPORTER
    -DASSIMP_BUILD_NO_OFF_IMPORTER
    -DASSIMP_BUILD_NO_AC_IMPORTER
    -DASSIMP_BUILD_NO_BVH_IMPORTER
    -DASSIMP_BUILD_NO_IRRMESH_IMPORTER
    -DASSIMP_BUILD_NO_IRR_IMPORTER
    -DASSIMP_BUILD_NO_Q3D_IMPORTER
    -DASSIMP_BUILD_NO_B3D_IMPORTER
    -DASSIMP_BUILD_NO_TERRAGEN_IMPORTER
    -DASSIMP_BUILD_NO_CSM_IMPORTER
    -DASSIMP_BUILD_NO_3D_IMPORTER
    -DASSIMP_BUILD_NO_LWS_IMPORTER
    -DASSIMP_BUILD_NO_OGRE_IMPORTER
    -DASSIMP_BUILD_NO_OPENGEX_IMPORTER
    -DASSIMP_BUILD_NO_MS3D_IMPORTER
    -DASSIMP_BUILD_NO_COB_IMPORTER
    -DASSIMP_BUILD_NO_BLEND_IMPORTER
    -DASSIMP_BUILD_NO_Q3BSP_IMPORTER
    -DASSIMP_BUILD_NO_NDO_IMPORTER
    -DASSIMP_BUILD_NO_IFC_IMPORTER
    -DASSIMP_BUILD_NO_XGL_IMPORTER
    -DASSIMP_BUILD_NO_FBX_IMPORTER
    -DASSIMP_BUILD_NO_ASSBIN_IMPORTER
    -DASSIMP_BUILD_NO_C4D_IMPORTER
    -DASSIMP_BUILD_NO_3MF_IMPORTER
    -DASSIMP_BUILD_NO_X3D_IMPORTER
    -DASSIMP_BUILD_NO_MMD_IMPORTER
    -DASSIMP_BUILD_NO_STEP_EXPORTER
    -DASSIMP_BUILD_NO_SIB_IMPORTER
    -DASSIMP_BUILD_NO_JOINVERTICES_PROCESS
    -DASSIMP_BUILD_NO_GENFACENORMALS_PROCESS
    -DASSIMP_BUILD_NO_REMOVEVC_PROCESS
    -DASSIMP_BUILD_NO_SPLITLARGEMESHES_PROCESS
    -DASSIMP_BUILD_NO_PRETRANSFORMVERTICES_PROCESS
    -DASSIMP_BUILD_NO_LIMITBONEWEIGHTS_PROCESS
    -DASSIMP_BUILD_NO_IMPROVECACHELOCALITY_PROCESS
    -DASSIMP_BUILD_NO_FIXINFACINGNORMALS_PROCESS
    -DASSIMP_BUILD_NO_REMOVE_REDUNDANTMATERIALS_PROCESS
    -DASSIMP_BUILD_NO_FINDINVALIDDATA_PROCESS
    -DASSIMP_BUILD_NO_FINDDEGENERATES_PROCESS
    -DASSIMP_BUILD_NO_SORTBYPTYPE_PROCESS
    -DASSIMP_BUILD_NO_GENUVCOORDS_PROCESS
    -DASSIMP_BUILD_NO_TRANSFORMTEXCOORDS_PROCESS
    -DASSIMP_BUILD_NO_FINDINSTANCES_PROCESS
    -DASSIMP_BUILD_NO_OPTIMIZEMESHES_PROCESS
    -DASSIMP_BUILD_NO_OPTIMIZEGRAPH_PROCESS
    -DASSIMP_BUILD_NO_SPLITBYBONECOUNT_PROCESS
    -DASSIMP_BUILD_NO_DEBONE_PROCESS
    -DASSIMP_BUILD_NO_EMBEDTEXTURES_PROCESS
    -DASSIMP_BUILD_NO_GLOBALSCALE_PROCESS
)

# Include directories
include_directories(
    include
    contrib/irrXML
    contrib/zlib
    contrib/rapidjson/include
)

# Source files
file(GLOB_RECURSE SOURCES 
      "include/**"
      "code/Assimp.cpp"
      "code/BaseImporter.cpp"
      "code/ColladaLoader.cpp"
      "code/ColladaParser.cpp"
      "code/CreateAnimMesh.cpp"
      "code/PlyParser.cpp"
      "code/PlyLoader.cpp"
      "code/BaseProcess.cpp"
      "code/EmbedTexturesProcess.cpp"
      "code/ConvertToLHProcess.cpp"
      "code/DefaultIOStream.cpp"
      "code/DefaultIOSystem.cpp"
      "code/DefaultLogger.cpp"
      "code/GenVertexNormalsProcess.cpp"
      "code/Importer.cpp"
      "code/ImporterRegistry.cpp"
      "code/MaterialSystem.cpp"
      "code/PostStepRegistry.cpp"
      "code/ProcessHelper.cpp"
      "code/scene.cpp"
      "code/ScenePreprocessor.cpp"
      "code/ScaleProcess.cpp"
      "code/SGSpatialSort.cpp"
      "code/SkeletonMeshBuilder.cpp"
      "code/SpatialSort.cpp"
      "code/TriangulateProcess.cpp"
      "code/ValidateDataStructure.cpp"
      "code/Version.cpp"
      "code/VertexTriangleAdjacency.cpp"
      "code/ObjFileImporter.cpp"
      "code/ObjFileMtlImporter.cpp"
      "code/ObjFileParser.cpp"
      "code/glTFImporter.cpp"
      "code/glTF2Importer.cpp"
      "code/MakeVerboseFormat.cpp"
      "code/CalcTangentsProcess.cpp"
      "code/ScaleProcess.cpp"
      "code/EmbedTexturesProcess.cpp"
      "contrib/irrXML/*"
)

# Create static library
add_library(${PROJECT_NAME} STATIC ${SOURCES})

# Set properties for different configurations
set_target_properties(${PROJECT_NAME} PROPERTIES 
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Set Debug/Release specific properties
set_target_properties(${PROJECT_NAME} PROPERTIES 
    DEBUG_POSTFIX "d"
)

install(TARGETS ${PROJECT_NAME}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/ DESTINATION include)