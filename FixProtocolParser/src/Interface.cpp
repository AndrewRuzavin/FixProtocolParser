#include "Interface.hpp"
#include <iostream>

namespace Interface {

	void Interface::outputeOrderBook( const OrderBook &bidOB
										, const OrderBook &askOB
										, const int maxEntryOnBook ) const {
		std::cout << "Order Book:\n";
		outputTotalSize( "SELL", askOB.size() );
		outputTopBook( askOB, maxEntryOnBook );
		outputDelimiter();
		outputBottomBook( bidOB, maxEntryOnBook );
		outputTotalSize( "BUY", bidOB.size() );

	}

	void Interface::outputTopBook( const OrderBook &ob, const int maxEntryOnBook ) const {
		auto currentElem = maxEntryOnBook - 1;
		const auto startIter = std::next( ob.rbegin(), ob.size() - maxEntryOnBook );

		for ( auto entryIter = startIter
				; entryIter != ob.rend()
				; ++entryIter, --currentElem ) {

			outputeEntry( *entryIter, currentElem );
		}
	}

	template<class Iterator>
		void Interface::outputeEntry( const Iterator &entry, const int currentEntry ) const {
			std::cout << "[" << currentEntry << "] " << "price: "
					  << entry.first << " (" << entry.second << ")\n";
		}

	void Interface::outputBottomBook( const OrderBook &ob, const int maxEntryOnBook ) const {
		auto currentElem = 0;
		const auto maxIter = std::next( ob.begin(), maxEntryOnBook );
		for ( auto entryIter = ob.begin()
				; ( entryIter != ob.end() )
					&& entryIter != maxIter
				; ++entryIter, ++currentElem ) {

			outputeEntry( *entryIter, currentElem );
		}
	}

	void Interface::outputTotalSize( const std::string name, const int size ) const {
		std::cout << "Total " << name << ": " << size << "\n";
	}

	void Interface::outputDelimiter() const {
		std::cout << "======================\n";
	}

	void outputeOrderBook( const OrderBook &bidOB
				, const OrderBook &askOB
				, const int maxEntryOnBook ) {
		Interface interface;
		interface.outputeOrderBook( bidOB, askOB, maxEntryOnBook );
	}

}

