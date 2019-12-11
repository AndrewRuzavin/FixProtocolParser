#include "Parser.hpp"
#include "MessageHandlerDecorator.hpp"
#include "FieldInfo.hpp"
#include <iostream>
#include <exception>

namespace Parser {

	const char* FileException::what() const _GLIBCXX_USE_NOEXCEPT {
		return "File not open.";
	}

	void Parser::RAIILoader::open() {
		is.open( filePath );
		if ( !is ) {
			throw FileException();
		}
	}

	std::stringstream Parser::RAIILoader::read() {
		std::stringstream buf;
		buf << is.rdbuf();
		return buf;
	}

	bool Parser::exec() {
		if ( readMessages() ) {
			while( handleNextMessage() ) {}
			return true;
		} else {
			return false;
		}
	}

	bool Parser::readMessages() {
		try {
			RAIILoader loader( PATH_TO_INPUT_FILE );
			loader.open();
			messages = loader.read().str();
		}
		catch( FileException &e ) {
			std::cout << e.what() << "\n";
			return false;
		}
		return true;
	}

	bool Parser::handleNextMessage() {
		if ( !makeNextMessageRange() ) {
			return false;
		} else {
			const auto entries = takeEntries();
			processEntries( entries );

			return true;
		}
	}

	bool Parser::makeNextMessageRange() {
		messageRange.first = messageRange.second;
		const auto bufMessageRange = findRange( fullTag( getEndTag() ) );
		if ( isNpos( bufMessageRange.second ) ) {
			return false;
		} else {
			const auto sizeOfCheckSum = takeTagValue( getEndTag(), bufMessageRange.second ).length();
			messageRange = std::make_pair( messageRange.second
					, bufMessageRange.second + getEndTagSize() + sizeOfCheckSum + tag( DELIMITER ).length() + 1 );
			refreshGroupRange();
			return true;
		}
	}

	Parser::PosRange Parser::findRange( const std::string &separator ) const {
		const auto lastPos = findPos( separator, messageRange.first );
		return std::make_pair( messageRange.first, lastPos );
	}

	inline
	Parser::Pos Parser::findPos( const std::string &separator, const Pos pos ) const {
		return messages.find( separator, pos );
	}

	inline
	void Parser::refreshGroupRange() {
		groupRange = messageRange;
	}

	inline
	size_t Parser::getEndTagSize() const {
		return fullTagSize( getEndTag() );
	}

	inline
	size_t Parser::fullTagSize( const std::string &tag ) const {
		return fullTag( tag ).size();
	}

	inline
	std::string Parser::fullTag( const std::string &originTag ) const {
		return tag( DELIMITER ) + originTag + "=";
	}

	inline
	std::string Parser::getEndTag() const {
		return tag( CHECK_SUM );
	}

	inline
	bool Parser::isNpos( const size_t posToCompare ) const {
		return std::string::npos == posToCompare;
	}

	inline
	std::string Parser::takeTagValue( const std::string &tag ) const {
		return takeTagValue( tag, messageRange.second );
	}

	std::string Parser::takeTagValue( const std::string &tag, const Pos lastPos ) const {
		const auto range = findRange( fullTag( tag ) );
		if ( isNpos( range.second )
				|| ( range.second > lastPos ) ) {
			return std::string();
		} else {
			const auto posOfValueEnd = findPos( "^", range.second + 1 );
			return std::string( messages.begin() + range.second + fullTagSize( tag )
								, messages.begin() + posOfValueEnd );
		}
	}

	IncrementalRefreshData Parser::takeEntries() const {
		std::unique_ptr<MessageHandler<IncrementalRefreshData>> handler
				= std::make_unique<TypeMessageHandler<IncrementalRefreshData>>( *this );
		handler = std::make_unique<EntryMessageHandler<IncrementalRefreshData>>( *this, std::move(handler) );

		IncrementalRefreshData data;
		data.messageType = tag( INCREMENTAL_REFRESH );

		handler->handle( tag( MSG_TYPE ), data );

		return data;
	}

	void Parser::processEntries( const IncrementalRefreshData &entries ) {
		for ( const auto &entry : entries.groupData ) {

			switch ( entry.updateAction ) {
				case UpdateAction::NEW:
					addEntryToOB( entry );
					break;
				case UpdateAction::CHANGE:
					changeOBEntry( entry );
					break;
				case UpdateAction::DELETE:
				default:
					deleteEntryFromOB( entry );
					break;
			}
		}
	}

	void Parser::addEntryToOB( const IncrementalRefreshGroupData &entry ) {
		entryOperationSideDepend( entry
			, [&]( const auto &entry, auto &ob ) {
					addEntryToOB( entry, ob );
				} );
	}

	template<class Lambda>
		void Parser::entryOperationSideDepend( const IncrementalRefreshGroupData &entry, const Lambda &l ) {
			switch ( entry.side ) {
				case Side::BID:
					l( entry, bidOB );
					break;
				case Side::ASK:
					l( entry, askOB );
					break;
				default:
					break;
			}
		}

	void Parser::addEntryToOB( const IncrementalRefreshGroupData &entry, OrderBook &ob ) {
		if ( isKeyInOB( entry.price, ob ) ) {
//TODO:	при попытке добавить уже существующую запись, следует выдать исключение
			return;
		}

		ob[entry.price] = entry.entrySize;
	}

	bool Parser::isKeyInOB( const OBKey &key, const OrderBook &ob ) const {
		return ob.end() != ob.find( key );
	}

	void Parser::changeOBEntry( const IncrementalRefreshGroupData &entry ) {
		entryOperationSideDepend( entry
			, [&]( const auto &entry, auto &ob ) {
					changeOBEntry( entry, ob );
				} );
	}

	void Parser::changeOBEntry( const IncrementalRefreshGroupData &entry, OrderBook &ob ) {
		if ( !isKeyInOB( entry.price, ob ) ) {
//TODO:	при попытке изменить несуществующую запись, следует выдать исключение
			return;
		}
		ob[entry.price] = entry.entrySize;
	}

	void Parser::deleteEntryFromOB( const IncrementalRefreshGroupData &entry ) {
		entryOperationSideDepend( entry
			, [&]( const auto &entry, auto &ob ) {
					deleteEntryFromOB( entry, ob );
				} );
	}

	void Parser::deleteEntryFromOB( const IncrementalRefreshGroupData &entry, OrderBook &ob ) {
		if ( !isKeyInOB( entry.price, ob ) ) {
//TODO:	при попытке удалить несуществующую запись, следует выдать исключение
			return;
		}
		ob.erase( ob.find( entry.price ) );
	}

	inline
	int Parser::findGroupSize( const std::string &tag ) const {
		return groupIsOver()
				? 0
				: std::atoi( takeTagValue( tag ).c_str() );
	}

	inline
	bool Parser::groupIsOver() const {
		return groupRange.first > groupRange.second;
	}

	void Parser::findNextGroupEntries( const std::string &originTag ) {
		const auto nextTag = tagAfterGroupTag( originTag );
		if ( !groupIsOver()
				&& !nextTag.empty() ) {

			groupRange.first = findPos( fullTag( nextTag ), groupRange.first + 1 );
		}
	}

	std::string Parser::takeTagValueFromGroup( const std::string &tag ) const {
		const auto startPos = findPos( fullTag( tag ), groupRange.first );
		const auto endPos = findPos( "^", startPos );

		if ( isNpos( endPos )
				|| ( endPos > groupRange.second ) ) {
			return std::string();
		} else {
			const auto posOfValueEnd = findPos( "^", endPos + 1 );
			return std::string( messages.begin() + endPos + fullTagSize( tag )
								, messages.begin() + posOfValueEnd );
		}
	}

	const OrderBook& Parser::getBidOB() const {
		return bidOB;
	}

	const OrderBook& Parser::getAskOB() const {
		return askOB;
	}

}
