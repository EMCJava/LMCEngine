//
// Created by EMCJava on 3/19/2024.
//

#pragma once

#include "DataBase/CreateTable.hpp"

class DBController
{
public:
    DBController( );

    auto GetStorage( ) const noexcept { return m_Storage; }

    std::string        GetUserNameByID( int ID ) const;
    std::optional<int> GetUserIDByName( std::string_view Name ) const;

private:
    decltype( CreateDatabase( ) ) m_Storage;
};
