#include <Engine/Engine.hpp>
#include <exception>
#include <iostream>

int
main( )
{
    Engine engine;

    try
    {
        while ( !engine.ShouldShutdown( ) )
        {
            engine.Update( );
        }
    }
    catch ( std::exception& e )
    {
        std::cerr << "Uncaught exception: " << e.what( ) << std::endl;
    }

    return 0;
}

//FIXME: All relative paths should be relative to the project root.