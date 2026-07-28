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

#include "job_pcb_query.h"

#include <jobs/job_registry.h>
#include <wildcards_and_files_ext.h>
#include <wx/filename.h>

NLOHMANN_JSON_SERIALIZE_ENUM( JOB_PCB_QUERY::UNITS, {
                                                            { JOB_PCB_QUERY::UNITS::INCH, "in" },
                                                            { JOB_PCB_QUERY::UNITS::MM, "mm" },
                                                            { JOB_PCB_QUERY::UNITS::MILS, "mils" },
                                                    } )

JOB_PCB_QUERY::JOB_PCB_QUERY() :
        JOB( "pcb_query", false ),
        m_filename(),
        m_units( UNITS::MM ),
        m_includeBoard( true ),
        m_includeLayers( true ),
        m_includeFootprints( true ),
        m_includePads( false ),
        m_includeNets( true ),
        m_includeTracks( false ),
        m_includeVias( false ),
        m_includeZones( true )
{
    m_params.emplace_back( new JOB_PARAM<UNITS>( "units", &m_units, m_units ) );
    m_params.emplace_back( new JOB_PARAM<bool>( "include_board", &m_includeBoard, m_includeBoard ) );
    m_params.emplace_back( new JOB_PARAM<bool>( "include_layers", &m_includeLayers, m_includeLayers ) );
    m_params.emplace_back( new JOB_PARAM<bool>( "include_footprints", &m_includeFootprints,
                                                m_includeFootprints ) );
    m_params.emplace_back( new JOB_PARAM<bool>( "include_pads", &m_includePads, m_includePads ) );
    m_params.emplace_back( new JOB_PARAM<bool>( "include_nets", &m_includeNets, m_includeNets ) );
    m_params.emplace_back( new JOB_PARAM<bool>( "include_tracks", &m_includeTracks, m_includeTracks ) );
    m_params.emplace_back( new JOB_PARAM<bool>( "include_vias", &m_includeVias, m_includeVias ) );
    m_params.emplace_back( new JOB_PARAM<bool>( "include_zones", &m_includeZones, m_includeZones ) );
}


wxString JOB_PCB_QUERY::GetDefaultDescription() const
{
    return _( "Query board contents" );
}


wxString JOB_PCB_QUERY::GetSettingsDialogTitle() const
{
    return _( "Query Board Contents Job Settings" );
}


void JOB_PCB_QUERY::SetDefaultOutputPath( const wxString& aReferenceName )
{
    wxFileName fn( aReferenceName );

    // DRC/ERC reports and the statistics export already claim the plain board name, so
    // qualify ours to avoid clobbering them when several jobs share an output directory.
    fn.SetName( fn.GetName() + wxS( "_query" ) );
    fn.SetExt( FILEEXT::JsonFileExtension );

    SetWorkingOutputPath( fn.GetFullName() );
}


REGISTER_JOB( pcb_query, _( "PCB: Query Board Contents" ), KIWAY::FACE_PCB, JOB_PCB_QUERY );
