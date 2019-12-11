#pragma once
#include "Parser.hpp"

namespace Parser {

	//MessageHandler
	template<class T>
	class MessageHandler {
		protected:
			using Pos = size_t;
			using HandlerP = std::unique_ptr<MessageHandler<T>>;
		public:
			MessageHandler() = default;
			MessageHandler( const Parser &parser ) : parser( parser ) {}
			virtual ~MessageHandler() = default;
			virtual bool handle( const std::string &tag, T &data ) = 0;

		protected:
			Parser parser;
	};


	//EmptyMessageHandler
	template<class T>
	class EmptyMessageHandler : public MessageHandler<T> {
			using Pos = typename MessageHandler<T>::Pos;
		public:
			bool handle( const std::string &tag, T &data ) final override {}
	};


	//TypeMessageHandler
	template<class T>
	class TypeMessageHandler : public MessageHandler<T> {
			using Pos = typename MessageHandler<T>::Pos;
			using HandlerP = typename MessageHandler<T>::HandlerP;
		public:
			explicit TypeMessageHandler( const Parser &parser
				, HandlerP handler = std::unique_ptr<EmptyMessageHandler<T>>() )
					: MessageHandler<T>( parser ), handler( std::move(handler) ) {}

			bool handle( const std::string &tag, T &data ) override;

		private:
			bool operation( const std::string &tag, T &data );

			HandlerP handler;
	};

	template<class T>
		bool TypeMessageHandler<T>::handle( const std::string &tag, T &data ) {
			if ( handler ) {
				handler->handle( tag, data );
			}
			return operation( tag, data );
		}

	template<class T>
		bool TypeMessageHandler<T>::operation( const std::string &tag, T &data ) {
			const auto val = this->parser.takeTagValue( tag );
			return val == data.messageType;
		}


	//EntryMessageHandler
	template<class T>
	class EntryMessageHandler : public MessageHandler<T> {
			using Pos = typename MessageHandler<T>::Pos;
			using HandlerP = typename MessageHandler<T>::HandlerP;
		public:
			explicit EntryMessageHandler( const Parser &parser, HandlerP handler )
				: MessageHandler<T>( parser ), handler( std::move(handler) ) {}

			bool handle( const std::string &tag, T &data ) override;

		private:
			void operation( T &data );
			bool nextGroup( const std::string &tag );
			UpdateAction takeUpdateAction() const;
			Side takeSide() const;
			int takeIntField( const std::string &tag ) const;
			double takeDoubleField( const std::string &tag ) const;
			HandlerP handler;
	};

	template<class T>
		bool EntryMessageHandler<T>::handle( const std::string &tag, T &data ) {
			if ( handler->handle( tag, data ) ) {
				operation( data );
				return true;
			} else {
				return false;
			}
		}

	template<class T>
		void EntryMessageHandler<T>::operation( T &data ) {
			const auto numOfEntries = this->parser.findGroupSize( tag( NUM_OF_ENTRIES ) );
			for ( auto i = 0; i < numOfEntries; ++i ) {
				if ( !nextGroup( tag( NUM_OF_ENTRIES ) ) ) {
					break;
				}

				IncrementalRefreshGroupData groupData;
				groupData.updateAction = takeUpdateAction();
				groupData.side = takeSide();
				groupData.price = takeDoubleField( tag( ENTRY_PRICE ) );
				if ( UpdateAction::DELETE != groupData.updateAction ) {
					groupData.entrySize = takeDoubleField( tag( ENTRY_SIZE ) );
				}
				data.groupData.push_back( groupData );
			}
		}

	template<class T>
		bool EntryMessageHandler<T>::nextGroup( const std::string &tag ) {
			this->parser.findNextGroupEntries( tag );
			return !this->parser.groupIsOver();
		}

	template<class T>
		UpdateAction EntryMessageHandler<T>::takeUpdateAction() const {
			const auto updateAction = takeIntField( tag( UPDATE_ACTION ) );

			switch ( updateAction ) {
				case 0:
					return UpdateAction::NEW;
				case 1:
					return UpdateAction::CHANGE;
				case 2:
				default:
					return UpdateAction::DELETE;
			}
		}

	template<class T>
		Side EntryMessageHandler<T>::takeSide() const {
			const auto sideNum = takeIntField( tag( ENTRY_TYPE ) );

			switch ( sideNum ) {
				case 0:
					return Side::BID;
				case 1:
					return Side::ASK;
				case 2:
					return Side::TRADE;
				default:
					return Side::NONE;
			}
		}

	template<class T>
		int EntryMessageHandler<T>::takeIntField( const std::string &tag ) const {
			const auto priceStr = this->parser.takeTagValueFromGroup( tag );
			return std::stoi( priceStr.c_str() );
		}

	template<class T>
		double EntryMessageHandler<T>::takeDoubleField( const std::string &tag ) const {
			const auto priceStr = this->parser.takeTagValueFromGroup( tag );
			return std::stod( priceStr );
		}
}
