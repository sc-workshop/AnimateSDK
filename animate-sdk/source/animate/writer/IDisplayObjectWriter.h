#pragma once

#include "AnimateCore.h"
#include "AnimateDOM.h"

#include <functional>

namespace Animate::Publisher
{
	struct GlowFilter
	{
		double blurX = 0.0;
		double blurY = 0.0;
		int strength = 0;
		DOM::Utils::COLOR color;
	};

	class IDisplayObjectWriter
	{
	public:
		IDisplayObjectWriter(SymbolContext& context) : m_symbol(context) {};
		virtual ~IDisplayObjectWriter() = default;

	public:
		virtual void SetGlowFilter(const GlowFilter&) { };

	public:
		std::size_t HashCode() const
		{
			if (!m_hash_code)
			{
				m_hash_code = GenerateHash();
			}

			return m_hash_code;
		}

	protected:
		virtual std::size_t GenerateHash() const = 0;

	public:
		/// <summary>
		/// Writer must finalize object and add it to its own resource palette here
		/// </summary>
		/// <param name="id"> Identifier of object </param>
		/// <param name="required"> If True then writer must return positive status, else writer can skip object writing and return False </param>
		/// <returns> True if object was written </returns>
		virtual bool Finalize(uint16_t id, bool required) = 0;

	protected:
		SymbolContext& m_symbol;
		mutable std::size_t m_hash_code = 0;
	};

	struct DisplayObjectWriterHasher {
		size_t operator()(const std::size_t& key) const {
			return key;
		}
	};
}