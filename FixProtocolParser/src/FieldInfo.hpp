#pragma once
#include<string>
#include<array>
#include<map>
#include<vector>

enum TagsEnum {
	DELIMITER = 0,
	CHECK_SUM,
	MSG_TYPE,
	INCREMENTAL_REFRESH,
	NUM_OF_ENTRIES,
	ENTRY_TYPE,
	ENTRY_PRICE,
	ENTRY_SIZE,
	UPDATE_ACTION,

	TAGS_SIZE
};

enum class Side {
	BID = 0,
	ASK,
	TRADE,
	NONE,
};

using OBKey = double;
using OrderBook = std::map<OBKey, double>;

namespace {

	class FieldInfo;

	class FieldInfoDestroyer {
		private:
			FieldInfo* p_instance;
		public:
			~FieldInfoDestroyer();
			void initialize( FieldInfo* p );
	};

	class FieldInfo {
			using Tag = std::string;
			using Tags = std::array<Tag, TAGS_SIZE>;
			using GroupTagFollowedBy = std::vector<std::pair<Tag, Tag>>;
		public:
			FieldInfo( const FieldInfo& ) = delete;
			FieldInfo& operator=( FieldInfo& ) = delete;

			static FieldInfo& getInstance();
			std::string getTag( const TagsEnum tag );
			std::string getTagAfterGroupTag( const std::string& tag );

		private:
			static FieldInfo* p_instance;
			static FieldInfoDestroyer destroyer;
			FieldInfo() = default;
			~FieldInfo() = default;

			static void init();

			friend class FieldInfoDestroyer;

			static Tags tags;
			static GroupTagFollowedBy groupTagFollowedBy;
	};

}

std::string tag( const TagsEnum tag );
std::string tagAfterGroupTag( const std::string& tag );
