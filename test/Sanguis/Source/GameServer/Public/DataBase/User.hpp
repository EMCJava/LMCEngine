//
// Created by EMCJava on 3/19/2024.
//

#pragma once

#include <string>
#include <memory>
#include <optional>

struct User {
    int ID;

    static constexpr int MaxNameLength = 10;
    std::string          UserName;
    int64_t              PasswordHash;

    int64_t GetPasswordHash( const std::string& Password )
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

struct UserLogin {
    std::optional<int> UserID;
    size_t             LoginCookie;
    std::string        Timeout;
};

struct UserRelationship {
    std::optional<int> RelatingUserId;
    std::optional<int> RelatedUserId;
    enum { None,
           Friend,
           Block };
    int Type;
};