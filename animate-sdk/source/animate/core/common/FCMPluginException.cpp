#include "FCMPluginException.h"

#include "animate/publisher/symbol/SymbolContext.h"

namespace FCM
{
	FCMPluginException::FCMPluginException(const Animate::Publisher::SymbolContext& context, Reason reason) : 
		m_context(wk::CreateRef<Animate::Publisher::SymbolContext>(context)),
		m_reason(reason)
	{
	}

	FCMPluginException::FCMPluginException(Reason reason) : 
		m_reason(reason), 
		m_context(wk::CreateRef<Animate::Publisher::SymbolContext>(u"", Animate::Publisher::SymbolContext::SymbolType::Unknown))
	{
	}

	const Animate::Publisher::SymbolContext& FCMPluginException::Symbol() const { return *m_context.get(); }
	const FCMPluginException::Reason FCMPluginException::ExceptionReason() const { return m_reason; }

	const char* FCMPluginException::what() const noexcept {
		switch (m_reason)
		{
		case Reason::UNKNOWN_LIBRARY_ITEM:
			return "Unknown library item type in library";
		case Reason::UNKNOWN_FILL_STYLE:
			return "Unknown fill style in symbol";
		case Reason::SERVICE_FAIL:
			return "Failed to get FCM serivce";
		case Reason::SYMBOL_EXPORT_FAIL:
			return "Failed to export symbol";
		default:
			return "";
		}
	}
}