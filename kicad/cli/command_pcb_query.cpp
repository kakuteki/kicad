/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright The KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "command_pcb_query.h"
#include <cli/exit_codes.h>
#include "jobs/job_pcb_query.h"
#include <string_utils.h>
#include <wx/crt.h>
#include <wx/tokenzr.h>

#include <macros.h>

#define ARG_UNITS "--units"
#define ARG_SELECT "--select"

CLI::PCB_QUERY_COMMAND::PCB_QUERY_COMMAND() : COMMAND( "query" )
{
    addCommonArgs( true, true, IO_TYPE::FILE, IO_TYPE::FILE );
    addDefineArg();

    m_argParser.add_description( UTF8STDSTR( _( "Dumps the contents of the PCB as JSON" ) ) );

    m_argParser.add_argument( ARG_UNITS )
            .default_value( std::string( "mm" ) )
            .help( UTF8STDSTR( _( "Coordinate units; valid options: in, mm, mils" ) ) )
            .metavar( "UNITS" );

    m_argParser.add_argument( ARG_SELECT )
            .default_value( std::string( "board,layers,footprints,nets,zones" ) )
            .help( UTF8STDSTR( _( "Comma separated list of sections to emit; valid options: "
                                  "board, layers, footprints, pads, nets, tracks, vias, zones, "
                                  "all" ) ) )
            .metavar( "SECTIONS" );
}


int CLI::PCB_QUERY_COMMAND::doPerform( KIWAY& aKiway )
{
    std::unique_ptr<JOB_PCB_QUERY> queryJob( new JOB_PCB_QUERY() );

    queryJob->SetConfiguredOutputPath( m_argOutput );
    queryJob->m_filename = m_argInput;
    queryJob->SetVarOverrides( m_argDefineVars );

    wxString units = From_UTF8( m_argParser.get<std::string>( ARG_UNITS ).c_str() );

    if( units == wxS( "mm" ) )
    {
        queryJob->m_units = JOB_PCB_QUERY::UNITS::MM;
    }
    else if( units == wxS( "in" ) )
    {
        queryJob->m_units = JOB_PCB_QUERY::UNITS::INCH;
    }
    else if( units == wxS( "mils" ) )
    {
        queryJob->m_units = JOB_PCB_QUERY::UNITS::MILS;
    }
    else if( !units.IsEmpty() )
    {
        wxFprintf( stderr, _( "Invalid units specified\n" ) );
        return EXIT_CODES::ERR_ARGS;
    }

    // An explicit --select replaces the defaults wholesale rather than adding to them, so that
    // a caller asking for just "footprints" gets a small document instead of the default set
    // plus footprints.
    queryJob->m_includeBoard = false;
    queryJob->m_includeLayers = false;
    queryJob->m_includeFootprints = false;
    queryJob->m_includePads = false;
    queryJob->m_includeNets = false;
    queryJob->m_includeTracks = false;
    queryJob->m_includeVias = false;
    queryJob->m_includeZones = false;

    wxString        select = From_UTF8( m_argParser.get<std::string>( ARG_SELECT ).c_str() );
    wxStringTokenizer tokenizer( select, wxS( "," ) );

    while( tokenizer.HasMoreTokens() )
    {
        wxString token = tokenizer.GetNextToken().Trim( true ).Trim( false ).Lower();

        if( token.IsEmpty() )
            continue;

        if( token == wxS( "all" ) )
        {
            queryJob->m_includeBoard = true;
            queryJob->m_includeLayers = true;
            queryJob->m_includeFootprints = true;
            queryJob->m_includePads = true;
            queryJob->m_includeNets = true;
            queryJob->m_includeTracks = true;
            queryJob->m_includeVias = true;
            queryJob->m_includeZones = true;
        }
        else if( token == wxS( "board" ) )
        {
            queryJob->m_includeBoard = true;
        }
        else if( token == wxS( "layers" ) )
        {
            queryJob->m_includeLayers = true;
        }
        else if( token == wxS( "footprints" ) )
        {
            queryJob->m_includeFootprints = true;
        }
        else if( token == wxS( "pads" ) )
        {
            // Pads are emitted nested inside their owning footprint, so asking for pads
            // implies the footprint list that carries them.
            queryJob->m_includePads = true;
            queryJob->m_includeFootprints = true;
        }
        else if( token == wxS( "nets" ) )
        {
            queryJob->m_includeNets = true;
        }
        else if( token == wxS( "tracks" ) )
        {
            queryJob->m_includeTracks = true;
        }
        else if( token == wxS( "vias" ) )
        {
            queryJob->m_includeVias = true;
        }
        else if( token == wxS( "zones" ) )
        {
            queryJob->m_includeZones = true;
        }
        else
        {
            wxFprintf( stderr, _( "Invalid section specified: %s\n" ), token );
            return EXIT_CODES::ERR_ARGS;
        }
    }

    int exitCode = aKiway.ProcessJob( KIWAY::FACE_PCB, queryJob.get() );

    return exitCode;
}
