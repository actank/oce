#include "gtest/gtest.h"
#include "third-party/mongo-c-driver/bson.h"
#include "third-party/mongo-c-driver/mongo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

TEST(mongo, connection) {
    mongo_connection conn[1]; /* ptr */
  
    int status = mongo_connect( conn, "10.3.16.96", 27017 );

    switch (status) {
        case mongo_conn_success: printf( "connection succeeded\n" ); break;
        case mongo_conn_bad_arg: printf( "bad arguments\n" ); break;
        case mongo_conn_no_socket: printf( "no socket\n" ); break;
        case mongo_conn_fail: printf( "connection failed\n" ); break;
        case mongo_conn_not_master: printf( "not master\n" ); break;
    }


    mongo_cursor *cursor;
    bson empty[1];
    bson_empty( empty );

    cursor = mongo_find( conn, "test.bar", empty, empty, 0, 0, 0 );
    while( mongo_cursor_next( cursor ) ) {
        bson_print( &cursor->current );
    }

    mongo_cursor_destroy( cursor );
    bson_destroy( empty );

    /* CODE WILL GO HERE */ 

    mongo_destroy( conn );
    printf( "\nconnection closed\n" );
}


