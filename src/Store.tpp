/*
 * Docklike Taskbar - A modern, minimalist taskbar for XFCE
 * Copyright (c) 2019-2020 Nicolas Szabo <nszabo@vivaldi.net>
 * gnu.org/licenses/gpl-3.0
 */

#ifndef STORE_HPP
#define STORE_HPP

#include <algorithm>
#include <functional>
#include <list>
#include <map>
#include <utility>

namespace Store
{
	template <typename K, typename V>
	class KeyStore
	{
	  public:
		void push(K k, V v) { mList.push_front(std::make_pair(k, v)); }

		void pushSecond(K k, V v)
		{
			mList.insert(std::next(mList.begin()), std::make_pair(k, v));
		}

		V pop(K k)
		{
			typename std::list<std::pair<const K, V>>::iterator it = std::find_if(
				mList.begin(), mList.end(), [&k](std::pair<const K, V> o) -> bool { return (o.first == k); });

			if (it != mList.end())
			{
				V v = it->second;
				mList.erase(it);
				return v;
			}

			return NULL;
		}

		V get(K k)
		{
			typename std::list<std::pair<const K, V>>::iterator it = std::find_if(
				mList.begin(), mList.end(), [&k](std::pair<const K, V> o) -> bool { return (o.first == k); });

			if (it != mList.end())
				return it->second;

			return NULL;
		}

		V moveToStart(K k)
		{
			V v = pop(k);
			mList.push_front(std::make_pair(k, v));

			return v;
		}

		V findIf(std::function<bool(std::pair<const K, V>)> pred)
		{
			typename std::list<std::pair<const K, V>>::iterator it = std::find_if(mList.begin(), mList.end(), pred);
			if (it != mList.end())
				return it->second;

			return NULL;
		}

		void forEach(std::function<void(std::pair<const K, V>)> funct)
		{
			std::for_each(mList.begin(), mList.end(), funct);
		}

		uint size() { return mList.size(); }

		V first() { return mList.front().second; }

		std::list<std::pair<const K, V>> mList;
	};

	template <typename K, typename V>
	class Map
	{
	  public:
		void set(K k, V v) { mMap[k] = v; }

		V get(K k)
		{
			typename std::map<const K, V>::iterator it = mMap.find(k);
			if (it != mMap.end())
				return it->second;

			return NULL;
		}

	  private:
		std::map<const K, V> mMap;
	};

	template <typename V>
	class List
	{
	  public:
		void push(V v) { mList.push_back(v); }

		void pop(V v) { mList.remove(v); }

		V get(uint index) { return *std::next(mList.begin(), index); }

		uint getIndex(V v)
		{
			typename std::list<V>::iterator it = std::find(mList.begin(), mList.end(), v);
			return std::distance(mList.begin(), it);
		}

		void forEach(std::function<void(V)> funct)
		{
			std::for_each(mList.begin(), mList.end(), funct);
		}

		V findIf(std::function<bool(V)> pred)
		{
			typename std::list<V>::iterator it = std::find_if(mList.begin(), mList.end(), pred);
			if (it != mList.end())
				return *it;

			return NULL;
		}

		uint size() { return mList.size(); }

	  private:
		std::list<V> mList;
	};
} // namespace Store

#endif