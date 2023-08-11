//
// Created by loyus on 11/8/2023.
//

#pragma once

// https://www.scs.stanford.edu/~dm/blog/va-opt.html

#define PARENS ( )

// Rescan macro tokens 256 times
#define EXPAND( arg )  EXPAND1( EXPAND1( EXPAND1( EXPAND1( arg ) ) ) )
#define EXPAND1( arg ) EXPAND2( EXPAND2( EXPAND2( EXPAND2( arg ) ) ) )
#define EXPAND2( arg ) EXPAND3( EXPAND3( EXPAND3( EXPAND3( arg ) ) ) )
#define EXPAND3( arg ) EXPAND4( EXPAND4( EXPAND4( EXPAND4( arg ) ) ) )
#define EXPAND4( arg ) arg

#define FOR_EACH( macro, ... ) \
    __VA_OPT__( EXPAND( FOR_EACH_HELPER( macro, __VA_ARGS__ ) ) )
#define FOR_EACH_HELPER( macro, a1, ... ) \
    macro( a1 )                           \
        __VA_OPT__( FOR_EACH_AGAIN PARENS( macro, __VA_ARGS__ ) )
#define FOR_EACH_AGAIN( ) FOR_EACH_HELPER


#define EXPOSE_TYPE( VAR )               using __##VAR##_IMGUI_TY = decltype( VAR );
#define TO_IMGUI_DECLARE( VAR )          void ToImGuiWidget( const char* Name, Sprite::__##VAR##_IMGUI_TY* Value );
#define SIMPLE_DEFAULT_IMGUI_TYPE( VAR ) ToImGuiWidget( #VAR, &Value->VAR );

#define EXPOSE_TYPE_TO_IMGUI( ... )           FOR_EACH( EXPOSE_TYPE, __VA_ARGS__ )
#define DECLARE_FOR_IMGUI( ... )              FOR_EACH( TO_IMGUI_DECLARE, __VA_ARGS__ )
#define SIMPLE_LIST_DEFAULT_IMGUI_TYPE( ... ) FOR_EACH( SIMPLE_DEFAULT_IMGUI_TYPE, __VA_ARGS__ )
#define DEFINE_SIMPLE_IMGUI_TYPE( class_name, ... )                  \
    inline void ToImGuiWidget( const char* Name, class_name* Value ) \
    {                                                                \
        FOR_EACH( SIMPLE_DEFAULT_IMGUI_TYPE, __VA_ARGS__ )           \
    }

#define ENABLE_IMGUI( class_name, ... )                               \
public:                                                               \
    EXPOSE_TYPE_TO_IMGUI( __VA_ARGS__ )                               \
                                                                      \
    friend void ToImGuiWidget( const char* Name, class_name* Value ); \
    }                                                                 \
    ;                                                                 \
                                                                      \
    DECLARE_FOR_IMGUI( __VA_ARGS__ )                                  \
    /* For formatting */ namespace                                    \
    {
