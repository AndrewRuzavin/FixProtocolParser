TEMPLATE = subdirs

DISTFILES += \
	config.pri \
        Task/file1.txt \
        Task/test_task.txt \

SUBDIRS += FixProtocolParser FixProtocolParserTest

CONFIG += ordered
