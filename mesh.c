#pragma once

#include "mesh.h"

texture CreateTexture(char *Filename, texture_type TextureType)
{
    Assert(Filename);

    texture Result = {0};

    Result.Type = TextureType;

    i32 RequestedChannelCount = 0;
    i32 FlipVertically = 1;
    stbi_set_flip_vertically_on_load(FlipVertically);
    u8 *Data = stbi_load(Filename, &Result.Width, &Result.Height, &Result.ChannelCount, RequestedChannelCount);
    if(Data)
    {
        glGenTextures(1, &Result.Handle);
        glBindTexture(GL_TEXTURE_2D, Result.Handle);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        if(Result.ChannelCount == 3)
        {
            Result.Format = GL_RGB;
            Result.InternalFormat = GL_SRGB;
        }
        else if(Result.ChannelCount == 4)
        {
            Result.Format = GL_RGBA;
            // NOTE: If anything looks weird when drawing textures,
            // maybe toggle between GL_SRGB and GL_SRGB_ALPHA. I'm too
            // lazy to check opengl docs right now.
            // Result->InternalFormat = GL_SRGB;
            Result.InternalFormat = GL_SRGB_ALPHA;
        }
        else
        {
            fprintf(stderr, "%s Texture format is not GL_RGB or GL_RGBA\n", Filename);
            stbi_image_free(Data);
            Result.Type = TextureType_Null;
        }

        i32 MipMapDetailLevel = 0;
        // REMINDER: Textures to be used for data should not be uploaded as GL_SRGB!
        // NOTE: InternalFormat is the format we want to store the data, Format is the input format
        glTexImage2D(GL_TEXTURE_2D, MipMapDetailLevel, Result.InternalFormat, Result.Width, Result.Height, 0, Result.Format, GL_UNSIGNED_BYTE, Data);
        // NOTE(Jorge): Set custom MipMaps filtering values here!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(Data);
    }
    else
    {
        printf("Could not load image file: %s\n", Filename);
        Result.Type = TextureType_Null;
    }

    return Result;
}

mesh CreateMesh(vertex *Vertices, i32 VertexCount, u32 *Indices, i32 IndexCount, texture *Textures, i32 TextureCount, shader Shader)
{
    // Zero everyting and initialize dynamic arrays
    mesh Result = {0};
    ArrayInit(Result.Attributes);
    ArrayInit(Result.Indices);
    ArrayInit(Result.Textures);

    // Set Shader
    Result.Shader = Shader;

    // Copy Vertex Attributes
    for(int i = 0; i < VertexCount; i++)
    {
        ArrayPush(Result.Attributes, Vertices[i]);
    }

    // Copy Indices
    for(int i = 0; i < IndexCount; i++)
    {
        ArrayPush(Result.Indices, Indices[i]);
    }

    // Copy Textures
    for(int i = 0; i < TextureCount; i++)
    {
        ArrayPush(Result.Textures, Textures[i]);
    }

    //
    // Create GL Buffers and Upload data
    //

    glGenVertexArrays(1, &Result.VAO);
    glGenBuffers(1, &Result.VBO);
    glGenBuffers(1, &Result.EBO);

    glBindVertexArray(Result.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, Result.VBO);
    glBufferData(GL_ARRAY_BUFFER, ArrayCount(Result.Attributes) * sizeof(vertex), Result.Attributes, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Result.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ArrayCount(Result.Indices) * sizeof(u32), Result.Indices, GL_STATIC_DRAW);

    // Enable/Setup Vertex Attrib 0 : Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0);
    // Enable/Setup Vertex Attrib 1 : Normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, Normals));
    // Enable/Setup Vertex Attrib 2 : TexCoords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, TextureCoordinates));

    glBindVertexArray(0);


    // TODO(Jorge): Implement Multiple texture support
    // Bind Texture
    glUseProgram(Result.Shader);
    glBindTexture(GL_TEXTURE_2D, Result.Textures[0].Handle);

    return Result;
}
