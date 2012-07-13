// **********************************************************************
//
// Copyright (c) 2003-2005 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

#include "Parser.h"
#include <cstring>
#include <expat.h>
#include <list>
#include <fstream>

using namespace std;
using namespace MyUtil;

//
// ParserException
//
ParserException::ParserException(const string& reason) :
    IceUtil::Exception(), _reason(reason)
{
}

ParserException::ParserException(const char* file, int line, const string& reason) :
    IceUtil::Exception(file, line), _reason(reason)
{
}

ParserException::~ParserException() throw()
{
}

const char* ParserException::_name = "ParserException";

string
ParserException::ice_name() const
{
    return _name;
}

void
ParserException::ice_print(std::ostream& out) const
{
    Exception::ice_print(out);
    if(!_reason.empty())
    {
        out << "\n" << _reason;
    }
    else
    {
        out << ":\nXML parser exception";
    }
}

IceUtil::Exception*
ParserException::ice_clone() const
{
    return new ParserException(*this);
}

void
ParserException::ice_throw() const
{
    throw *this;
}

string
ParserException::reason() const
{
    return _reason;
}

//
// Node
//
Node::Node(const NodePtr& parent, const string& name, const string& value, int line, int column) :
    _parent(parent), _name(name), _value(value), _line(line), _column(column)
{
}

Node::~Node()
{
}

NodePtr
Node::getParent() const
{
    return _parent;
}

string
Node::getName() const
{
    return _name;
}

string
Node::getValue() const
{
    return _value;
}

NodeList
Node::getChildren() const
{
    return NodeList();
}

Attributes
Node::getAttributes() const
{
    return Attributes();
}

string
Node::getAttribute(const string&) const
{
    return string();
}

bool
Node::addChild(const NodePtr&)
{
    return false;
}

int
Node::getLine() const
{
    return _line;
}

int
Node::getColumn() const
{
    return _column;
}

//
// Element
//
Element::Element(const NodePtr& parent, const string& name, const Attributes& attributes, int line,
                         int column) :
    Node(parent, name, "", line, column), _attributes(attributes)
{
}

Element::~Element()
{
}

NodeList
Element::getChildren() const
{
    return _children;
}

Attributes
Element::getAttributes() const
{
    return _attributes;
}

string
Element::getAttribute(const string& name) const
{
    Attributes::const_iterator p = _attributes.find(name);
    if(p != _attributes.end())
    {
        return p->second;
    }
    return string();
}

bool
Element::addChild(const NodePtr& child)
{
    _children.push_back(child);
    return true;
}

//
// Text
//
Text::Text(const NodePtr& parent, const string& value, int line, int column) :
    Node(parent, "", value, line, column)
{
}

Text::~Text()
{
}

//
// Document
//
Document::Document() :
    Node(0, "", "", 0, 0)
{
}

Document::~Document()
{
}

NodeList
Document::getChildren() const
{
    return _children;
}

bool
Document::addChild(const NodePtr& child)
{
    _children.push_back(child);
    return true;
}

//
// Handler
//
Handler::~Handler()
{
}

void
Handler::error(const string& msg, int line, int column)
{
    ostringstream out;
    out << "XML error at input line " << line << ", column " << column << ":" << endl << msg;
    throw ParserException(__FILE__, __LINE__, out.str());
}

//
// DocumentBuilder
//
namespace  MyUtil
{

class DocumentBuilder : public Handler
{
public:
    DocumentBuilder();

    virtual void startElement(const string&, const Attributes&, int, int);
    virtual void endElement(const string&, int, int);
    virtual void characters(const string&, int, int);

    DocumentPtr getDocument() const;

private:
    list<NodePtr> _nodeStack;
    DocumentPtr _document;
};

}

DocumentBuilder::DocumentBuilder()
{
    _document = new Document;
    _nodeStack.push_front(_document);
}

void
DocumentBuilder::startElement(const string& name, const Attributes& attributes, int line, int column)
{
    NodePtr parent = _nodeStack.front();

    Element* element = new Element(parent, name, attributes, line, column);
#if NDEBUG
    parent->addChild(element);
#else
    assert(parent->addChild(element));
#endif

    _nodeStack.push_front(element);
}

void
DocumentBuilder::endElement(const string& name, int, int)
{
    assert(!_nodeStack.empty());
    _nodeStack.pop_front();
}

void
DocumentBuilder::characters(const string& data, int line, int column)
{
    NodePtr parent = _nodeStack.front();
    TextPtr text = new Text(parent, data, line, column);
    parent->addChild(text);
}

DocumentPtr
DocumentBuilder::getDocument() const
{
    return _document;
}

//
// expat callbacks
//
struct CallbackData
{
    XML_Parser parser;
    Handler* handler;
};

extern "C"
{

static void
startElementHandler(void* data, const XML_Char* name, const XML_Char** attr)
{
    CallbackData* cb = static_cast<CallbackData*>(data);

    Attributes attributes;
    for(int i = 0; attr[i]; i += 2)
    {
        attributes[attr[i]] = attr[i + 1];
    }

    int line = XML_GetCurrentLineNumber(cb->parser);
    int column = XML_GetCurrentColumnNumber(cb->parser);
    cb->handler->startElement(name, attributes, line, column);
}

static void
endElementHandler(void* data, const XML_Char* name)
{
    CallbackData* cb = static_cast<CallbackData*>(data);
    int line = XML_GetCurrentLineNumber(cb->parser);
    int column = XML_GetCurrentColumnNumber(cb->parser);
    cb->handler->endElement(name, line, column);
}

static void
characterDataHandler(void* data, const XML_Char* s, int len)
{
    CallbackData* cb = static_cast<CallbackData*>(data);

    string str(s, len);
    int line = XML_GetCurrentLineNumber(cb->parser);
    int column = XML_GetCurrentColumnNumber(cb->parser);
    cb->handler->characters(str, line, column);
}

}

//
// Parser
//
DocumentPtr
Parser::parse(const string& file)
{
    DocumentBuilder builder;
    parse(file, builder);
    return builder.getDocument();
}

DocumentPtr
Parser::parseBuffer(const string& in)
{
    DocumentBuilder builder;
    parseBuffer(in, builder);
    return builder.getDocument();
}


DocumentPtr
Parser::parse(istream& in)
{
    DocumentBuilder builder;
    parse(in, builder);
    return builder.getDocument();
}

void
Parser::parse(const string& file, Handler& handler)
{
    ifstream in(file.c_str());
    if(!in.good())
    {
        ostringstream out;
        out << "unable to open file `" << file << "'";
        throw ParserException(__FILE__, __LINE__, out.str());
    }
    parse(in, handler);
}

void
Parser::parse(istream& in, Handler& handler)
{
    XML_Parser parser = XML_ParserCreate(NULL);
    CallbackData cb;
    cb.parser = parser;
    cb.handler = &handler;
    XML_SetUserData(parser, &cb);
    XML_SetElementHandler(parser, startElementHandler, endElementHandler);
    XML_SetCharacterDataHandler(parser, characterDataHandler);

    try
    {
        char buff[1024];
        int isFinal = 0;
        while(!isFinal)
        {
            in.read(buff, 1024);
            if(in.gcount() < 1024)
            {
                isFinal = 1;
            }
            if(XML_Parse(parser, buff, static_cast<int>(in.gcount()), isFinal) != 1)
            {
                handler.error(XML_ErrorString(XML_GetErrorCode(parser)), XML_GetCurrentLineNumber(parser),
                              XML_GetCurrentColumnNumber(parser));
                return;
            }
        }
    }
    catch(...)
    {
        XML_ParserFree(parser);
        throw;
    }

    XML_ParserFree(parser);
}
   
    void
Parser::parseBuffer(const string& in, Handler& handler)
{
    XML_Parser parser = XML_ParserCreate("UTF-8");
    CallbackData cb;
    cb.parser = parser;
    cb.handler = &handler;
    XML_SetUserData(parser, &cb);
    XML_SetElementHandler(parser, startElementHandler, endElementHandler);
    XML_SetCharacterDataHandler(parser, characterDataHandler);

    try
    {
            if(XML_Parse(parser, in.c_str(), strlen(in.c_str()), 1) != 1)
            {
//                cout << XML_ErrorString(XML_GetErrorCode(parser)) << endl;;
 //               cout << XML_GetCurrentLineNumber(parser) << endl;;
  //              cout << XML_GetCurrentColumnNumber(parser) << endl;
                handler.error(XML_ErrorString(XML_GetErrorCode(parser)), XML_GetCurrentLineNumber(parser),
                              XML_GetCurrentColumnNumber(parser));
                return;
            }
    }
    catch(...)
    {
        XML_ParserFree(parser);
        throw;
    }

    XML_ParserFree(parser);
}
