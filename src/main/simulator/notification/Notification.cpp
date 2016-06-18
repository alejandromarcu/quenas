#include <string>
#include <iostream>
#include <fstream>
#include <map>

#include "Command.h"
#include "Notification.h"
#include "Simulator.h"
#include "TypeFilter.h"
#include "Exceptions.h"

namespace simulator {
    namespace notification {
    
using namespace simulator::command;
using namespace simulator;
using namespace std;


//---------------------------------------------------------------------
//----------------------------< XMLFormatter >-------------------------
//---------------------------------------------------------------------

/**
 * @brief Format a query result in XML.
 * 
 * @param qr the query result to be formatted.
 * @result qr formatted in XML.
 */
string XMLFormatter::format(QueryResult *qr)
{
    return formatMultiValue(qr, 1);
}

/**
 * @brief Set the name of the XSLT stylesheet to use.
 *
 * @param name XSLT stylesheet to use.
 */
void XMLFormatter::setStylesheet(const string &name)
{
    stylesheet = name;
}

/**
 * @brief Writes the start tag for XML: <simulation>
 *
 * @return "<simulation>"
 */
string XMLFormatter::start()
{
    string s;

    if (stylesheet != "") s = "<?xml-stylesheet type=\"text/xsl\" href=\"" + stylesheet + "\"?>\n";
    
    return s + "<simulation>\n";
}

/**
 * @brief Writes the end tag for XML: </simulation>
 *
 * @return "</simulation>"
 */
string XMLFormatter::finish()
{
    return "</simulation>\n";
}

/**
 * @brief Format a multi value in XML.
 * All the values contained in the multi value are formatted and concatenated.
 *
 * @param value the value to format.
 * @param indentationLevel indentation level to make XML look nice.
 * @return the multi value formatted in xml.
 */
string XMLFormatter::formatMultiValue(MultiValue *value, int indentationLevel)
{
    MultiValue::TProperties prop = value->getProperties();
    string output;

    string indent0, indent;
    for (int i=1; i< indentationLevel; i++) 
        indent0 += "    ";

    indent = indent0 +  "    ";
    string id = value->getId() == ""? "" : " id=\"" + value->getId() + "\"";
    output += indent0 + "<" + value->getName()  + id + ">\n"; 
    for (int i = 0; i < prop.size(); i++) 
    {
        pair<string, TValue*> p = prop[i];
    
        StringValue *sv = dynamic_cast<StringValue *>(p.second);
        if (sv != NULL) {
            output += indent + "<" + p.first + ">";
            output += sv->getValue();
            output += "</" + p.first + ">\n";        
        } else {
            MultiValue *mv = dynamic_cast<MultiValue *>(p.second);
            if (mv != NULL) {
                output += formatMultiValue(mv, indentationLevel+1);
            } else {                
                BinaryValue *bv = dynamic_cast<BinaryValue *>(p.second);
                if (bv != NULL) {
                    output += indent + "<" + p.first + ">\n";
                    output += formatBinaryValue(*bv, indentationLevel +1) + indent;
                    output += "</" + p.first + ">\n";        
                }                
            }
        }
        

    }
    output += indent0 + "</" + value->getName() + ">\n"; 
    
    return output;    
}

/** 
 * @brief Format a binary value in XML, creating a tag for every 16 bytes.
 *
 * @param bv binary value to format
 * @param indentationLevel indentation level to make XML look nice.
 * @return the binary value formatted in xml.
 */ 
string XMLFormatter::formatBinaryValue(const BinaryValue &bv, int indentationLevel)
{
    VB data = bv.getValue();
    string indent;
    for (int i=0; i< indentationLevel; i++) 
        indent += "    ";
    
    string output = "";
    for (int i = 0; i < data.size(); i++) {
        if ((i % 16) == 0) {
            if (i > 0) output += "</bytes>\n";
            if (i < (data.size() -1)) {
                int to = i + 15;
                if ((data.size() - 1) < to) to = data.size() - 1;
                output += indent + "<bytes from=\"" + toStr(i) + "\" to=\"" + toStr(to) + "\">";
            }
        }       
        output += toHex(data[i]);
        if ((i % 16) != 15) output += " ";
    }
    output += "</bytes>\n";
    
    return output;
}


/**
 * @brief Run a command.
 *       
 * @param function function to run.
 */
TCommandResult *XMLFormatter::runCommand(const Function &function)
{
    if (function.getName() == "setStylesheet") 
    {
        setStylesheet(function.getStringParam(0));
        return this;
    }   
}

/**
 * @brief Get the name of this object.
 * 
 * @return "XMLFormatter"
 */
string XMLFormatter::getName() const
{
    return "XMLFormatter";
}


//---------------------------------------------------------------------
//-----------------------------< MultiValue >--------------------------
//---------------------------------------------------------------------
/**
 * @brief Create a Multi Value
 *
 * @param name name of the object
 * @param id id of the object
 */
MultiValue::MultiValue(const string &name, const string &id) : name(name), id(id)
{
}       

/**
 * @brief Destroy the MultiValue object, returning the property values.
 */
MultiValue::~MultiValue()
{
    for (int i=0; i < properties.size(); i++) 
    {
       if (properties[i].second != NULL) delete properties[i].second;
    }
}
        
/**
 * @brief Return stored properties.
 *
 * @return stored properties.
 */
const MultiValue::TProperties &MultiValue::getProperties() const 
{ 
    return properties; 
}

/**
 * @brief Insert a new key-value property.
 *
 * @param key name of the property.
 * @param value value of the property.
 */
void MultiValue::insert(const string &key, TValue *value) 
{
    properties.push_back(make_pair(key, value));
}

/**
 * @brief Insert a new key-value property, where value is a string.
 *
 * @param key name of the property.
 * @param value string value of the property.
 */
void MultiValue::insert(const string &key, const string &value) 
{
    insert(key, new StringValue(value));
}

/**
 * @brief Get the name of this MultiValue.
 *
 * @return the name of this MultiValue.
 */
string MultiValue::getName() const 
{
    return name;
}

/**
 * @brief Get the id of this MultiValue.
 *
 * @return the id of this MultiValue.
 */
string MultiValue::getId() const 
{
    return id;
}

//---------------------------------------------------------------------
//-----------------------------< QueryResult >-------------------------
//---------------------------------------------------------------------
/**
 * @brief Create an empty query result.
 *
 * @param name name of the query result.
 * @param time time when the query was done; if no specified, current time is used.
 */
QueryResult::QueryResult(const string &name, Time time) : MultiValue(name)
{
    this->time = time.getValue() < 0? Simulator::getInstance()->getTime() : time;
}       

/**
 * @brief Create an empty query result.
 *
 * @param name name of the query result.
 * @param id id of the object
 * @param time time when the query was done; if no specified, current time is used.
 */
QueryResult::QueryResult(const string &name, const string &id, Time time) : MultiValue(name, id)
{
    this->time = time.getValue() < 0? Simulator::getInstance()->getTime() : time;
}       

/**
 * @brief Create a query result containing one property.
 *
 * @param name name of the query result.
 * @param propName name of the property in the query result.
 * @param propValue value of the property in the query result.
 * @param time time when the query was done; if no specified, current time is used.
 */
QueryResult::QueryResult(const string &name, const string &propName, const string &propValue, Time time) : MultiValue(name)
{
    this->time = time.getValue() < 0 ? Simulator::getInstance()->getTime() : time;
    insert(propName, propValue);
}


/**
 * @brief Return the name of the query result.
 *
 * @return the name of the query result.
 */
Time QueryResult::getTime() const 
{
    return time;
}

//---------------------------------------------------------------------
//-----------------------------< Notificator >-------------------------
//---------------------------------------------------------------------
/**
 * @brief Create a notificator that uses XMLFormatter for formatting and
 * that writes to sdt out.
 */
Notificator::Notificator() 
{
    formatter = new XMLFormatter();
    filename = "";
    file = NULL;
}

/**
 * @brief Destroy the Notificator, closing the file if it wasn't closed.
 */
Notificator::~Notificator()
{
    if (file != NULL) close();
}

/**
 * @brief Format a query result and write it to file.
 *
 * @param qr the query result to format.
 */
void Notificator::write(QueryResult *qr)
{
    if (file == NULL) open();
    
    *file << formatter->format(qr); 

}

/**
 * @brief Open the file and writes formatters's start string.
 * If the filename is emtpy, standard output is used.
 */
void Notificator::open()
{
    if (filename == "") {
        file = &std::cout;
    } else {
        file = new ofstream(filename.c_str());    
    }
    *file << formatter->start();
}

/**
 * @brief Write formatter's finish string and closes the file.
 */
void Notificator::close()
{
    if (file == NULL) return;
    
    *file << formatter->finish();     
    delete file;
    file = NULL;
}      

/**
 * @brief Set the formatter to be used.
 *
 * @param formatter formatter to be used.
 */
void Notificator::setFormatter(TFormatter *formatter)
{
    this->formatter = formatter;
}    
    
/**
 * @brief Return the used formatter.
 * 
 * @return the used formatter.
 */
TFormatter *Notificator::getFormatter() const
{
    return formatter;
}


/**
 * @brief Set the name of the output file, closing the previous file.
 *
 * @param fname name of the output file or emtpy ("") to use std output.
 */
void Notificator::setFilename(const string &fname)
{
    close();
    filename = fname;
}


    
}
}
