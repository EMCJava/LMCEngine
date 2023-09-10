//
// Created by samsa on 8/31/2023.
//

#pragma once

inline const char* DefaultFontShaderVertexSource = "#version 330 core\n"
                                                   "layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>\n"
                                                   "out vec2 TexCoords;\n"
                                                   "uniform mat4 projectionMatrix;\n"
                                                   "void main()\n"
                                                   "{\n"
                                                   "   gl_Position = projectionMatrix * vec4(vertex.xy, 0.0, 1.0);\n"
                                                   "   TexCoords = vertex.zw;\n"
                                                   "}\0";

inline const char* DefaultFontShaderFragmentSource = "#version 330 core\n"
                                                     "in vec2 TexCoords;\n"
                                                     "out vec4 FragColor;\n"
                                                     "uniform sampler2D text;\n"
                                                     "uniform vec3 textColor;\n"
                                                     "void main()\n"
                                                     "{\n"
                                                     "   vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);\n"
                                                     "   FragColor = vec4(textColor, 1.0) * sampled;"
                                                     "}\n\0";