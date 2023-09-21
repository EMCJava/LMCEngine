//
// Created by samsa on 8/31/2023.
//

#pragma once

inline const char* DefaultTextureShaderInstancingVertexSource   = "#version 330 core\n"
                                                                  "layout (location = 0) in vec3 aPos;\n"
                                                                  "layout (location = 1) in vec2 aTexCoord;\n"
                                                                  "layout (location = 2) in mat4 aModelMatrix;\n"
                                                                  "layout (location = 6) in float aInvertAlpha;\n"
                                                                  "out float Alpha;\n"
                                                                  "out vec2 TexCoord;\n"
                                                                  "uniform mat4 projectionMatrix;\n"
                                                                  "void main()\n"
                                                                  "{\n"
                                                                  "   gl_Position = projectionMatrix * aModelMatrix * vec4(aPos, 1.0);\n"
                                                                  "   Alpha = (1 - aInvertAlpha);\n"
                                                                  "   TexCoord = aTexCoord;\n"
                                                                  "}\0";
inline const char* DefaultTextureShaderInstancingFragmentSource = "#version 330 core\n"
                                                                  "out vec4 FragColor;\n"
                                                                  "in float Alpha;\n"
                                                                  "in vec2 TexCoord;\n"
                                                                  "uniform sampler2D sample_texture;\n"
                                                                  "void main()\n"
                                                                  "{\n"
                                                                  "   vec4 texColor = texture(sample_texture, TexCoord);\n"
                                                                  "   FragColor = vec4(texColor.xyz, texColor.w * Alpha);\n"
                                                                  "}\n\0";