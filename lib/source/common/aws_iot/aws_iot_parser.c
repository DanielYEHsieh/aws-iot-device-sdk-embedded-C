/*
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 * @file aws_iot_parser.c
 * @brief Parses topics for Thing Name and status.
 */

/* The config header is always included first. */
#include "iot_config.h"

/* Standard includes. */
#include <string.h>

/* AWS IoT include. */
#include "private/aws_iot.h"

/* Error handling include. */
#include "private/iot_error.h"

/**
 * @brief Minimum allowed topic length for an AWS IoT status topic.
 *
 * Topics must contain at least:
 * - The common prefix
 * - The suffix "/accepted" or "/rejected"
 * - 1 character for the Thing Name
 * - 2 characters for the operation name and the enclosing slashes
 */
#define MINIMUM_TOPIC_NAME_LENGTH                   \
    ( uint16_t ) ( AWS_IOT_TOPIC_PREFIX_LENGTH +    \
                   AWS_IOT_ACCEPTED_SUFFIX_LENGTH + \
                   1 + 2 )

/*-----------------------------------------------------------*/

bool AwsIot_ParseThingName( const char * pTopicName,
                            uint16_t topicNameLength,
                            const char ** pThingName,
                            size_t * pThingNameLength )
{
    IOT_FUNCTION_ENTRY( bool, true );
    const char * pThingNameStart = NULL;
    size_t thingNameLength = 0;

    /* Check that the topic name is at least as long as the minimum allowed. */
    if( topicNameLength < MINIMUM_TOPIC_NAME_LENGTH )
    {
        IOT_SET_AND_GOTO_CLEANUP( false );
    }

    /* Check that the given topic starts with the common prefix. */
    if( strncmp( AWS_IOT_TOPIC_PREFIX,
                 pTopicName,
                 AWS_IOT_TOPIC_PREFIX_LENGTH ) != 0 )
    {
        IOT_SET_AND_GOTO_CLEANUP( false );
    }

    /* The Thing Name starts immediately after the topic prefix. */
    pThingNameStart = pTopicName + AWS_IOT_TOPIC_PREFIX_LENGTH;

    /* Calculate the length of the Thing Name, which is terminated with a '/'. */
    while( ( thingNameLength + AWS_IOT_TOPIC_PREFIX_LENGTH < ( size_t ) topicNameLength ) &&
           ( pThingNameStart[ thingNameLength ] != '/' ) )
    {
        thingNameLength++;
    }

    /* The end of the topic name was reached without finding a '/'. The topic
     * name is invalid. */
    if( thingNameLength + AWS_IOT_TOPIC_PREFIX_LENGTH >= ( size_t ) topicNameLength )
    {
        IOT_SET_AND_GOTO_CLEANUP( false );
    }

    /* Set the output parameters. */
    *pThingName = pThingNameStart;
    *pThingNameLength = thingNameLength;

    IOT_FUNCTION_EXIT_NO_CLEANUP();
}

/*-----------------------------------------------------------*/

AwsIotStatus_t AwsIot_ParseStatus( const char * pTopicName,
                                   uint16_t topicNameLength )
{
    IOT_FUNCTION_ENTRY( AwsIotStatus_t, AWS_IOT_UNKNOWN );
    const char * pSuffixStart = NULL;

    /* Check that the status topic name is at least as long as the
     * "accepted" suffix. */
    if( topicNameLength > AWS_IOT_ACCEPTED_SUFFIX_LENGTH )
    {
        /* Calculate where the "accepted" suffix should start. */
        pSuffixStart = pTopicName + topicNameLength - AWS_IOT_ACCEPTED_SUFFIX_LENGTH;

        /* Check if the end of the status topic name is "/accepted". */
        if( strncmp( pSuffixStart,
                     AWS_IOT_ACCEPTED_SUFFIX,
                     AWS_IOT_ACCEPTED_SUFFIX_LENGTH ) == 0 )
        {
            IOT_SET_AND_GOTO_CLEANUP( AWS_IOT_ACCEPTED );
        }
    }

    /* Check that the status topic name is at least as long as the
     * "rejected" suffix. */
    if( topicNameLength > AWS_IOT_REJECTED_SUFFIX_LENGTH )
    {
        /* Calculate where the "rejected" suffix should start. */
        pSuffixStart = pTopicName + topicNameLength - AWS_IOT_REJECTED_SUFFIX_LENGTH;

        /* Check if the end of the status topic name is "/rejected". */
        if( strncmp( pSuffixStart,
                     AWS_IOT_REJECTED_SUFFIX,
                     AWS_IOT_REJECTED_SUFFIX_LENGTH ) == 0 )
        {
            IOT_SET_AND_GOTO_CLEANUP( AWS_IOT_REJECTED );
        }
    }

    IOT_FUNCTION_EXIT_NO_CLEANUP();
}

/*-----------------------------------------------------------*/