#pragma once

#include "renderer.h"

renderer CreateRenderer()
{
    renderer Result = {0};

    { // Create Matrices Uniform buffer object who holds the camera matrices
        glGenBuffers(1, &Result.MatricesUBO);
        glBindBuffer(GL_UNIFORM_BUFFER, Result.MatricesUBO);
        i32 UniformMatrixCount = 3;
        glBufferData(GL_UNIFORM_BUFFER, sizeof(mat4) * UniformMatrixCount, NULL, GL_STATIC_DRAW);
        // glBindBufferBase links MatricesUBO to binding point 0 in the active shader, do i call this for each shader? only once? on each frame?
        // TODO: Test BindBufferBase to be called only once
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, Result.MatricesUBO);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    return Result;
}

void UploadCameraMatrices(u32 MatricesUBO, mat4 *Projection, mat4 *Orthographic, mat4 *View)
{
    Assert(Projection);
    Assert(Orthographic);
    Assert(View);

    glBindBuffer(GL_UNIFORM_BUFFER, MatricesUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(mat4), Projection);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4), sizeof(mat4), Orthographic);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4)*2, sizeof(mat4), View);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UpdateCameraViewMatrix(u32 MatricesUBO, mat4 *View)
{
    Assert(View);
    Assert(MatricesUBO != 0);

    // NOTE: MatricesUBO consist of 3 Matrices, Projection,
    // Orthographic and View. View is the third on the struct, that's
    // why the offset is sizeof(mat4) * 2
    u32 Offset = sizeof(mat4) * 2;

    glBindBuffer(GL_UNIFORM_BUFFER, MatricesUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, Offset, sizeof(mat4), View);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
