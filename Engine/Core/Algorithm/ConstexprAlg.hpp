//
// Created by loyus on 7/14/2023.
//

#pragma once

template<auto... A>
struct ConstexprContainer {
	static constexpr auto
	Contain(auto &&X)
	{
		return ((X == A) || ...);
	}
};

template<typename Container1, typename Container2>
struct CombineContainers;

template<auto... A, auto... B>
struct CombineContainers<ConstexprContainer<A...>, ConstexprContainer<B...>> {
	using type = ConstexprContainer<A..., B...>;
};
