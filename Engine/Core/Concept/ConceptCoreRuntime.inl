//
// Created by loyus on 13/8/2023.
//

#include "ConceptCoreRuntime.hpp"

LMC_API
NamingCollectionMap*
GetConceptNames( )
{
    auto* result = new NamingCollectionMap;

    const auto& Names  = NamingCollection<NamingCollectionConcept>::Names;
    result->ConceptLen = Names.size( );
    result->Concepts   = new NameIDPair[ result->ConceptLen ];

    size_t Index = 0;
    for ( const auto& It : Names )
    {
        const auto Size                   = It.first.size( ) + 1;
        result->Concepts[ Index ].NamePre = new char[ Size ];
        memcpy( result->Concepts[ Index ].NamePre, It.first.c_str( ), Size );
        result->Concepts[ Index ].ID = It.second;

        Index++;
    }

    return result;
}
