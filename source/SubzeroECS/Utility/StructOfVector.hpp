#pragma once

#include <tuple>
#include <vector>

namespace SubzeroECS {
namespace Utility
{
	/**
	* @tparam Types  Struct member types which will each be contained in their own vector type
	* @todo Integrate tparam: class A = std::allocator<T> typedef A allocator_type;
	*/
	template <typename... Types>
	class StructOfVector
	{
	public:
		typedef std::tuple<std::vector<Types>...> StructVectors;
		
		typedef std::tuple<Types...> value_type;
		typedef std::tuple<Types&...> reference;
		typedef std::tuple<const Types&...> const_reference;
		typedef std::ptrdiff_t difference_type;
		typedef std::size_t size_type;

		class iterator
		{
		public:
			typedef std::ptrdiff_t difference_type;
			typedef std::tuple<Types...> value_type;
			typedef std::tuple<Types&...> reference;
			typedef typename &std::tuple_element<0U,Types>::type pointer;
			typedef std::random_access_iterator_tag iterator_category;

			iterator();
			iterator( const iterator& );
			~iterator();

			iterator& operator=( const iterator& );
			bool operator==( const iterator& ) const;
			bool operator!=( const iterator& ) const;
			bool operator<( const iterator& ) const; //optional
			bool operator>( const iterator& ) const; //optional
			bool operator<=( const iterator& ) const; //optional
			bool operator>=( const iterator& ) const; //optional

			iterator& operator++();
			iterator operator++( int ); //optional
			iterator& operator--(); //optional
			iterator operator--( int ); //optional
			iterator& operator+=( size_type ); //optional
			iterator operator+( size_type ) const; //optional
			friend iterator operator+( size_type, const iterator& ); //optional
			iterator& operator-=( size_type ); //optional            
			iterator operator-( size_type ) const; //optional
			difference_type operator-( iterator ) const; //optional

			reference operator*() const;
			pointer operator->() const;
			reference operator[]( size_type ) const; //optional
		};
		class const_iterator
		{
		public:
			typedef std::ptrdiff_t difference_type;
			typedef std::tuple<Types...>  value_type;
			typedef typename const A::reference reference;
			typedef typename const A::pointer pointer;
			typedef std::random_access_iterator_tag iterator_category; //or another tag

			const_iterator();
			const_iterator( const const_iterator& );
			const_iterator( const iterator& );
			~const_iterator();

			const_iterator& operator=( const const_iterator& );
			bool operator==( const const_iterator& ) const;
			bool operator!=( const const_iterator& ) const;
			bool operator<( const const_iterator& ) const; //optional
			bool operator>( const const_iterator& ) const; //optional
			bool operator<=( const const_iterator& ) const; //optional
			bool operator>=( const const_iterator& ) const; //optional

			const_iterator& operator++();
			const_iterator operator++( int ); //optional
			const_iterator& operator--(); //optional
			const_iterator operator--( int ); //optional
			const_iterator& operator+=( size_type ); //optional
			const_iterator operator+( size_type ) const; //optional
			friend const_iterator operator+( size_type, const const_iterator& ); //optional
			const_iterator& operator-=( size_type ); //optional            
			const_iterator operator-( size_type ) const; //optional
			difference_type operator-( const_iterator ) const; //optional

			reference operator*() const;
			pointer operator->() const;
			reference operator[]( size_type ) const; //optional
		};

		typedef std::reverse_iterator<iterator> reverse_iterator; //optional
		typedef std::reverse_iterator<const_iterator> const_reverse_iterator; //optional

		X();
		X( const X& );
		~X();

		X& operator=( const X& );
		bool operator==( const X& ) const;
		bool operator!=( const X& ) const;
		bool operator<( const X& ) const; //optional
		bool operator>( const X& ) const; //optional
		bool operator<=( const X& ) const; //optional
		bool operator>=( const X& ) const; //optional

		iterator begin();
		const_iterator begin() const;
		const_iterator cbegin() const;
		iterator end();
		const_iterator end() const;
		const_iterator cend() const;
		//reverse_iterator rbegin(); //optional
		//const_reverse_iterator rbegin() const; //optional
		//const_reverse_iterator crbegin() const; //optional
		//reverse_iterator rend(); //optional
		//const_reverse_iterator rend() const; //optional
		//const_reverse_iterator crend() const; //optional

		//reference front(); //optional
		//const_reference front() const; //optional
		//reference back(); //optional
		//const_reference back() const; //optional
		template<class ...Args>
		void emplace_front( Args&&... )
		{ emplace_back( value_type( std::forward(Args)... ); }

		void emplace_front( value_type&& arg)
		{ std::tie(  }

		template<class ...Args>
		void emplace_back( Args&&... ); //optional
		void push_front( const T& ); //optional
		void push_front( T&& ); //optional
		void push_back( const T& ); //optional
		void push_back( T&& ); //optional
		void pop_front(); //optional
		void pop_back(); //optional
		reference operator[]( size_type ); //optional
		const_reference operator[]( size_type ) const; //optional
		reference at( size_type ); //optional
		const_reference at( size_type ) const; //optional

		template<class ...Args>
		iterator emplace( const_iterator, Args&&... ); //optional
		iterator insert( const_iterator, const T& ); //optional
		iterator insert( const_iterator, T&& ); //optional
		//iterator insert( const_iterator, size_type, T& ); //optional
		//template<class iter>
		//iterator insert( const_iterator, iter, iter ); //optional
		//iterator insert( const_iterator, std::initializer_list<T> ); //optional
		iterator erase( const_iterator ); //optional
		iterator erase( const_iterator, const_iterator ); //optional
		void clear(); //optional
		//template<class iter>
		//void assign( iter, iter ); //optional
		//void assign( std::initializer_list<T> ); //optional
		//void assign( size_type, const T& ); //optional

		void swap( X& );
		size_type size() const;
		size_type max_size() const;
		bool empty() const;


	private:
		StructVectors vectors_; ///< Vectors that make up the struct
	};
	//template <class T >
	//void swap( StructOfVector<T>&, StructOfVector<T>& ); //optional


} //END: Utility
} //END: SubzeroECS
