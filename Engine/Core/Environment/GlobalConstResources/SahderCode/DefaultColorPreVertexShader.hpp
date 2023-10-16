//
// Created by samsa on 8/31/2023.
//

#pragma once

inline const char* DefaultColorPreVertexShaderVertexSource = "#version 330 core\n"
                                                             "layout (location = 0) in vec3 aPos;\n"
                                                             "layout (location = 1) in vec3 aCol;\n"
                                                             "uniform mat4 projectionMatrix;\n"
                                                             "uniform mat4 modelMatrix;\n"
                                                             "out vec3 VColor;\n"
                                                             "void main()\n"
                                                             "{\n"
                                                             "   VColor = aCol;\n"
                                                             "   gl_Position = projectionMatrix * modelMatrix * vec4(aPos, 1.0);\n"
                                                             "}\0";

inline const char* DefaultColorPreVertexShaderFragmentSource = "#version 330 core\n"
                                                               "out vec4 FragColor;\n"
                                                               "in vec3 VColor;\n"
                                                               "void main()\n"
                                                               "{\n"
                                                               "   FragColor = vec4(VColor, 1.0);\n"
                                                               "}\n\0";