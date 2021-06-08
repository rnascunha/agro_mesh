#ifndef NUMBER_H__
#define NUMBER_H__
//
//template<typename T>
//T exponencial(T base, T exp){
//	T value = base;
//
//	for(int i = exp; i >= 0; i--){
//		value *= base;
//	}
//
//	return value;
//}
//
//template<unsigned int precision = 3>
//class Number{
//	Number(int interger = 0, unsigned int fractional = 0){
//		this->interger = (unsigned int)interger;
//		this->fractional = fractional;
//
//		factor = (unsigned int)exponencial((unsigned int)10, precision);
//	}
//
//	Number& operator+=(const Number& a){
//		this->interger += a.interger;
//		this->fractional += a.fractional;
//		correct_number(this);
//
//		return *this;
//	}
//
//	Number& operator-=(const Number& a){
//		this->interger += -(a.interger);
//		this->fractional += -(a.fractional);
//
//		return *this;
//	}
//
//	Number operator*(const Number& a){
//
//	}
//
//	Number operator/(const Number& a){
//
//	}
//
//	Number operator^(const Number& a){
//
//	}
//
//	protected:
//		int interger;
//		unsigned int fractional;
//
//		static const unsigned int factor;
//
//		static void correct_number(Number& n){
//			n.interger = n.fractional / factor;
//			n.fractional = n.fractional % factor;
//		}
//};


#endif /* NUMBER_H__ */
