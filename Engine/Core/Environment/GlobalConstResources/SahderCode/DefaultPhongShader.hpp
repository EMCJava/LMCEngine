//
// Created by samsa on 8/31/2023.
//

#pragma once

inline const char* DefaultPhongShaderVertexSource =
    R"(
#version 330 core
layout (location = 0) in vec4 aPos_Color;
layout (location = 1) in vec3 aNormal;

out vec3 Normal;
out vec3 FragPos;
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
    FragPos = vec3(modelMatrix * vec4(aPos_Color.xyz, 1.0));
    Normal = mat3(transpose(inverse(modelMatrix))) * aNormal;
    FragCol = unpackColor(aPos_Color.w);

    gl_Position = projectionMatrix * vec4(FragPos, 1.0);
})";

inline const char* DefaultPhongShaderFragmentSource =
    R"(
#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec3 FragCol;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

void main()
{
    // ambient
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * FragCol;
    FragColor = vec4(result, 1.0);
})";