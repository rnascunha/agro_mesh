#ifndef HELPER_HASH_SHA256_HPP__
#define HELPER_HASH_SHA256_HPP__

#include <cstdint>
#include "mbedtls/md.h"
#include <stdio.h>

static constexpr const std::size_t SHA256_SIZE = 32;
using SHA256_hash = std::uint8_t[SHA256_SIZE];

template<std::size_t BufferStackSize = 512>
bool calculate_sha256hash(const char* file_name, SHA256_hash result) noexcept
{
	mbedtls_md_context_t ctx;
	mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;
	std::uint8_t payload[BufferStackSize];
	std::size_t size;

	mbedtls_md_init(&ctx);
	mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 0);
	mbedtls_md_starts(&ctx);
	FILE* f = fopen(file_name, "rb");
	if(!f)
	{
		return false;
	}
	do{
		size = fread(payload, sizeof(std::uint8_t), BufferStackSize, f);
		if(!size) break;
		mbedtls_md_update(&ctx, (const unsigned char *) payload, size);
	}while(true);
	fclose(f);
	mbedtls_md_finish(&ctx, result);
	mbedtls_md_free(&ctx);

	return true;
}

#endif /* HELPER_HASH_SHA256_HPP__ */
