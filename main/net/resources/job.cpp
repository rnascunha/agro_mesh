#include "esp_log.h"
#include "../coap_engine.hpp"

#include "datetime.h"
#include "../../types/rtc_time.hpp"
#include "../../types/job.hpp"
#include "../../storage.hpp"

extern const char* RESOURCE_TAG;

//extern RTC_Time device_clock;
extern const char* job_path;

//static void print_job(Jobs::job const& jb) noexcept
//{
//	DateTime dt;
//	dt.setUnixTime(device_clock.get_local_time());
//
//	printf("%02u/%02u/%04u %02u:%02u:%02u %d|%02u:%02u-%02u:%02u dow=%d/pri:%u/act:%u|%s\n",
////			device_clock.get_time(), device_clock.get_local_time(),
//			dt.getDay(), dt.getMonth(), dt.getYear(),
//			dt.getHour(), dt.getMinute(), dt.getSecond(),
//			static_cast<int>(dt.dayOfWeek()),
//			jb.sch.time_before.hour, jb.sch.time_before.minute,
//			jb.sch.time_after.hour, jb.sch.time_after.minute,
//			static_cast<int>(jb.sch.dw), jb.priority, jb.active,
//			jb.sch.is_active(device_clock.get_local_time()) ? "active" : "not");
//}

static void get_job_handler(engine::message const& request,
								engine::response& response,
								void*) noexcept
{
	ESP_LOGD(RESOURCE_TAG, "Called get job handler");

	if(!storage_is_mounted())
	{
		response
			.code(CoAP::Message::code::internal_server_error)
			.payload("storage not mounted")
			.serialize();
		return;
	}

	FILE *f = fopen(job_path, "rb");
	if (f == NULL)
	{
		/**
		 * File doesnt exits... no jobs
		 */
		response
			.code(CoAP::Message::code::content)
			.serialize();
		return;
	}

	std::uint8_t data[engine::packet_size];
	std::size_t readed = fread(data, 1, engine::packet_size, f);
	fclose(f);

//	std::size_t len = readed;
//	std::uint8_t *data1 = data;
//	while(len)
//	{
//		Jobs::job j{
//			Jobs::scheduler{
//				Jobs::time{data1[0], data1[1]},
//				Jobs::time{data1[2], data1[3]},
//				static_cast<Jobs::dow>(data1[4])},
//			data1[5], data1[6]};
//		print_job(j);
//
//		len -= Jobs::job::packet_size;
//		data1 += Jobs::job::packet_size;
//	}

	response
		.code(CoAP::Message::code::content)
		.payload(data, readed)
		.serialize();
}

static void put_job_handler(engine::message const& request,
								engine::response& response,
								void*) noexcept
{
	ESP_LOGD(RESOURCE_TAG, "Called put job handler");

	if(!storage_is_mounted())
	{
		response
			.code(CoAP::Message::code::internal_server_error)
			.payload("storage not mounted")
			.serialize();
		return;
	}

	if(request.payload_len % Jobs::job::packet_size)
	{
		response
			.code(CoAP::Message::code::bad_request)
			.payload("invalid payload")
			.serialize();
		return;
	}

//	const uint8_t* data = static_cast<const uint8_t*>(request.payload);
//	std::size_t len = request.payload_len;
//	while(len)
//	{
//		Jobs::job j{
//			Jobs::scheduler{
//				Jobs::time{data[0], data[1]},
//				Jobs::time{data[2], data[3]},
//				static_cast<Jobs::dow>(data[4])},
//			data[5], data[6]};
//		print_job(j);
//
//		len -= Jobs::job::packet_size;
//		data += Jobs::job::packet_size;
//	}

	FILE *f = fopen(job_path, "wb");
	if (f == NULL)
	{
		response
			.code(CoAP::Message::code::internal_server_error)
			.payload("write error")
			.serialize();
		return;
	}
	fwrite(request.payload, 1, request.payload_len, f);
	fclose(f);

	response
		.code(CoAP::Message::code::changed)
		.serialize();
}

static void delete_job_handler(engine::message const& request,
								engine::response& response,
								void*) noexcept
{
	ESP_LOGD(RESOURCE_TAG, "Called delete job handler");

	if(!storage_is_mounted())
	{
		response
			.code(CoAP::Message::code::internal_server_error)
			.payload("storage not mounted")
			.serialize();
		return;
	}

	unlink(job_path);

	response
		.code(CoAP::Message::code::deleted)
		.serialize();
}

engine::resource_node res_job("job",
								get_job_handler,
								nullptr,
								put_job_handler,
								delete_job_handler);
