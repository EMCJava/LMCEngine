//
// Created by EMCJava on 4/2/2024.
//

#pragma once

#include <Engine/Core/Concept/ConceptList.hpp>

#include <functional>

class RectInput;
class LoginScene : public ConceptList
{
    DECLARE_CONCEPT( LoginScene, ConceptList )

public:
    LoginScene( const std::function<void( const std::string&, const std::string& )>& LoginFnc );

protected:
    std::shared_ptr<RectInput> m_NameInput;
    std::shared_ptr<RectInput> m_PasswordInput;

    std::function<void( const std::string&, const std::string& )> m_LoginFnc;
};