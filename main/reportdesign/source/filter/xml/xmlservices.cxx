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


#include "precompiled_reportdesign.hxx"
#include <cppuhelper/factory.hxx>
#include <osl/diagnose.h>
#include <cppuhelper/implementationentry.hxx>
#include "xmlfilter.hxx"
#include "xmlExport.hxx"
#include "dbloader2.hxx"
#include "xmlExportDocumentHandler.hxx"
#include "xmlImportDocumentHandler.hxx"


/********************************************************************************************/

using namespace ::rptxml;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::registry;
/********************************************************************************************/
// registry functions
namespace
{

cppu::ImplementationEntry entries[] = {
	{ &ORptFilter::create, &ORptFilter::getImplementationName_Static, &ORptFilter::getSupportedServiceNames_Static,
		&cppu::createSingleComponentFactory, 0, 0 },
	{ &ORptTypeDetection::create, &ORptTypeDetection::getImplementationName_Static, &ORptTypeDetection::getSupportedServiceNames_Static,
		&cppu::createSingleComponentFactory, 0, 0 },
	{ &ODBFullExportHelper::create, &ODBFullExportHelper::getImplementationName_Static, &ODBFullExportHelper::getSupportedServiceNames_Static,
		&cppu::createSingleComponentFactory, 0, 0 },
	{ &ORptExportHelper::create, &ORptExportHelper::getImplementationName_Static, &ORptExportHelper::getSupportedServiceNames_Static,
		&cppu::createSingleComponentFactory, 0, 0 },
	{ &ORptExport::create, &ORptExport::getImplementationName_Static, &ORptExport::getSupportedServiceNames_Static,
		&cppu::createSingleComponentFactory, 0, 0 },
    { &ORptContentExportHelper::create, &ORptContentExportHelper::getImplementationName_Static, &ORptContentExportHelper::getSupportedServiceNames_Static,
		&cppu::createSingleComponentFactory, 0, 0 },
    { &ORptStylesExportHelper::create, &ORptStylesExportHelper::getImplementationName_Static, &ORptStylesExportHelper::getSupportedServiceNames_Static,
		&cppu::createSingleComponentFactory, 0, 0 },
    { &ORptMetaExportHelper::create, &ORptMetaExportHelper::getImplementationName_Static, &ORptMetaExportHelper::getSupportedServiceNames_Static,
		&cppu::createSingleComponentFactory, 0, 0 },

    { &ORptImportHelper::create, &ORptImportHelper::getImplementationName_Static, &ORptImportHelper::getSupportedServiceNames_Static,
		&cppu::createSingleComponentFactory, 0, 0 },
    { &ORptContentImportHelper::create, &ORptContentImportHelper::getImplementationName_Static, &ORptContentImportHelper::getSupportedServiceNames_Static,
		&cppu::createSingleComponentFactory, 0, 0 },
    { &ORptStylesImportHelper::create, &ORptStylesImportHelper::getImplementationName_Static, &ORptStylesImportHelper::getSupportedServiceNames_Static,
		&cppu::createSingleComponentFactory, 0, 0 },
    { &ExportDocumentHandler::create, &ExportDocumentHandler::getImplementationName_Static, &ExportDocumentHandler::getSupportedServiceNames_static,
		&cppu::createSingleComponentFactory, 0, 0 },
    { &ImportDocumentHandler::create, &ImportDocumentHandler::getImplementationName_Static, &ImportDocumentHandler::getSupportedServiceNames_static,
		&cppu::createSingleComponentFactory, 0, 0 },
    { &ORptMetaImportHelper::create, &ORptMetaImportHelper::getImplementationName_Static, &ORptMetaImportHelper::getSupportedServiceNames_Static,
		&cppu::createSingleComponentFactory, 0, 0 },
	{ 0, 0, 0, 0, 0, 0 } 
};
}

extern "C" void * SAL_CALL component_getFactory(
    char const * implName, void * serviceManager, void * registryKey)
{
    return cppu::component_getFactoryHelper(
        implName, serviceManager, registryKey, entries);
}

extern "C" void SAL_CALL component_getImplementationEnvironment(
    char const ** envTypeName, uno_Environment **)
{
    *envTypeName = CPPU_CURRENT_LANGUAGE_BINDING_NAME;
}
