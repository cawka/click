/**
 *	@file Some general algorithms
 */

#ifndef DEFS_ALGORITHM_HH_DTCAST
#define DEFS_ALGORITHM_HH_DTCAST


template <class InputIterator,class T>
inline InputIterator
find( InputIterator begin, InputIterator end, const T &val )
{
    while( begin!=end && !(*begin==val) )
	begin++;
    return begin;
}

template <class B,class T>
inline void
purge( T &list )
{
	while( !list.empty() )
	{
		B *tmp=list.front( );
		list.pop_front();
		delete tmp;
	}
};

#endif
