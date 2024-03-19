//
// Created by EMCJava on 3/19/2024.
//

#include "DataBase/DBController.hpp"

using namespace sqlite_orm;

DBController::DBController( )
    : m_Storage( CreateDatabase( ) )
{
    m_Storage.sync_schema( );

    m_Storage.remove_all<User>( );
    m_Storage.insert( User { 0, "Player1" } );
    m_Storage.insert( User { 0, "Player2" } );
    m_Storage.insert( User { 0, "Player3" } );

    m_Storage.remove_all<UserRelationship>( );
    m_Storage.replace( UserRelationship { std::make_unique<int>( 1 ), std::make_unique<int>( 2 ), UserRelationship::Friend } );
    m_Storage.replace( UserRelationship { std::make_unique<int>( 2 ), std::make_unique<int>( 1 ), UserRelationship::Friend } );
    m_Storage.replace( UserRelationship { std::make_unique<int>( 1 ), std::make_unique<int>( 3 ), UserRelationship::Block } );
}
