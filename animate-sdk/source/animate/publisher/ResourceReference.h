#pragma once

#include <cstdint>

namespace Animate::Publisher
{
	class ResourceReference {
	public:
		static inline uint16_t Null = UINT16_MAX;

	public:
		ResourceReference(uint16_t identifier = Null) : id(identifier) {}

	public:
		uint16_t GetId() const {
			return id;
		}

		bool IsNull() const {
			return id == UINT16_MAX;
		}

	public:
		explicit operator bool() const {
			return !IsNull();
		}

	private:
		uint16_t id;
	};
}