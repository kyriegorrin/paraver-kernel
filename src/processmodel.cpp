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

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- *\
 | @file: $HeadURL$
 | @last_commit: $Date$
 | @version:     $Revision$
\* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

#include <sstream>
#include <iostream>
#include <fstream>
#include "processmodel.h"
#include "traceheaderexception.h"

#include <stdlib.h>

TApplOrder ProcessModel::totalApplications() const
{
  return applications.size();
}


TTaskOrder ProcessModel::totalTasks() const
{
  return tasks.size();
}


void ProcessModel::getTaskLocation( TTaskOrder globalTask,
                                    TApplOrder& inAppl,
                                    TTaskOrder& inTask ) const
{
  inAppl = tasks[ globalTask ].appl;
  inTask = tasks[ globalTask ].task;
}


TTaskOrder ProcessModel::getGlobalTask( const TApplOrder& inAppl,
                                        const TTaskOrder& inTask ) const
{
  return applications[ inAppl ].tasks[ inTask ].traceGlobalOrder;
}


TThreadOrder ProcessModel::totalThreads() const
{
  return threads.size();
}


TThreadOrder ProcessModel::getGlobalThread( const TApplOrder& inAppl,
    const TTaskOrder& inTask,
    const TThreadOrder& inThread ) const
{
  return applications[ inAppl ].tasks[ inTask ].threads[ inThread ].traceGlobalOrder;
}


void ProcessModel::getThreadLocation( TThreadOrder globalThread,
                                      TApplOrder& inAppl,
                                      TTaskOrder& inTask,
                                      TThreadOrder& inThread ) const
{
  inAppl = threads[ globalThread ].appl;
  inTask = threads[ globalThread ].task;
  inThread = threads[ globalThread ].thread;
}


ProcessModel::ProcessModel( istringstream& headerInfo )
{
  TApplOrder numberApplications;
  TTaskOrder globalTasks = 0;
  TThreadOrder globalThreads = 0;
  ready = false;

  string stringNumberApplications;
  std::getline( headerInfo, stringNumberApplications, ':' );
  istringstream sstreamNumberAppl( stringNumberApplications );

  if ( !( sstreamNumberAppl >> numberApplications ) ||
       numberApplications == 0 )
  {
    throw TraceHeaderException( TraceHeaderException::invalidApplNumber,
                                stringNumberApplications.c_str() );
  }

  // Insert applications
  for ( TApplOrder countAppl = 0; countAppl < numberApplications; ++countAppl )
  {
    TTaskOrder numberTasks;

    applications.push_back( ProcessModelAppl( countAppl ) );

    string stringNumberTasks;
    std::getline( headerInfo, stringNumberTasks, '(' );
    istringstream sstreamNumberTasks( stringNumberTasks );

    if ( !( sstreamNumberTasks >> numberTasks ) ||
         numberTasks == 0 )
    {
      throw TraceHeaderException( TraceHeaderException::invalidTaskNumber,
                                  stringNumberTasks.c_str() );
    }

    // Insert tasks
    for ( TTaskOrder countTask = 0; countTask < numberTasks; ++countTask )
    {
      TThreadOrder numberThreads;
      TNodeOrder numberNode;

      applications[ countAppl ].tasks.push_back( ProcessModelTask( globalTasks ) );
      tasks.push_back( TaskLocation() );
      tasks[ globalTasks ].appl = countAppl;
      tasks[ globalTasks ].task = countTask;
      ++globalTasks;

      string stringNumberThreads;
      std::getline( headerInfo, stringNumberThreads, ':' );
      istringstream sstreamNumberThreads( stringNumberThreads );

      if ( !( sstreamNumberThreads >> numberThreads ) ||
           numberThreads == 0 )
      {
        throw TraceHeaderException( TraceHeaderException::invalidThreadNumber,
                                    stringNumberThreads.c_str() );
      }

      string stringNumberNode;

      if ( countTask < numberTasks - 1 )
        std::getline( headerInfo, stringNumberNode, ',' );
      else
        std::getline( headerInfo, stringNumberNode, ')' );

      istringstream sstreamNumberNode( stringNumberNode );

      if ( !( sstreamNumberNode >> numberNode ) )
      {
        throw TraceHeaderException( TraceHeaderException::invalidNodeNumber,
                                    stringNumberNode.c_str() );
      }

      // Insert threads
      for ( TThreadOrder countThread = 0; countThread < numberThreads; ++countThread )
      {
        applications[ countAppl ].tasks[ countTask ].threads.push_back( ProcessModelThread( globalThreads, numberNode - 1 ) );
        threads.push_back( ThreadLocation() );
        threads[ globalThreads ].appl = countAppl;
        threads[ globalThreads ].task = countTask;
        threads[ globalThreads ].thread = countThread;
        ++globalThreads;
      }
      // End inserting threads

    }
    // End inserting tasks

    // Gets a useless character: ':' or ','
    headerInfo.get();

  }
  // End inserting applications

  ready = true;
}


void ProcessModel::dumpToFile( fstream& file ) const
{
  ostringstream ostr;
  ostr << fixed;
  ostr << dec;
  ostr.precision( 0 );

  ostr << applications.size() << ':';
  for ( TApplOrder iAppl = 0; iAppl < applications.size(); ++iAppl )
  {
    ostr << applications[ iAppl ].tasks.size() << '(';
    for ( TTaskOrder iTask = 0; iTask < applications[ iAppl ].tasks.size(); ++iTask )
    {
      ostr << applications[ iAppl ].tasks[ iTask ].threads.size() << ':';
      ostr << applications[ iAppl ].tasks[ iTask ].threads[ 0 ].nodeExecution;

      if ( iTask < applications[ iAppl ].tasks.size() - 1 )
        ostr << ',';
    }
    ostr << ')';

    if ( iAppl < applications.size() - 1 )
      ostr << ':';
  }
  file << ostr.str();
}


TTaskOrder ProcessModel::getFirstTask( TApplOrder inAppl ) const
{
  return applications[ inAppl ].tasks[ 0 ].traceGlobalOrder;
}


TTaskOrder ProcessModel::getLastTask( TApplOrder inAppl ) const
{
  return applications[ inAppl ].tasks[
           applications[ inAppl ].tasks.size() - 1 ].traceGlobalOrder;
}


TThreadOrder ProcessModel::getFirstThread( TApplOrder inAppl, TTaskOrder inTask ) const
{
  return applications[ inAppl ].tasks[ inTask ].threads[ 0 ].traceGlobalOrder;
}


TThreadOrder ProcessModel::getLastThread( TApplOrder inAppl, TTaskOrder inTask )const
{
  return applications[ inAppl ].tasks[ inTask ].threads[
           applications[ inAppl ].tasks[ inTask ].threads.size() - 1 ].traceGlobalOrder;
}

void ProcessModel::getThreadsPerNode( TNodeOrder inNode, vector<TThreadOrder>& onVector ) const
{
  onVector.clear();

  for ( vector<ProcessModelAppl>::const_iterator itAppl = applications.begin();
        itAppl != applications.end(); ++itAppl )
  {
    for ( vector<ProcessModelTask>::const_iterator itTask = itAppl->tasks.begin();
          itTask != itAppl->tasks.end(); ++itTask )
    {
      for ( vector<ProcessModelThread>::const_iterator itThread = itTask->threads.begin();
            itThread != itTask->threads.end(); ++itThread )
      {
        if ( itThread->nodeExecution == inNode )
          onVector.push_back( itThread->traceGlobalOrder );
      }
    }
  }
}

bool ProcessModel::isValidThread( TThreadOrder whichThread ) const
{
  return whichThread < threads.size();
}

bool ProcessModel::isValidThread( TApplOrder whichAppl,
                                  TTaskOrder whichTask,
                                  TThreadOrder whichThread ) const
{
  if( !isValidAppl( whichAppl ) )
    return false;

  if( whichTask >= applications[ whichAppl ].tasks.size() )
    return false;

  return whichThread < applications[ whichAppl ].tasks[ whichTask ].threads.size();
}

bool ProcessModel::isValidTask( TTaskOrder whichTask ) const
{
  return whichTask < tasks.size();
}

bool ProcessModel::isValidTask( TApplOrder whichAppl,
                                TTaskOrder whichTask ) const
{
  if( !isValidAppl( whichAppl ) )
    return false;

  return whichTask < applications[ whichAppl ].tasks.size();
}

bool ProcessModel::isValidAppl( TApplOrder whichAppl ) const
{
  return whichAppl < applications.size();
}
