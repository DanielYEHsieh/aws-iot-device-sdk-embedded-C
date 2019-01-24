/*
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/**
 * @file aws_iot_tests_mqtt.c
 * @brief Test runner for the MQTT tests on POSIX systems.
 */

/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* POSIX includes. */
#include <signal.h>

/* Test framework includes. */
#include "unity_fixture.h"

/*-----------------------------------------------------------*/

/**
 * @brief Signal handler. Terminates the tests if called.
 */
static void _signalHandler( int signum )
{
    /* Immediately terminate the tests if this signal handler is called. */
    if( signum == SIGSEGV )
    {
        printf( "\nSegmentation fault.\n" );
        exit( EXIT_FAILURE );
    }
    else if( signum == SIGABRT )
    {
        printf( "\nAssertion failed.\n" );
        exit( EXIT_FAILURE );
    }
}

/*-----------------------------------------------------------*/

int main( int argc,
          char ** argv )
{
    struct sigaction signalAction;

    /* Set a signal handler for segmentation faults and assertion failures. */
    ( void ) memset( &signalAction, 0x00, sizeof( struct sigaction ) );
    signalAction.sa_handler = _signalHandler;

    if( sigaction( SIGSEGV, &signalAction, NULL ) != 0 )
    {
        return -1;
    }

    if( sigaction( SIGABRT, &signalAction, NULL ) != 0 )
    {
        return -1;
    }

    /* Unity setup. */
    UnityFixture.Verbose = 1;
    UnityFixture.RepeatCount = 1;
    UnityFixture.NameFilter = NULL;
    UnityFixture.GroupFilter = NULL;
    UNITY_BEGIN();

    /* Run short tests. */
    RUN_TEST_GROUP( MQTT_Unit_Subscription );
    RUN_TEST_GROUP( MQTT_Unit_Validate );
    RUN_TEST_GROUP( MQTT_Unit_Receive );
    RUN_TEST_GROUP( MQTT_Unit_API );
    RUN_TEST_GROUP( MQTT_System );

    /* The stress tests may take several minutes to run. Only run them if the
     * -l command line argument was given. */
    if( getopt( argc, argv, "l" ) != -1 )
    {
        RUN_TEST_GROUP( MQTT_Stress );
    }

    /* Return the number of test failures. This will cause a non-zero exit code
     * if any test fails. */
    return UNITY_END();
}

/*-----------------------------------------------------------*/