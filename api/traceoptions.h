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

/* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- *\
 | @file: $HeadURL$
 | @last_commit: $Date$
 | @version:     $Revision$
\* -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- */

#ifndef TRACEOPTIONS_H_INCLUDED
#define TRACEOPTIONS_H_INCLUDED

class KernelConnection;

class TraceOptions
{
  public:

    typedef char TTasksList[256];

//    static TraceOptions *create( KernelConnection *whichKernel, char *xmldocname );
    static TraceOptions *create( KernelConnection *whichKernel );

    TraceOptions() {}
//    TraceOptions( const KernelConnection *whichKernel, char *xmldocname ) {}
    TraceOptions( const KernelConnection *whichKernel ) {}
    virtual ~TraceOptions() {}

    /* Global Settings */
    virtual void set_max_trace_size( int traceSize )
    {}

    virtual int get_max_trace_size( )
    { return 0; }

    /* Sets for Cutter */
    virtual void set_by_time( bool whichByTime )
    {}
    virtual void set_min_cutting_time( unsigned long long minCutTime )
    {}
    virtual void set_max_cutting_time( unsigned long long maxCutTime )
    {}
    virtual void set_minimum_time_percentage( unsigned long long whichMinimumTimePercentage )
    {}
    virtual void set_maximum_time_percentage( unsigned long long whichMaximumTimePercentage )
    {}
    virtual void set_tasks_list( char *tasksList )
    {}
    virtual void set_original_time( char originalTime )
    {}
    virtual void set_break_states( int breakStates )
    {}
    virtual void set_remFirstStates( int remStates )
    {}
    virtual void set_remLastStates( int remStates )
    {}

    virtual bool get_by_time()
    { return false; }
    virtual unsigned long long get_min_cutting_time()
    { return 0; }
    virtual unsigned long long get_max_cutting_time()
    { return 0; }
    virtual unsigned long long get_minimum_time_percentage()
    { return 0; }
    virtual unsigned long long get_maximum_time_percentage()
    { return 0; }
    virtual void get_tasks_list( TTasksList &whichTasksList )
    { }
    virtual char get_original_time()
    { return 0; }
    virtual int get_break_states()
    { return 0; }
    virtual int get_remFirstStates()
    { return 0; }
    virtual int get_remLastStates()
    { return 0; }



    /* Sets for filtering */
    virtual void set_filter_events( char filterEvents )
    {}
    virtual void set_filter_states( char filterStates )
    {}
    virtual void set_filter_comms( char filterComms )
    {}
    virtual void set_discard_given_types( char discardGivenTypes )
    {}
    virtual void set_filter_by_call_time( char filterByCallTime )
    {}
    virtual void set_state_names( char *stateNames[20] )
    {}
    virtual void set_all_states( char allStates )
    {}
    virtual void set_min_state_time( unsigned long long minStateTime )
    {}
    virtual void set_min_comm_size( int minCommSize )
    {}
    virtual void set_filter_types( struct allowed_types filterTypes[20] )
    {}
    virtual void set_filter_last_type(  int filterLastType )
    {}

    /* Sets for Sofware Counters */
    virtual void set_sc_onInterval( int scOnInterval )
    {}
    virtual void set_sc_global_counters( int scGlobalCounters )
    {}
    virtual void set_sc_acumm_counters( int scAcummCounters )
    {}
    virtual void set_sc_summarize_states( int scSummarizeStates )
    {}
    virtual void set_sc_only_in_bursts( int scOnlyInBursts )
    {}
    virtual void set_sc_remove_states( int scRemoveStates )
    {}
    virtual void set_sc_interval( unsigned long long scInterval )
    {}
    virtual void set_sc_frequency( int scFrequency )
    {}
    virtual void set_types( char *whichTypes )
    {}
    virtual void set_types_kept( char *typesKept )
    {}

    /* Sets for comm_fusion */
    virtual void set_reduce_comms( char reduceComms )
    {}
    virtual void set_comm_fusion_big_interval( unsigned long long bigInterval )
    {}
    virtual void set_comm_fusion_small_interval( unsigned long long smallInterval )
    {}

    /* Parameters for Stats */
    virtual void set_bursts_plot( int burstsPlot )
    {}
    virtual void set_comms_plot( int commsPlot )
    {}
    virtual void set_events_plot( int eventsPlot )
    {}

    virtual void parseDoc( char *docname )
    {}

  protected:
  //  KernelConnection *myKernel;
};


class TraceOptionsProxy :public TraceOptions
{
  public:
    virtual ~TraceOptionsProxy();

    /* Global Settings */
    virtual void set_max_trace_size( int traceSize );
    virtual int get_max_trace_size();

    /* Sets for Cutter */
    virtual void set_by_time( bool whichByTime );
    virtual void set_min_cutting_time( unsigned long long minCutTime );
    virtual void set_max_cutting_time( unsigned long long maxCutTime );
    virtual void set_minimum_time_percentage( unsigned long long whichMinimumTimePercentage );
    virtual void set_maximum_time_percentage( unsigned long long whichMaximumTimePercentage );
    virtual void set_tasks_list( TTasksList &tasksList );
    virtual void set_original_time( char originalTime );
    virtual void set_break_states( int breakStates );
    virtual void set_remFirstStates( int remStates );
    virtual void set_remLastStates( int remStates );

    virtual bool get_by_time();
    virtual unsigned long long get_min_cutting_time();
    virtual unsigned long long get_max_cutting_time();
    virtual unsigned long long get_minimum_time_percentage();
    virtual unsigned long long get_maximum_time_percentage();
    virtual void get_tasks_list( TTasksList &whichTasksList );
    virtual char get_original_time();
    virtual int get_break_states();
    virtual int get_remFirstStates();
    virtual int get_remLastStates();

    /* Sets for filtering */
    virtual void set_filter_events( char filterEvents );
    virtual void set_filter_states( char filterStates );
    virtual void set_filter_comms( char filterComms );
    virtual void set_discard_given_types( char discardGivenTypes );
    virtual void set_filter_by_call_time( char filterByCallTime );
    virtual void set_state_names( char *stateNames[20] );
    virtual void set_all_states( char allStates );
    virtual void set_min_state_time( unsigned long long minStateTime );
    virtual void set_min_comm_size( int minCommSize );
    virtual void set_filter_types( struct allowed_types filterTypes[20] );
    virtual void set_filter_last_type(  int filterLastType );

    /* Sets for Software Counters */
    virtual void set_sc_onInterval( int scOnInterval );
    virtual void set_sc_global_counters( int scGlobalCounters );
    virtual void set_sc_acumm_counters( int scAcummCounters );
    virtual void set_sc_summarize_states( int scSummarizeStates );
    virtual void set_sc_only_in_bursts( int scOnlyInBursts );
    virtual void set_sc_remove_states( int scRemoveStates );
    virtual void set_sc_interval( unsigned long long scInterval );
    virtual void set_sc_frequency( int scFrequency );
    virtual void set_types( char *whichTypes );
    virtual void set_types_kept( char *typesKept );

    /* Sets for comm_fusion */
    virtual void set_reduce_comms( char reduceComms );
    virtual void set_comm_fusion_big_interval( unsigned long long bigInterval );
    virtual void set_comm_fusion_small_interval( unsigned long long smallInterval );

    /* Parameters for Stats */
    virtual void set_bursts_plot( int burstsPlot );
    virtual void set_comms_plot( int commsPlot );
    virtual void set_events_plot( int eventsPlot );

    virtual void parseDoc( char *docname );

  protected:

  private:
    TraceOptions *myTraceOptions;

    // TraceOptionsProxy( const KernelConnection *whichKernel, char *xmldocname );
    TraceOptionsProxy( const KernelConnection *whichKernel );

    // friend TraceOptions *TraceOptions::create( KernelConnection *, char * );
    friend TraceOptions *TraceOptions::create( KernelConnection * );
};
#endif // TRACEOPTIONS_H_INCLUDED
