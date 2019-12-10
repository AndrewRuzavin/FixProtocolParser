#include "FixProtocolHandler.hpp"
#include "Interface.hpp"
#include "Parser.hpp"

namespace FixHandler {

	void FixProtocolHandler::exec() {
		Parser::Parser parser;
		if ( parser.exec() ) {
			const auto &bidOB = parser.getBidOB();
			const auto &askOB = parser.getAskOB();

			output( bidOB, askOB );
		}
	}

	void FixProtocolHandler::output( const OrderBook &bidOB, const OrderBook &askOB ) const {
		const auto maxEntryOnBook = 5;
		Interface::outputeOrderBook( bidOB, askOB, maxEntryOnBook );
	}

}
