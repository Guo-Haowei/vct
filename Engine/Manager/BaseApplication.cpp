#include "BaseApplication.hpp"

#include <list>
#include <string>
#include <filesystem>
#include <fstream>
#include <sstream>

#include "Base/Logger.h"

#include "Core/GlfwApplication.hpp"

#include "Manager/SceneManager.hpp"

#define DEFINE_DVAR
#include "Core/com_dvars.h"

using std::list;
using std::string;

static void RegisterDvars();
static bool ProcessDvarFromCmdLine( int argc, const char** argv );

bool BaseApplication::ProcessCommandLine( int argc, const char** argv )
{
    RegisterDvars();

    return ProcessDvarFromCmdLine( argc - 1, argv + 1 );
}

bool BaseApplication::Initialize()
{
    int i = 0;
    for ( auto& module : m_runtimeModules ) {
        LOG_DEBUG( "BaseApplication::Initialize(): Initializing module %i", i++ );
        if ( !module->Initialize() ) {
            LOG_FATAL( "BaseApplication::Initialize(): Failed to initialize module" );
            return false;
        }
    }

    return true;
}

void BaseApplication::Finalize()
{
    int i = 0;
    for ( auto it = m_runtimeModules.rbegin(); it != m_runtimeModules.rend(); ++it ) {
        LOG_DEBUG( "BaseApplication::Finalize(): Finalizing module %i", i++ );
        ( *it )->Finalize();
    }

    m_runtimeModules.clear();
}

void BaseApplication::Tick()
{
    for ( auto it = m_runtimeModules.rbegin(); it != m_runtimeModules.rend(); ++it ) {
        ( *it )->Tick();
    }
}

bool BaseApplication::ShouldQuit()
{
    return m_bShouldQuit;
}

void BaseApplication::RegisterManagerModule( IAssetLoader* mgr )
{
    m_pAssetLoader = mgr;
    mgr->SetAppPointer( this );
    m_runtimeModules.push_back( mgr );
}

void BaseApplication::RegisterManagerModule( ISceneManager* mgr )
{
    m_pSceneManager = mgr;
    mgr->SetAppPointer( this );
    m_runtimeModules.push_back( mgr );
}

void BaseApplication::RegisterManagerModule( IGraphicsManager* mgr )
{
    m_pGraphicsManager = mgr;
    mgr->SetAppPointer( this );
    m_runtimeModules.push_back( mgr );
}

void BaseApplication::RegisterManagerModule( IPipelineStateManager* mgr )
{
    m_pPipelineStateManager = mgr;
    mgr->SetAppPointer( this );
    m_runtimeModules.push_back( mgr );
}

void BaseApplication::RegisterManagerModule( IGameLogic* mgr )
{
    m_pGameLogic = mgr;
    mgr->SetAppPointer( this );
    m_runtimeModules.push_back( mgr );
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

#define PRINT_CMDLINE_DVARS IN_USE

static bool ProcessDvarFromCmdLine( int argc, const char** argv )
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

static void RegisterDvars()
{
#define REGISTER_DVAR
#include "Core/com_dvars.h"
}