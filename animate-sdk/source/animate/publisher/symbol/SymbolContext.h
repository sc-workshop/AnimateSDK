#pragma once

#include <stdint.h>
#include <memory>
#include <string>

#include "animate/publisher/symbol/SlicingContext.h"
//#include "AnimateDOM.h"

namespace Animate {
	namespace Publisher
	{
		class SymbolContext
		{
		public:
			enum class SymbolType : uint8_t
			{
				Unknown = 0,
				MovieClip,
				Graphic
			};

		public:
			SymbolContext(FCM::AutoPtr<DOM::ILibraryItem> item, const std::string& linkage_name = "");
			SymbolContext(const std::u16string& name, SymbolType type);

		public:
			const std::u16string name;
			const std::string linkage_name;
			const SymbolType type = SymbolType::Unknown;

			SlicingContext slicing;
		private:
			static std::u16string GetName(FCM::AutoPtr<DOM::ILibraryItem> symbol);
			static SymbolType GetType(FCM::AutoPtr<DOM::ILibraryItem> symbol);
		};
	}
}