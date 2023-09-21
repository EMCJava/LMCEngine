//
// Created by samsa on 8/31/2023.
//

#pragma once

inline const char* DefaultTextureShaderInstancingVertexSource   = "#version 330 core\n"
                                                                  "layout (location = 0) in vec3 aPos;\n"
                                                                  "layout (location = 1) in vec2 aTexCoord;\n"
                                                                  "layout (location = 2) in mat4 aModelMatrix;\n"
                                                                  "out vec2 TexCoord;\n"
                                                                  "uniform mat4 projectionMatrix;\n"
                                                                  "void main()\n"
                                                                  "{\n"
                                                                  "   gl_Position = projectionMatrix * aModelMatrix * vec4(aPos, 1.0);\n"
                                                                  "   TexCoord = aTexCoord;\n"
                                                                  "}\0";
inline const char* DefaultTextureShaderInstancingFragmentSource = "#version 330 core\n"
                                                                  "out vec4 FragColor;\n"
                                                                  "in vec2 TexCoord;\n"
                                                                  "uniform sampler2D sample_texture;\n"
                                                                  "void main()\n"
                                                                  "{\n"
                                                                  "   vec4 texColor = texture(sample_texture, TexCoord);\n"
                                                                  "   FragColor = texColor;\n"
                                                                  "}\n\0";