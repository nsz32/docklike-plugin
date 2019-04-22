#include <functional>

template <typename V>
class State
{
	public:
		void init(V value, std::function<void(V)> feedback)
		{
			v = value;
			f = feedback;
		}

		void setState(V value)
		{
			v = value;
			f(v);
		}

		operator V() const
		{
			return v;
		}

		

	private:
		V v;
		std::function<void(V)> f;
};