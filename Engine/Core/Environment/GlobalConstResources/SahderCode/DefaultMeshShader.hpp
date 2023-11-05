//
// Created by samsa on 8/31/2023.
//

#pragma once

inline const char* DefaultMeshShaderVertexSource =
    R"(
#version 330 core
layout (location = 0) in vec4 aPos_Color;
layout (location = 1) in vec3 aNormal;

out vec3 FragCol;

uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;

vec3 unpackColor(float f) {
    vec3 color;
    color.b = floor(f / 256.0 / 256.0);
    color.g = floor((f - color.b * 256.0 * 256.0) / 256.0);
    color.r = floor(f - color.b * 256.0 * 256.0 - color.g * 256.0);
    return color / 255.0;
}

void main()
{
    FragCol = unpackColor(aPos_Color.w);
    gl_Position = projectionMatrix * vec4(vec3(modelMatrix * vec4(aPos_Color.xyz, 1.0)), 1.0);
})";

inline const char* DefaultMeshShaderFragmentSource =
    R"(
#version 330 core
out vec4 FragColor;

in vec3 FragCol;

void main()
{
    FragColor = vec4(FragCol, 1.0);
})";