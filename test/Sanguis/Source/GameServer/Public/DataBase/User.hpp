//
// Created by EMCJava on 3/19/2024.
//

#pragma once

#include <string>
#include <memory>

struct User {
    int         ID;
    std::string UserName;
};

struct UserRelationship {
    std::unique_ptr<int> RelatingUserId;
    std::unique_ptr<int> RelatedUserId;
    enum { None,
           Friend,
           Block };
    int Type;
};