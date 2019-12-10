#pragma once
#include "FieldInfo.hpp"

namespace FixHandler {

	class FixProtocolHandler {
		public:
			FixProtocolHandler() = default;
			void exec();

		private:
			void output( const OrderBook &bidOB, const OrderBook &askOB ) const;
	};

}
