#ifndef _NOTIFICATION_H
#define _NOTIFICATION_H

#include <string>
#include <iostream>
#include <vector>

#include "Units.h"
#include "TCommandRunner.h"

namespace simulator {
    namespace notification {

using namespace std;
using namespace simulator;
using namespace simulator::command;

 
 
/**
 * @brief Base class that represents a value to be written in a notification.
 */
class TValue {
    public:
        /**
         * @brief Virtual destructor required for polymorphism.
         */
        virtual ~TValue() {};
};


/**
 * @brief Stores a string value.
 */
class StringValue : public TValue {
    public:
        /**
         * @brief Create a StringValue.
         *
         * @param value the value to be stored.
         */
        StringValue(const string &value) : value(value) {};
        
        /**
         * @brief Return the stored string value.
         *
         * @return the stored string value.
         */
        string getValue() const { return value; };
    private:
        /// Stored value
        string value;
};

/**
 * @brief Stores a binary value.
 */
class BinaryValue : public TValue {
    public:
        /**
         * @brief Create a BinaryValue.
         *
         * @param value the value to be stored.
         */
        BinaryValue(const VB& value) : value(value) {};
        
        /**
         * @brief Return the stored binary value.
         *
         * @return the stored binary value.
         */
        const VB &getValue() const { return value; };
    private:
        /// Stored value
        VB value;
};

/**
 * @brief Stores a collection of key-value properties.
 * Each value can be any TValue instance, incuding this class, so nested
 * values are available.
 */
class MultiValue : public TValue {
    public:
        typedef vector<pair<string, TValue*> > TProperties;

        MultiValue(const string &name, const string &id = "");
        
        virtual ~MultiValue();
        
        const TProperties &getProperties() const;

        void insert(const string &key, TValue *value);
        void insert(const string &key, const string &value);

        string getName() const;
        string getId() const;

    protected:
        /// name of the query result.
        string name;
        
        /// identification of the object
        string id;
        
        /// collection of stored key-value properties
        TProperties properties;

    private:
        MultiValue(const MultiValue &m) {};
        MultiValue& operator=(const MultiValue &m) { return *this;};
};

/**
 * @brief Result of a Query.  It's just a MultiValue with time.
 */
class QueryResult : public MultiValue {
    public:
        QueryResult(const string &name="", Time time = -1);
        QueryResult(const string &name, const string &id, Time time = -1);
        QueryResult(const string &name, const string &propName, const string &propValue, Time time = -1);
        Time getTime() const;

    private:
        /// time when the query was done.
        Time time;
};


/**
 * @brief Base class for notification formatting.
 */
class TFormatter : public TCommandRunner {
    public:
        /**
         * @brief Virtual destructor required for polymorphism.
         */        
        virtual ~TFormatter() {};
        
        /**
         * @brief Formats a query result.
         * 
         * @param qr the query result to format.
         * @return the query resulted formatted.
         */
        virtual string format(QueryResult *qr) = 0;
        
        /**
         * @brief Return a string that needs to be written at start.
         *
         * @return a string that needs to be written at start.
         */
        virtual string start() = 0;

        /**
         * @brief Return a string that needs to be written at end.
         *
         * @return a string that needs to be written at end.
         */
        virtual string finish() = 0;        
};


/**
 * @brief Formats a query result in XML.
 */
class XMLFormatter : public TFormatter {
    public:
        virtual string format(QueryResult *qr);
        virtual string start();
        virtual string finish();  
        
        void setStylesheet(const string &name);      

        virtual TCommandResult *runCommand(const Function &function);
        virtual string getName() const;
        
    private:
        string formatMultiValue(MultiValue *value, int indentationLevel);
        string formatBinaryValue(const BinaryValue &bv, int indentationLevel);
        
        string stylesheet;
};


/**
 * @brief This class formats a query result and writes it to a file or std output.
 */
class Notificator {
    public:
        Notificator();
        ~Notificator();
        
        void write(QueryResult *qr);
        
        void open();
        void close();        

        void setFormatter(TFormatter *formatter);
        TFormatter *getFormatter() const;
        
        void setFilename(const string &fname);
    private:
        /// Pointer to the formatter for query results.
        TFormatter *formatter;
        
        /// Output file.
        ostream *file;  
        
        /// Name of the output file or empty to use std output.      
        string filename;
};        

/**
 * @brief Represents an object that can be queryed.
 */
class TQueryable {
    public:
        virtual QueryResult *query(const vector<string> *options = NULL) const = 0;    
};

        
}
}


#endif
