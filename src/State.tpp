/*
 * Docklike Taskbar - A modern, minimalist taskbar for XFCE
 * Copyright (c) 2019-2020 Nicolas Szabo <nszabo@vivaldi.net>
 * gnu.org/licenses/gpl-3.0
 */

#ifndef STATE_TPP
#define STATE_TPP

#include <functional>

template <typename V>
class State
{
  public:
	void setup(V value, std::function<void(V)> feedback)
	{
		v = value;
		f = feedback;
	}

	void set(V value)
	{
		bool change = (v != value);
		v = value;

		if (change)
			f(v);
	}

	operator V() const { return v; }

  private:
	V v;
	std::function<void(V)> f;
};

template <typename V>
class LogicalState
{
  public:
	void setup(V value, std::function<V()> eval,
		std::function<void(V)> feedback)
	{
		v = value;
		e = eval;
		f = feedback;
	}

	void updateState()
	{
		V value = e();
		if (v != value)
		{
			v = value;
			f(v);
		}
	}

	operator V() const { return v; }

  private:
	V v;
	std::function<V()> e;
	std::function<void(V)> f;
};

#endif