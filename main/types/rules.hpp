#ifndef AGRO_MESH_RULES_HPP__
#define AGRO_MESH_RULES_HPP__

class rule{
	public:
		virtual bool is_active() const noexcept = 0;

		virtual ~rule();
};

template<typename Number>
class rule_inner_range : public rule{
	public:
		rule_inner_range(Number value, Number top, Number bottom = top)
			: value_(value), top_(top), bottom_(bottom){}

		bool is_active() const noexcept override
		{
//			if(value)
		}

	private:
		Number	value_;
		Number	top_;
		Number	bottom_;
};

#endif /* AGRO_MESH_RULES_HPP__ */
