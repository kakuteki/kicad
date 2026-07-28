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

#ifndef JOB_PCB_QUERY_H
#define JOB_PCB_QUERY_H

#include "job.h"

/**
 * Dump the machine-readable contents of a board as JSON.
 *
 * This is the read half of the scripted-editing pair; JOB_PCB_EDIT consumes the same
 * object identity (footprint reference designators, net names, layer names) that this
 * job emits, so a caller can round-trip query -> decide -> edit without holding any
 * state that only the GUI knows about.
 */
class KICOMMON_API JOB_PCB_QUERY : public JOB
{
public:
    enum class UNITS
    {
        MM,
        INCH,
        MILS
    };

    JOB_PCB_QUERY();

    wxString GetDefaultDescription() const override;
    wxString GetSettingsDialogTitle() const override;

    void SetDefaultOutputPath( const wxString& aReferenceName );

    wxString m_filename;
    UNITS    m_units;

    bool m_includeBoard;
    bool m_includeLayers;
    bool m_includeFootprints;
    bool m_includePads;
    bool m_includeNets;
    bool m_includeTracks;
    bool m_includeVias;
    bool m_includeZones;
};

#endif
