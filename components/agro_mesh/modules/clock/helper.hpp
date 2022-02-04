#ifndef AGRO_MESH_RTC_TIME_HELPER_HPP__
#define AGRO_MESH_RTC_TIME_HELPER_HPP__

#include "rtc_time.hpp"
#include "datetime.h"

namespace Agro{

value_time get_time() noexcept;
value_time get_local_time() noexcept;

DateTime get_datetime() noexcept;
DateTime get_local_datetime() noexcept;

}//Agro

#endif /* AGRO_MESH_RTC_TIME_HELPER_HPP__ */
