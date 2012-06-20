#pragma once

#include <vector>
#include <ppl.h>
#include <concurrent_vector.h>

using namespace std;
using namespace Concurrency;

class FunctionalOps
{
	template < class T > vector< T > static Filter(const vector< T >& sequence, std::tr1::function< bool (T) > predicate)
	{
	   vector< T > result;

	   for(auto it = sequence.begin(); it != sequence.end(); ++it)
		  if(predicate(*it))
			 result.push_back(*it);

	   return result;
	}

	template < class T > concurrent_vector< T > static Parallel_Filter(const vector< T >& sequence, std::tr1::function< bool (T) > predicate)
	{
	   concurrent_vector< T > result;

	   parallel_for_each(sequence.begin(), sequence.end(), [&](T it) {
		  if(predicate(it))
			 result.push_back(it);
	   });

	   return result;
	}

	template < class T, class U > concurrent_vector< U > static  Parallel_Map(const concurrent_vector< T >& sequence, std::tr1::function< U (T) > predicate)
	{
	   concurrent_vector< U > result;

	   parallel_for_each(sequence.begin(), sequence.end(), [&] (T it) {
		   result.push_back(predicate(it));
	   });

	   return result;
	}

	template < class T, class U > concurrent_vector< U > static Parallel_Choose(const vector< T >& sequence, std::tr1::function< bool (T) > predicate, std::tr1::function< U (T) > transformer)
	{
	   concurrent_vector< U > result;

	   parallel_for_each(sequence.begin(), sequence.end(), [&] (T it) {
		   if(predicate(it))
			   result.push_back(transformer(it));
	   });

	   return result;
	}

	template < class T, class U > vector< U > static Map(const vector< T >& sequence, std::tr1::function< U (T) > predicate)
	{
	   vector< U > result;

	   for(auto it = sequence.begin(); it != sequence.end(); ++it)
		   result.push_back(predicate(*it));

	   return result;
	}

	template < class T, class U > vector< U > static Choose(const vector< T >& sequence, std::tr1::function< bool (T) > predicate, std::tr1::function< U (T) > transformer)
	{
	   vector< U > result;

	   for(auto it = sequence.begin(); it != sequence.end(); ++it)
		   if(predicate(*it))
			   result.push_back(transformer(*it));

	   return result;
	}
}