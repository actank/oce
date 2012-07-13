#ifndef __TAG_H__
#define __TAG_H__

#include <string>
#include <list>
#include <utility>
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include "pugixml.hpp"

namespace com {
namespace xiaonei {
namespace talk {
namespace util {

using namespace pugi;
typedef IceUtil::Handle<IceUtil::Shared> SharedPtr;
//---------------------------------------------------------------------------
class TagHandler {
public:
//	virtual void started(Ice::ObjectPtr channel) = 0;
//	virtual void stopped(Ice::ObjectPtr channel) = 0;
	virtual void messaged(SharedPtr channel, xml_document_ptr& doc) = 0;
  virtual ~TagHandler(){}
};

class TagParser : public IceUtil::Shared {
public:
	/**
	 * Constructs a new Parser object.
	 * @param ph The object to send incoming Tags to.
	 */
	//TagParser(TagHandlerPtr handler);
	TagParser(TagHandler* _handler);
	/**
	 * Virtual destructor.
	 */
	virtual ~TagParser();
	//virtual void handleTag(xml_document_ptr& doc) = 0;
	/**
	 * Use this function to feed the parser with more XML.
	 * @param data Raw xml to parse.
	 * @return Returns @b true if parsing was successful, @b false otherwise.
	 */
	bool feed(const SharedPtr& channel, const std::string& data);

private:
	void addTag(const SharedPtr& channel);
	void addAttribute();
	void addCData();
	bool closeTag(const SharedPtr& channel);
	void cleanup();
	bool isWhitespace(unsigned char c);
	bool isValid(unsigned char c);
	//void streamEvent(Tag *tag);
	void streamEvent(const SharedPtr& channel, xml_document_ptr& tag);

	enum ParserInternalState
	{
		Initial,
		TagOpening,
		TagOpeningSlash,
		TagOpeningLt,
		TagInside,
		TagNameCollect,
		TagNameComplete,
		TagAttribute,
		TagAttributeComplete,
		TagAttributeEqual,
		TagClosing,
		TagClosingSlash,
		TagValueApos,
		TagValue,
		TagPreamble
	};

	TagHandler* _handler;
	xml_node _current;
	xml_node _root;
	xml_document_ptr _doc;

	ParserInternalState _state;
	std::map<std::string,std::string> _attribs;

	std::string _name;
	std::string _cdata;
	std::string _attrib;
	std::string _value;
	int _preamble;
	bool _quote;

};
typedef IceUtil::Handle<TagParser> TagParserPtr;
}
}
}
}
#endif // TAG_H__
