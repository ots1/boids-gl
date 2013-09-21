#ifndef SPATIALHASH_H
#define SPATIALHASH_H

#include <map>
#include <vector>

template <class T>
class SpatialHash {
	double bin_width;
	std::map< int, std::vector<T> > h;
public:
	//typedef typename std::map< int, std::vector<T> >::iterator iterator;

	const int y_stride;
	const int z_stride;
	
	int hash(vec3 pos) const
	{
		return int(pos[0]/bin_width) 
			+ y_stride * int(pos[1]/bin_width) 
			+ z_stride * int(pos[2]/bin_width);
	}

	// compute the spatial index of neighbours.
	int neigh_offset(int neigh_idx) const 
	{
		int x=neigh_idx % 3 - 1;
		int y=(neigh_idx/3) % 3 - 1;
		int z=(neigh_idx/9) % 3 - 1;
		return x + y*y_stride + z*z_stride;
	}

	void insert(vec3 pos, T a)
	{
		h[hash(pos)].push_back(a);
	}

	void clear()
	{
		for (auto ii = h.begin(); ii != h.end(); ++ii) {
			(*ii).second.clear();
		}
	}

	// could make this more like an STL iterator
	class iterator {
		const SpatialHash* a; 
		const int idx;
		int neigh_idx;
		typename std::vector<T>::const_iterator current;
	public:
		// advance to point to the next valid element.
		// Assumes that if neigh_idx has a corresponding
		// vector, then 'current' points to it.  This is
		// important for constructing the object, where if
		// current is not set at all, the 'count' check in the
		// while loop could fail and the 'end' check is then
		// nonsensical.
		void advance_to_next_valid()
		{
			// do not wish to insert a key if it isn't present: 
			// check the count and use c++11 at().
			while (!(a->h.count(idx + a->neigh_offset(neigh_idx))) ||
			       current == a->h.at(idx + a->neigh_offset(neigh_idx)).end())
			{
				if (++neigh_idx >= 27) break;
				if (a->h.count(idx + a->neigh_offset(neigh_idx)))
					current = a->h.at(idx + a->neigh_offset(neigh_idx)).begin();
			}
			
			// while (!(a->h.count(idx + a->neigh_offset(neigh_idx))) ||
			//        current == a->h[idx + a->neigh_offset(neigh_idx)].end()) {
			// 	while (!(a->h.count(idx + a->neigh_offset(neigh_idx))))
			// 		if (++neigh_idx >= 27) return;
				
			// 	while (current == a->h.at(idx + a->neigh_offset(neigh_idx)).end()) {
			// 		if (++neigh_idx >= 27) return;
			// 	}
			// }

		}
		
		iterator& operator++()
		{
			++current; // move along the current vector
			// if we've reached the end, find the next valid element
			advance_to_next_valid();
			return (*this);
		}

		const T& operator*()
		{
			return (*current);
		}

		// are we at the end?
		bool end() {
			return (neigh_idx >= 27);
		}		

		iterator(SpatialHash& a_, vec3 x)
			: a(&a_), idx(a_.hash(x)), neigh_idx(0)//, current(a->h[idx+a->neigh_offset(neigh_idx)].begin())
		{
			// if there is a vector at the location where we start, set to current
			if (a->h.count(idx + a->neigh_offset(neigh_idx)))
				current = a->h.at(idx+a->neigh_offset(neigh_idx)).begin();

			// advance until actually pointing to an
			// element (the vector might be empty, or
			// there might not be one.
			advance_to_next_valid();
		}
	};

	iterator neighbours(vec3 x) {
		return iterator(*this, x);
	}

	SpatialHash(double bin_width_, int y_stride_=10007, int z_stride_=100000007)
		: bin_width(bin_width_), y_stride(y_stride_), z_stride(z_stride_)
	{}
};

#endif
