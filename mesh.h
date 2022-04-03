#pragma once

#include "jsanchez_shared.h"
#include "jsanchez_dynamic_array.h"
#include "jsanchez_math.h"

typedef enum texture_type
{
    TextureType_Null,
    TextureType_Diffuse,
    TextureType_Specular,
} texture_type;

typedef struct texture
{
    texture_type Type;

    u32 Handle;
    i32 Width;
    i32 Height;
    i32 ChannelCount;
    GLenum InternalFormat;
    GLenum Format;
} texture;

typedef struct vertex
{
    vec3 Vertices;
    vec3 Normals;
    vec2 TextureCoordinates;
} vertex;

typedef struct mesh
{
    vertex  *Attributes;
    u32     *Indices;
    texture *Textures;

    u32 Shader;

    u32 VAO;
    u32 VBO;
    u32 EBO;
} mesh;
