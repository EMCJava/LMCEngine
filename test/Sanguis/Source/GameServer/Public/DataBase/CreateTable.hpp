//
// Created by EMCJava on 3/19/2024.
//

#include "DataBase/User.hpp"

#include <sqlite_orm/sqlite_orm.h>

inline auto
CreateDatabase( )
{
    using namespace sqlite_orm;

    return make_storage( "db.sqlite",
                         make_table( "users",
                                     make_column( "id", &User::ID, primary_key( ).autoincrement( ) ),
                                     make_column( "user_name", &User::UserName ),
                                     make_column( "password", &User::PasswordHash ) ),
                         make_table( "user_logins",
                                     make_column( "user_id", &UserLogin::UserID, primary_key( ) ),
                                     make_column( "login_cookie", &UserLogin::LoginCookie ),
                                     make_column( "timeout", &UserLogin::Timeout, default_value( datetime( "now", "localtime" ) ) ),
                                     foreign_key( &UserLogin::UserID ).references( &User::ID ) ),
                         make_table( "user_relationship",
                                     make_column( "relating_user_id", &UserRelationship::RelatingUserId ),
                                     make_column( "related_user_id", &UserRelationship::RelatedUserId ),
                                     make_column( "type", &UserRelationship::Type, default_value( (int) UserRelationship::None ) ),
                                     primary_key( &UserRelationship::RelatingUserId, &UserRelationship::RelatedUserId ),
                                     foreign_key( &UserRelationship::RelatingUserId ).references( &User::ID ),
                                     foreign_key( &UserRelationship::RelatedUserId ).references( &User::ID ) ) );
}