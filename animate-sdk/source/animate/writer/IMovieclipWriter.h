#pragma once

#include <vector>
#include <string>
#include <memory>
#include <optional>

#include "AnimateCore.h"
#include "AnimateDOM.h"

namespace Animate::Publisher
{
	class SharedMovieclipWriter {
	public:
		virtual ~SharedMovieclipWriter() = default;

	protected:
		uint32_t m_position = 0;

	public:
		void Next() {
			m_position++;
		};

		virtual void InitializeTimeline(uint32_t fps, uint32_t frameCount) = 0;

		virtual void SetLabel(const std::u16string& label) = 0;

		virtual void AddFrameElement(
			uint16_t id,
			FCM::BlendMode blending,
			const std::u16string& name,
			std::optional<DOM::Utils::MATRIX2D> matrix,
			std::optional<DOM::Utils::COLOR_MATRIX> color
		) = 0;

		/// <summary>
		/// Writer must finalize object and add it to its own resource palette here
		/// </summary>
		/// <param name="id"> Identifier of object </param>
		/// <param name="required"> If True then writer must return positive status, else writer can skip object writing and return False </param>
		/// <returns> True if object was written </returns>
		virtual bool Finalize(uint16_t id, bool required) = 0;
	};
}