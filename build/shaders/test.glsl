#ifdef VERTEX_SHADER

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 UV;

out vec3 FS_Normal;
out vec2 FS_UV;

uniform mat4 Model;
// uniform mat4 View;
// uniform mat4 Projection;

layout (std140, binding = 0) uniform CameraMatrices
{
    mat4 Projection;
    mat4 Orthographic;
    mat4 View;
};

void main()
{
    gl_Position = Projection * View * Model * vec4(Position, 1.0);
    // gl_Position = Model * vec4(Position, 1.0);
    FS_Normal = Normal;
    FS_UV = UV;
}

#endif

#ifdef FRAGMENT_SHADER

in vec3 FS_Normal;
in vec2 FS_UV;

out vec4 FragmentColor;

uniform sampler2D DiffuseTexture;

void main()
{
    FragmentColor = texture(DiffuseTexture, FS_UV);
}

#endif
