#pragma once
#include <fstream>
#include <sstream>
#include <memory>
#include <vector>
#include "FieldInfo.hpp"
#include <exception>

namespace Parser {

	class FileException : public std::exception {
		public:
			const char* what() const _GLIBCXX_USE_NOEXCEPT override;
	};

	struct IncrementalRefreshGroupData {
			double price = 0;
			Side side = Side::NONE;
			double entrySize = 0;
	};

	struct IncrementalRefreshData {
			std::string messageType;
			std::vector<IncrementalRefreshGroupData> groupData;
	};

	class Parser {
			using OBKey = double;
			using Pos = size_t;
			using PosRange = std::pair<Pos, Pos>;

			class RAIILoader {
				public:
					explicit RAIILoader( const std::string &filePath )
						: filePath( filePath ) {}

					~RAIILoader() {
						is.close();
					}

					void open();
					std::stringstream read();

				private:
					std::string filePath;
					std::ifstream is;
			};

		public:
			explicit Parser() = default;

			bool exec();
			void refreshGroupRange();
			bool isNpos( const size_t posToCompare ) const;
			std::string takeTagValue( const std::string &tag ) const;
			int findGroupSize( const std::string &tag ) const;
			bool groupIsOver() const;
			void findNextGroupEntries( const std::string &originTag );
			std::string takeTagValueFromGroup( const std::string &tag ) const;
			const OrderBook& getBidOB() const;
			const OrderBook& getAskOB() const;

		private:
			bool readMessages();
			bool handleNextMessage();
			bool makeNextMessageRange();
			PosRange findRange( const std::string &separator ) const;
			Pos findPos( const std::string &separator, const Pos pos ) const;
			size_t getEndTagSize() const;
			size_t fullTagSize( const std::string &tag ) const;
			std::string fullTag( const std::string &originTag ) const;
			std::string getEndTag() const;
			IncrementalRefreshData takeEntries() const;
			void addEntries( const IncrementalRefreshData &entries );
			void addToOB( const IncrementalRefreshGroupData &entry, OrderBook &ob );
			bool isKeyInOB( const OBKey &key, const OrderBook &ob ) const;
			std::string takeTagValue( const std::string &tag, const Pos lastPos ) const;

			std::string messages;
			OrderBook bidOB;	//покупатель
			OrderBook askOB;	//продавец

			PosRange messageRange;
			PosRange groupRange;
	};

}
