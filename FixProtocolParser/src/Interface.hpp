#pragma once
#include "Parser.hpp"

namespace Interface {

	class Interface {
		public:
			void outputeOrderBook( const OrderBook &bidOB
					, const OrderBook &askOB
					, const int maxEntryOnBook ) const;
			void outputTopBook( const OrderBook &ob, const int maxEntryOnBook ) const;
			template<class Iterator>
				void outputeEntry( const Iterator &entry, const int currentEntry ) const;
			void outputBottomBook( const OrderBook &ob, const int maxEntryOnBook ) const;
			void outputTotalSize( const std::string name, const int size ) const;
			void outputDelimiter() const;
	};

	void outputeOrderBook( const OrderBook &bidOB
				, const OrderBook &askOB
				, const int maxEntryOnBook );
}
