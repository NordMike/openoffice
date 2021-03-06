/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_forms.hxx"

#include "enumeration.hxx"

#include <com/sun/star/container/NoSuchElementException.hpp>
#include <com/sun/star/container/XIndexAccess.hpp>
#include <com/sun/star/lang/WrappedTargetException.hpp>
#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/uno/RuntimeException.hpp>

using com::sun::star::container::NoSuchElementException;
using com::sun::star::container::XIndexAccess;
using com::sun::star::lang::WrappedTargetException;
using com::sun::star::uno::Any;
using com::sun::star::uno::Reference;
using com::sun::star::uno::RuntimeException;


Enumeration::Enumeration( XIndexAccess* pContainer )
    : mxContainer( pContainer ),
      mnIndex( 0 )
{
    OSL_ENSURE( mxContainer.is(), "no container?" );
}

sal_Bool Enumeration::hasMoreElements()
    throw( RuntimeException )
{
    if( ! mxContainer.is() )
        throw RuntimeException();

    return mnIndex < mxContainer->getCount();
}

Any Enumeration::nextElement()
    throw( NoSuchElementException,
           WrappedTargetException,
           RuntimeException )
{
    if( ! mxContainer.is() )
        throw RuntimeException();
    if( mnIndex >= mxContainer->getCount() )
        throw NoSuchElementException();

    return mxContainer->getByIndex( mnIndex++ );
}
