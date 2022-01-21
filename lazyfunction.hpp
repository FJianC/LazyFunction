#ifndef __LAZY_FUNCTION_HPP__
#define __LAZY_FUNCTION_HPP__

#include <functional>

/*
 * using std::bind
 * auto lazy = LazyFunction<Return>(std::bind(&Class::MemberFunction, this, args...));
 * 
 * using lambda
 * 1.auto lazy = CreateLazyFunction(
 *		[function, args...]() {
 *			return function(args...);
 *		})
 * );
 * 
 * 2.auto lazy = CreateLazyFunction(
 *		[](args...) {
 *			return function(args...);
 *		},
 *		arg...)
 * );
 * 
 * get return
 * auto ret = lazy();
 * 
 */

template<typename Ret>
class LazyFunction
{
public:
	LazyFunction() = delete;
	LazyFunction(std::function<Ret()>&& func) :
		m_func(std::move(func)), m_is_init(false) {}
	LazyFunction(const LazyFunction<Ret>&) = delete;
	~LazyFunction() = default;

	template<typename Func, typename ...Args>
	LazyFunction(Func&& f, Args&&... args) {
		m_func = std::bind(std::forward<Func>(f), std::forward<Args>(args)...);
		m_is_init = false;
	}

	LazyFunction<Ret>& operator=(const LazyFunction<Ret>&) = delete;

	bool IsInit() const { return m_is_init; }

	void Reset() { m_is_init = false; }

	auto& operator()() {
		if (!IsInit() && m_func.operator bool()) {
			m_is_init = true;
			m_value = m_func();
		}
		return m_value;
	}

private:
	std::function<Ret()>() m_func;
	Ret m_value;
	bool m_is_init;
};

template<>
class LazyFunction<void>
{
public:
	LazyFunction() = delete;
	LazyFunction(std::function<void()>&& func) :
		m_func(std::move(func)) {}
	LazyFunction(const LazyFunction<void>&) = delete;

	template<typename Func, typename ...Args>
	LazyFunction(Func&& f, Args&&... args) {
		m_func = std::bind(std::forward<Func>(f), std::forward<Args>(args)...);
	}

	LazyFunction<void>& operator=(const LazyFunction<void>&) = delete;

	void operator()() {
		if (m_func.operator bool())
			m_func();
	}

private:
	std::function<void()> m_func;
};

template<typename Func, typename ...Args>
LazyFunction<typename std::result_of<Func(Args...)>::type> CreateLazyFunction(Func&& f, Args&&... args) {
	return LazyFunction<typename std::result_of<Func(Args...)>::type>(std::forward<Func>(f), std::forward<Args>(args)...);
}

#endif // !__LAZY_FUNCTION_HPP__
