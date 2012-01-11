/* 
 * File:   wtscWrapper.cpp
 * Author: Marc
 * 
 * Created on March 16, 2011, 2:22 PM
 * 
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */

#include "wtscWrapper.h"

wtscWrapper::wtscWrapper() {
    InitializeCriticalSection(&protectedAction);
}

wtscWrapper::~wtscWrapper() {
    DeleteCriticalSection(&protectedAction);
}

void wtscWrapper::enterCS() {
    EnterCriticalSection(&protectedAction);
}

void wtscWrapper::leaveCS() {
    LeaveCriticalSection(&protectedAction);
}