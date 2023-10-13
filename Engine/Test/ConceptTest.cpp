//
// Created by samsa on 10/12/2023.
//
#include <catch2/catch_test_macros.hpp>

#include <Engine/Core/Concept/Concept.hpp>

#include <string>

class CA : public Concept
{
    DECLARE_CONCEPT( CA, Concept )
};
DEFINE_CONCEPT_DS( CA )

class CAB : public CA
{
    DECLARE_CONCEPT( CAB, CA )
};
DEFINE_CONCEPT_DS( CAB )

class PC : public PureConcept
{
    DECLARE_CONCEPT( PC, PureConcept )
};
DEFINE_CONCEPT_DS( PC )

class PCD : public PC
{
    DECLARE_CONCEPT( PCD, PC )
};
DEFINE_CONCEPT_DS( PCD )

class Foo
{
public:
    int Test = 0;
};

class Bar : public Foo
{
public:
    int Test = 1;
};

TEST_CASE( "Concept", "[Concept]" )
{
    REQUIRE( CA::ParentSet::Contain( Concept::TypeID ) );
    REQUIRE( CA::CanCastS<Concept>( ) );

    REQUIRE( CA::ParentSet::Contain( PureConcept::TypeID ) );
    REQUIRE( CA::CanCastS<PureConcept>( ) );

    REQUIRE( CAB::ParentSet::Contain( CA::TypeID ) );
    REQUIRE( CAB::CanCastS<CA>( ) );

    REQUIRE( !CAB::ParentSet::Contain( PC::TypeID ) );
    REQUIRE( !CAB::CanCastS<PC>( ) );

    REQUIRE( PCD::ParentSet::Contain( PC::TypeID ) );
    REQUIRE( PCD::CanCastS<PC>( ) );

    REQUIRE( !PC::ParentSet::Contain( PCD::TypeID ) );
    REQUIRE( !PC::CanCastS<PCD>( ) );

    REQUIRE( !PC::ParentSet::Contain( CAB::TypeID ) );
    REQUIRE( !PC::CanCastS<CAB>( ) );

    SECTION( "Dynamic" )
    {
        auto cab = std::make_unique<CAB>( );

        REQUIRE( cab->CanCastVT<CA>( ) );
        REQUIRE( cab->CanCastVT<Concept>( ) );
        REQUIRE( cab->CanCastVT<PureConcept>( ) );

        REQUIRE( !cab->CanCastVT<PC>( ) );
        REQUIRE( !cab->CanCastVT<PCD>( ) );

        PureConcept* cab_pure_ptr = cab.get( );

        REQUIRE( cab->IsEnabled( ) );
        REQUIRE( cab_pure_ptr->IsEnabled( ) );
        cab->SetEnabled( false );
        REQUIRE( !cab->IsEnabled( ) );
        REQUIRE( !cab_pure_ptr->IsEnabled( ) );

        REQUIRE( cab_pure_ptr->CanCastVT<CA>( ) );
        REQUIRE( cab_pure_ptr->CanCastVT<Concept>( ) );
        REQUIRE( cab_pure_ptr->CanCastVT<PureConcept>( ) );

        REQUIRE( !cab_pure_ptr->CanCastVT<PC>( ) );
        REQUIRE( !cab_pure_ptr->CanCastVT<PCD>( ) );
    }

    SECTION( "Sub-concept" )
    {
        auto cab = std::make_unique<CAB>( );
        REQUIRE( cab->GetSubConceptCount( ) == 0 );
        REQUIRE( cab->GetConceptAt( 0 ) == nullptr );
        auto FirstChild = cab->AddConcept<CA>( );
        REQUIRE( cab->GetSubConceptCount( ) == 1 );
        REQUIRE( cab->GetConceptAt( 0 ) == FirstChild );
        auto SecondChild = cab->AddConcept<PCD>( );
        REQUIRE( cab->GetSubConceptCount( ) == 2 );
        REQUIRE( cab->GetConceptAt( 0 ) == FirstChild );
        REQUIRE( cab->GetConceptAt( 1 ) == SecondChild );

        REQUIRE( FirstChild->GetRuntimeName( ) == "CA" );
        REQUIRE( SecondChild->GetRuntimeName( ) == "PCD" );

        REQUIRE( cab->GetConcept<PC>( ) == SecondChild );
        REQUIRE( cab->GetConcept<PCD>( ) == SecondChild );

        REQUIRE( cab->GetConcept<Concept>( ) == FirstChild );
        REQUIRE( cab->GetConcept<CA>( ) == FirstChild );

        REQUIRE( cab->GetConcept<PureConcept>( ) == FirstChild );
        REQUIRE( cab->GetConceptAt( 0 ) == FirstChild );
        SecondChild->MoveToFirstAsSubConcept( );
        REQUIRE( cab->GetConcept<PureConcept>( ) == SecondChild );
        REQUIRE( cab->GetConceptAt( 0 ) == SecondChild );

        FirstChild->Destroy( );
        REQUIRE( cab->GetSubConceptCount( ) == 1 );
    }
}