#ifndef DATA_WRAPPER_ICE
#define DATA_WRAPPER_ICE

#include <Util.ice>

module MyUtil
{

    exception RowSetNullHandleException extends MceException{};
    exception OutOfRowSetIndexRangeException extends MceException {};
    
    class Column
    {
        Str2IntMap value;
    };
    
    class Row
    {
        StrSeq value;
    };
    
    sequence<Row> RowSeq;
    
    class RowSet
    {
    	Column keys;
    	RowSeq rows;
    };   
};
#endif
