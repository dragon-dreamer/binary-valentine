#pragma once

namespace bv::core
{

template<typename... Func>
struct [[nodiscard]] overloaded final
	: public Func...
{
	using Func::operator()...;
};

template<typename... Func>
overloaded(Func...) -> overloaded<Func...>;

} //namespace bv::core
