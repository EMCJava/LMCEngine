//
// Created by samsa on 8/31/2023.
//

#pragma once

inline const char* DefaultTextureShaderVertexSource   = "#version 330 core\n"
                                                        "layout (location = 0) in vec3 aPos;\n"
                                                        "layout (location = 1) in vec2 aTexCoord;\n"
                                                        "out vec2 TexCoord;\n"
                                                        "uniform mat4 projectionMatrix;\n"
                                                        "uniform mat4 modelMatrix;\n"
                                                        "void main()\n"
                                                        "{\n"
                                                        "   gl_Position = projectionMatrix * modelMatrix * vec4(aPos, 1.0);\n"
                                                        "   TexCoord = aTexCoord;\n"
                                                        "}\0";
inline const char* DefaultTextureShaderFragmentSource = "#version 330 core\n"
                                                        "out vec4 FragColor;\n"
                                                        "in vec2 TexCoord;\n"
                                                        "uniform sampler2D sample_texture;\n"
                                                        "void main()\n"
                                                        "{\n"
                                                        "   vec4 texColor = texture(sample_texture, TexCoord);\n"
                                                        "   FragColor = texColor;\n"
                                                        "}\n\0";