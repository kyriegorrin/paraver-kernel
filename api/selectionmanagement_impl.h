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


#include <algorithm>
#include <functional>
#include "trace.h"
#include "histogramtotals.h"
#include "paraverkernelexception.h"

template < typename SelType, typename LevelType >
SelectionManagement< SelType, LevelType >::SelectionManagement()
{
}

template < typename SelType, typename LevelType >
SelectionManagement< SelType, LevelType >::SelectionManagement( const SelectionManagement& whichSelection )
{
  for ( size_t i = 0 ; i < whichSelection.selectedSet.size(); ++i )
  {
    selectedSet.push_back( whichSelection.selectedSet[ i ] );
  }
  for ( size_t i = 0 ; i < whichSelection.selected.size(); ++i )
  {
    selected.push_back( whichSelection.selected[ i ] );
  }
}


template < typename SelType, typename LevelType >
SelectionManagement< SelType, LevelType >::~SelectionManagement()
{
}


template <>
inline void SelectionManagement< TObjectOrder, TTraceLevel >::init( Trace *trace )
{
  selected.clear();
  selectedSet.clear();

  std::vector< bool > auxSelected;

  for ( TTraceLevel level = TTraceLevel::NONE; level <= TTraceLevel::CPU; ++level )
  {
    auxSelected.clear();
    selected.push_back( std::vector< bool >( ) );
    selectedSet.push_back( std::vector< TObjectOrder >( ) );

    switch ( level )
    {
      case TTraceLevel::APPLICATION:
        auxSelected.insert( auxSelected.begin(),
                            ( size_t ) trace->totalApplications(),
                            true );
        setSelected( auxSelected, level );
        break;

      case TTraceLevel::TASK:
        auxSelected.insert( auxSelected.begin(),
                            ( size_t ) trace->totalTasks(),
                            true );
        setSelected( auxSelected, level );
        break;

      case TTraceLevel::THREAD:
        auxSelected.insert( auxSelected.begin(),
                            ( size_t ) trace->totalThreads(),
                            true );
        setSelected( auxSelected, level );
        break;

      case TTraceLevel::NODE:
        auxSelected.insert( auxSelected.begin(),
                            trace->totalNodes(),
                            true );
        setSelected( auxSelected, level );
        break;

      case TTraceLevel::CPU:
        auxSelected.insert( auxSelected.begin(),
                            trace->totalCPUs(),
                            true );
        setSelected( auxSelected, level );
        break;

//    NONE, WORKLOAD, SYSTEM
      default:
        auxSelected.insert( auxSelected.begin(),
                            1,
                            true );
        setSelected( auxSelected, level );
        break;
    }
  }
}


template < typename SelType, typename LevelType >
void SelectionManagement< SelType, LevelType >::init( HistogramTotals *totals,
    PRV_UINT16 idStat,
    THistogramColumn numColumns,
    THistogramColumn whichPlane )
{
  selected.clear();
  selectedSet.clear();

  selected.push_back( std::vector< bool >( ) );
  selectedSet.push_back( std::vector< SelType >( ) );

  std::vector< bool > auxSelected;
  for ( THistogramColumn i = 0; i < numColumns; ++i )
  {
    if ( totals->getTotal( idStat, i, whichPlane ) == 0 )
      auxSelected.push_back( false );
    else
      auxSelected.push_back( true );
  }

  setSelected( auxSelected, 0 );
}


template < typename SelType, typename LevelType >
void SelectionManagement<SelType, LevelType>::copy( const SelectionManagement &whichSelection )
{
  selected = whichSelection.selected;
  selectedSet = whichSelection.selectedSet;
}


template < typename SelType, typename LevelType >
bool SelectionManagement<SelType, LevelType>:: operator== ( const SelectionManagement<SelType, LevelType> &whichSelection ) const
{
  bool equal = false;

  if ( selected.size()    == whichSelection.selected.size() &&
       selectedSet.size() == whichSelection.selectedSet.size() )
  {
    std::vector< std::vector< bool > >::const_iterator it1 = whichSelection.selected.begin();
    for ( std::vector< std::vector< bool > >::const_iterator it2 = selected.begin(); it2 != selected.end(); ++it2 )
    {
      equal = std::equal( (*it1).begin(), (*it1).end(), (*it2).begin() );
      if ( !equal )
      {
        break;
      }
      else
      {
        ++it1;
      }
    }

    if ( equal )
    {
      for ( size_t i = 0 ; i < selectedSet.size(); ++i ) // O( selected.size() )
      {
        equal = whichSelection.selectedSet[ i ].size() == selectedSet[ i ].size();
        if ( !equal )
        {
          break;
        }
      }

      if ( equal ) // O( selected.size() * num_elems )
      {
        for ( size_t i = 0 ; i < selectedSet.size(); ++i )
        {
          equal = std::equal( selectedSet[ i ].begin(), selectedSet[ i ].end(), whichSelection.selectedSet[ i ].begin() );
          if ( !equal )
          {
            break;
          }
        }
      }
    }
  }

  return equal;
}


template < typename SelType, typename LevelType >
void SelectionManagement< SelType, LevelType >::setSelected( std::vector< bool > &selection, LevelType level )
{
  selectedSet[ static_cast<size_t>( level ) ].clear();
  if ( selected[ static_cast<size_t>( level ) ].size() >= selection.size() )
  {
    std::copy( selection.begin(), selection.end(), selected[ static_cast<size_t>( level ) ].begin() );
  }
  else
  {
    size_t size = selected[ static_cast<size_t>( level ) ].size();
    if ( size > 0 )
      selected[ static_cast<size_t>( level ) ].resize( size );
    selected[ static_cast<size_t>( level ) ] = selection;
  }

  if ( !selected[ static_cast<size_t>( level ) ].empty() )
  {
    for ( size_t current = 0; current < selected[ static_cast<size_t>( level ) ].size(); ++current )
    {
      if ( selected[ static_cast<size_t>( level ) ][ current ] )
        selectedSet[ static_cast<size_t>( level ) ].push_back( current );
    }
  }
}


template < typename SelType, typename LevelType >
void SelectionManagement< SelType, LevelType >::setSelected( std::vector< SelType > &selection,
    SelType maxElems,
    LevelType level )
{
  // Prepare vectors for update
  selected[ static_cast<size_t>( level ) ].clear();
  selectedSet[ static_cast<size_t>( level ) ] = selection;

  typename std::vector<SelType>::iterator it;

  // Delete any SelType greater than maxElems ( number of level objects)
  it = std::find_if( selectedSet[ static_cast<size_t>( level ) ].begin(),
                     selectedSet[ static_cast<size_t>( level ) ].end(),
                     std::bind2nd( std::greater_equal<SelType>(), maxElems ) );
  if ( it != selectedSet[ static_cast<size_t>( level ) ].end() )
    selectedSet[ static_cast<size_t>( level ) ].erase( it, selectedSet[ static_cast<size_t>( level ) ].end() );

  // Any reamining row?
  if ( !selectedSet[ static_cast<size_t>( level ) ].empty() )
  {
    it = selectedSet[ static_cast<size_t>( level ) ].begin();
    for ( size_t current = 0; current < ( size_t ) maxElems; ++current )
    {
      if ( it != selectedSet[ static_cast<size_t>( level ) ].end() && current == ( size_t )*it )
      {
        selected[ static_cast<size_t>( level ) ].push_back( true );
        ++it;
      }
      else
      {
        selected[ static_cast<size_t>( level ) ].push_back( false );
      }
    }
  }
}


template < typename SelType, typename LevelType >
bool SelectionManagement< SelType, LevelType >::isSelectedPosition( SelType whichSelected,
    LevelType level ) const
{
  return selected[ static_cast<size_t>( level ) ][ whichSelected ];
}


template < typename SelType, typename LevelType >
void SelectionManagement< SelType, LevelType >::getSelected( std::vector< bool > &whichSelected,
    LevelType level ) const
{
  whichSelected = selected[ static_cast<size_t>( level ) ];
}


template < typename SelType, typename LevelType >
void SelectionManagement< SelType, LevelType >::getSelected( std::vector< bool > &whichSelected,
    SelType first,
    SelType last,
    LevelType level ) const
{
  whichSelected.clear();

  for ( SelType i = first; i <= last; ++ i )
    whichSelected.push_back( ( selected[ static_cast<size_t>( level ) ] )[ i ] );
}

template < typename SelType, typename LevelType >
void SelectionManagement< SelType, LevelType >::getSelected( std::vector< SelType > &whichSelected,
    LevelType level ) const
{
  whichSelected = selectedSet[ static_cast<size_t>( level ) ];
}


template < typename SelType, typename LevelType >
void SelectionManagement< SelType, LevelType >::getSelected( std::vector< SelType > &whichSelected,
    SelType first,
    SelType last,
    LevelType level ) const
{
  whichSelected.clear();
  typename std::vector< SelType >::const_iterator it;
  for ( it = selectedSet[ static_cast<size_t>( level ) ].begin(); it != selectedSet[ static_cast<size_t>( level ) ].end(); ++it )
  {
    if ( ( *it >= first ) && ( *it <= last ) )
      whichSelected.push_back( *it );
    if ( *it == last )
      break;
  }
}


template < typename SelType, typename LevelType >
SelType SelectionManagement< SelType, LevelType >::shiftFirst( SelType whichFirst, PRV_INT64 shiftAmount, PRV_INT64& appliedAmount, LevelType level ) const
{
  const typename std::vector<SelType>& tmpSelectedSet = selectedSet[ static_cast<size_t>( level ) ];
  const std::vector<bool>& tmpSelected = selected[ static_cast<size_t>( level ) ];

  if( whichFirst >= tmpSelected.size() )
    throw ParaverKernelException( TErrorCode::indexOutOfRange );

  SelType iFirst = 0;
  if( tmpSelected.size() == tmpSelectedSet.size() )
  {
    iFirst = whichFirst;
  }
  else
  {
    while( whichFirst > tmpSelectedSet[ iFirst ] )
    {
      ++iFirst;
      if( iFirst >= tmpSelectedSet.size() )
      {
        iFirst = tmpSelectedSet.size() - 1;
        break;
      }
    }
  }

  if( (PRV_INT64)iFirst + shiftAmount < 0 )
  {
    appliedAmount = -(PRV_INT64)iFirst;
    return tmpSelectedSet[ 0 ];
  }
  else if( (PRV_INT64)iFirst + shiftAmount >= tmpSelectedSet.size() )
  {
    appliedAmount = (PRV_INT64)tmpSelectedSet.size() - 1 - (PRV_INT64)iFirst;
    return tmpSelectedSet[ tmpSelectedSet.size() - 1 ];
  }

  appliedAmount = shiftAmount;
  return tmpSelectedSet[ iFirst + shiftAmount ];
}


template < typename SelType, typename LevelType >
SelType SelectionManagement< SelType, LevelType >::shiftLast( SelType whichLast, PRV_INT64 shiftAmount, PRV_INT64& appliedAmount, LevelType level ) const
{
  const typename std::vector<SelType>& tmpSelectedSet = selectedSet[ static_cast<size_t>( level ) ];
  const std::vector<bool>& tmpSelected = selected[ static_cast<size_t>( level ) ];

  if( whichLast >= tmpSelected.size() )
    throw ParaverKernelException( TErrorCode::indexOutOfRange );

  SelType iLast = tmpSelectedSet.size() - 1;
  if( tmpSelected.size() == tmpSelectedSet.size() )
  {
    iLast = whichLast;
  }
  else
  {
    while( whichLast < tmpSelectedSet[ iLast ] )
    {
      --iLast;
      if( iLast == 0 )
      {
        break;
      }
    }
  }

  if( (PRV_INT64)iLast + shiftAmount < 0 )
  {
    appliedAmount = -(PRV_INT64)iLast;
    return tmpSelectedSet[ 0 ];
  }
  else if( (PRV_INT64)iLast + shiftAmount >= tmpSelectedSet.size() )
  {
    appliedAmount = (PRV_INT64)tmpSelectedSet.size() - 1 - (PRV_INT64)iLast;
    return tmpSelectedSet[ tmpSelectedSet.size() - 1 ];
  }

  appliedAmount = shiftAmount;
  return tmpSelectedSet[ iLast + shiftAmount ];
}
