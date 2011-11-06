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
#include "xmlMasterFields.hxx"
#include "xmlfilter.hxx"
#include <xmloff/xmltoken.hxx>
#include <xmloff/xmlnmspe.hxx>
#include <xmloff/nmspmap.hxx>
#include "xmlEnums.hxx"
#include "xmlReport.hxx"
#include <tools/debug.hxx>


namespace rptxml
{
	using namespace ::com::sun::star;
	using namespace ::com::sun::star::report;
	using namespace ::com::sun::star::uno;
	using namespace ::com::sun::star::xml::sax;
DBG_NAME( rpt_OXMLMasterFields )

OXMLMasterFields::OXMLMasterFields( ORptFilter& rImport,
				sal_uInt16 nPrfx, const ::rtl::OUString& rLName,
				const Reference< XAttributeList > & _xAttrList
				,IMasterDetailFieds* _pReport
                ) :
	SvXMLImportContext( rImport, nPrfx, rLName)
,m_pReport(_pReport)
{
    DBG_CTOR( rpt_OXMLMasterFields,NULL);

	const SvXMLNamespaceMap& rMap = rImport.GetNamespaceMap();
	const SvXMLTokenMap& rTokenMap = rImport.GetSubDocumentElemTokenMap();

    ::rtl::OUString sMasterField,sDetailField;
	const sal_Int16 nLength = (_xAttrList.is()) ? _xAttrList->getLength() : 0;
	for(sal_Int16 i = 0; i < nLength; ++i)
	{
	    ::rtl::OUString sLocalName;
		const rtl::OUString sAttrName = _xAttrList->getNameByIndex( i );
		const sal_uInt16 nPrefix = rMap.GetKeyByAttrName( sAttrName,&sLocalName );
		const rtl::OUString sValue = _xAttrList->getValueByIndex( i );

		switch( rTokenMap.Get( nPrefix, sLocalName ) )
		{
			case XML_TOK_MASTER:
                sMasterField = sValue;
				break;
            case XML_TOK_SUB_DETAIL:
                sDetailField = sValue;
				break;
            default:
                break;
		}
	}
    if ( !sDetailField.getLength() )
        sDetailField = sMasterField;
    if ( sMasterField.getLength() )
        m_pReport->addMasterDetailPair(::std::pair< ::rtl::OUString,::rtl::OUString >(sMasterField,sDetailField));
}
// -----------------------------------------------------------------------------

OXMLMasterFields::~OXMLMasterFields()
{
    DBG_DTOR( rpt_OXMLMasterFields,NULL);
}

// -----------------------------------------------------------------------------
SvXMLImportContext* OXMLMasterFields::CreateChildContext(
		sal_uInt16 _nPrefix,
		const ::rtl::OUString& _rLocalName,
		const Reference< XAttributeList > & xAttrList )
{
	SvXMLImportContext *pContext = 0;
	const SvXMLTokenMap&	rTokenMap	= static_cast<ORptFilter&>(GetImport()).GetSubDocumentElemTokenMap();

	switch( rTokenMap.Get( _nPrefix, _rLocalName ) )
	{
		case XML_TOK_MASTER_DETAIL_FIELD:
			{
				GetImport().GetProgressBarHelper()->Increment( PROGRESS_BAR_STEP );
                pContext = new OXMLMasterFields(static_cast<ORptFilter&>(GetImport()), _nPrefix, _rLocalName,xAttrList ,m_pReport);
            }
            break;
        default:
            break;
	}

	if( !pContext )
		pContext = new SvXMLImportContext( GetImport(), _nPrefix, _rLocalName );

	return pContext;
}

//----------------------------------------------------------------------------
} // namespace rptxml
// -----------------------------------------------------------------------------
