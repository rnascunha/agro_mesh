#include "esp_log.h"
#include "../coap_engine.hpp"

extern const char* RESOURCE_TAG;

static void get_rssi_handler(engine::message const& request,
								engine::response& response, void*) noexcept
{
	ESP_LOGD(RESOURCE_TAG, "Called get net rssi handler");

	/**
	 * Making the payload
	 */
	char buffer[20];

	wifi_ap_record_t ap;
	esp_wifi_sta_get_ap_info(&ap);
	snprintf(buffer, 20, "%d", ap.rssi);

	CoAP::Message::content_format format = CoAP::Message::content_format::text_plain;
	CoAP::Message::Option::node content{format};

	/**
	 * Sending response (always call serialize)
	 */
	response
			.code(CoAP::Message::code::content)
			.add_option(content)
			.payload(buffer)
			.serialize();
}

static void put_waive_root_handler(engine::message const&,
								engine::response& response, void*) noexcept
{
	ESP_LOGD(RESOURCE_TAG, "Called put net waive root handler");

	if(!esp_mesh_is_root())
	{
		response
			.code(CoAP::Message::code::precondition_failed)
			.payload("not root")
			.serialize();
	}
	esp_mesh_waive_root(nullptr, MESH_VOTE_REASON_ROOT_INITIATED);

	/**
	 * Sending response (always call serialize)
	 */
	response
			.code(CoAP::Message::code::success)
			.serialize();
}

static void get_parent_handler(engine::message const&,
								engine::response& response, void*) noexcept
{
	ESP_LOGD(RESOURCE_TAG, "Called get net parent handler");

	mesh_addr_t daddy{};
	esp_mesh_get_parent_bssid(&daddy);

	CoAP::Message::content_format format = CoAP::Message::content_format::text_plain;
	CoAP::Message::Option::node content{format};

	char buffer[20];
	snprintf(buffer, 20, MACSTR, MAC2STR(daddy.addr));

	response
		.code(CoAP::Message::code::content)
		.add_option(content)
		.payload(buffer)
		.serialize();
}

engine::resource_node res_net_rssi{"rssi",
								get_rssi_handler};
engine::resource_node res_net_waive_root{"waive",
								static_cast<engine::resource::callback_t*>(nullptr),
								nullptr,
								put_waive_root_handler};
engine::resource_node res_net_parent{"parent",
								get_parent_handler};
