#include "com_cmdline.h"

#include <filesystem>
#include <fstream>
#include <list>
#include <sstream>
#include <string>

#include "com_filesystem.h"
#include "lua_script.h"
#include "universal/dvar_api.h"

#include "Base/Asserts.h"
#include "Base/Logger.h"

using std::list;
using std::string;

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
        char path[kMaxOSPath];
        Com_FsBuildPath( path, kMaxOSPath, file, "scripts" );

        if ( !std::filesystem::exists( path ) ) {
            LOG_WARN( "[filesystem] file '%s' does not exist", path );
            return;
        }

        std::ifstream fs( path );
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

bool Com_ProcessCmdLine( int argc, const char** argv )
{
    CommandHelper cmdHelper;
    cmdHelper.SetFromCommandLine( argc, argv );

    string str;
    while ( cmdHelper.TryConsume( str ) ) {
        if ( str == "+set" ) {
            cmdHelper.Consume( str );
            dvar_t* dvar = Dvar_FindByName_Internal( str.c_str() );
            if ( dvar == nullptr ) {
                LOG_ERROR( "[dvar] Dvar '%s' not found", str.c_str() );
                return false;
            }
            cmdHelper.Consume( str );
            Dvar_SetFromString_Internal( *dvar, str.c_str() );
        }
        else if ( str == "+exec" ) {
            cmdHelper.Consume( str );
            if ( !Com_ExecLua( str.c_str() ) ) {
                LOG_ERROR( "[lua] failed to execute script '%s'", str.c_str() );
                return false;
            }
            // Com_PrintInfo( "Executing '%s'", str.c_str() );
            // cmdHelper.PushCfg( str.c_str() );
        }
        else {
            LOG_ERROR( "Unknown command '%s'", str.c_str() );
            return false;
        }
    }

    LOG_OK( "cmd line processed" );
    return true;
}
