//
// Created by loyus on 7/3/2023.
//

#pragma once

#include <exception>

/*
 *
 * Act as a signal for ImGui context rebuild
 *
 * */
class ImGuiContextInvalid : public std::exception
{
    using std::exception::exception;
};