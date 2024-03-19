//
// Created by EMCJava on 3/19/2024.
//

#pragma once

#include <string>
#include <memory>

struct User {
    int         ID;
    std::string UserName;
    size_t      PasswordHash;

    size_t GetPasswordHash( const std::string& Password )
    {
        return std::hash<std::string> { }(
            UserName + Password + "D;%yL9TS:5PalS/d" );
    }

    static User MakeUser( const std::string& UserName, const std::string& Password )
    {
        User Result;
        Result.UserName     = UserName;
        Result.PasswordHash = Result.GetPasswordHash( Password );
        return Result;
    }
};

struct UserRelationship {
    std::unique_ptr<int> RelatingUserId;
    std::unique_ptr<int> RelatedUserId;
    enum { None,
           Friend,
           Block };
    int Type;
};