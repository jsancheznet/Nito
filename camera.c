#pragma once

#include "camera.h"

camera CreateCamera(i32 Width, i32 Height, vec3 Position, vec3 Front, vec3 Up)
{
    camera Result = {0}; // Zeroes out everything, including Acceleration and Velocity

    Result.Position = Position;

    Result.Front = Front;
    Result.Up = Up;

    f32 CameraSpeed = 5.0f;
    Result.Speed = CameraSpeed;
    f32 FieldOfView = 90.0f;
    Result.FoV = FieldOfView;
    f32 Near = 0.1f;
    Result.Near = Near;
    f32 Far = 1000.0f;
    Result.Far = Far;

    Result.View = Mat4_LookAt(Position, Vec3(0.0f, 0.0f, 0.0f), Up);
    Result.Projection = Mat4_Perspective(ToRadians(Result.FoV), (f32)Width / (f32)Height, Result.Near, Result.Far);
    Result.Orthographic = Mat4_Orthographic(0.0f, (f32)Width, 0.0f, (f32)Height, Result.Near, Result.Far);

    // This was previously on the game loop, it needs to set this values to avoid a camera jump in the first frame
    // Implementation Reference: https://learnopengl.com/Getting-started/Camera
    Result.Yaw = -90.0f; // Set the Yaw to -90 so the mouse faces to 0, 0, 0 in the first frame X
    Result.Pitch = 0.0f;

    return Result;
}

void UpdateCamera(camera *Camera, keyboard *Keyboard, mouse *Mouse, f64 DeltaTime)
{
    // TODO: Use Newton movement equations for camera movement

    Camera->Yaw += Mouse->RelX * Mouse->Sensitivity * (f32)DeltaTime;
    Camera->Pitch += -Mouse->RelY * Mouse->Sensitivity * (f32)DeltaTime; // reversed since y-coordinates range from bottom to top

    if (Camera->Pitch > 89.0f)
    {
        Camera->Pitch = 89.0f;
    }
    else if (Camera->Pitch < -89.0f)
    {
        Camera->Pitch = -89.0f;
    }

    vec3 Front;
    Front.x = Cos(ToRadians(Camera->Yaw)) * Cos(ToRadians(Camera->Pitch));
    Front.y = Sin(ToRadians(Camera->Pitch));
    Front.z = Sin(ToRadians(Camera->Yaw)) * Cos(ToRadians(Camera->Pitch));
    Camera->Front = Vec3_Normalize(Front);

#if 1
    // Original movement code
    // FrameSpeed exists so Camera->Speed * DeltaTime is only computed once
    f32 FrameSpeed = Camera->Speed * (f32)DeltaTime;
    if (KeyIsPressed(Keyboard, SDL_SCANCODE_W))
    {
        Camera->Position = Vec3_Add(Camera->Position, Vec3_Scale(Camera->Front, FrameSpeed));
    }
    if (KeyIsPressed(Keyboard, SDL_SCANCODE_S))
    {
        Camera->Position = Vec3_Subtract(Camera->Position, Vec3_Scale(Camera->Front, FrameSpeed));
    }
    if (KeyIsPressed(Keyboard, SDL_SCANCODE_A))
    {
        Camera->Position = Vec3_Subtract(Camera->Position, Vec3_Normalize(Vec3_Scale(Vec3_Cross(Camera->Front, Camera->Up), FrameSpeed)));
    }
    if (KeyIsPressed(Keyboard, SDL_SCANCODE_D))
    {
        Camera->Position = Vec3_Add(Camera->Position, Vec3_Normalize(Vec3_Scale(Vec3_Cross(Camera->Front, Camera->Up), FrameSpeed)));
    }
    if (KeyIsPressed(Keyboard, SDL_SCANCODE_SPACE))
    {
        // TODO: Reset camera position
    }
#endif

#if 0
    //
    // Attempt to move the camera by using newtom motion, not working correctly atm
    //

    // FrameSpeed exists so Camera->Speed * DeltaTime is only computed once
    f32 FrameSpeed = Camera->Speed * (f32)DeltaTime;
    if (KeyIsPressed(Keyboard, SDL_SCANCODE_W))
    {
        Camera->Acceleration = Vec3_Subtract(Camera->Acceleration, Vec3_Add(Camera->Position, Vec3_Scale(Camera->Front, FrameSpeed)));
    }
    if (KeyIsPressed(Keyboard, SDL_SCANCODE_S))
    {
        Camera->Acceleration = Vec3_Add(Camera->Acceleration, Vec3_Subtract(Camera->Position, Vec3_Scale(Camera->Front, FrameSpeed)));
    }
    if (KeyIsPressed(Keyboard, SDL_SCANCODE_A))
    {
        Camera->Acceleration = Vec3_Add(Camera->Acceleration, Vec3_Subtract(Camera->Position, Vec3_Normalize(Vec3_Scale(Vec3_Cross(Camera->Front, Camera->Up), FrameSpeed))));
    }
    if (KeyIsPressed(Keyboard, SDL_SCANCODE_D))
    {
        Camera->Acceleration = Vec3_Add(Camera->Acceleration, Vec3_Add(Camera->Position, Vec3_Normalize(Vec3_Scale(Vec3_Cross(Camera->Front, Camera->Up), FrameSpeed))));
    }
    if (KeyIsPressed(Keyboard, SDL_SCANCODE_SPACE))
    {
        // TODO: Reset camera position
    }

    // Entity->Position = 0.5f * Entity->Acceleration * (TimeStep * TimeStep) + Entity->Velocity + Entity->Position;
    // Entity->Velocity = Entity->Acceleration * TimeStep + Entity->Velocity;
    // Entity->Acceleration = {};
    // Entity->Velocity *= Entity->Drag;

    f32 CameraDrag = 0.8f;
    Camera->Position = Vec3_Add(Vec3_Add(Vec3_Scale(Vec3_Scale(Camera->Acceleration, 0.5f), ((f32)DeltaTime * (f32)DeltaTime)), Camera->Velocity), Camera->Position);
    Camera->Velocity = Vec3_Add(Vec3_Scale(Camera->Acceleration, (f32)DeltaTime),  Camera->Velocity);
    Camera->Acceleration = Vec3(0.0f, 0.0f, 0.0f);
    Camera->Velocity = Vec3_Scale(Camera->Velocity, CameraDrag);
#endif

    // Update Camera's view matrix
    Camera->View = Mat4_LookAt(Camera->Position, Vec3_Add(Camera->Position, Camera->Front), Camera->Up);
}
