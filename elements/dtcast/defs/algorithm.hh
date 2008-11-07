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


template <class InputIterator,class T,class Q>
inline InputIterator
find( InputIterator begin, InputIterator end, const T &member, const Q &val )//bool (T::*member)(const Q&), const Q &val )
{
    while( begin!=end && !((*begin).second->*member)(val) )
	begin++;
    return begin;
}


template <class B,class T>
inline void
purge( T &list )
{
	for( typename T::iterator i=list.begin(); i!=list.end(); i++ )
	{
		delete i->second;
	}
	list.clear( );
//	while( !list.empty() )
//	{
//		B *tmp=list.front( );
//		list.pop_front();
//		delete tmp;
//	}
};

template <class B,class T>
inline void
purgeList( T &list )
{
	while( !list.empty() )
	{
		B *tmp=list.front( );
		list.pop_front();
		delete tmp;
	}
};

#endif
