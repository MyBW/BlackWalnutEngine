#pragma once
		template<typename... TypeList>
		struct typelist
		{
		};
		template<typename T , typename... TypeList>
		struct typelist<T , TypeList...> : typelist<TypeList...>
		{
			typelist(T t , TypeList... ts) : typelist<TypeList...>(ts),Value(t){ }
			T Value;
		};
		typedef typelist<> nulltypelist;
////////////////////////////////////////////////////////////////////
		template<typename T, typename... TypeList> struct  push_front;
		template<typename T , typename... TypeList> 
		struct push_front<T , typelist<TypeList...>>
		{
			typedef typelist<T, TypeList...> type;
		};
////////////////////////////////////////////////////////////////////
		template<typename... TList> struct length;
		template<typename... TList>
		struct length< typelist<TList...> >
		{
			enum { value = sizeof...(TList) };
		};

////////////////////////////////////////////////////////////////////
		template<bool flag, typename T, typename U>
		struct type_select
		{
			typedef T Result;

		};
		template<typename T, typename U>
		struct type_select<false, T, U>
		{
			typedef U Result;
		};
//////////////////////////////////////////////////////////////////
		template<unsigned int Index, typename... TypeList> struct at;
		template<unsigned int Index, typename T , typename... TypeList>
		struct at<Index , typelist<T , TypeList...>>
		{
			typedef typename at<Index - 1, typelist<TypeList...>>::type type;
		};
		template<typename T , typename... TypeList>
		struct at<0, T , TypeList...>
		{
			typedef T type;
		};
		template<>
		struct at<0, nulltypelist>
		{
			typedef nulltypelist type;
		};
////////////////////////////////////////////////////////////////////
		template<int index , typename T , typename...Ts>
		typename std::enable_if<index != 0, typename at<index, typelist<T, Ts...>>::type&>::type
	    get(typelist<T, Ts...>& TypeList)
		{
			typelist<Ts...>& NextTypeList = TypeList;
			return get<index - 1>(NextTypeList);
		}
		template<int index, typename T, typename...Ts>
		typename std::enable_if<index == 0, typename at<0, typelist<T, Ts...>>::type&>::type
		  get(typelist<T, Ts...>& TypeValue)
		{
			return TypeValue.Vaule;
		}
	

////////////////////////////////////////////////////////////////////
		template<typename TypeList , typename CallBackFunction>
		void foreach_type(  )
		{

		}