#include "math.h"
#include "histogram.h"
#include "histogramstatistic.h"
#include "histogramexception.h"
#include "histogramtotals.h"


RowsTranslator::RowsTranslator( vector<KWindow *>& kwindows )
{
  for ( size_t ii = 0; ii < kwindows.size() - 1; ii++ )
  {
    childInfo.push_back( RowChildInfo() );
    childInfo[ii].oneToOne = ( kwindows[ii]->getWindowLevelObjects() ==
                               kwindows[ii + 1]->getWindowLevelObjects() );
    if ( !childInfo[ii].oneToOne )
    {
      Trace *auxTrace = kwindows[ii]->getTrace();
      for ( TObjectOrder iRow = 0; iRow < kwindows[ii]->getWindowLevelObjects(); iRow++ )
      {
        pair< TObjectOrder, TObjectOrder > range;

        range.first = auxTrace->getFirst( iRow,
                                          kwindows[ii]->getLevel(),
                                          kwindows[ii+1]->getLevel() );
        range.second =  auxTrace->getLast( iRow,
                                           kwindows[ii]->getLevel(),
                                           kwindows[ii+1]->getLevel() );
        childInfo[ii].rowChilds.push_back( range );
      }
    }
  }
}


RowsTranslator::~RowsTranslator()
{}


TObjectOrder RowsTranslator::globalTranslate( UINT16 winIndex,
    TObjectOrder rowIndex ) const
{
  // This method will translate Kwindow rows to 2D rows.
  return rowIndex;
}


void RowsTranslator::getRowChilds( UINT16 winIndex,
                                   TObjectOrder rowIndex,
                                   TObjectOrder& iniRow,
                                   TObjectOrder& endRow ) const
{
  if ( childInfo[winIndex].oneToOne )
  {
    iniRow = rowIndex;
    endRow = rowIndex;
  }
  else
  {
    iniRow = childInfo[ winIndex ].rowChilds[ rowIndex ].first;
    endRow = childInfo[ winIndex ].rowChilds[ rowIndex ].second;
  }
}

//
TObjectOrder RowsTranslator::totalRows() const
{
  return childInfo[ 0 ].rowChilds.size() - 1;
}


ColumnTranslator::ColumnTranslator( THistogramLimit whichMin,
                                    THistogramLimit whichMax,
                                    THistogramLimit whichDelta ):
    minLimit( whichMin ), maxLimit( whichMax ), delta( whichDelta )
{
  // PRECOND: Min < Max
  numColumns = THistogramColumn( ceil( ( maxLimit - minLimit ) / delta ) );
}


ColumnTranslator::~ColumnTranslator()
{}

// returns whichValue in [min,max)
bool ColumnTranslator::getColumn( THistogramLimit whichValue,
                                  THistogramColumn& column ) const
{
  if ( whichValue < minLimit || whichValue > maxLimit )
    return false;

  column = THistogramColumn( floor( ( whichValue * numColumns ) / ( maxLimit - minLimit ) ) );

  if ( column >= numColumns )
    column = numColumns - 1;

  return true;
}


THistogramColumn ColumnTranslator::totalColumns() const
{
  return numColumns;
}


Histogram::Histogram()
{
  controlWindow = NULL;
  dataWindow = NULL;
  xtraControlWindow = NULL;

  threeDimensions = false;

  controlMin = 0;
  controlMax = 1;
  controlDelta = 1;
  xtraControlMin = 0;
  xtraControlMax = 1;
  xtraControlDelta = 1;
  dataMin = 0;
  dataMax = 1;

  inclusive = false;

  rowsTranslator = NULL;
  columnTranslator = NULL;
  planeTranslator = NULL;

  cube = NULL;
  matrix = NULL;
  commCube = NULL;
  commMatrix = NULL;

  totals = NULL;
  rowTotals = NULL,
              commTotals = NULL;
  rowCommTotals = NULL;
}


Histogram::~Histogram()
{
  if ( rowsTranslator != NULL )
    delete rowsTranslator;
  if ( columnTranslator != NULL )
    delete columnTranslator;
  if ( planeTranslator != NULL )
    delete planeTranslator;

  if ( cube != NULL )
    delete cube;
  if ( matrix != NULL )
    delete matrix;
  if ( commCube != NULL )
    delete commCube;
  if ( commMatrix != NULL )
    delete commMatrix;

  if ( totals != NULL )
    delete totals;
  if ( rowTotals != NULL )
    delete rowTotals;
  if ( commTotals != NULL )
    delete commTotals;
  if ( rowCommTotals != NULL )
    delete rowCommTotals;

  clearStatistics();
}


bool Histogram::getThreeDimensions() const
{
  return threeDimensions;
}


TRecordTime Histogram::getBeginTime() const
{
  return beginTime;
}


TRecordTime Histogram::getEndTime() const
{
  return endTime;
}


KWindow *Histogram::getControlWindow() const
{
  return controlWindow;
}


KWindow *Histogram::getDataWindow() const
{
  return dataWindow;
}


KWindow *Histogram::getExtraControlWindow() const
{
  return xtraControlWindow;
}


void Histogram::setControlWindow( KWindow *whichWindow )
{
  controlWindow = whichWindow;
}


void Histogram::setDataWindow( KWindow *whichWindow )
{
  dataWindow = whichWindow;
}


void Histogram::setExtraControlWindow( KWindow *whichWindow )
{
  xtraControlWindow = whichWindow;
}


void Histogram::clearControlWindow()
{
  controlWindow = NULL;
}


void Histogram::clearDataWindow()
{
  dataWindow = NULL;
}


void Histogram::clearExtraControlWindow()
{
  xtraControlWindow = NULL;
}


void Histogram::setControlMin( THistogramLimit whichMin )
{
  controlMin = whichMin;
}


void Histogram::setControlMax( THistogramLimit whichMax )
{
  controlMax = whichMax;
}


void Histogram::setControlDelta( THistogramLimit whichDelta )
{
  controlDelta = whichDelta;
}


void Histogram::setExtraControlMin( THistogramLimit whichMin )
{
  xtraControlMin = whichMin;
}


void Histogram::setExtraControlMax( THistogramLimit whichMax )
{
  xtraControlMax = whichMax;
}


void Histogram::setExtraControlDelta( THistogramLimit whichDelta )
{
  xtraControlDelta = whichDelta;
}


void Histogram::setDataMin( THistogramLimit whichMin )
{
  dataMin = whichMin;
}


void Histogram::setDataMax( THistogramLimit whichMax )
{
  dataMax = whichMax;
}


THistogramLimit Histogram::getControlMin() const
{
  return controlMin;
}


THistogramLimit Histogram::getControlMax() const
{
  return controlMax;
}


THistogramLimit Histogram::getControlDelta() const
{
  return controlDelta;
}


THistogramLimit Histogram::getExtraControlMin() const
{
  return xtraControlMin;
}


THistogramLimit Histogram::getExtraControlMax() const
{
  return xtraControlMax;
}


THistogramLimit Histogram::getExtraControlDelta() const
{
  return xtraControlDelta;
}


THistogramLimit Histogram::getDataMin() const
{
  return dataMin;
}


THistogramLimit Histogram::getDataMax() const
{
  return dataMax;
}


void Histogram::setInclusive( bool newValue )
{
  inclusive = newValue;
}


THistogramColumn Histogram::getNumPlanes() const
{
  if ( threeDimensions )
    return planeTranslator->totalColumns();
  return 1;
}


THistogramColumn Histogram::getNumColumns() const
{
  return numCols;
}


TObjectOrder Histogram::getNumRows() const
{
  return numRows;
}


TSemanticValue Histogram::getCurrentValue( UINT32 col,
    UINT16 idStat,
    UINT32 plane ) const
{
  if ( threeDimensions )
    return cube->getCurrentValue( plane, col, idStat );
  else
    return matrix->getCurrentValue( col, idStat );

  return TSemanticValue( 0 );
}

UINT32 Histogram::getCurrentRow( UINT32 col, UINT32 plane ) const
{
  if ( threeDimensions )
    return cube->getCurrentRow( plane, col );
  else
    return matrix->getCurrentRow( col );

  return 0;
}

void Histogram::setNextCell( UINT32 col, UINT32 plane )
{
  if ( threeDimensions )
    cube->setNextCell( plane, col );
  else
    matrix->setNextCell( col );
}

void Histogram::setFirstCell( UINT32 col, UINT32 plane )
{
  if ( threeDimensions )
    cube->setFirstCell( plane, col );
  else
    matrix->setFirstCell( col );
}

bool Histogram::endCell( UINT32 col, UINT32 plane )
{
  if ( threeDimensions )
    return cube->endCell( plane, col );
  else
    return matrix->endCell( col );

  return true;
}

bool Histogram::planeWithValues( UINT32 plane ) const
{
  if ( threeDimensions )
    return cube->planeWithValues( plane );

  return true;
}


TSemanticValue Histogram::getCommCurrentValue( UINT32 col,
    UINT16 idStat,
    UINT32 plane ) const
{
  if ( threeDimensions )
    return commCube->getCurrentValue( plane, col, idStat );
  else
    return commMatrix->getCurrentValue( col, idStat );

  return TSemanticValue( 0 );
}

UINT32 Histogram::getCommCurrentRow( UINT32 col, UINT32 plane ) const
{
  if ( threeDimensions )
    return commCube->getCurrentRow( plane, col );
  else
    return commMatrix->getCurrentRow( col );

  return 0;
}

void Histogram::setCommNextCell( UINT32 col, UINT32 plane )
{
  if ( threeDimensions )
    commCube->setNextCell( plane, col );
  else
    commMatrix->setNextCell( col );
}

void Histogram::setCommFirstCell( UINT32 col, UINT32 plane )
{
  if ( threeDimensions )
    commCube->setFirstCell( plane, col );
  else
    commMatrix->setFirstCell( col );
}

bool Histogram::endCommCell( UINT32 col, UINT32 plane )
{
  if ( threeDimensions )
    return commCube->endCell( plane, col );
  else
    return commMatrix->endCell( col );

  return true;
}

bool Histogram::planeCommWithValues( UINT32 plane ) const
{
  if ( threeDimensions )
    return commCube->planeWithValues( plane );

  return true;
}


HistogramTotals *Histogram::getColumnTotals() const
{
  return totals;
}


HistogramTotals *Histogram::getCommColumnTotals() const
{
  return commTotals;
}


HistogramTotals *Histogram::getRowTotals() const
{
  return rowTotals;
}


HistogramTotals *Histogram::getCommRowTotals() const
{
  return rowCommTotals;
}


void Histogram::clearStatistics()
{
  vector<HistogramStatistic *>::iterator it = statisticFunctions.begin();

  while ( it != statisticFunctions.end() )
  {
    delete *it;
    it++;
  }
  statisticFunctions.clear();

  it = commStatisticFunctions.begin();

  while ( it != commStatisticFunctions.end() )
  {
    delete *it;
    it++;
  }
}


void Histogram::pushbackStatistic( HistogramStatistic *whichStatistic )
{
  if ( whichStatistic->createComms() )
    commStatisticFunctions.push_back( whichStatistic );
  else
    statisticFunctions.push_back( whichStatistic );
}


void Histogram::execute( TRecordTime whichBeginTime, TRecordTime whichEndTime )
{
  if ( controlWindow == NULL )
    throw HistogramException( HistogramException::noControlWindow );

  if ( dataWindow == NULL )
    dataWindow = controlWindow;

  beginTime = whichBeginTime;
  endTime = whichEndTime;

  threeDimensions = ( xtraControlWindow != NULL );

  orderWindows();

  initTranslators();

  numRows = rowsTranslator->totalRows();
  numCols = columnTranslator->totalColumns();
  if ( threeDimensions )
    numPlanes = planeTranslator->totalColumns();

  initMatrix( numPlanes, numCols, numRows );

  initSemantic( beginTime );

  initStatistics();

  initTotals();

  recursiveExecution( beginTime, endTime, 0, rowsTranslator->totalRows() );

  if ( threeDimensions )
  {
    cube->finish();
    if ( createComms() )
      commCube->finish();
  }
  else
  {
    matrix->finish();
    if ( createComms() )
      commMatrix->finish();
  }

  if ( totals != NULL )
    totals->finish();
  if ( rowTotals != NULL )
    rowTotals->finish();
  if ( commTotals != NULL )
    commTotals->finish();
  if ( rowCommTotals != NULL )
    rowCommTotals->finish();
  // - Se ordenan las columnas si es necesario.
}


void Histogram::orderWindows()
{
  orderedWindows.clear();

  if ( controlWindow->getLevel() >= xtraControlWindow->getLevel() )
  {
    orderedWindows.push_back( controlWindow );
    orderedWindows.push_back( xtraControlWindow );
  }
  else
  {
    orderedWindows.push_back( xtraControlWindow );
    orderedWindows.push_back( controlWindow );
  }

  orderedWindows.push_back( dataWindow );
}


bool Histogram::createComms() const
{
  return commStatisticFunctions.size() > 0;
}


void Histogram::initTranslators()
{
  if ( rowsTranslator != NULL )
    delete rowsTranslator;
  rowsTranslator = new RowsTranslator( orderedWindows );

  if ( columnTranslator != NULL )
    delete columnTranslator;
  columnTranslator = new ColumnTranslator( controlMin, controlMax, controlDelta );

  if ( planeTranslator != NULL )
  {
    delete planeTranslator;
    planeTranslator = NULL;
  }
  if ( threeDimensions )
    planeTranslator = new ColumnTranslator( xtraControlMin, xtraControlMax,
                                            xtraControlDelta );
}


void Histogram::initMatrix( THistogramColumn planes, THistogramColumn cols,
                            TObjectOrder rows )
{
  if ( cube != NULL )
  {
    delete cube;
    cube = NULL;
  }
  if ( matrix != NULL )
  {
    delete matrix;
    matrix = NULL;
  }
  if ( commCube != NULL )
  {
    delete commCube;
    commCube = NULL;
  }
  if ( commMatrix != NULL )
  {
    delete commMatrix;
    commMatrix = NULL;
  }

  if ( threeDimensions )
  {
    cube = new Cube<TSemanticValue>( planes, cols, statisticFunctions.size() );
    if ( createComms() )
      commCube = new Cube<TSemanticValue>( planes, rows, commStatisticFunctions.size() );
  }
  else
  {
    matrix = new Matrix<TSemanticValue>( cols, statisticFunctions.size() );
    if ( createComms() )
      commMatrix = new Matrix<TSemanticValue>( rows, commStatisticFunctions.size() );
  }
}


void Histogram::initTotals()
{
  if ( totals != NULL )
    delete totals;
  if ( rowTotals != NULL )
    delete rowTotals;
  if ( commTotals != NULL )
    delete commTotals;
  if ( rowCommTotals != NULL )
    delete rowCommTotals;

  if ( threeDimensions )
  {
    totals = new HistogramTotals( statisticFunctions.size(), numCols, numPlanes );
    rowTotals = new HistogramTotals( statisticFunctions.size(), numRows, numPlanes );
    if ( createComms() )
    {
      commTotals = new HistogramTotals( commStatisticFunctions.size(),
                                        numRows, numPlanes );
      rowCommTotals = new HistogramTotals( commStatisticFunctions.size(),
                                           numRows, numPlanes );
    }
  }
  else
  {
    totals = new HistogramTotals( statisticFunctions.size(), numCols, 1 );
    rowTotals = new HistogramTotals( statisticFunctions.size(), numRows, 1 );
    if ( createComms() )
    {
      commTotals = new HistogramTotals( commStatisticFunctions.size(),
                                        numRows, 1 );
      rowCommTotals = new HistogramTotals( commStatisticFunctions.size(),
                                           numRows, 1 );
    }
  }
}


void Histogram::initSemantic( TRecordTime beginTime )
{
  TCreateList create = NOCREATE;

  if ( createComms() )
    create = CREATECOMMS;

  controlWindow->init( beginTime, create );

  if ( xtraControlWindow != controlWindow )
    xtraControlWindow->init( beginTime, NOCREATE );

  if ( dataWindow != controlWindow && dataWindow != xtraControlWindow )
    dataWindow->init( beginTime, NOCREATE );
}


void Histogram::initStatistics()
{
  vector<HistogramStatistic *>::iterator it = statisticFunctions.begin();

  while ( it != statisticFunctions.end() )
  {
    ( *it )->init( this );
  }

  it = commStatisticFunctions.begin();

  while ( it != commStatisticFunctions.end() )
  {
    ( *it )->init( this );
  }
}


void Histogram::recursiveExecution( TRecordTime fromTime, TRecordTime toTime,
                                    TObjectOrder fromRow, TObjectOrder toRow,
                                    UINT16 winIndex, CalculateData *data )
{
  KWindow *currentWindow = orderedWindows[ winIndex ];

  if ( data == NULL )
    data = new CalculateData;

  for ( TObjectOrder iRow = fromRow; iRow <= toRow; iRow++ )
  {
    if ( winIndex == 0 )
      data->row = rowsTranslator->globalTranslate( winIndex, iRow );
    if ( currentWindow == controlWindow )
      data->controlRow = iRow;
    if ( currentWindow == dataWindow )
      data->dataRow = iRow;

    while ( currentWindow->getEndTime( iRow ) <= fromTime )
      currentWindow->calcNext( iRow );

    while ( currentWindow->getEndTime( iRow ) < toTime )
    {
      calculate( iRow, fromTime, toTime, fromRow, toRow, winIndex, data );
      currentWindow->calcNext( iRow );
    }

    if ( currentWindow->getBeginTime( iRow ) < toTime )
      calculate( iRow, fromTime, toTime, fromRow, toRow, winIndex, data );

    if ( winIndex == 0 )
      finishRow( data );
  }

  delete data;
}


void Histogram::calculate( TObjectOrder iRow,
                           TRecordTime fromTime, TRecordTime toTime,
                           TObjectOrder fromRow, TObjectOrder toRow,
                           UINT16 winIndex, CalculateData *data )
{
  TObjectOrder childFromRow;
  TObjectOrder childToRow;
  TRecordTime childFromTime;
  TRecordTime childToTime;
  KWindow *currentWindow = orderedWindows[ winIndex ];

  if ( currentWindow == controlWindow )
  {
    if ( !columnTranslator->getColumn( controlWindow->getValue( iRow ),
                                       data->column ) )
      return;
    data->rList = controlWindow->getRecordList( iRow );
  }
  if ( threeDimensions && currentWindow == xtraControlWindow )
    if ( !planeTranslator->getColumn( xtraControlWindow->getValue( iRow ),
                                      data->plane ) )
      return;

  if ( winIndex == orderedWindows.size() - 1 )
  {
    TSemanticValue value;

    data->beginTime = fromTime;
    data->endTime = toTime;

    // Communication statistics
    RecordList::iterator itComm = data->rList->begin();
    while ( itComm != data->rList->end() &&
            ( *itComm )->getTime() >= fromTime &&
            ( *itComm )->getTime() <= toTime )
    {
      data->comm = *itComm;
      for ( UINT16 iStat = 0; iStat < commStatisticFunctions.size(); iStat++ )
      {
        value = commStatisticFunctions[ iStat ]->execute( data );
        if ( value != 0.0 )
        {
          TObjectOrder column = commStatisticFunctions[ iStat ]->getPartner( data );
          if ( threeDimensions )
            commCube->addValue( data->plane, column, iStat, value );
          else
            commMatrix->addValue( column, iStat, value );
        }
      }

      delete *itComm;
      itComm++;
    }

    // Semantic statistics
    for ( UINT16 iStat = 0; iStat < statisticFunctions.size(); iStat++ )
    {
      value = statisticFunctions[ iStat ]->execute( data );

      if ( threeDimensions )
        cube->addValue( data->plane, data->column, iStat, value );
      else
        matrix->addValue( data->column, iStat, value );
    }
  }
  else
  {
    childFromTime = ( fromTime < currentWindow->getBeginTime( iRow ) ) ?
                    currentWindow->getBeginTime( iRow ) :
                    fromTime;
    childToTime = ( toTime > currentWindow->getEndTime( iRow ) ) ?
                  currentWindow->getEndTime( iRow ) :
                  toTime;
    rowsTranslator->getRowChilds( winIndex, iRow, childFromRow, childToRow );

    recursiveExecution( childFromTime, childToTime, childFromRow, childToRow,
                        winIndex + 1, data );
  }
}


void Histogram::finishRow( CalculateData *data )
{
  // Communication statistics
  if ( threeDimensions )
  {
    for ( THistogramColumn iPlane = 0; iPlane < planeTranslator->totalColumns();
          iPlane++ )
    {
      if ( commCube->planeWithValues( iPlane ) )
      {
        for ( TObjectOrder iColumn = 0;
              iColumn < rowsTranslator->totalRows(); iColumn++ )
        {
          if ( commCube->currentCellModified( iPlane, iColumn ) )
          {
            TSemanticValue value;
            for ( UINT16 iStat = 0; iStat < commStatisticFunctions.size(); iStat++ )
            {
              value = commCube->getCurrentValue( iPlane, iColumn, iStat );
              value = commStatisticFunctions[ iStat ]->finishRow( value, iColumn, iPlane );
              commCube->setValue( iPlane, iColumn, iStat, value );
              commTotals->newValue( value, iStat, iColumn, iPlane );
              rowCommTotals->newValue( value, iStat, data->row, iPlane );
            }
          }
        }
      }
    }
  }
  else
  {
    for ( TObjectOrder iColumn = 0;
          iColumn < rowsTranslator->totalRows(); iColumn++ )
    {
      if ( commMatrix->currentCellModified( iColumn ) )
      {
        TSemanticValue value;
        for ( UINT16 iStat = 0; iStat < commStatisticFunctions.size(); iStat++ )
        {
          value = commMatrix->getCurrentValue( iColumn, iStat );
          value = commStatisticFunctions[ iStat ]->finishRow( value, iColumn );
          commMatrix->setValue( iColumn, iStat, value );
          commTotals->newValue( value, iStat, iColumn );
          rowCommTotals->newValue( value, iStat, data->row );
        }
      }
    }
  }

  for ( UINT16 iStat = 0; iStat < commStatisticFunctions.size(); iStat++ )
    commStatisticFunctions[ iStat ]->reset();

  // Semantic statistics
  if ( threeDimensions )
  {
    for ( THistogramColumn iPlane = 0; iPlane < planeTranslator->totalColumns();
          iPlane++ )
    {
      if ( cube->planeWithValues( iPlane ) )
      {
        for ( THistogramColumn iColumn = 0;
              iColumn < columnTranslator->totalColumns(); iColumn++ )
        {
          if ( cube->currentCellModified( iPlane, iColumn ) )
          {
            TSemanticValue value;
            for ( UINT16 iStat = 0; iStat < statisticFunctions.size(); iStat++ )
            {
              value = cube->getCurrentValue( iPlane, iColumn, iStat );
              value = statisticFunctions[ iStat ]->finishRow( value, iColumn, iPlane );
              cube->setValue( iPlane, iColumn, iStat, value );
              totals->newValue( value, iStat, iColumn, iPlane );
              rowTotals->newValue( value, iStat, data->row, iPlane );
            }
          }
        }
      }
    }
  }
  else
  {
    for ( THistogramColumn iColumn = 0;
          iColumn < columnTranslator->totalColumns(); iColumn++ )
    {
      if ( matrix->currentCellModified( iColumn ) )
      {
        TSemanticValue value;
        for ( UINT16 iStat = 0; iStat < statisticFunctions.size(); iStat++ )
        {
          value = matrix->getCurrentValue( iColumn, iStat );
          value = statisticFunctions[ iStat ]->finishRow( value, iColumn );
          matrix->setValue( iColumn, iStat, value );
          totals->newValue( value, iStat, iColumn );
          rowTotals->newValue( value, iStat, data->row );
        }
      }
    }
  }

  for ( UINT16 iStat = 0; iStat < statisticFunctions.size(); iStat++ )
    statisticFunctions[ iStat ]->reset();

  // Next row
  if ( createComms() )
    if ( threeDimensions )
      commCube->newRow();
    else
      commMatrix->newRow();

  if ( threeDimensions )
    cube->newRow();
  else
    matrix->newRow();
}