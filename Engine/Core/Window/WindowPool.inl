//
// Created by loyus on 7/2/2023.
//

#include <stdexcept>

template <typename WindowTy>
WindowTy*
WindowPool::CreateWindow( int Width, int Height, const char* Title, bool Fullscreen, bool Create )
{
    const bool FirstWindow = m_Windows.empty( );
    if ( !Create && FirstWindow )
    {
        throw std::runtime_error( "WindowPool::CreateWindow: First window should always be created for glad and ImGui to initialize." );
    }

    auto* NewWindow = new WindowTy( Width, Height, Title, Fullscreen, Create );
    if ( NewWindow == nullptr )
    {
        throw std::runtime_error( "Failed to allocate window" );
    }

    NewWindow->SetSwapInterval( 1 );
    m_Windows.push_back( (class Window*) NewWindow );

    return NewWindow;
}

template <typename WindowTy>
WindowTy*
WindowPool::CreateWindowFullscreen( const char* Title, bool Create )
{
    const auto VideoMode = GetPrimaryMonitorMode( );
    return CreateWindow<WindowTy>( VideoMode.first, VideoMode.second, Title, true, Create );
}
