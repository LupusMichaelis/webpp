
template <typename base_type, typename former_type, typename latter_type>
struct compare_traits
{
	static void constraint()
	{
		former_type * p_former = nullptr;
		latter_type * p_latter = nullptr;

		value * p_base = nullptr;
		
		p_base = p_former;
		p_base = p_latter;

		(void) p_base;
	}

	static bool const equals(former_type const & former, latter_type const & latter);
	static bool const greater_than(former_type const & former, latter_type const & latter);
	static bool const less_than(former_type const & former, latter_type const & latter);
};

// Generic comparision (works for same type comparision)
template <typename value_type, typename former_type, typename latter_type>
bool const compare_traits<value_type, former_type, latter_type>::equals(former_type const & former, latter_type const & latter)
{
	return compare_traits<value_type, value_type, value_type>::equals(former, latter)
		or (nullptr != former and nullptr != latter
				and former.operator ==(latter));
}

template <typename value_type, typename former_type, typename latter_type>
bool const compare_traits<value_type, former_type, latter_type>::greater_than(former_type const & former, latter_type const & latter)
{
	return compare_traits<value_type, value_type, value_type>::greater_than(former, latter)
		or former.operator >(latter);
}

template <typename value_type, typename former_type, typename latter_type>
bool const compare_traits<value_type, former_type, latter_type>::less_than(former_type const & former, latter_type const & latter)
{
	return compare_traits<value_type, value_type, value_type>::less_than(former, latter)
		or former.operator <(latter);
}

