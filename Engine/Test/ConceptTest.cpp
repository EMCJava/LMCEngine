//
// Created by samsa on 10/12/2023.
//
#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include <Engine/Core/Concept/Concept.hpp>

#include <string>
#include <algorithm>
#include <random>
#include <ranges>

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

    SECTION( "Performance" )
    {
        std::vector<PureConcept*> Concepts( 60000 );
        for ( size_t i = 0; i < 10000; ++i )
            Concepts[ i ] = new PureConcept;
        for ( size_t i = 0; i < 10000; ++i )
            Concepts[ i + 10000 ] = new Concept;
        for ( size_t i = 0; i < 10000; ++i )
            Concepts[ i + 20000 ] = new CA;
        for ( size_t i = 0; i < 10000; ++i )
            Concepts[ i + 30000 ] = new CAB;
        for ( size_t i = 0; i < 10000; ++i )
            Concepts[ i + 40000 ] = new PC;
        for ( size_t i = 0; i < 10000; ++i )
            Concepts[ i + 50000 ] = new PCD;

        std::random_device rd;
        std::mt19937       gen { rd( ) };
        std::ranges::shuffle( Concepts, gen );

        BENCHMARK( "dynamic_cast -> *" )
        {
            long Counter = 0;
            for ( auto*& ConceptPtr : Concepts )
                if ( dynamic_cast<PureConcept*>( ConceptPtr ) != nullptr )
                    Counter++;
            for ( auto*& ConceptPtr : Concepts )
                if ( dynamic_cast<Concept*>( ConceptPtr ) != nullptr )
                    Counter++;
            for ( auto*& ConceptPtr : Concepts )
                if ( dynamic_cast<CA*>( ConceptPtr ) != nullptr )
                    Counter++;
            for ( auto*& ConceptPtr : Concepts )
                if ( dynamic_cast<CAB*>( ConceptPtr ) != nullptr )
                    Counter++;
            for ( auto*& ConceptPtr : Concepts )
                if ( dynamic_cast<PC*>( ConceptPtr ) != nullptr )
                    Counter++;
            for ( auto*& ConceptPtr : Concepts )
                if ( dynamic_cast<PCD*>( ConceptPtr ) != nullptr )
                    Counter++;

            return Counter;
        };

        BENCHMARK( "dynamic_cast() -> *, single loop" )
        {
            long Counter = 0;
            for ( auto*& ConceptPtr : Concepts )
            {
                if ( dynamic_cast<PureConcept*>( ConceptPtr ) != nullptr )
                    Counter++;
                if ( dynamic_cast<Concept*>( ConceptPtr ) != nullptr )
                    Counter++;
                if ( dynamic_cast<CA*>( ConceptPtr ) != nullptr )
                    Counter++;
                if ( dynamic_cast<CAB*>( ConceptPtr ) != nullptr )
                    Counter++;
                if ( dynamic_cast<PC*>( ConceptPtr ) != nullptr )
                    Counter++;
                if ( dynamic_cast<PCD*>( ConceptPtr ) != nullptr )
                    Counter++;
            }

            return Counter;
        };

        BENCHMARK( "CanCastVT -> bool" )
        {
            long Counter = 0;
            for ( auto*& ConceptPtr : Concepts )
                if ( ConceptPtr->CanCastVT<PureConcept>( ) )
                    Counter++;
            for ( auto*& ConceptPtr : Concepts )
                if ( ConceptPtr->CanCastVT<Concept>( ) )
                    Counter++;
            for ( auto*& ConceptPtr : Concepts )
                if ( ConceptPtr->CanCastVT<CA>( ) )
                    Counter++;
            for ( auto*& ConceptPtr : Concepts )
                if ( ConceptPtr->CanCastVT<CAB>( ) )
                    Counter++;
            for ( auto*& ConceptPtr : Concepts )
                if ( ConceptPtr->CanCastVT<PC>( ) )
                    Counter++;
            for ( auto*& ConceptPtr : Concepts )
                if ( ConceptPtr->CanCastVT<PCD>( ) )
                    Counter++;

            return Counter;
        };

        BENCHMARK( "CanCastVT() -> bool, single loop" )
        {
            long Counter = 0;
            for ( auto*& ConceptPtr : Concepts )
            {
                if ( ConceptPtr->CanCastVT<PureConcept>( ) )
                    Counter++;
                if ( ConceptPtr->CanCastVT<Concept>( ) )
                    Counter++;
                if ( ConceptPtr->CanCastVT<CA>( ) )
                    Counter++;
                if ( ConceptPtr->CanCastVT<CAB>( ) )
                    Counter++;
                if ( ConceptPtr->CanCastVT<PC>( ) )
                    Counter++;
                if ( ConceptPtr->CanCastVT<PCD>( ) )
                    Counter++;
            }

            return Counter;
        };

        BENCHMARK( "TryCast() -> *, single loop" )
        {
            long Counter = 0;
            for ( auto*& ConceptPtr : Concepts )
            {
                if ( ConceptPtr->TryCast<PureConcept>( ) != nullptr )
                    Counter++;
                if ( ConceptPtr->TryCast<Concept>( ) != nullptr )
                    Counter++;
                if ( ConceptPtr->TryCast<CA>( ) != nullptr )
                    Counter++;
                if ( ConceptPtr->TryCast<CAB>( ) != nullptr )
                    Counter++;
                if ( ConceptPtr->TryCast<PC>( ) != nullptr )
                    Counter++;
                if ( ConceptPtr->TryCast<PCD>( ) != nullptr )
                    Counter++;
            }

            return Counter;
        };
    }
}