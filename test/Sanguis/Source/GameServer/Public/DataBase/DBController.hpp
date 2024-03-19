//
// Created by EMCJava on 3/19/2024.
//

#pragma once

#include "DataBase/CreateTable.hpp"

class DBController
{
public:
    DBController( );

    auto GetStorage( ) { return m_Storage; }

private:
    decltype( CreateDatabase( ) ) m_Storage;
};
