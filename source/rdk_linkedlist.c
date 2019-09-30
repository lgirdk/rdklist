/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2019 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/
#include <stdio.h>
#include "rdk_linkedlist.h"

rdkList_t* gListHead = NULL;
rdkList_t* gListTail = NULL;
//unsigned int gNumberOfEntries;

rdkList_t* rdk_list_add_node (rdkList_t* rdkListHndl, void* pData)
{
    rdkList_t* tmpHndl;
    tmpHndl = (rdkList_t*) malloc (sizeof(rdkList_t));
    if (!tmpHndl)
    {
        printf ("Failed to allocate memory\n");
    }
    else
    {
        tmpHndl->m_pUserData = pData;
    }

    if ((!rdkListHndl) && (gListHead))
    {
        printf ("List head existed but asked to create new HEAD; existing List is going to be memory leak\n");
    }

    /* Add to List */
    insque(tmpHndl, gListTail);

    /* Update the List */
    if (!rdkListHndl)
    {
        /* Update the handle */
        rdkListHndl = tmpHndl;

        /* Update the head & tail */
        gListHead = gListTail = rdkListHndl;
        printf ("Added first element\n");
    }
    else
    {
        /* update the tail */
        gListTail = tmpHndl;
        printf ("Added 2nd element\n");
    }

    return rdkListHndl;
}

void rdk_list_delete_node (rdkList_t* rdkListHndl)
{
    if (gListHead && rdkListHndl)
    {
        if (gListHead == rdkListHndl)
        {
            gListHead = gListHead->m_pForward;
            remque(rdkListHndl);
            free (rdkListHndl);
        }
        else if (gListTail == rdkListHndl)
        {
            gListTail = gListTail->m_pBackward;
            remque(rdkListHndl);
            free (rdkListHndl);
        }
        else
        {
            int found = 0;
            rdkList_t* tmpHndl = gListHead;
            while (tmpHndl->m_pForward)
            {
                tmpHndl = tmpHndl->m_pForward;
                if (rdkListHndl == tmpHndl)
                {
                    remque(rdkListHndl);
                    free(tmpHndl);
                    found = 1;
                    break;
                }
            }
            if (0 == found)
            {
                printf ("Given handle is not present in the list. Is it Fake!?!\n");
            }
        }
    }
    return ;
}

rdkList_t* rdk_list_find_first_node (rdkList_t* rdkListHndl)
{
    if (gListHead && rdkListHndl)
    {
        while (rdkListHndl->m_pBackward)
        {
            rdkListHndl = rdkListHndl->m_pBackward;
        }
        if (rdkListHndl != gListHead)
        {
            printf ("From a given handle, we got into the first Node; BUT the node is not in the same as HEAD.\n");
        }
    }
    else if (gListHead)
    {
        return gListHead;
    }

    return rdkListHndl;
}

rdkList_t* rdk_list_find_next_node (rdkList_t* rdkListHndl)
{
    if (gListHead && rdkListHndl)
    {
        return rdkListHndl->m_pForward;
    }

    return rdkListHndl;
}

rdkList_t* rdk_list_find_node_custom (rdkList_t* rdkListHndl, void* pData, fnRDKListCustomCompare compareFunction)
{
    if (gListHead && rdkListHndl && compareFunction)
    {
        while (rdkListHndl)
        {
            if (0 == compareFunction(pData, rdkListHndl->m_pUserData))
            {
                return rdkListHndl;
            }
            else
                rdkListHndl = rdkListHndl->m_pForward;
        }
    }

    return NULL;
}

void rdk_list_free_all_nodes (rdkList_t* rdkListHndl)
{

    if (gListHead && rdkListHndl)
    {
        rdkList_t* tmpHndl = gListHead; 
        do
        {
            gListHead = tmpHndl->m_pForward;
            remque(tmpHndl);
            free (tmpHndl);
            tmpHndl = gListHead;
        }while (gListHead != NULL);

        gListHead = NULL;
        gListTail = NULL;
    }
    return;
}

void rdk_list_foreach (rdkList_t* rdkListHndl, fnRDKListCustomExecute executeFunction, void* pUserActionData)
{
    if (rdkListHndl && executeFunction)
    {
        while (rdkListHndl)
        {
            rdkList_t* tmpHndl = rdkListHndl->m_pForward;
            executeFunction(rdkListHndl->m_pUserData, pUserActionData);
            rdkListHndl = tmpHndl;
        }
    }
}

void rdk_list_free_all_nodes_custom (rdkList_t* rdkListHndl, fnRDKListCustomFree freeFunction)
{
    if (gListHead && rdkListHndl && freeFunction)
    {
        while (rdkListHndl)
        {
            rdkList_t* tmpHndl = rdkListHndl->m_pForward;
            freeFunction(rdkListHndl->m_pUserData);
            rdk_list_delete_node(rdkListHndl);
            rdkListHndl = tmpHndl;
        }
    }
}

