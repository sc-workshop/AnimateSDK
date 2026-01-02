#pragma once

#include "AnimateDOM.h"
#include "animate/publisher/ResourceReference.h"

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

	struct DisplayObjectWriterHasher {
		static inline std::size_t counter = 0;
		size_t operator()(const std::size_t& key) const {
			return key;
		}
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
				if (!m_hash_code) m_hash_code = ++DisplayObjectWriterHasher::counter;
			}

			return m_hash_code;
		}

	protected:
		virtual std::size_t GenerateHash() const = 0;

	public:
		/// <summary>
		/// Writer must finalize object and add it to its own resource palette here
		/// </summary>
		/// <param name="reference"> Reference of object in library </param>
		/// <param name="required"> If True then writer must return positive status, else writer can skip object writing and return False </param>
		/// <param name="new_symbol"> True when symbol is new in library, otherwise it is used only to notify about object id </param>
		/// <returns> True if object was written </returns>
		virtual bool Finalize(ResourceReference reference, bool required, bool new_symbol) = 0;

	protected:
		SymbolContext& m_symbol;
		mutable std::size_t m_hash_code = 0;
	};
}