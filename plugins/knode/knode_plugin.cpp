/*
    This file is part of Kontact.
    Copyright (c) 2003 Zack Rusin

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

    As a special exception, permission is given to link this program
    with any edition of Qt, and distribute the resulting executable,
    without including the source code for Qt in the source distribution.
*/

#include "knode_plugin.h"

#include "core.h"

#include <kparts/componentfactory.h>
#include <kgenericfactory.h>
#include <kapplication.h>
#include <kaction.h>
#include <kiconloader.h>
#include <kdebug.h>

#include <qwidget.h>


typedef KGenericFactory<KNodePlugin, Kontact::Core> KNodePluginFactory;
K_EXPORT_COMPONENT_FACTORY( libkontact_knodeplugin,
                            KNodePluginFactory( "kontact_knodeplugin" ) )


KNodePlugin::KNodePlugin( Kontact::Core *core, const char *, const QStringList& )
  : Kontact::Plugin( core, core, "knode" ),
    mPart( 0 )
{
  setInstance( KNodePluginFactory::instance() );
}


KNodePlugin::~KNodePlugin()
{
}

bool KNodePlugin::createDCOPInterface( const QString& serviceType )
{
  return false;
}

KParts::Part* KNodePlugin::part()
{
  if ( !mPart ) {
    mPart = loadPart();
  }
  return mPart;
}

#include "knode_plugin.moc"
