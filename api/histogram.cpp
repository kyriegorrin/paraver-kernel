#include <string>
#include "kernelconnection.h"
#include "histogram.h"
#include "window.h"
#include "histogramtotals.h"
#include "paraverconfig.h"
#include "labelconstructor.h"

Histogram *Histogram::create( KernelConnection *whichKernel )
{
  return new HistogramProxy( whichKernel );
}


Histogram::Histogram( KernelConnection *whichKernel ) : myKernel( whichKernel )
{}

void Histogram::setWindowBeginTime( TRecordTime whichTime )
{}

void Histogram::setWindowEndTime( TRecordTime whichTime )
{}

HistogramProxy::HistogramProxy( KernelConnection *whichKernel ):
    Histogram( whichKernel )
{
  controlWindow = NULL;
  dataWindow = NULL;
  extraControlWindow = NULL;
  myHisto = myKernel->newHistogram();

  horizontal = Histogram::getHorizontal();
  hideColumns = Histogram::getHideColumns();
  showUnits = Histogram::getShowUnits();
  sortColumns = Histogram::getSortColumns();
  sortCriteria = Histogram::getSortCriteria();
  minGradient = Histogram::getMinGradient();
  maxGradient = Histogram::getMaxGradient();
  computeScale = Histogram::getComputeScale();
  computeGradient = Histogram::getComputeGradient();
  futurePlane = false;
  planeMinValue = 0.0;
  selectedPlane = 0;
  commSelectedPlane = 0;
}

HistogramProxy::~HistogramProxy()
{
  delete myHisto;
}

void HistogramProxy::setWindowBeginTime( TRecordTime whichTime )
{
  winBeginTime = whichTime;
}

void HistogramProxy::setWindowEndTime( TRecordTime whichTime )
{
  winEndTime = whichTime;
}

bool HistogramProxy::getThreeDimensions() const
{
  return myHisto->getThreeDimensions();
}

TRecordTime HistogramProxy::getBeginTime() const
{
  return winBeginTime;
}

TRecordTime HistogramProxy::getEndTime() const
{
  return winEndTime;
}

Window *HistogramProxy::getControlWindow() const
{
  return controlWindow;
}

Window *HistogramProxy::getDataWindow() const
{
  return dataWindow;
}

Window *HistogramProxy::getExtraControlWindow() const
{
  return extraControlWindow;
}

void HistogramProxy::setControlWindow( Window *whichWindow )
{
  controlWindow = whichWindow;
  myHisto->setControlWindow( whichWindow->getConcrete() );
}

void HistogramProxy::setDataWindow( Window *whichWindow )
{
  dataWindow = whichWindow;
  myHisto->setDataWindow( whichWindow->getConcrete() );
}

void HistogramProxy::setExtraControlWindow( Window *whichWindow )
{
  extraControlWindow = whichWindow;
  myHisto->setExtraControlWindow( whichWindow->getConcrete() );
}

void HistogramProxy::clearControlWindow()
{
  controlWindow = NULL;
  myHisto->clearControlWindow();
}

void HistogramProxy::clearDataWindow()
{
  dataWindow = NULL;
  myHisto->clearDataWindow();
}

void HistogramProxy::clearExtraControlWindow()
{
  extraControlWindow = NULL;
  myHisto->clearExtraControlWindow();
}

void HistogramProxy::setControlMin( THistogramLimit whichMin )
{
  myHisto->setControlMin( whichMin );
}

void HistogramProxy::setControlMax( THistogramLimit whichMax )
{
  myHisto->setControlMax( whichMax );
}

void HistogramProxy::setControlDelta( THistogramLimit whichDelta )
{
  myHisto->setControlDelta( whichDelta );
}

void HistogramProxy::setExtraControlMin( THistogramLimit whichMin )
{
  myHisto->setExtraControlMin( whichMin );
}

void HistogramProxy::setExtraControlMax( THistogramLimit whichMax )
{
  myHisto->setExtraControlMax( whichMax );
}

void HistogramProxy::setExtraControlDelta( THistogramLimit whichDelta )
{
  myHisto->setExtraControlDelta( whichDelta );
}

void HistogramProxy::setDataMin( TSemanticValue whichMin )
{
  myHisto->setDataMin( whichMin );
}

void HistogramProxy::setDataMax( TSemanticValue whichMax )
{
  myHisto->setDataMax( whichMax );
}

void HistogramProxy::setBurstMin( TRecordTime whichTime )
{
  myHisto->setBurstMin( whichTime );
}

void HistogramProxy::setBurstMax( TRecordTime whichTime )
{
  myHisto->setBurstMax( whichTime );
}

void HistogramProxy::setCommSizeMin( TCommSize whichSize )
{
  myHisto->setCommSizeMin( whichSize );
}

void HistogramProxy::setCommSizeMax( TCommSize whichSize )
{
  myHisto->setCommSizeMax( whichSize );
}

void HistogramProxy::setCommTagMin( TCommTag whichTag )
{
  myHisto->setCommTagMin( whichTag );
}

void HistogramProxy::setCommTagMax( TCommTag whichTag )
{
  myHisto->setCommTagMax( whichTag );
}

THistogramLimit HistogramProxy::getControlMin() const
{
  return myHisto->getControlMin();
}

THistogramLimit HistogramProxy::getControlMax() const
{
  return myHisto->getControlMax();
}

THistogramLimit HistogramProxy::getControlDelta() const
{
  return myHisto->getControlDelta();
}

THistogramLimit HistogramProxy::getExtraControlMin() const
{
  return myHisto->getExtraControlMin();
}

THistogramLimit HistogramProxy::getExtraControlMax() const
{
  return myHisto->getExtraControlMax();
}

THistogramLimit HistogramProxy::getExtraControlDelta() const
{
  return myHisto->getExtraControlDelta();
}

TSemanticValue HistogramProxy::getDataMin() const
{
  return myHisto->getDataMin();
}

TSemanticValue HistogramProxy::getDataMax() const
{
  return myHisto->getDataMax();
}

TRecordTime HistogramProxy::getBurstMin() const
{
  return myHisto->getBurstMin();
}

TRecordTime HistogramProxy::getBurstMax() const
{
  return myHisto->getBurstMax();
}

TCommSize HistogramProxy::getCommSizeMin() const
{
  return myHisto->getCommSizeMin();
}

TCommSize HistogramProxy::getCommSizeMax() const
{
  return myHisto->getCommSizeMax();
}

TCommTag HistogramProxy::getCommTagMin() const
{
  return myHisto->getCommTagMin();
}

TCommTag HistogramProxy::getCommTagMax() const
{
  return myHisto->getCommTagMax();
}

void HistogramProxy::setInclusive( bool newValue )
{
  myHisto->setInclusive( newValue );
}

THistogramColumn HistogramProxy::getNumPlanes() const
{
  return myHisto->getNumPlanes();
}

THistogramColumn HistogramProxy::getNumColumns() const
{
  return myHisto->getNumColumns();
}

TObjectOrder HistogramProxy::getNumRows() const
{
  return myHisto->getNumRows();
}


TSemanticValue HistogramProxy::getCurrentValue( UINT32 col,
    UINT16 idStat,
    UINT32 plane ) const
{
  return myHisto->getCurrentValue( col, idStat, plane );
}

UINT32 HistogramProxy::getCurrentRow( UINT32 col, UINT32 plane ) const
{
  return myHisto->getCurrentRow( col, plane );
}

void HistogramProxy::setNextCell( UINT32 col, UINT32 plane )
{
  myHisto->setNextCell( col, plane );
}

void HistogramProxy::setFirstCell( UINT32 col, UINT32 plane )
{
  myHisto->setFirstCell( col, plane );
}

bool HistogramProxy::endCell( UINT32 col, UINT32 plane )
{
  return myHisto->endCell( col, plane );
}

bool HistogramProxy::planeWithValues( UINT32 plane ) const
{
  return myHisto->planeWithValues( plane );
}

TSemanticValue HistogramProxy::getCommCurrentValue( UINT32 col,
    UINT16 idStat,
    UINT32 plane ) const
{
  return myHisto->getCommCurrentValue( col, idStat, plane );
}

UINT32 HistogramProxy::getCommCurrentRow( UINT32 col, UINT32 plane ) const
{
  return myHisto->getCommCurrentRow( col, plane );
}

void HistogramProxy::setCommNextCell( UINT32 col, UINT32 plane )
{
  myHisto->setCommNextCell( col, plane );
}

void HistogramProxy::setCommFirstCell( UINT32 col, UINT32 plane )
{
  myHisto->setCommFirstCell( col, plane );
}

bool HistogramProxy::endCommCell( UINT32 col, UINT32 plane )
{
  return myHisto->endCommCell( col, plane );
}

bool HistogramProxy::planeCommWithValues( UINT32 plane ) const
{
  return myHisto->planeCommWithValues( plane );
}

HistogramTotals *HistogramProxy::getTotals( UINT16 idStat ) const
{
  if ( itsCommunicationStat( calcStat[ idStat ] ) )
  {
    if ( horizontal )
      return getCommColumnTotals();
    else
      return getCommRowTotals();
  }
  else
  {
    if ( horizontal )
      return getColumnTotals();
    else
      return getRowTotals();
  }
}

HistogramTotals *HistogramProxy::getColumnTotals() const
{
  return HistogramTotals::create( myHisto->getColumnTotals() );
}

HistogramTotals *HistogramProxy::getCommColumnTotals() const
{
  return HistogramTotals::create( myHisto->getCommColumnTotals() );
}

HistogramTotals *HistogramProxy::getRowTotals() const
{
  return HistogramTotals::create( myHisto->getRowTotals() );
}

HistogramTotals *HistogramProxy::getCommRowTotals() const
{
  return HistogramTotals::create( myHisto->getCommRowTotals() );
}

void HistogramProxy::clearStatistics()
{
  myHisto->clearStatistics();
  calcStat.clear();
}

void HistogramProxy::pushbackStatistic( string& whichStatistic )
{
  myHisto->pushbackStatistic( whichStatistic );
  calcStat.push_back( whichStatistic );
}

void HistogramProxy::execute( TRecordTime whichBeginTime, TRecordTime whichEndTime )
{
  winBeginTime = whichBeginTime;
  winEndTime = whichEndTime;

  if ( computeScale )
  {
    compute2DScale();
    if ( getThreeDimensions() )
      compute3DScale();
  }

  if ( computeGradient )
  {
    // Idem que para computeScale pero con la ventana de datos, aunque hace
    // falta una clase para hacer gradientes.
  }

  myHisto->execute( whichBeginTime, whichEndTime );

  if ( getThreeDimensions() && futurePlane )
  {
    THistogramLimit nPlanes = getNumPlanes();
    THistogramLimit min, max;
    UINT32 i;
    bool commFuturePlane = true;
    i = selectedPlane = commSelectedPlane = 0;

    while ( i < nPlanes )
    {
      min = getExtraControlMin() + getExtraControlDelta() * i;
      max = getExtraControlMin() + getExtraControlDelta() * i + 1;

      // get the first plane with values
      if ( futurePlane && planeWithValues( i ) )
      {
        selectedPlane = i;
        futurePlane = false;
      }

      if ( commFuturePlane && planeCommWithValues( i ) )
      {
        commSelectedPlane = i;
        commFuturePlane = false;
      }

      if ( planeMinValue >= min && planeMinValue < max )
      {
        if ( planeWithValues( i ) )
          selectedPlane = i;

        if ( planeCommWithValues( i ) )
          commSelectedPlane = i;
      }

      i++;
    }
    futurePlane = false;
  }
  else if ( getThreeDimensions() )
  {
    THistogramLimit nPlanes = getNumPlanes();
    UINT32 i;
    bool commFuturePlane = true;
    bool stop = false;
    i = selectedPlane = commSelectedPlane = 0;

    while ( i < nPlanes )
    {
      // get the first plane with values
      if ( futurePlane && planeWithValues( i ) )
      {
        selectedPlane = i;
        futurePlane = false;
        if ( stop ) break;
        else stop = true;
      }

      if ( commFuturePlane && planeCommWithValues( i ) )
      {
        commSelectedPlane = i;
        commFuturePlane = false;
        if ( stop ) break;
        else stop = true;
      }

      i++;
    }
  }
}

void HistogramProxy::setHorizontal( bool newValue )
{
  horizontal = newValue;
}

bool HistogramProxy::getHorizontal() const
{
  return horizontal;
}

void HistogramProxy::setHideColumns( bool newValue )
{
  hideColumns = newValue;
}

bool HistogramProxy::getHideColumns() const
{
  return hideColumns;
}

void HistogramProxy::setShowUnits( bool newValue )
{
  showUnits = newValue;
}

bool HistogramProxy::getShowUnits() const
{
  return showUnits;
}

void HistogramProxy::setSortColumns( bool newValue )
{
  sortColumns = newValue;
}

bool HistogramProxy::getSortColumns() const
{
  return sortColumns;
}

void HistogramProxy::setSortCriteria( THistoTotals whichCriteria )
{
  sortCriteria = whichCriteria;
}

THistoTotals HistogramProxy::getSortCriteria() const
{
  return sortCriteria;
}

void HistogramProxy::setMinGradient( double whichMin )
{
  minGradient = whichMin;
}

double HistogramProxy::getMinGradient() const
{
  return minGradient;
}

void HistogramProxy::setMaxGradient( double whichMax )
{
  maxGradient = whichMax;
}

double HistogramProxy::getMaxGradient() const
{
  return maxGradient;
}

void HistogramProxy::setComputeScale( bool newValue )
{
  computeScale = newValue;
}

bool HistogramProxy::getComputeScale() const
{
  return computeScale;
}

void HistogramProxy::setComputeGradient( bool newValue )
{
  computeGradient = newValue;
}

bool HistogramProxy::getComputeGradient() const
{
  return computeGradient;
}

void HistogramProxy::setPlaneMinValue( double whichMin )
{
  planeMinValue = whichMin;
  futurePlane = true;
}

INT32 HistogramProxy::getSelectedPlane() const
{
  return selectedPlane;
}

INT32 HistogramProxy::getCommSelectedPlane() const
{
  return commSelectedPlane;
}

bool HistogramProxy::itsCommunicationStat( const string& whichStat ) const
{
  return myHisto->itsCommunicationStat( whichStat );
}

void HistogramProxy::compute2DScale()
{
  TSemanticValue minY = controlWindow->getMinimumY();
  TSemanticValue maxY = controlWindow->getMaximumY();

  setControlMin( minY );
  setControlMax( maxY );

  if ( ( maxY - minY ) == 0 )
  {
    setControlDelta( 1.0 );
  }
  else if ( ( maxY - minY ) < 1.0 )
  {
    setControlDelta( ( maxY - minY ) /
                     ParaverConfig::getInstance()->getHistoNumColumns() );
  }
  else if ( ( maxY - minY ) <
            ParaverConfig::getInstance()->getHistoNumColumns() )
  {
    setControlDelta( 1.0 );
  }
  else
  {
    setControlDelta( ( maxY - minY ) /
                     ParaverConfig::getInstance()->getHistoNumColumns() );
  }
}

void HistogramProxy::compute3DScale()
{
  TSemanticValue minY = extraControlWindow->getMinimumY();
  TSemanticValue maxY = extraControlWindow->getMaximumY();

  setExtraControlMin( minY );
  setExtraControlMax( maxY );

  if ( ( maxY - minY ) == 0 )
  {
    setExtraControlDelta( 1.0 );
  }
  else if ( ( maxY - minY ) < 1.0 )
  {
    setExtraControlDelta( ( maxY - minY ) /
                          ParaverConfig::getInstance()->getHistoNumColumns() );
  }
  else if ( ( maxY - minY ) <
            ParaverConfig::getInstance()->getHistoNumColumns() )
  {
    setExtraControlDelta( 1.0 );
  }
  else
  {
    setExtraControlDelta( ( maxY - minY ) /
                          ParaverConfig::getInstance()->getHistoNumColumns() );
  }
}

string HistogramProxy::getRowLabel( TObjectOrder whichRow ) const
{
  return LabelConstructor::objectLabel( whichRow, controlWindow->getLevel(),
                                        controlWindow->getTrace() );
}

string HistogramProxy::getColumnLabel( THistogramColumn whichColumn ) const
{
  return LabelConstructor::histoColumnLabel( whichColumn, this,
         getControlMin(),
         getControlMax(),
         getControlDelta() );
}

string HistogramProxy::getPlaneLabel( THistogramColumn whichPlane ) const
{
  return LabelConstructor::histoColumnLabel( whichPlane, this,
         getExtraControlMin(),
         getExtraControlMax(),
         getExtraControlDelta() );
}