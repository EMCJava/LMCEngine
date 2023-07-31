//
// Created by loyus on 7/14/2023.
//

#pragma once

template <auto... A>
struct ConstexprContainer {
    static constexpr auto
    Contain( auto&& X )
    {
        return ( ( X == A ) || ... );
    }
};

template <typename... Containers>
struct CombineContainers;

template <auto... A>
struct CombineContainers<ConstexprContainer<A...>> {
    using type = ConstexprContainer<A...>;
};

template <auto... A, auto... B, typename... Rest>
struct CombineContainers<ConstexprContainer<A...>, ConstexprContainer<B...>, Rest...> {
    using type = typename CombineContainers<ConstexprContainer<A..., B...>, Rest...>::type;
};

template <template <typename> typename ValueWrapper, template <typename> typename SetWrapper, typename... Containers>
struct CombineContainersWrap;

template <template <typename> typename ValueWrapper, template <typename> typename SetWrapper>
struct CombineContainersWrap<ValueWrapper, SetWrapper> {
    using type = ConstexprContainer<>;
};

template <template <typename> typename ValueWrapper, template <typename> typename SetWrapper, typename A>
struct CombineContainersWrap<ValueWrapper, SetWrapper, A> {
    using type = typename CombineContainers<ConstexprContainer<ValueWrapper<A>::Value>, typename SetWrapper<A>::Container>::type;
};

template <template <typename> typename ValueWrapper, template <typename> typename SetWrapper, typename A, typename B, typename... Rest>
struct CombineContainersWrap<ValueWrapper, SetWrapper, A, B, Rest...> {
    using type = typename CombineContainers<
        typename CombineContainersWrap<ValueWrapper, SetWrapper, A>::type,
        typename CombineContainersWrap<ValueWrapper, SetWrapper, B, Rest...>::type>::type;
};

template <template <typename> typename ValueWrapper, template <typename> typename SetWrapper, typename... Containers>
struct CombineContainersWrapExcludeFirst;

template <template <typename> typename ValueWrapper, template <typename> typename SetWrapper, typename A, typename B, typename... Rest>
struct CombineContainersWrapExcludeFirst<ValueWrapper, SetWrapper, A, B, Rest...> {
    using type = typename CombineContainers<ConstexprContainer<ValueWrapper<A>::Value>, typename CombineContainersWrap<ValueWrapper, SetWrapper, B, Rest...>::type>::type;
};