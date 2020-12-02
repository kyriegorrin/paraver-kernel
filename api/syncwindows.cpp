/*****************************************************************************\
 *                        ANALYSIS PERFORMANCE TOOLS                         *
 *                               libparaver-api                              *
 *                       Paraver Main Computing Library                      *
 *****************************************************************************
 *     ___     This library is free software; you can redistribute it and/or *
 *    /  __         modify it under the terms of the GNU LGPL as published   *
 *   /  /  _____    by the Free Software Foundation; either version 2.1      *
 *  /  /  /     \   of the License, or (at your option) any later version.   *
 * (  (  ( B S C )                                                           *
 *  \  \  \_____/   This library is distributed in hope that it will be      *
 *   \  \__         useful but WITHOUT ANY WARRANTY; without even the        *
 *    \___          implied warranty of MERCHANTABILITY or FITNESS FOR A     *
 *                  PARTICULAR PURPOSE. See the GNU LGPL for more details.   *
 *                                                                           *
 * You should have received a copy of the GNU Lesser General Public License  *
 * along with this library; if not, write to the Free Software Foundation,   *
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA          *
 * The GNU LEsser General Public License is contained in the file COPYING.   *
 *                                 ---------                                 *
 *   Barcelona Supercomputing Center - Centro Nacional de Supercomputacion   *
\*****************************************************************************/


#include "syncwindows.h"
#include "window.h"
#include "histogram.h"

using std::vector;
using std::map;

SyncWindows *SyncWindows::instance = NULL;

SyncWindows *SyncWindows::getInstance()
{
  if( SyncWindows::instance == NULL )
    SyncWindows::instance = new SyncWindows();
  return SyncWindows::instance;
}

SyncWindows::SyncWindows()
{
  lastNewGroup = 0;
  syncGroupsTimeline[ lastNewGroup ] = vector<Window *>();
  syncGroupsHistogram[ lastNewGroup ] = vector<Histogram *>();
  removingAll = false;
}

SyncWindows::~SyncWindows()
{
}

bool SyncWindows::addWindow( Window *whichWindow, TGroupId whichGroup )
{
  if( syncGroupsTimeline.find( whichGroup ) == syncGroupsTimeline.end() )
    return false;

  syncGroupsTimeline[ whichGroup ].push_back( whichWindow );
  if( syncGroupsTimeline[ whichGroup ].size() > 1 || syncGroupsHistogram[ whichGroup ].size() > 0 )
  {
    TTime nanoBeginTime, nanoEndTime;
    getGroupTimes( whichGroup, nanoBeginTime, nanoEndTime );
    if( whichWindow->traceUnitsToCustomUnits( whichWindow->getWindowBeginTime(), NS ) != nanoBeginTime ||
        whichWindow->traceUnitsToCustomUnits( whichWindow->getWindowEndTime(), NS )   != nanoEndTime )
    {
      whichWindow->addZoom( nanoBeginTime, nanoEndTime, true );
      whichWindow->setWindowBeginTime( whichWindow->customUnitsToTraceUnits( nanoBeginTime, NS ), true );
      whichWindow->setWindowEndTime( whichWindow->customUnitsToTraceUnits( nanoEndTime, NS ), true );
      whichWindow->setChanged( true );
      whichWindow->setRedraw( true );
    }
  }

  return true;
}

bool SyncWindows::addWindow( Histogram *whichWindow, TGroupId whichGroup )
{
  if( syncGroupsHistogram.find( whichGroup ) == syncGroupsHistogram.end() )
    return false;

  syncGroupsHistogram[ whichGroup ].push_back( whichWindow );
  if( syncGroupsHistogram[ whichGroup ].size() > 1 || syncGroupsTimeline[ whichGroup ].size() > 0 )
  {
    TTime nanoBeginTime, nanoEndTime;
    getGroupTimes( whichGroup, nanoBeginTime, nanoEndTime );
    if( whichWindow->getControlWindow()->traceUnitsToCustomUnits( whichWindow->getBeginTime(), NS ) != nanoBeginTime ||
        whichWindow->getControlWindow()->traceUnitsToCustomUnits( whichWindow->getEndTime(), NS )   != nanoEndTime )
    {
      whichWindow->setWindowBeginTime( whichWindow->getControlWindow()->customUnitsToTraceUnits( nanoBeginTime, NS ), true );
      whichWindow->setWindowEndTime( whichWindow->getControlWindow()->customUnitsToTraceUnits( nanoEndTime, NS ), true );
      whichWindow->setChanged( true );
      whichWindow->setRecalc( true );
      whichWindow->setRedraw( true );
    }
  }

  return true;
}

void SyncWindows::removeWindow( Window *whichWindow, TGroupId whichGroup )
{
  if( syncGroupsTimeline.find( whichGroup ) == syncGroupsTimeline.end() || removingAll )
    return;

  for( vector<Window *>::iterator it = syncGroupsTimeline[ whichGroup ].begin();
       it != syncGroupsTimeline[ whichGroup ].end(); ++it )
  {
    if( *it == whichWindow )
    {
      syncGroupsTimeline[ whichGroup ].erase( it );
      break;
    }
  }
}

void SyncWindows::removeWindow( Histogram *whichWindow, TGroupId whichGroup )
{
  if( syncGroupsHistogram.find( whichGroup ) == syncGroupsHistogram.end() || removingAll )
    return;

  for( vector<Histogram *>::iterator it = syncGroupsHistogram[ whichGroup ].begin();
       it != syncGroupsHistogram[ whichGroup ].end(); ++it )
  {
    if( *it == whichWindow )
    {
      syncGroupsHistogram[ whichGroup ].erase( it );
      break;
    }
  }
}

void SyncWindows::removeAll( TGroupId whichGroup )
{
  if( syncGroupsTimeline.find( whichGroup ) == syncGroupsTimeline.end() )
    return;

  removingAll = true;

  for( vector<Window *>::iterator it = syncGroupsTimeline[ whichGroup ].begin();
       it != syncGroupsTimeline[ whichGroup ].end(); ++it )
    (*it)->removeFromSync();

  syncGroupsTimeline[ whichGroup ].clear();
  if( whichGroup != 0 )
    syncGroupsTimeline.erase( whichGroup );

  for( vector<Histogram *>::iterator it = syncGroupsHistogram[ whichGroup ].begin();
       it != syncGroupsHistogram[ whichGroup ].end(); ++it )
    (*it)->removeFromSync();

  syncGroupsHistogram[ whichGroup ].clear();
  if( whichGroup != 0 )
    syncGroupsHistogram.erase( whichGroup );

  removingAll = false;
}

TGroupId SyncWindows::newGroup()
{
  int groupID = 0;
  map< TGroupId, std::vector< Window *> >::iterator it;
  for ( it = syncGroupsTimeline.begin() ; it != syncGroupsTimeline.end() ; ++it )
  {
    if ( groupID < ( *it ).first )
    {
      it = --it;
      lastNewGroup = ( *it ).first + 1;
      syncGroupsTimeline[ lastNewGroup ]  = vector<Window *>();
      syncGroupsHistogram[ lastNewGroup ] = vector<Histogram *>();
      return lastNewGroup;
    }
    ++groupID;
  }
  if ( groupID == getNumGroups() )
  {
    ++lastNewGroup;
    syncGroupsTimeline[ lastNewGroup ]  = vector<Window *>();
    syncGroupsHistogram[ lastNewGroup ] = vector<Histogram *>();
    return lastNewGroup;
  } 
  return getNumGroups() + 1;
}

TGroupId SyncWindows::getNumGroups() const
{
  return syncGroupsTimeline.size();
}

TGroupId SyncWindows::getAvailableGroup()
{ 
  for( size_t i = 0; i < syncGroupsTimeline.size(); ++i )
  {
    if ( syncGroupsTimeline[ i ].size() == 0 && syncGroupsHistogram[ i ].size() == 0 )
      return i;
  }
  return newGroup();
}

void SyncWindows::getGroups( vector< TGroupId >& groups ) const
{
  for( std::map<TGroupId, std::vector<Window *> >::const_iterator it = syncGroupsTimeline.begin();
       it != syncGroupsTimeline.end(); ++it )
    groups.push_back( it->first );
}

void SyncWindows::broadcastTime( TGroupId whichGroup, Window *sendWindow, TTime beginTime, TTime endTime )
{
  if( syncGroupsTimeline.find( whichGroup ) == syncGroupsTimeline.end() )
    return;

  broadcastTimeTimelines( whichGroup, sendWindow, beginTime, endTime );
  broadcastTimeHistograms( whichGroup, NULL, beginTime, endTime );
}

void SyncWindows::broadcastTime( TGroupId whichGroup, Histogram *sendWindow, TTime beginTime, TTime endTime )
{
  if( syncGroupsHistogram.find( whichGroup ) == syncGroupsHistogram.end() )
    return;

  broadcastTimeTimelines( whichGroup, NULL, beginTime, endTime );
  broadcastTimeHistograms( whichGroup, sendWindow, beginTime, endTime );
}

void SyncWindows::broadcastTimeTimelines( TGroupId whichGroup, Window *sendWindow, TTime beginTime, TTime endTime )
{
  for( vector<Window *>::iterator it = syncGroupsTimeline[ whichGroup ].begin();
       it != syncGroupsTimeline[ whichGroup ].end(); ++it )
  {
    TTime tmpBeginTime, tmpEndTime;
    tmpBeginTime = ( *it )->customUnitsToTraceUnits( beginTime, NS );
    tmpEndTime = ( *it )->customUnitsToTraceUnits( endTime, NS );
    if( ( *it ) != sendWindow  &&
        ( ( *it )->getWindowBeginTime() != tmpBeginTime ||
          ( *it )->getWindowEndTime()   != tmpEndTime )
      )
    {
      ( *it )->addZoom( tmpBeginTime, tmpEndTime, true );
      ( *it )->setWindowBeginTime( tmpBeginTime, true );
      ( *it )->setWindowEndTime( tmpEndTime, true );
      ( *it )->setChanged( true );
      ( *it )->setRedraw( true );
    }
  }
}

void SyncWindows::broadcastTimeHistograms( TGroupId whichGroup, Histogram *sendWindow, TTime beginTime, TTime endTime )
{
  for( vector<Histogram *>::iterator it = syncGroupsHistogram[ whichGroup ].begin();
       it != syncGroupsHistogram[ whichGroup ].end(); ++it )
  {
    TTime tmpBeginTime, tmpEndTime;
    tmpBeginTime = ( *it )->getControlWindow()->customUnitsToTraceUnits( beginTime, NS );
    tmpEndTime = ( *it )->getControlWindow()->customUnitsToTraceUnits( endTime, NS );
    if( ( *it ) != sendWindow  &&
        ( ( *it )->getBeginTime() != tmpBeginTime ||
          ( *it )->getEndTime()   != tmpEndTime )
      )
    {
      ( *it )->setWindowBeginTime( tmpBeginTime, true );
      ( *it )->setWindowEndTime( tmpEndTime, true );
      ( *it )->setChanged( true );
      ( *it )->setRecalc( true );
    }
  }
}

void SyncWindows::getGroupTimes( TGroupId whichGroup, TTime& beginTime, TTime& endTime ) const
{
  if( syncGroupsTimeline.find( whichGroup ) == syncGroupsTimeline.end() )
    return;

  std::map<TGroupId, std::vector<Window *> >::const_iterator itTimeline = syncGroupsTimeline.find( whichGroup );
  if( (*itTimeline).second.size() > 0 )
  {
    beginTime = (*itTimeline).second[ 0 ]->traceUnitsToCustomUnits( (*itTimeline).second[ 0 ]->getWindowBeginTime(), NS );
    endTime   = (*itTimeline).second[ 0 ]->traceUnitsToCustomUnits( (*itTimeline).second[ 0 ]->getWindowEndTime(), NS );
    return;
  }

  std::map<TGroupId, std::vector<Histogram *> >::const_iterator itHistogram = syncGroupsHistogram.find( whichGroup );
  if( (*itHistogram).second.size() > 0 )
  {
    const Window *tmpControl = (*itHistogram).second[ 0 ]->getControlWindow();
    beginTime = tmpControl->traceUnitsToCustomUnits( (*itHistogram).second[ 0 ]->getBeginTime(), NS );
    endTime   = tmpControl->traceUnitsToCustomUnits( (*itHistogram).second[ 0 ]->getEndTime(), NS );
    return;
  }
}
