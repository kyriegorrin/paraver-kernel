#include <algorithm>
#include <functional>
#include "trace.h"

template < typename SelType >
SelectionManagement< SelType >::SelectionManagement()
{
}


template < typename SelType >
SelectionManagement< SelType >::~SelectionManagement()
{
}


template < typename SelType >
void SelectionManagement< SelType >::init( Trace *trace )
{
  selected.clear();
  selectedSet.clear();

  vector< bool > auxSelected;

  for (  UINT32 level = (UINT32)NONE; level <= (UINT32)CPU; ++level )
  {
    auxSelected.clear();
    selected.push_back( vector< bool >( ) );
    selectedSet.push_back( vector< TObjectOrder >( ) );

    switch ( level )
    {
      case APPLICATION:
        auxSelected.insert( auxSelected.begin(),
                            ( size_t ) trace->totalApplications(),
                            true );
        setSelected( auxSelected, ( TWindowLevel )level );
        break;

      case TASK:
        auxSelected.insert( auxSelected.begin(),
                            ( size_t ) trace->totalTasks(),
                            true );
        setSelected( auxSelected, ( TWindowLevel )level );
        break;

      case THREAD:
        auxSelected.insert( auxSelected.begin(),
                            ( size_t ) trace->totalThreads(),
                            true );
        setSelected( auxSelected, ( TWindowLevel )level );
        break;

      case NODE:
        auxSelected.insert( auxSelected.begin(),
                            trace->totalNodes(),
                            true );
        setSelected( auxSelected, ( TWindowLevel )level );
        break;

      case CPU:
        auxSelected.insert( auxSelected.begin(),
                            trace->totalCPUs(),
                            true );
        setSelected( auxSelected, ( TWindowLevel )level );
        break;

//    NONE, WORKLOAD, SYSTEM
      default:
        break;
    }
  }
}


template < typename SelType >
void SelectionManagement<SelType>::copy( const SelectionManagement &selection )
{
  selected = selection.selected;
  selectedSet = selection.selectedSet;
}


template < typename SelType >
void SelectionManagement< SelType >::setSelected( vector< bool > &selection,
                                                  TWindowLevel level )
{
  selectedSet[ level ].clear();
  if( selected[ level ].size() > selection.size() )
  {
    std::copy( selection.begin(), selection.end(), selected[ level ].begin() );
  }
  else
  {
    size_t size = selected[ level ].size();
    selected[ level ] = selection;
    if( size > 0 )
      selected[ level ].resize( size );
  }

  if ( !selection.empty() )
  {
    for ( size_t current = 0; current < selected[ level ].size(); ++current )
      if ( selected[ level ][ current ] )
        selectedSet[ level ].push_back( current );
  }
}


template < typename SelType >
void SelectionManagement< SelType >::setSelected( vector< SelType > &selection,
                                                  SelType maxElems,
                                                  TWindowLevel level )
{
  selected[ level ].clear();
  selectedSet[ level ] = selection;
  typename vector<SelType>::iterator maxIt;
  maxIt = std::find_if( selectedSet[ level ].begin(),
                        selectedSet[ level ].end(),
                        std::bind2nd( std::greater_equal<SelType>(), maxElems ) );
  if( maxIt != selectedSet[ level ].end() )
    selectedSet[ level ].erase( maxIt, selectedSet[ level ].end() );

  if ( !selection.empty() )
  {
    typename vector< SelType >::iterator it;
    it = selection.begin();
    for ( size_t current = 0; current < ( size_t ) maxElems; ++current )
    {
      if ( it != selection.end() && current == ( size_t )*it )
      {
        selected[ level ].push_back( true );
        ++it;
      }
      else
        selected[ level ].push_back( false );
    }
  }
}


template < typename SelType >
bool SelectionManagement< SelType >::isSelectedPosition( SelType whichSelected,
                                                         TWindowLevel level )
{
  return selected[ level ][ whichSelected ];
}


template < typename SelType >
void SelectionManagement< SelType >::getSelected( vector< bool > &whichSelected,
                                                  TWindowLevel level )
{
  whichSelected = selected[ level ];
}


template < typename SelType >
void SelectionManagement< SelType >::getSelected( vector< SelType > &whichSelected,
                                                  TWindowLevel level )
{
  whichSelected = selectedSet[ level ];
}


template < typename SelType >
void SelectionManagement< SelType >::getSelected( vector< SelType > &whichSelected,
                                                  SelType first,
                                                  SelType last,
                                                  TWindowLevel level )
{
  whichSelected.clear();
  typename vector< SelType >::iterator it;
  for ( it = selectedSet[ level ].begin(); it != selectedSet[ level ].end(); ++it )
  {
    if (( *it >= first ) && ( *it <= last ))
      whichSelected.push_back( *it );
    if ( *it == last )
      break;
  }
}
