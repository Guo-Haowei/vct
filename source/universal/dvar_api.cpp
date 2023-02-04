#include "dvar_api.h"

#include <filesystem>
#include <fstream>
#include <list>
#include <unordered_map>
#include <sstream>
#include <string>

#include "Base/Asserts.h"
#include "Base/Logger.h"

using std::list;
using std::string;

static std::unordered_map<std::string, dvar_t*> s_dvarLookupTable;

#define PRINT_CMDLINE_DVARS IN_USE
#define DVAR_VERBOSE        NOT_IN_USE

#if USING( DVAR_VERBOSE )
#define DVAR_PRINTF( fmt, ... ) LOG_DEBUG( "[dvar] " fmt, ##__VA_ARGS__ )
#else
#define DVAR_PRINTF( ... ) ( (void)0 )
#endif

static void RegisterDvar_Internal( const char* key, dvar_t* dvar )
{
    const std::string keyStr( key );
    auto it = s_dvarLookupTable.find( keyStr );
    if ( it != s_dvarLookupTable.end() ) {
        LOG_ERROR( "[dvar] duplicated dvar %s detected", key );
    }

    strncpy( dvar->debugName_, key, dvar_t::kMaxStringLengh );
    dvar->debugName_[dvar_t::kMaxStringLengh - 1] = 0;

    s_dvarLookupTable.insert( std::make_pair( keyStr, dvar ) );
    const auto& v = dvar->vec_;
    switch ( dvar->type_ ) {
        case dvar_t::Integer:
            DVAR_PRINTF( "register dvar '%s'(int) %d", key, dvar->int_ );
            break;
        case dvar_t::Float:
            DVAR_PRINTF( "register dvar '%s'(float) %f", key, dvar->float_ );
            break;
        case dvar_t::String:
            DVAR_PRINTF( "register dvar '%s'(string) \"%s\"", key, dvar->str_ );
            break;
        case dvar_t::Vec2:
            DVAR_PRINTF( "register dvar '%s'(vec2) { %f, %f }", key, v.x, v.y );
            break;
        case dvar_t::Vec3:
            DVAR_PRINTF( "register dvar '%s'(vec3) { %f, %f, %f }", key, v.x, v.y, v.z );
            break;
        case dvar_t::Vec4:
            DVAR_PRINTF( "register dvar '%s'(vec4) { %f, %f, %f, %f }", key, v.x, v.y, v.z, v.w );
            break;
        default:
            LOG_FATAL( "Unknown dvar type %d", static_cast<int>( dvar->type_ ) );
            break;
    }
}

static void SafeCopyDvarString( dvar_t& dvar, const char* value )
{
    const size_t len = strlen( value );
    if ( len > dvar_t::kMaxStringLengh - 1 ) {
        LOG_FATAL( "string [%s] too long", value );
    }

    strcpy( dvar.str_, value );
}

void Dvar_RegisterInt_Internal( dvar_t& dvar, const char* key, int value )
{
    dvar.type_ = dvar_t::Integer;
    dvar.int_ = value;
    RegisterDvar_Internal( key, &dvar );
}

void Dvar_RegisterFloat_Internal( dvar_t& dvar, const char* key, float value )
{
    dvar.type_ = dvar_t::Float;
    dvar.float_ = value;
    RegisterDvar_Internal( key, &dvar );
}

void Dvar_RegisterVec_Internal( dvar_t& dvar, const char* key, float x, float y, float z, float w, int n )
{
    ASSERTRANGE( n, 2, 4 );
    dvar.type_ = static_cast<dvar_t::Type>( dvar_t::_VecBase + n );
    dvar.vec_.x = x;
    dvar.vec_.y = y;
    dvar.vec_.z = z;
    dvar.vec_.w = w;
    RegisterDvar_Internal( key, &dvar );
}

void Dvar_RegisterString_Internal( dvar_t& dvar, const char* key, const char* value )
{
    dvar.type_ = dvar_t::String;
    SafeCopyDvarString( dvar, value );
    RegisterDvar_Internal( key, &dvar );
}

int Dvar_GetInt_Internal( const dvar_t& dvar )
{
    ASSERT( dvar.type_ == dvar_t::Integer );
    return dvar.int_;
}

float Dvar_GetFloat_Internal( const dvar_t& dvar )
{
    ASSERT( dvar.type_ == dvar_t::Float );
    return dvar.float_;
}

void* Dvar_GetPtr_Internal( dvar_t& dvar )
{
    return &dvar.int_;
}

vec4 Dvar_GetVec_Internal( const dvar_t& dvar, int n )
{
    ASSERT( dvar.type_ == static_cast<dvar_t::Type>( dvar_t::_VecBase + n ) );
    return vec4( dvar.vec_.x, dvar.vec_.y, dvar.vec_.z, dvar.vec_.w );
}

const char* Dvar_GetString_Internal( const dvar_t& dvar )
{
    ASSERT( dvar.type_ == dvar_t::String );
    return dvar.str_;
}

dvar_t* Dvar_FindByName_Internal( const char* name )
{
    auto it = s_dvarLookupTable.find( name );
    if ( it == s_dvarLookupTable.end() ) {
        return nullptr;
    }
    return it->second;
}

DvarError Dvar_SetInt_Internal( dvar_t& dvar, int value )
{
    if ( dvar.type_ != dvar_t::Integer ) {
        return DvarError::TypeMismatch;
    }

    dvar.int_ = value;
    return DvarError::Ok;
}

DvarError Dvar_SetFloat_Internal( dvar_t& dvar, float value )
{
    if ( dvar.type_ != dvar_t::Float ) {
        return DvarError::TypeMismatch;
    }

    dvar.float_ = value;
    return DvarError::Ok;
}

DvarError Dvar_SetVec_Internal( dvar_t& dvar, float x, float y, float z, float w )
{
    switch ( dvar.type_ ) {
        case dvar_t::Vec2:
            dvar.vec_.x = x;
            dvar.vec_.y = y;
        case dvar_t::Vec3:
            dvar.vec_.z = z;
        case dvar_t::Vec4:
            dvar.vec_.w = w;
            return DvarError::Ok;
        default:
            return DvarError::TypeMismatch;
    }
}

DvarError Dvar_SetString_Internal( dvar_t& dvar, const char* value )
{
    if ( dvar.type_ != dvar_t::String ) {
        return DvarError::TypeMismatch;
    }

    SafeCopyDvarString( dvar, value );
    return DvarError::Ok;
}

DvarError Dvar_SetFromString_Internal( dvar_t& dvar, const char* str )
{
    auto& v = dvar.vec_;
    int n = 0;
    switch ( dvar.type_ ) {
        case dvar_t::Integer:
            dvar.int_ = atoi( str );
            DVAR_PRINTF( "change dvar '%s'(int) to %d", dvar.debugName_, dvar.int_ );
            break;
        case dvar_t::Float:
            dvar.float_ = float( atof( str ) );
            DVAR_PRINTF( "change dvar '%s'(float) to %f", dvar.debugName_, dvar.float_ );
            break;
        case dvar_t::String:
            SafeCopyDvarString( dvar, str );
            DVAR_PRINTF( "change dvar '%s'(string) to \"%s\"", dvar.debugName_, dvar.str_ );
            break;
        case dvar_t::Vec2:
            n = sscanf( str, "%f,%f", &v.x, &v.y );
            ASSERT( n == 2 );
            DVAR_PRINTF( "change dvar '%s'(vec2) to { %f, %f } ", dvar.debugName_, v.x, v.y );
            break;
        case dvar_t::Vec3:
            n = sscanf( str, "%f,%f,%f", &v.x, &v.y, &v.z );
            ASSERT( n == 3 );
            DVAR_PRINTF( "change dvar '%s'(vec3) to { %f, %f, %f } ", dvar.debugName_, v.x, v.y, v.z );
            break;
        case dvar_t::Vec4:
            n = sscanf( str, "%f,%f,%f,%f", &v.x, &v.y, &v.z, &v.w );
            ASSERT( n == 4 );
            DVAR_PRINTF( "change dvar '%s'(vec4) to { %f, %f, %f, %f } ", dvar.debugName_, v.x, v.y, v.z, v.w );
            break;
        default:
            LOG_FATAL( "attempt to set unknown dvar type %d to %s", static_cast<int>( dvar.type_ ), str );
            break;
    }

    return DvarError::Ok;
}

DvarError Dvar_SetIntByName_Internal( const char* name, int value )
{
    dvar_t* dvar = Dvar_FindByName_Internal( name );
    ASSERT( dvar );
    if ( !dvar ) {
        return DvarError::NotExisted;
    }

    return Dvar_SetInt_Internal( *dvar, value );
}

DvarError Dvar_SetFloatByName_Internal( const char* name, float value )
{
    dvar_t* dvar = Dvar_FindByName_Internal( name );
    ASSERT( dvar );
    if ( !dvar ) {
        return DvarError::NotExisted;
    }

    return Dvar_SetFloat_Internal( *dvar, value );
}

DvarError Dvar_SetStringByName_Internal( const char* name, const char* value )
{
    dvar_t* dvar = Dvar_FindByName_Internal( name );
    ASSERT( dvar );
    if ( !dvar ) {
        return DvarError::NotExisted;
    }

    return Dvar_SetString_Internal( *dvar, value );
}

class CommandHelper {
    list<string> commands_;

public:
    void SetFromCommandLine( int argc, const char** argv )
    {
        for ( int idx = 0; idx < argc; ++idx ) {
            commands_.emplace_back( string( argv[idx] ) );
        }
    }

    void PushCfg( const char* file )
    {
        if ( !std::filesystem::exists( file ) ) {
            LOG_WARN( "[filesystem] file '%s' does not exist", file );
            return;
        }

        std::ifstream fs( file );
        list<string> cfg;
        string line;
        while ( std::getline( fs, line ) ) {
            std::istringstream iss( line );
            string token;
            if ( iss >> token ) {
                if ( token.front() == '#' ) {
                    continue;
                }
            }

            do {
                cfg.emplace_back( token );
            } while ( iss >> token );
        }
        cfg.insert( cfg.end(), commands_.begin(), commands_.end() );
        commands_ = std::move( cfg );
    }

    bool TryConsume( string& str )
    {
        if ( commands_.empty() ) {
            str.clear();
            return false;
        }

        str = commands_.front();
        commands_.pop_front();
        return true;
    }

    bool Consume( string& str )
    {
        if ( commands_.empty() ) {
            LOG_ERROR( "Unexpected EOF" );
            str.clear();
            return false;
        }

        return TryConsume( str );
    }
};

bool dvar_process_from_cmdline( int argc, const char** argv )
{
    CommandHelper cmdHelper;
    cmdHelper.SetFromCommandLine( argc, argv );
    
#if USING( PRINT_CMDLINE_DVARS )
    string debugBuffer;
#endif

    string str;
    while ( cmdHelper.TryConsume( str ) ) {
        if ( str == "+set" ) {
            cmdHelper.Consume( str );

            dvar_t* dvar = Dvar_FindByName_Internal( str.c_str() );

#if USING( PRINT_CMDLINE_DVARS )
            debugBuffer.append( "\n\t+set " ).append( str );
#endif
            if ( dvar == nullptr ) {
                LOG_ERROR( "[dvar] Dvar '%s' not found", str.c_str() );
                return false;
            }
            cmdHelper.Consume( str );
            Dvar_SetFromString_Internal( *dvar, str.c_str() );
#if USING( PRINT_CMDLINE_DVARS )
            debugBuffer.append( " " ).append( str );
#endif
        }
        else if ( str == "+exec" ) {
            cmdHelper.Consume( str );
            LOG_INFO( "Executing '%s'", str.c_str() );
            cmdHelper.PushCfg( str.c_str() );
        }
        else {
            LOG_ERROR( "Unknown command '%s'", str.c_str() );
            return false;
        }
    }

#if USING( PRINT_CMDLINE_DVARS )
    LOG_INFO( "Debug variables from command lines:%s", debugBuffer.c_str() );
#endif

    return true;
}