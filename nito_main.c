/*
  ============================================================================

  - Reference Video: https://www.youtube.com/watch?v=esC1HnyD9Bk&list=PLplnkTzzqsZS3R5DjmCQsqupu43oS9CFN&index=8

      - Be able to compute the following matrices on the CPU
        - Model-View             (move to camera space for transforming positions)
        - Model-View-Projection  (Move positions to canonical view volume)
        - Model-View for Normals (Used to transform directions, watch the video in order to know how this works)

      - Load Model
        - Be able to load an obj model

      Things the video specifies we need to be able to do

      - Compute Matrices
        - Model View
        - MVP
        - Model view for normals

      - Vertex shader
        - Transform position with model-view
        - Transform position with MVP
        - Transform normal with model view for normals

      - Fragment shader
        - Implement Shading
        - Light direction can be a uniform but the matrix transform _must_ be calculated on the CPU for performance reasons. If not it will run every vertex or worse, every fragment

      - Skybox

    TODO(Jorge): Load a model
    TODO(Jorge): Integrate Dear ImGui, can be used to change the light's direction!

    - Known Bugs
      - Sometimes the program crashes at startup

  ============================================================================
 */
#include <stdio.h>

// OpenGL debug context
#ifndef OPENGL_DEBUG_MODE
#define OPENGL_DEBUG_MODE
#endif

#include <SDL.h>

#include "vendor/glad/src/glad.c"

#define STB_IMAGE_IMPLEMENTATION
#include "vendor/stb_image.h"

// These 2 pragma warnings are needed because tinyobj uses strcpy instead of s_strcpy
#pragma warning(disable:4996)
#pragma warning(disable:4706)
#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tinyobj_loader_c.h"

#include "jsanchez_shared.h"
#include "jsanchez_math.h"
#include "jsanchez_dynamic_array.h"

#include "keyboard.c"
#include "mouse.c"
#include "camera.c"
#include "clock.c"
#include "renderer.c"

#include "mesh.c"
#include "model.c"

//
// Mesh Types
//

typedef struct window
{
    SDL_Window *Handle;
    SDL_GLContext Context;

    i32 Width;
    i32 Height;
} window;

//
//
//

#define SmoothPurple (Vec4(0.4f, 0.3f, 0.5f, 1.0f))

//
//
//

void InitApplication()
{
    SDL_Init(SDL_INIT_VIDEO);

    // SDL mouse application settings
    SDL_SetRelativeMouseMode(SDL_TRUE);
    SDL_ShowCursor(SDL_ENABLE);
}

void ClearScreen(color Color)
{
    // NOTE(Jorge): We are only clearing the color buffer.
    glClearColor(Color.r, Color.g, Color.b, Color.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

char *ReadTextFile(char *Filename)
{
    // IMPORTANT(Jorge): The caller of this function needs to free the allocated pointer!
    Assert(Filename);

    SDL_RWops *RWops = SDL_RWFromFile(Filename, "rb");
    if (RWops == NULL)
    {
        return NULL;
    }

    size_t FileSize = SDL_RWsize(RWops);
    char* Result = (char*)Malloc(FileSize + 1);
    char* Buffer = Result;

    size_t BytesReadTotal = 0, BytesRead = 1;
    while (BytesReadTotal < FileSize && BytesRead != 0)
    {
        BytesRead = SDL_RWread(RWops, Buffer, 1, (FileSize - BytesReadTotal));
        BytesReadTotal += BytesRead;
        Buffer += BytesRead;
    }

    SDL_RWclose(RWops);
    if (BytesReadTotal != FileSize)
    {
        Free(Result);
        return NULL;
    }

    Result[BytesReadTotal] = '\0';

    return Result;
}

u32 CreateShader(char *Filename)
{
    Assert(Filename);

    u32 Result;
    char *SourceFile = ReadTextFile(Filename);

    // Compile Vertex Shader
    u32 VertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
    char *VertexSource[2] = {"#version 330 core\n#define VERTEX_SHADER\n", SourceFile};
    glShaderSource(VertexShaderObject, 2, VertexSource, NULL);
    glCompileShader(VertexShaderObject);
    i32 Compiled;
    glGetShaderiv(VertexShaderObject, GL_COMPILE_STATUS, &Compiled);
    if (Compiled != GL_TRUE)
    {
        i32 LogLength = 0;
        char ErrorMessage[1024];
        glGetShaderInfoLog(VertexShaderObject, 1024, &LogLength, ErrorMessage);
        fprintf(stderr, "%s-%s\n", Filename, ErrorMessage);
        VertexShaderObject = 0;
    }

    // Compile Fragment Shader
    u32 FragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
    char *FragmentSource[2] = {"#version 330 core\n#define FRAGMENT_SHADER\n", SourceFile};
    glShaderSource(FragmentShaderObject, 2, FragmentSource, NULL);
    glCompileShader(FragmentShaderObject);
    // i32 Compiled;
    glGetShaderiv(FragmentShaderObject, GL_COMPILE_STATUS, &Compiled);
    if (Compiled != GL_TRUE)
    {
        i32 LogLength = 0;
        char ErrorMessage[1024];
        glGetShaderInfoLog(FragmentShaderObject, 1024, &LogLength, ErrorMessage);
        fprintf(stderr, "%s-%s\n", Filename, ErrorMessage);
        FragmentShaderObject = 0;
    }

    // Link program
    Result = glCreateProgram();
    glAttachShader(Result, VertexShaderObject);
    glAttachShader(Result, FragmentShaderObject);
    glLinkProgram(Result);
    i32 IsLinked = 0;
    glGetProgramiv(Result, GL_LINK_STATUS, (GLint *)&IsLinked);
    if (IsLinked == GL_FALSE)
    {
        i32 MaxLogLength = 1024;
        char InfoLog[1024] = {0};
        glGetProgramInfoLog(Result, MaxLogLength, &MaxLogLength, &InfoLog[0]);
        printf("%s: SHADER PROGRAM FAILED TO COMPILE/LINK\n", Filename);
        printf("%s\n", InfoLog);
        glDeleteProgram(Result);
        Result = 0;
    }

    glDeleteShader(VertexShaderObject);
    glDeleteShader(FragmentShaderObject);
    Free(SourceFile);

    return Result;
}

void ActivateShader(shader Shader)
{
    glUseProgram(Shader);
}

void BindTexture2D(u32 TextureHandle)
{
    glBindTexture(GL_TEXTURE_2D, TextureHandle);
}

window CreateGLWindow(char *Title, i32 Width, i32 Height)
{
    window Result = {0};

    Result.Width = Width;
    Result.Height = Height;

    Result.Handle = SDL_CreateWindow(Title,
                                     SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                     Width, Height,
                                     SDL_WINDOW_OPENGL);
    Assert(Result.Handle);

    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
#ifdef OPENGL_DEBUG_MODE
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

    Result.Context = SDL_GL_CreateContext(Result.Handle);
    if(!gladLoadGL()) { printf("gladLoadGL failed!\n"); }
    SDL_GL_MakeCurrent(Result.Handle, Result.Context);

    SDL_GL_SetSwapInterval(1); // Enable vsync

    glViewport(0, 0, Width, Height);

    // TODO(Jorge): No direct opengl code in main, wrap it!
    i32 MajorVersion, MinorVersion;
    glGetIntegerv(GL_MAJOR_VERSION, &MajorVersion);
    glGetIntegerv(GL_MINOR_VERSION, &MinorVersion);

    printf("OpenGL Version %d.%d\n", MajorVersion, MinorVersion);

    return Result;
}

void SwapGLBuffers(window Window)
{
    SDL_GL_SwapWindow(Window.Handle);
}

#if 0
void DrawMesh(mesh Mesh) // TODO: Should we specify shader as input? DrawMesh(Mesh, BlinnPhongShader) ?
{
    // by passing the shader to the mesh we can set several uniforms
    // before drawing (like linking samplers to texture units).


    // Activate and Bind textures
    // glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
    // glBindTexture(GL_TEXTURE_2D, texture);

    // NOTE(Jorge): If there is more than one texture, we also need to
    // set the textures uniforms, do it only once since it is saved in
    // the VAO

    // TODO(Jorge): Upload View, Projection Matrices in Uniform Buffer Objects
    glBindVertexArray(Mesh.VAO);
    ActivateShader(Mesh.Shader);
    BindTexture2D(Mesh.Textures[0].Handle);

    // TODO(Jorge): Remove this static Translation Matrix, it does not belong here
    mat4 Model = Mat4_Translation(Vec3(0.5f, 0.7f, 0.1f));
    u32 ModelUniformLocation = glGetUniformLocation(Mesh.Shader, "Model");
    glUniformMatrix4fv(ModelUniformLocation, 1, GL_FALSE, &Model.Flat[0]);

    glDrawElements(GL_TRIANGLES, ArrayCount(Mesh.Indices), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
#endif

int main(int Argc, char **Argv)
{
    // Argc; Argv; are here to avoid the unused variable warning
    Argc; Argv;

    InitApplication();

    i32 Width = 1280;
    i32 Height = 720;
    window Window = CreateGLWindow("Project Nito", Width, Height);
    renderer Renderer = CreateRenderer();

    f32 MouseSensitivity = 5.0f;
    mouse Mouse = CreateMouse(MouseSensitivity);
    keyboard Keyboard = CreateKeyboard();

    vec3 CameraPosition = {0.0f, 0.0f, 3.0f};
    vec3 CameraFront = {0.0f, 0.0f, -1.0f};
    vec3 CameraUp = {0.0f, 1.0f, 0.0f};
    camera MainCamera = CreateCamera(Width, Height, CameraPosition, CameraFront, CameraUp);
    UploadCameraMatrices(Renderer.MatricesUBO, &MainCamera.Projection, &MainCamera.Orthographic, &MainCamera.View);

    clock  MainClock = CreateClock();



    { // Testing grounds
        char *Filename = "models\teapot\teapot.obj";

        tinyobj_attrib_t Attrib;
        tinyobj_shape_t* Shapes = NULL;
        size_t ShapeCount;
        tinyobj_material_t* Materials = NULL;
        size_t MaterialCount;

        {
            u32 Flags = TINYOBJ_FLAG_TRIANGULATE;
            i32 Ret = tinyobj_parse_obj(&Attrib, &Shapes, &ShapeCount, &Materials, &MaterialCount, Filename, get_file_data, NULL, Flags);
        }
    }


    b32 Running = 1;
    while(Running)
    {
        UpdateClock(&MainClock);

        SDL_Event Event;
        while (SDL_PollEvent(&Event))
        {
            switch (Event.type)
            {
                // TODO(Jorge): Alt + Tab windowed fullscreen
                case SDL_KEYDOWN:
                {
                    if(Event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
                    {
                        Running = 0;
                    }

                    break;
                }
                case SDL_KEYUP:
                {
                    break;
                }
                case SDL_QUIT:
                {
                    Running = 0;
                    break;
                }
                default:
                {
                    break;
                }
            }
        }

        UpdateKeyboard(&Keyboard);
        UpdateMouse(&Mouse);
        UpdateCamera(&MainCamera, &Keyboard, &Mouse, MainClock.DeltaTime);


        { // Render frame
            UpdateCameraViewMatrix(Renderer.MatricesUBO, &MainCamera.View);

            ClearScreen(SmoothPurple);

            // DrawMesh(NewMesh);
            // DrawMesh(WallMesh);

            SwapGLBuffers(Window);
        }
    }

    return 0;
}
