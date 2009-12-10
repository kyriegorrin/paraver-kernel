
template <typename RecordType>
Index<RecordType>::Index( UINT32 step )
{
  indexStep = step;
  counter = 0;
}

template <typename RecordType>
Index<RecordType>::~Index()
{}

template <typename RecordType>
void Index<RecordType>::indexRecord( TRecordTime time, RecordType rec )
{
  counter++;
  if ( counter == indexStep )
  {
    baseIndex[ time ] = rec;
    counter = 0;
  }
}

template <typename RecordType>
bool Index<RecordType>::findRecord( TRecordTime time, RecordType& record ) const
{
  typename TTraceIndex::const_iterator it = baseIndex.lower_bound( time );

  if ( it == baseIndex.end() )
    return false;

  record = it->second;

  return true;
}

