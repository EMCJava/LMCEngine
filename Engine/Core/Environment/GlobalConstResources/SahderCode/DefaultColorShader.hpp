//
// Created by samsa on 8/31/2023.
//

#pragma once

inline const char* DefaultColorShaderVertexSource = "#version 330 core\n"
                                                    "layout (location = 0) in vec3 aPos;\n"
                                                    "uniform mat4 projectionMatrix;\n"
                                                    "void main()\n"
                                                    "{\n"
                                                    "   gl_Position = projectionMatrix * vec4(aPos, 1.0);\n"
                                                    "}\0";

inline const char* DefaultColorShaderFragmentSource = "#version 330 core\n"
                                                      "out vec4 FragColor;\n"
                                                      "uniform vec4 fragColor = vec4(0.9, 0.0, 0.9, 1.0);\n"
                                                      "void main()\n"
                                                      "{\n"
                                                      "   FragColor = fragColor;\n"
                                                      "}\n\0";