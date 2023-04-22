#pragma once

#include "jsanchez_shared.h"
#include "jsanchez_math.h"
#include "jsanchez_dynamic_array.h"

#include "model.h"

i32 StringSize(char *Begin, char *End)
{
    i32 Result = 0;

    while(Begin != End)
    {
        Begin++;
        Result++;
    }

    return Result;
}

char *PathFromFilename(char *Filename)
{
    char *Result = NULL;

    char *LastOcurrence = strrchr(Filename, '/');
    i32 Size = StringSize(Filename, LastOcurrence);
    Result = Malloc(Size + 2);
    memcpy(Result, Filename, Size);
    Result[Size] = '/';
    Result[Size+1] = '\0';

    return Result;
}

#if 0
void ProcessNode(struct aiNode *Node, const struct aiScene *Scene)
{
    // process all the node's meshes (if any)
    for(u32 i = 0; i < Node->mNumMeshes; i++)
    {
        struct aiMesh *Mesh = Scene->mMeshes[Node->mMeshes[i]];

        printf("Aasdas!\n");
        Mesh;

        ContinueWorkHere;
        // - Study how Assimp Scene really works
        // - Continue working on ProcessNode
        // - Create CreateModel Function and try to store as much of the model loading as we can there.

        // 1 - CreateModel Functions
        //     - Call LoadModel from CreateFunction
        //     - Initialize the Meshes Array!
        // THE MESHES VARIABLE IS INSIDE mesh struct, we should ini
        // meshes.push_back(processMesh(mesh, scene)); // meshes is a private class member, we need to create one and init its dyn array
        // ArrayPush(Meshes, ProcessMesh(Mesh, Scene));
    }

    // then do the same for each of its children
    for(u32 i = 0; i < Node->mNumChildren; i++)
    {
        ProcessNode(Node->mChildren[i], Scene);
    }
}
#endif

#if 0
model LoadModel(char *Filename)
{
    model Result = {0};

    // Start the import on the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll t
    // probably to request more postprocessing than we do in this example.

    // List of postprocessing options: http://assimp.sourceforge.net/lib_html/postprocess_8h.html
    // aiProcess_GenNormals: creates normal vectors for each vertex if the model doesn't contain normal vectors.
    const struct aiScene* Scene = aiImportFile(Filename, aiProcess_Triangulate | aiProcess_FlipUVs);

    // Handle possible loading errors
    if(!Scene || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !Scene->mRootNode)
    {
        printf("Assimp: %s\n", aiGetErrorString());
    }

    Result.Path = PathFromFilename(Filename);

    ProcessNode(Scene->mRootNode, Scene);

    // We're done. Release all resources associated with this import
    aiReleaseImport(Scene);

    return Result;
}
#endif
#if 0
void DrawModel(model Model)
{
    // for(unsigned int i = 0; i < meshes.size(); i++)
    //     meshes[i].Draw(shader);

    // NOTE: Loop over all the meshes and call draw on each mesh
    Model;
}
#endif
