//
// Created by EMCJava on 3/19/2024.
//

#include "DataBase/DBController.hpp"

using namespace sqlite_orm;

DBController::DBController( )
    : m_Storage( CreateDatabase( ) )
{
    m_Storage.sync_schema( );

    return;

    m_Storage.remove_all<User>( );
    m_Storage.insert( User::MakeUser( "Player1", "1" ) );
    m_Storage.insert( User::MakeUser( "Player2", "2" ) );
    m_Storage.insert( User::MakeUser( "Player3", "3" ) );
    m_Storage.insert( User::MakeUser( "Player4", "4" ) );

    m_Storage.remove_all<UserRelationship>( );
    m_Storage.replace( UserRelationship { 1, 2, UserRelationship::Friend } );
    m_Storage.replace( UserRelationship { 2, 1, UserRelationship::Friend } );
    m_Storage.replace( UserRelationship { 1, 3, UserRelationship::Block } );
    m_Storage.replace( UserRelationship { 1, 4, UserRelationship::Friend } );

    m_Storage.remove_all<UserLogin>( );
    m_Storage.insert( into<UserLogin>( ),
                      columns( &UserLogin::UserID, &UserLogin::LoginCookie ),
                      values( std::make_tuple( std::make_unique<int>( 1 ), 123 ) ) );
}

std::string
DBController::GetUserNameByID( int ID ) const
{
    using namespace sqlite_orm;

    const auto Name = GetStorage( ).select(
        columns( &User::UserName ),
        where( c( &User::ID ) == ID ) );

    if ( Name.empty( ) ) return { };
    return std::get<0>( Name[ 0 ] );
}
