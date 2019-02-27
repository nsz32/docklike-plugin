// ** opensource.org/licenses/GPL-3.0

#ifndef STORE_HPP
#define STORE_HPP

#include <list>
#include <utility>

namespace nmt
{
	template <typename K, typename V>
	class KeyStore
	{
		public:
			void push(K k, V v)
			{
				mList.push_back(std::make_pair(k, v));
			}

			V first()
			{
				return mList.front().second;
			}

			V last()
			{
				return mList.back().second;
			}

			V pop(K k)
			{
				typename std::list<std::pair<const K, V>>::iterator it = std::find_if(mList.begin(), mList.end(),
				[&k](std::pair<const K, V> o)->bool { return (o.first == k); });

				if(it != mList.end())
				{
					V v = it->second;
					mList.erase(it);
					return v;
				}

				return NULL;
			}

			V get(K k)
			{
				typename std::list<std::pair<const K, V>>::iterator it = std::find_if(mList.begin(), mList.end(),
				[&k](std::pair<const K, V> o)->bool { return (o.first == k); });
				
				if(it != mList.end())
					return it->second;

				return NULL;
			}

			V findIf(std::function<bool(std::pair<const K, V>)> pred)
			{
				typename std::list<std::pair<const K, V>>::iterator it = std::find_if(mList.begin(), mList.end(), pred);
				if(it != mList.end())
					return it->second;

				return NULL;
			}

			V moveBack(K k)
			{
				V v = pop(k);
				push(k, v);

				return v;
			}

			void shiftToBack()
			{
				mList.push_back(mList.front());
				mList.pop_front();
			}

			void shiftToFront()
			{
				mList.push_front(mList.back());
				mList.pop_back();
			}

			void forEach(std::function<void(std::pair<const K, V>)> funct)
			{
				std::for_each(mList.begin(), mList.end(), funct);
			}

			uint size()
			{
				return mList.size();
			}

		private:
			std::list<std::pair<const K, V>> mList;
	};
}

#endif