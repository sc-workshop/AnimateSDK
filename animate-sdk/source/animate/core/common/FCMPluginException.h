#pragma once

#include <exception>
#include "core/memory/ref.h"

namespace Animate::Publisher
{
	class SymbolContext;
}

namespace FCM
{
	class FCMPluginException : public std::exception
	{
	public:
		enum class Reason
		{
			UNKNOWN_LIBRARY_ITEM,
			UNKNOWN_FILL_STYLE,
			SERVICE_FAIL,
			SYMBOL_EXPORT_FAIL
		};

	public:
		FCMPluginException(const Animate::Publisher::SymbolContext& context, Reason reason);
		FCMPluginException(Reason reason);

		virtual ~FCMPluginException() = default;

	public:
		const Animate::Publisher::SymbolContext& Symbol() const;
		const Reason ExceptionReason() const;

		const char* what() const noexcept override;

	private:
		wk::Ref<Animate::Publisher::SymbolContext> m_context;
		Reason m_reason;
	};
}