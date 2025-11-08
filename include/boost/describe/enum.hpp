#ifndef BOOST_DESCRIBE_ENUM_HPP_INCLUDED
#define BOOST_DESCRIBE_ENUM_HPP_INCLUDED

// Copyright 2020 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/describe/detail/config.hpp>

#if !defined(BOOST_DESCRIBE_CXX14)

#define BOOST_DESCRIBE_ENUM(E, ...)
#define BOOST_DESCRIBE_NESTED_ENUM(E, ...)

#else

#include <boost/describe/detail/pp_for_each.hpp>
#include <boost/describe/detail/list.hpp>
#include <type_traits>
#include <algorithm>

namespace boost
{
namespace describe
{
namespace detail
{

template<class D> struct enum_descriptor
{
    // can't use auto here because of the need to supply the definitions below
    static constexpr decltype(D::value()) value = D::value();
    static constexpr decltype(D::name()) name = D::name();
};

template <std::size_t len>
struct compile_time_string
{
	char data_[len] = {};
	constexpr explicit compile_time_string(char const (&str)[len]){
		std::copy_n(str, len, data_);
	}
	constexpr char const* data() const { return data_; }
};
template <std::size_t N>
compile_time_string(char const (&str)[N]) -> compile_time_string<N>;


template <class D, D v, int N, compile_time_string<N> S>
struct _boost_desc {
	static constexpr auto value() noexcept { return v; }
	static constexpr auto name() noexcept { return S.data(); }
};

#ifndef __cpp_inline_variables
// GCC requires these definitions
template<class D> constexpr decltype(D::value()) enum_descriptor<D>::value;
template<class D> constexpr decltype(D::name()) enum_descriptor<D>::name;
#endif

template<class... T> auto enum_descriptor_fn_impl( int, T... )
{
    return list<enum_descriptor<T>...>();
}

#define BOOST_DESCRIBE_ENUM_BEGIN(E) \
    inline auto boost_enum_descriptor_fn( E** ) \
    { return boost::describe::detail::enum_descriptor_fn_impl( 0

#define BOOST_DESCRIBE_ENUM_ENTRY_B(E, e) boost::describe::detail::_boost_desc<E, E::e, sizeof(#e), boost::describe::detail::compile_time_string<sizeof(#e)>(#e)>{}
#define BOOST_DESCRIBE_ENUM_ENTRY(E, e) , BOOST_DESCRIBE_ENUM_ENTRY_B(E, e)

#define BOOST_DESCRIBE_ENUM_END(E) ); }

#define BOOST_DESCRIBE_ENUM_ENTRY_RETURN(E, e) , decltype(BOOST_DESCRIBE_ENUM_ENTRY_B(E, e)) 
#define BOOST_DESCRIBE_NESTED_ENUM_RETURN_TYPE(E, First, ...) decltype(BOOST_DESCRIBE_ENUM_ENTRY_B(E, First)) \
    	BOOST_DESCRIBE_PP_FOR_EACH(BOOST_DESCRIBE_ENUM_ENTRY_RETURN, E, ##__VA_ARGS__) \

} // namespace detail

#if defined(_MSC_VER) && !defined(__clang__)

#define BOOST_DESCRIBE_ENUM(E, ...) \
    namespace should_use_BOOST_DESCRIBE_NESTED_ENUM {} \
    static_assert(std::is_enum<E>::value, "BOOST_DESCRIBE_ENUM should only be used with enums"); \
    BOOST_DESCRIBE_ENUM_BEGIN(E) \
    BOOST_DESCRIBE_PP_FOR_EACH(BOOST_DESCRIBE_ENUM_ENTRY, E, __VA_ARGS__) \
    BOOST_DESCRIBE_ENUM_END(E)

#define BOOST_DESCRIBE_NESTED_ENUM(E, ...) \
    static_assert(std::is_enum<E>::value, "BOOST_DESCRIBE_NESTED_ENUM should only be used with enums"); \
    friend BOOST_DESCRIBE_ENUM_BEGIN(E) \
    BOOST_DESCRIBE_PP_FOR_EACH(BOOST_DESCRIBE_ENUM_ENTRY, E, __VA_ARGS__) \
    BOOST_DESCRIBE_ENUM_END(E)

#else

#define BOOST_DESCRIBE_ENUM(E, ...) \
    namespace should_use_BOOST_DESCRIBE_NESTED_ENUM {} \
    static_assert(std::is_enum<E>::value, "BOOST_DESCRIBE_ENUM should only be used with enums"); \
    BOOST_DESCRIBE_MAYBE_UNUSED BOOST_DESCRIBE_ENUM_BEGIN(E) \
    BOOST_DESCRIBE_PP_FOR_EACH(BOOST_DESCRIBE_ENUM_ENTRY, E, ##__VA_ARGS__) \
    BOOST_DESCRIBE_ENUM_END(E)

#define BOOST_DESCRIBE_NESTED_ENUM(E, ...) \
    static_assert(std::is_enum<E>::value, "BOOST_DESCRIBE_NESTED_ENUM should only be used with enums"); \
    BOOST_DESCRIBE_MAYBE_UNUSED friend \
    auto boost_enum_descriptor_fn( E** ) \
	    -> decltype(boost::describe::detail::enum_descriptor_fn_impl(0 \
    BOOST_DESCRIBE_PP_FOR_EACH(BOOST_DESCRIBE_ENUM_ENTRY, E, ##__VA_ARGS__) \
	 )) \
    { return boost::describe::detail::enum_descriptor_fn_impl( 0 \
    BOOST_DESCRIBE_PP_FOR_EACH(BOOST_DESCRIBE_ENUM_ENTRY, E, ##__VA_ARGS__) \
    BOOST_DESCRIBE_ENUM_END(E)

#endif

} // namespace describe
} // namespace boost

#endif // defined(BOOST_DESCRIBE_CXX14)

#if defined(_MSC_VER) && !defined(__clang__)

#define BOOST_DEFINE_ENUM(E, ...) enum E { __VA_ARGS__ }; BOOST_DESCRIBE_ENUM(E, __VA_ARGS__)
#define BOOST_DEFINE_ENUM_CLASS(E, ...) enum class E { __VA_ARGS__ }; BOOST_DESCRIBE_ENUM(E, __VA_ARGS__)

#define BOOST_DEFINE_FIXED_ENUM(E, Base, ...) enum E: Base { __VA_ARGS__ }; BOOST_DESCRIBE_ENUM(E, __VA_ARGS__)
#define BOOST_DEFINE_FIXED_ENUM_CLASS(E, Base, ...) enum class E: Base { __VA_ARGS__ }; BOOST_DESCRIBE_ENUM(E, __VA_ARGS__)

#else

#define BOOST_DEFINE_ENUM(E, ...) enum E { __VA_ARGS__ }; BOOST_DESCRIBE_ENUM(E, ##__VA_ARGS__)
#define BOOST_DEFINE_ENUM_CLASS(E, ...) enum class E { __VA_ARGS__ }; BOOST_DESCRIBE_ENUM(E, ##__VA_ARGS__)

#define BOOST_DEFINE_FIXED_ENUM(E, Base, ...) enum E: Base { __VA_ARGS__ }; BOOST_DESCRIBE_ENUM(E, ##__VA_ARGS__)
#define BOOST_DEFINE_FIXED_ENUM_CLASS(E, Base, ...) enum class E: Base { __VA_ARGS__ }; BOOST_DESCRIBE_ENUM(E, ##__VA_ARGS__)

#endif

#endif // #ifndef BOOST_DESCRIBE_ENUM_HPP_INCLUDED
