/*****************************************************************************\
 *                        ANALYSIS PERFORMANCE TOOLS                         *
 *                               libparaver-api                              *
 *                      API Library for libparaver-kernel                    *
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


#pragma once


#include <map>

#include "kernelconnection.h"

class KTimeline;
class PreviousFiles;

constexpr size_t MAX_TRACES_HISTORY_LENGTH = 256;

class LocalKernel: public KernelConnection
{
  public:
    static void init();

    LocalKernel( bool (*messageFunction)(UserMessageID) );
    virtual ~LocalKernel();

    virtual bool checkTraceSize( const std::string& filename, TTraceSize maxSize ) const override;
    virtual TTraceSize getTraceSize( const std::string& filename ) const override;
    virtual Trace *newTrace( const std::string& whichFile, bool noLoad, ProgressController *progress = nullptr, TTraceSize traceSize = 0 ) const override;
    virtual std::string getPCFFileLocation( const std::string& traceFile ) const override;
    virtual std::string getROWFileLocation( const std::string& traceFile ) const override;
    virtual Timeline *newSingleWindow() const override;
    virtual Timeline *newSingleWindow( Trace *whichTrace ) const override;
    virtual Timeline *newDerivedWindow() const override;
    virtual Timeline *newDerivedWindow( Timeline *window1, Timeline * window2 ) const override;
    virtual Histogram *newHistogram() const override;
//    virtual RecordList *newRecordList() const;
    virtual ProgressController *newProgressController() const override;
    virtual Filter *newFilter( Filter *concreteFilter ) const override;
    virtual TraceEditSequence *newTraceEditSequence() const override;

    virtual std::string getToolID( const std::string &toolName ) const override;
    virtual std::string getToolName( const std::string &toolID ) const override;
    virtual TraceOptions *newTraceOptions() const override;
    virtual TraceCutter *newTraceCutter( TraceOptions *options,
                                         const std::vector< TEventType > &whichTypesWithValuesZero ) const override;
    virtual TraceFilter *newTraceFilter( char *trace_in,
                                         char *trace_out,
                                         TraceOptions *options,
                                         const std::map< TTypeValuePair, TTypeValuePair >& whichTranslationTable,
                                         ProgressController *progress = nullptr ) const override;
    virtual TraceSoftwareCounters *newTraceSoftwareCounters( char *trace_in,
                                                             char *trace_out,
                                                             TraceOptions *options,
                                                             ProgressController *progress = nullptr ) const override;
    virtual TraceShifter *newTraceShifter( std::string traceIn,
                                           std::string traceOut,
                                           std::string shiftTimesFile,
                                           TWindowLevel shiftLevel,
                                           ProgressController *progress ) const override;
    virtual EventDrivenCutter *newEventDrivenCutter( std::string traceIn,
                                                     std::string traceOut,
                                                     TEventType whichEvent,
                                                     ProgressController *progress = nullptr ) const override;
    virtual EventTranslator *newEventTranslator( std::string traceIn,
                                                 std::string traceOut,
                                                 std::string traceReference,
                                                 ProgressController *progress = nullptr ) const override;

    virtual void getAllStatistics( std::vector<std::string>& onVector ) const override;
    virtual void getAllFilterFunctions( std::vector<std::string>& onVector ) const override;
    virtual void getAllSemanticFunctions( TSemanticGroup whichGroup,
                                          std::vector<std::string>& onVector ) const override;

    virtual bool userMessage( UserMessageID messageID ) const override;

    virtual bool isTraceFile( const std::string &filename ) const override;
    virtual void copyPCF( const std::string& name, const std::string& traceToLoad ) const override;
    virtual void copyROW( const std::string& name, const std::string& traceToLoad ) const override;

    // WILL BE SUBSTITUTED
    virtual void getNewTraceName( char *name,
                                  char *new_trace_name,
                                  std::string action,
                                  bool saveNewNameInfo = true ) override;

    // Returns modified fullPathTraceName, with appended or modified filter suffix.
    virtual std::string getNewTraceName( const std::string& fullPathTraceName,
                                         const std::string& traceFilterID,
                                         const bool commitName = false ) const override;

    // Returns modified fullPathTraceName, with appended or modified filter suffixes.
    virtual std::string getNewTraceName( const std::string& fullPathTraceName,
                                         const std::vector< std::string >& traceFilterID,
                                         const bool commitName = false ) const override;

    // Returns modified fullPathTraceName, with appended or modified filter suffixes.
    // Takes path as root for the trace
    virtual std::string getNewTraceName( const std::string& fullPathTraceName,
                                         const std::string& outputPath,
                                         const std::vector< std::string >& traceFilterID,
                                         const bool commitName = false ) const override;

    //virtual char *composeName( char *name, char *newExtension );
    static std::string composeName( const std::string& name, const std::string& newExtension );
    bool isFileReadable( const std::string& filename,
                         const std::string& message,
                         const bool verbose = true,
                         const bool keepOpen = true,
                         const bool exitProgram = true ) const override;

    virtual void commitNewTraceName( const std::string& newTraceName ) const override;

    virtual std::string getPathSeparator() const override { return pathSeparator; }
    virtual void setPathSeparator( const std::string& whichPath ) override { pathSeparator = whichPath; }

    virtual std::string getDistributedCFGsPath() const override { return distributedCFGsPath; }

    virtual std::string getParaverUserDir() const override { return paraverUserDir; }

  protected:

  private:
    std::string pathSeparator;
    std::string distributedCFGsPath;
    std::string paraverUserDir;

    bool (*myMessageFunction)(UserMessageID);

    // FILTERS
    struct traces_table
    {
      char *name;
      int num_chop;
      int num_filter;
      int num_sc;
    };

    struct traces_table trace_names_table[ MAX_TRACES_HISTORY_LENGTH ];
    int trace_names_table_last; // should be static?

    // Paraver user file that stores path and names of last cutted/filtered traces
    // It's opened/created in LocalKernel constructor.
    // And written in getNewTraceName when commitName == true
    PreviousFiles *prevTraceNames;

    void copyFile( const std::string& in, const std::string& out ) const;
    void fileUnreadableError( const std::string& filename,
                              const std::string& message,
                              const bool verbose,
                              const bool exitProgram ) const;
};



