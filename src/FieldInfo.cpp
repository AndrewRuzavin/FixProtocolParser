#include "FieldInfo.hpp"
#include <algorithm>

namespace {

	FieldInfo::Tags FieldInfo::tags;
	FieldInfo::GroupTagFollowedBy FieldInfo::groupTagFollowedBy;

	FieldInfo *FieldInfo::p_instance = 0;
	FieldInfoDestroyer FieldInfo::destroyer;

	FieldInfoDestroyer::~FieldInfoDestroyer() {
		delete p_instance;
	}

	void FieldInfoDestroyer::initialize( FieldInfo* p ) {
		p_instance = p;
	}

	FieldInfo& FieldInfo::getInstance() {
		if ( !p_instance ) {
			p_instance = new FieldInfo();
			destroyer.initialize( p_instance );
			init();
		}
		return *p_instance;
	}

	void FieldInfo::init() {
		tags[DELIMITER] = "^";
		tags[CHECK_SUM] = "10";
		tags[MSG_TYPE] = "35";
		tags[DATA_REQUEST] = "V";
		tags[SNAPSHOT] = "W";
		tags[INCREMENTAL_REFRESH] = "X";
		tags[NUM_OF_ENTRIES] = "268";
		tags[ENTRY_TYPE] = "269";
		tags[ENTRY_PRICE] = "270";
		tags[ENTRY_SIZE] = "271";
		tags[UPDATE_ACTION] = "279";

		groupTagFollowedBy.emplace_back( std::make_pair( std::make_pair( MessageType::W, "268" ), "269" ) );
		groupTagFollowedBy.emplace_back( std::make_pair( std::make_pair( MessageType::X, "268" ), "279" ) );
	}

	std::string FieldInfo::getTag( const TagsEnum tag ) {
		return tags.at( tag );
	}

	std::string FieldInfo::getTagAfterGroupTag( const std::string &tag, const MessageType mt ) {
		const auto iter = std::find_if( groupTagFollowedBy.begin()
				, groupTagFollowedBy.end()
				, [&tag, &mt]( const auto &val ) {
						return val.first == std::make_pair( mt, tag );
					} );

		return groupTagFollowedBy.end() == iter
			? std::string()
			: iter->second;
	}

}

std::string tag( const TagsEnum tag ) {
	if ( tag == TAGS_SIZE ) {
		return std::string();
	} else {
		return FieldInfo::getInstance().getTag( tag );
	}
}

std::string messTypeTag( const MessageType mt ) {
	switch ( mt ) {
		case MessageType::V:
			return tag( DATA_REQUEST );
		case MessageType::W:
			return tag( SNAPSHOT );
		case MessageType::X:
			return tag( INCREMENTAL_REFRESH );
		default:
			return std::string();
	}
}

std::string tagAfterGroupTag( const std::string& tag, const MessageType mt ) {
	return FieldInfo::getInstance().getTagAfterGroupTag( tag, mt );
}

