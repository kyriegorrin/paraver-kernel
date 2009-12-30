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

#ifndef TRACEBODYIO_V2_H_INCLUDED
#define TRACEBODYIO_V2_H_INCLUDED

#include "tracebodyio.h"
#include "tracestream.h"

using namespace std;

// Paraver trace NEW format file
class TraceBodyIO_v2 : public TraceBodyIO
{
  public:
    static const UINT8 StateBeginRecord = '1';
    static const UINT8 StateEndRecord = '2';
    static const UINT8 EventRecord = '3';
    static const UINT8 CommRecord = 'd';
    static const UINT8 LogicalSendRecord = '4';
    static const UINT8 LogicalRecvRecord = '5';
    static const UINT8 PhysicalSendRecord = '6';
    static const UINT8 PhysicalRecvRecord = '7';
    static const UINT8 GlobalCommRecord = '8';
#ifdef BYTHREAD
    static const UINT8 RemoteLogicalSendRecord = 'W';
    static const UINT8 RemoteLogicalRecvRecord = 'X';
    static const UINT8 RemotePhysicalSendRecord = 'Y';
    static const UINT8 RemotePhysicalRecvRecord = 'Z';
#endif

    bool ordered() const;
    void read( TraceStream *file, MemoryBlocks& records,
               hash_set<TEventType>& events ) const;
    void write( fstream& whichStream,
                const KTrace& whichTrace,
                MemoryTrace::iterator *record,
                INT32 numIter = 0 ) const;
    void writeEvents( fstream& whichStream,
                      const KTrace& whichTrace,
                      vector<MemoryTrace::iterator *>& recordList,
                      INT32 numIter = 0 ) const;
    void writeCommInfo( fstream& whichStream,
                        const KTrace& whichTrace,
                        INT32 numIter = 1 ) const;
  protected:

  private:
    void readState( const string& line, MemoryBlocks& records ) const;
    void readEvent( const string& line, MemoryBlocks& records,
                    hash_set<TEventType>& events ) const;
    void readComm( const string& line, MemoryBlocks& records ) const;
    void readGlobalComm( const string& line, MemoryBlocks& records ) const;
    bool readCommon( istringstream& line,
                     TCPUOrder& CPU,
                     TThreadOrder& thread,
                     TRecordTime& time ) const;

    bool writeState( string& line,
                     const KTrace& whichTrace,
                     MemoryTrace::iterator *record,
                     INT32 numIter = 0 ) const;
    bool writeEvent( string& line,
                     const KTrace& whichTrace,
                     MemoryTrace::iterator *record,
                     bool needCommons = true,
                     INT32 numIter = 0 ) const;
    bool writeCommRecord( string& line,
                          const KTrace& whichTrace,
                          MemoryTrace::iterator *record,
                          INT32 numIter = 0 ) const;
    bool writeGlobalComm( string& line,
                          const KTrace& whichTrace,
                          MemoryTrace::iterator *record,
                          INT32 numIter = 0 ) const;
    void writeCommon( ostringstream& line,
                      const KTrace& whichTrace,
                      MemoryTrace::iterator *record,
                      INT32 numIter = 0 ) const;

};


#endif // TRACEBODYIO_V2_H_INCLUDED
