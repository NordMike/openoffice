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



#if !TEST_LAYOUT
// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_sc.hxx"
#endif /* !TEST_LAYOUT */

#undef SC_DLLIMPLEMENTATION



#include <vcl/msgbox.hxx>
#include <i18npool/mslangid.hxx>
#include <svtools/collatorres.hxx>
#include <unotools/collatorwrapper.hxx>
#include <unotools/localedatawrapper.hxx>
#include <comphelper/processfactory.hxx>

#include "scitems.hxx"
#include "uiitems.hxx"
#include "viewdata.hxx"
#include "document.hxx"
#include "global.hxx"
#include "dbcolect.hxx"
#include "userlist.hxx"
#include "rangeutl.hxx"
#include "scresid.hxx"
#include "sc.hrc"       // -> Slot IDs
#include "globstr.hrc"

#include "sortdlg.hxx"
#include "sortdlg.hrc"

#define _TPSORT_CXX
#include "tpsort.hxx"
#undef _TPSORT_CXX

using namespace com::sun::star;

// STATIC DATA -----------------------------------------------------------

static USHORT pSortRanges[] =
{
	SID_SORT,
	SID_SORT,
	0
};

// -----------------------------------------------------------------------

/*
 * Da sich Einstellungen auf der zweiten TabPage (Optionen) auf
 * die erste TabPage auswirken, muss es die Moeglichkeit geben,
 * dies der jeweils anderen Seite mitzuteilen.
 *
 * Im Moment wird dieses Problem ueber zwei Datenmember des TabDialoges
 * geloest. Wird eine Seite Aktiviert/Deaktiviert, so gleicht sie diese
 * Datenmember mit dem eigenen Zustand ab (->Activate()/Deactivate()).
 *
 * 31.01.95:
 * Die Klasse SfxTabPage bietet mittlerweile ein Verfahren an:
 *
 * virtual BOOL HasExchangeSupport() const; -> return TRUE;
 * virtual void ActivatePage(const SfxItemSet &);
 * virtual int	DeactivatePage(SfxItemSet * = 0);
 *
 * muss noch geaendert werden!
 */

//========================================================================
//========================================================================
// Sortierkriterien-Tabpage:

ScTabPageSortFields::ScTabPageSortFields( Window*			pParent,
										  const SfxItemSet& rArgSet )

	:	SfxTabPage		( pParent,
						  ScResId( RID_SCPAGE_SORT_FIELDS ),
						  rArgSet ),
		//
        aFlSort1        ( this, ScResId( FL_SORT1  ) ),
		aLbSort1		( this, ScResId( LB_SORT1  ) ),
        aBtnUp1         ( this, ScResId( BTN_UP1   ) ),
		aBtnDown1		( this, ScResId( BTN_DOWN1 ) ),
		//
        aFlSort2        ( this, ScResId( FL_SORT2  ) ),
		aLbSort2		( this, ScResId( LB_SORT2  ) ),
        aBtnUp2         ( this, ScResId( BTN_UP2   ) ),
		aBtnDown2		( this, ScResId( BTN_DOWN2 ) ),
		//
        aFlSort3        ( this, ScResId( FL_SORT3  ) ),
		aLbSort3		( this, ScResId( LB_SORT3  ) ),
        aBtnUp3         ( this, ScResId( BTN_UP3   ) ),
		aBtnDown3		( this, ScResId( BTN_DOWN3 ) ),

		aStrUndefined	( ScResId( SCSTR_UNDEFINED ) ),
		aStrColumn		( ScResId( SCSTR_COLUMN ) ),
		aStrRow 		( ScResId( SCSTR_ROW ) ),
		//
#if !TEST_LAYOUT
		nWhichSort		( rArgSet.GetPool()->GetWhich( SID_SORT ) ),
#else /* TEST_LAYOUT */
		nWhichSort		( 0 ),
#endif /* TEST_LAYOUT */
		pDlg			( (ScSortDlg*)(GetParent()->GetParent()) ),
		pViewData		( NULL ),
#if !TEST_LAYOUT
		rSortData		( ((const ScSortItem&)
						   rArgSet.Get( nWhichSort )).
								GetSortData() ),
#else /* TEST_LAYOUT */
		rSortData		( *new ScSortParam() ),
#endif /* TEST_LAYOUT */
		nFieldCount 	( 0 ),
		bHasHeader		( FALSE ),
		bSortByRows 	( FALSE )
{
	Init();
	FreeResource();
	SetExchangeSupport();
}

// -----------------------------------------------------------------------

__EXPORT ScTabPageSortFields::~ScTabPageSortFields()
{
}

// -----------------------------------------------------------------------

void ScTabPageSortFields::Init()
{
#if !TEST_LAYOUT
	const ScSortItem& rSortItem = (const ScSortItem&)
								  GetItemSet().Get( nWhichSort );

	pViewData = rSortItem.GetViewData();

	DBG_ASSERT( pViewData, "ViewData not found!" );
#endif /* !TEST_LAYOUT */

	nFieldArr[0] = 0;
    nFirstCol = 0;
    nFirstRow = 0;

	aLbSort1.SetSelectHdl( LINK( this, ScTabPageSortFields, SelectHdl ) );
	aLbSort2.SetSelectHdl( LINK( this, ScTabPageSortFields, SelectHdl ) );
	aLbSort3.SetSelectHdl( LINK( this, ScTabPageSortFields, SelectHdl ) );
	aLbSort1.Clear();
	aLbSort2.Clear();
	aLbSort3.Clear();

	aSortLbArr[0]		= &aLbSort1;
	aSortLbArr[1]		= &aLbSort2;
	aSortLbArr[2]		= &aLbSort3;
	aDirBtnArr[0][0]	= &aBtnUp1;
	aDirBtnArr[0][1]	= &aBtnDown1;
	aDirBtnArr[1][0]	= &aBtnUp2;
	aDirBtnArr[1][1]	= &aBtnDown2;
	aDirBtnArr[2][0]	= &aBtnUp3;
	aDirBtnArr[2][1]	= &aBtnDown3;
    aFlArr[0]           = &aFlSort1;
    aFlArr[1]           = &aFlSort2;
    aFlArr[2]           = &aFlSort3;
}

//------------------------------------------------------------------------

USHORT* __EXPORT ScTabPageSortFields::GetRanges()
{
	return pSortRanges;
}

// -----------------------------------------------------------------------

SfxTabPage* __EXPORT ScTabPageSortFields::Create( Window*	pParent,
										 const SfxItemSet&	rArgSet )
{
	return ( new ScTabPageSortFields( pParent, rArgSet ) );
}

// -----------------------------------------------------------------------

void __EXPORT ScTabPageSortFields::Reset( const SfxItemSet& /* rArgSet */ )
{
	bSortByRows = rSortData.bByRow;
	bHasHeader	= rSortData.bHasHeader;

	if ( aLbSort1.GetEntryCount() == 0 )
		FillFieldLists();

	// Selektieren der ListBoxen:

	if ( rSortData.bDoSort[0] )
	{
		for ( USHORT i=0; i<3; i++ )
		{
			if ( rSortData.bDoSort[i] )
			{
				aSortLbArr[i]->SelectEntryPos(
					 GetFieldSelPos( rSortData.nField[i] ) );

				(rSortData.bAscending[i])
					? aDirBtnArr[i][0]->Check() 	// Up
					: aDirBtnArr[i][1]->Check();	// Down
			}
			else
			{
				aSortLbArr[i]->SelectEntryPos( 0 ); // "keiner" selektieren
				aDirBtnArr[i][0]->Check();			// Up
			}
		}

		EnableField( 1 );
		EnableField( 2 );
		EnableField( 3 );
		if ( aLbSort1.GetSelectEntryPos() == 0 )
			DisableField( 2 );
		if ( aLbSort2.GetSelectEntryPos() == 0 )
			DisableField( 3 );
	}
	else
	{
		aLbSort1.SelectEntryPos( 1 );
		aLbSort2.SelectEntryPos( 0 );
		aLbSort3.SelectEntryPos( 0 );
		aBtnUp1.Check();
		aBtnUp2.Check();
		aBtnUp3.Check();
		EnableField ( 1 );
		EnableField ( 2 );
		DisableField( 3 );
	}

	if ( pDlg )
	{
		pDlg->SetByRows ( bSortByRows );
		pDlg->SetHeaders( bHasHeader );
	}
}

// -----------------------------------------------------------------------

BOOL __EXPORT ScTabPageSortFields::FillItemSet( SfxItemSet& rArgSet )
{
	ScSortParam theSortData = rSortData;
	if (pDlg)
	{
		const SfxItemSet* pExample = pDlg->GetExampleSet();
		const SfxPoolItem* pItem;
		if ( pExample && pExample->GetItemState( nWhichSort, TRUE, &pItem ) == SFX_ITEM_SET )
			theSortData = ((const ScSortItem*)pItem)->GetSortData();
	}

	USHORT	nSort1Pos = aLbSort1.GetSelectEntryPos();
	USHORT	nSort2Pos = aLbSort2.GetSelectEntryPos();
	USHORT	nSort3Pos = aLbSort3.GetSelectEntryPos();

	DBG_ASSERT(    (nSort1Pos <= SC_MAXFIELDS)
				&& (nSort2Pos <= SC_MAXFIELDS)
				&& (nSort3Pos <= SC_MAXFIELDS),
				"Array-Range Fehler!" );

	if ( nSort1Pos == LISTBOX_ENTRY_NOTFOUND ) nSort1Pos = 0;
	if ( nSort2Pos == LISTBOX_ENTRY_NOTFOUND ) nSort2Pos = 0;
	if ( nSort3Pos == LISTBOX_ENTRY_NOTFOUND ) nSort3Pos = 0;

	if ( nSort1Pos > 0 )
	{
		theSortData.bDoSort[0] = (nSort1Pos > 0);
		theSortData.bDoSort[1] = (nSort2Pos > 0);
		theSortData.bDoSort[2] = (nSort3Pos > 0);

		// wenn auf Optionen-Seite "OK" gewaehlt wurde und
		// dabei die Sortierrichtung umgestellt wurde, so
		// wird das erste Feld der jeweiligen Richtung als
		// Sortierkriterium gewaehlt (steht in nFieldArr[0]):
		if ( bSortByRows != pDlg->GetByRows() )
		{
			theSortData.nField[0] =
			theSortData.nField[1] =
            theSortData.nField[2] = ( bSortByRows ?
                    static_cast<SCCOLROW>(nFirstRow) :
                    static_cast<SCCOLROW>(nFirstCol) );
		}
		else
		{
			theSortData.nField[0] = nFieldArr[nSort1Pos];
			theSortData.nField[1] = nFieldArr[nSort2Pos];
			theSortData.nField[2] = nFieldArr[nSort3Pos];
		}

		theSortData.bAscending[0] = aBtnUp1.IsChecked();
		theSortData.bAscending[1] = aBtnUp2.IsChecked();
		theSortData.bAscending[2] = aBtnUp3.IsChecked();
		//	bHasHeader ist in ScTabPageSortOptions::FillItemSet, wo es hingehoert
	}
	else
	{
		theSortData.bDoSort[0] =
		theSortData.bDoSort[1] =
		theSortData.bDoSort[2] = FALSE;
	}

	rArgSet.Put( ScSortItem( SCITEM_SORTDATA, NULL, &theSortData ) );

	return TRUE;
}

// -----------------------------------------------------------------------

// fuer Datenaustausch ohne Dialog-Umweg: (! noch zu tun !)
// void ScTabPageSortFields::ActivatePage( const SfxItemSet& rSet )

void __EXPORT ScTabPageSortFields::ActivatePage()
{
	if ( pDlg )
	{
		if (   bHasHeader  != pDlg->GetHeaders()
			|| bSortByRows != pDlg->GetByRows()   )
		{
			USHORT	nCurSel1 = aLbSort1.GetSelectEntryPos();
			USHORT	nCurSel2 = aLbSort2.GetSelectEntryPos();
			USHORT	nCurSel3 = aLbSort3.GetSelectEntryPos();

			bHasHeader	= pDlg->GetHeaders();
			bSortByRows = pDlg->GetByRows();
			FillFieldLists();
			aLbSort1.SelectEntryPos( nCurSel1 );
			aLbSort2.SelectEntryPos( nCurSel2 );
			aLbSort3.SelectEntryPos( nCurSel3 );
		}
	}
}

// -----------------------------------------------------------------------

int __EXPORT ScTabPageSortFields::DeactivatePage( SfxItemSet* pSetP )
{
	if ( pDlg )
	{
		if ( bHasHeader != pDlg->GetHeaders() )
			pDlg->SetHeaders( bHasHeader );

		if ( bSortByRows != pDlg->GetByRows() )
			pDlg->SetByRows( bSortByRows );
	}

    if ( pSetP )
        FillItemSet( *pSetP );

	return SfxTabPage::LEAVE_PAGE;
}

// -----------------------------------------------------------------------

void ScTabPageSortFields::DisableField( USHORT nField )
{
	nField--;

    if ( nField<=2 )
	{
		aSortLbArr[nField]	 ->Disable();
		aDirBtnArr[nField][0]->Disable();
		aDirBtnArr[nField][1]->Disable();
        aFlArr[nField]       ->Disable();
	}
}

// -----------------------------------------------------------------------

void ScTabPageSortFields::EnableField( USHORT nField )
{
	nField--;

    if ( nField<=2 )
	{
		aSortLbArr[nField]	 ->Enable();
		aDirBtnArr[nField][0]->Enable();
		aDirBtnArr[nField][1]->Enable();
        aFlArr[nField]       ->Enable();
	}
}

// -----------------------------------------------------------------------

void ScTabPageSortFields::FillFieldLists()
{
	if ( pViewData )
	{
		ScDocument* pDoc = pViewData->GetDocument();

		if ( pDoc )
		{
			aLbSort1.Clear();
			aLbSort2.Clear();
			aLbSort3.Clear();
			aLbSort1.InsertEntry( aStrUndefined, 0 );
			aLbSort2.InsertEntry( aStrUndefined, 0 );
			aLbSort3.InsertEntry( aStrUndefined, 0 );

			SCCOL	nFirstSortCol	= rSortData.nCol1;
			SCROW	nFirstSortRow	= rSortData.nRow1;
			SCTAB	nTab		= pViewData->GetTabNo();
			USHORT	i			= 1;

			if ( bSortByRows )
			{
				String	aFieldName;
				SCCOL	nMaxCol = rSortData.nCol2;
				SCCOL	col;

				for ( col=nFirstSortCol; col<=nMaxCol && i<SC_MAXFIELDS; col++ )
				{
					pDoc->GetString( col, nFirstSortRow, nTab, aFieldName );
					if ( !bHasHeader || (aFieldName.Len() == 0) )
					{
						aFieldName	= aStrColumn;
						aFieldName += ' ';
						aFieldName += ColToAlpha( col );
					}
					nFieldArr[i] = col;
					aLbSort1.InsertEntry( aFieldName, i );
					aLbSort2.InsertEntry( aFieldName, i );
					aLbSort3.InsertEntry( aFieldName, i );
					i++;
				}
			}
			else
			{
				String	aFieldName;
				SCROW	nMaxRow = rSortData.nRow2;
				SCROW	row;

				for ( row=nFirstSortRow; row<=nMaxRow && i<SC_MAXFIELDS; row++ )
				{
					pDoc->GetString( nFirstSortCol, row, nTab, aFieldName );
					if ( !bHasHeader || (aFieldName.Len() == 0) )
					{
						aFieldName	= aStrRow;
						aFieldName += ' ';
						aFieldName += String::CreateFromInt32( row+1 );
					}
					nFieldArr[i] = row;
					aLbSort1.InsertEntry( aFieldName, i );
					aLbSort2.InsertEntry( aFieldName, i );
					aLbSort3.InsertEntry( aFieldName, i );
					i++;
				}
			}
			nFieldCount = i;
		}
	}
}

//------------------------------------------------------------------------

USHORT ScTabPageSortFields::GetFieldSelPos( SCCOLROW nField )
{
	USHORT	nFieldPos	= 0;
	BOOL	bFound		= FALSE;

	for ( USHORT n=1; n<nFieldCount && !bFound; n++ )
	{
		if ( nFieldArr[n] == nField )
		{
			nFieldPos = n;
			bFound = TRUE;
		}
	}

	return nFieldPos;
}

// -----------------------------------------------------------------------
// Handler:
//---------

IMPL_LINK( ScTabPageSortFields, SelectHdl, ListBox *, pLb )
{
	String aSelEntry = pLb->GetSelectEntry();

	if ( pLb == &aLbSort1 )
	{
		if ( aSelEntry == aStrUndefined )
		{
			aLbSort2.SelectEntryPos( 0 );
			aLbSort3.SelectEntryPos( 0 );

            if ( aFlSort2.IsEnabled() )
				DisableField( 2 );

            if ( aFlSort3.IsEnabled() )
				DisableField( 3 );
		}
		else
		{
            if ( !aFlSort2.IsEnabled() )
				EnableField( 2 );
		}
	}
	else if ( pLb == &aLbSort2 )
	{
		if ( aSelEntry == aStrUndefined )
		{
			aLbSort3.SelectEntryPos( 0 );
            if ( aFlSort3.IsEnabled() )
				DisableField( 3 );
		}
		else
		{
            if ( !aFlSort3.IsEnabled() )
				EnableField( 3 );
		}
	}
	return 0;
}

//========================================================================
// Sortieroptionen-Tabpage:
//========================================================================

#include <layout/layout-pre.hxx>

#if ENABLE_LAYOUT
#undef ScResId
#define ScResId(x) #x
#undef SfxTabPage
#define SfxTabPage( parent, id, args ) SfxTabPage( parent, "sort-options.xml", id, &args )
#endif /* ENABLE_LAYOUT */

ScTabPageSortOptions::ScTabPageSortOptions( Window* 			pParent,
											const SfxItemSet&	rArgSet )

	:	SfxTabPage		( pParent,
						  ScResId( RID_SCPAGE_SORT_OPTIONS ),
						  rArgSet ),
		//
		aBtnCase		( this, ScResId( BTN_CASESENSITIVE ) ),
		aBtnHeader		( this, ScResId( BTN_LABEL ) ),
		aBtnFormats 	( this, ScResId( BTN_FORMATS ) ),
		aBtnCopyResult	( this, ScResId( BTN_COPYRESULT ) ),
		aBtnNaturalSort	( this, ScResId( BTN_NATURALSORT ) ),
		aLbOutPos		( this, ScResId( LB_OUTAREA ) ),
		aEdOutPos		( this, ScResId( ED_OUTAREA ) ),
		aBtnSortUser	( this, ScResId( BTN_SORT_USER ) ),
		aLbSortUser 	( this, ScResId( LB_SORT_USER ) ),
        aFtLanguage     ( this, ScResId( FT_LANGUAGE ) ),
		aLbLanguage		( this, ScResId( LB_LANGUAGE ) ),
        aFtAlgorithm    ( this, ScResId( FT_ALGORITHM ) ),
		aLbAlgorithm	( this, ScResId( LB_ALGORITHM ) ),
		aLineDirection	( this, ScResId( FL_DIRECTION ) ),
		aBtnTopDown 	( this, ScResId( BTN_TOP_DOWN ) ),
		aBtnLeftRight	( this, ScResId( BTN_LEFT_RIGHT ) ),
// 		aFtAreaLabel	( this, ScResId( FT_AREA_LABEL ) ),
//		aFtArea 		( this, ScResId( FT_AREA ) ),
		//
#if ENABLE_LAYOUT
#undef this
#undef ScResId
#define ScResId(x) this, #x
#endif /* ENABLE_LAYOUT */
		aStrRowLabel	( ScResId( STR_ROW_LABEL ) ),
		aStrColLabel	( ScResId( STR_COL_LABEL ) ),
		aStrUndefined	( ScResId( SCSTR_UNDEFINED ) ),
		aStrNoName		( ScGlobal::GetRscString(STR_DB_NONAME) ),
		//
#if !TEST_LAYOUT
		nWhichSort		( rArgSet.GetPool()->GetWhich( SID_SORT ) ),
		rSortData		( ((const ScSortItem&)
						  rArgSet.Get( nWhichSort )).GetSortData() ),
#else /* TEST_LAYOUT */
		nWhichSort		( 0 ),
		rSortData		( *new ScSortParam() ),
#endif /* TEST_LAYOUT */
		pViewData		( NULL ),
		pDoc			( NULL ),
		pDlg			( (ScSortDlg*)(GetParent() ? GetParent()->GetParent() : 0 ) ),
		pColRes			( NULL ),
		pColWrap		( NULL )
{
#if TEST_LAYOUT
    (void) rArgSet;
#endif /* TEST_LAYOUT */
	Init();
	FreeResource();
	SetExchangeSupport();
}

// -----------------------------------------------------------------------

__EXPORT ScTabPageSortOptions::~ScTabPageSortOptions()
{
#if !TEST_LAYOUT
	USHORT nEntries = aLbOutPos.GetEntryCount();

	for ( USHORT i=1; i<nEntries; i++ )
		delete (String*)aLbOutPos.GetEntryData( i );
#endif /* !TEST_LAYOUT */

	delete pColRes;
	delete pColWrap;		//! not if from document
}

// -----------------------------------------------------------------------

void ScTabPageSortOptions::Init()
{
// 	aStrAreaLabel = aFtAreaLabel.GetText();
// 	aStrAreaLabel.Append( (sal_Unicode) ' ' );

	//	CollatorRessource has user-visible names for sort algorithms
	pColRes = new CollatorRessource();

	//!	use CollatorWrapper from document?
	pColWrap = new CollatorWrapper( comphelper::getProcessServiceFactory() );

#if !TEST_LAYOUT
	const ScSortItem&	rSortItem = (const ScSortItem&)
									GetItemSet().Get( nWhichSort );
#endif /* !TEST_LAYOUT */

	aLbOutPos.SetSelectHdl	  ( LINK( this, ScTabPageSortOptions, SelOutPosHdl ) );
	aBtnCopyResult.SetClickHdl( LINK( this, ScTabPageSortOptions, EnableHdl ) );
	aBtnSortUser.SetClickHdl  ( LINK( this, ScTabPageSortOptions, EnableHdl ) );
	aBtnTopDown.SetClickHdl	  ( LINK( this, ScTabPageSortOptions, SortDirHdl ) );
	aBtnLeftRight.SetClickHdl ( LINK( this, ScTabPageSortOptions, SortDirHdl ) );
	aLbLanguage.SetSelectHdl  ( LINK( this, ScTabPageSortOptions, FillAlgorHdl ) );

#if !TEST_LAYOUT
	pViewData = rSortItem.GetViewData();
#endif /* TEST_LAYOUT */
	pDoc	  = pViewData ? pViewData->GetDocument() : NULL;

	DBG_ASSERT( pViewData, "ViewData not found! :-/" );

#if !TEST_LAYOUT
	if ( pViewData && pDoc )
	{
		String			theArea;
		ScDBCollection* pDBColl 	= pDoc->GetDBCollection();
		String			theDbArea;
		String			theDbName	= aStrNoName;
		const SCTAB	nCurTab		= pViewData->GetTabNo();
        const ScAddress::Convention eConv = pDoc->GetAddressConvention();
#endif /* !TEST_LAYOUT */

		aLbOutPos.Clear();
		aLbOutPos.InsertEntry( aStrUndefined, 0 );
		aLbOutPos.Disable();

#if !TEST_LAYOUT
		ScAreaNameIterator aIter( pDoc );
		String aName;
		ScRange aRange;
		String aRefStr;
		while ( aIter.Next( aName, aRange ) )
		{
			USHORT nInsert = aLbOutPos.InsertEntry( aName );

			aRange.aStart.Format( aRefStr, SCA_ABS_3D, pDoc, eConv );
			aLbOutPos.SetEntryData( nInsert, new String( aRefStr ) );
		}
#endif /* !TEST_LAYOUT */

		aLbOutPos.SelectEntryPos( 0 );
		aEdOutPos.SetText( EMPTY_STRING );

#if !TEST_LAYOUT
		/*
		 * Ueberpruefen, ob es sich bei dem uebergebenen
		 * Bereich um einen Datenbankbereich handelt:
		 */

		ScAddress aScAddress( rSortData.nCol1, rSortData.nRow1, nCurTab );
		ScRange( aScAddress,
				 ScAddress( rSortData.nCol2, rSortData.nRow2, nCurTab )
			   ).Format( theArea, SCR_ABS, pDoc, eConv );

		if ( pDBColl )
		{
			ScDBData* pDBData
					= pDBColl->GetDBAtArea( nCurTab,
											rSortData.nCol1, rSortData.nRow1,
											rSortData.nCol2, rSortData.nRow2 );
			if ( pDBData )
			{
				pDBData->GetName( theDbName );
				aBtnHeader.Check( pDBData->HasHeader() );
			}
		}

		theArea.AppendAscii(RTL_CONSTASCII_STRINGPARAM(" ("));
		theArea += theDbName;
		theArea += ')';

		//aFtArea.SetText( theArea );
		//theArea.Insert( aStrAreaLabel, 0 );
		//aFtAreaLabel.SetText( theArea );

		aBtnHeader.SetText( aStrColLabel );
	}
#endif /* TEST_LAYOUT */

	FillUserSortListBox();

	//	get available languages

    aLbLanguage.SetLanguageList( LANG_LIST_ALL | LANG_LIST_ONLY_KNOWN, FALSE );
	aLbLanguage.InsertLanguage( LANGUAGE_SYSTEM );
}

//------------------------------------------------------------------------

USHORT* __EXPORT ScTabPageSortOptions::GetRanges()
{
	return pSortRanges;
}

// -----------------------------------------------------------------------

#if ENABLE_LAYOUT
#undef SfxTabPage
#endif /* ENABLE_LAYOUT */
SfxTabPage* __EXPORT ScTabPageSortOptions::Create(
											Window*				pParent,
											const SfxItemSet&	rArgSet )
{
	return ( new ScTabPageSortOptions( pParent, rArgSet ) );
}

// -----------------------------------------------------------------------

void __EXPORT ScTabPageSortOptions::Reset( const SfxItemSet& /* rArgSet */ )
{
	if ( rSortData.bUserDef )
	{
		aBtnSortUser.Check( TRUE );
		aLbSortUser.Enable();
		aLbSortUser.SelectEntryPos( rSortData.nUserIndex );
	}
	else
	{
		aBtnSortUser.Check( FALSE );
		aLbSortUser.Disable();
		aLbSortUser.SelectEntryPos( 0 );
	}

	aBtnCase.Check			( rSortData.bCaseSens );
	aBtnFormats.Check		( rSortData.bIncludePattern );
	aBtnHeader.Check		( rSortData.bHasHeader );
	aBtnNaturalSort.Check	( rSortData.bNaturalSort );

	if ( rSortData.bByRow )
	{
		aBtnTopDown.Check();
		aBtnHeader.SetText( aStrColLabel );
	}
	else
	{
		aBtnLeftRight.Check();
		aBtnHeader.SetText( aStrRowLabel );
	}

	LanguageType eLang = MsLangId::convertLocaleToLanguage( rSortData.aCollatorLocale );
	if ( eLang == LANGUAGE_DONTKNOW )
		eLang = LANGUAGE_SYSTEM;
	aLbLanguage.SelectLanguage( eLang );
	FillAlgorHdl( &aLbLanguage );				// get algorithms, select default
	if ( rSortData.aCollatorAlgorithm.Len() )
		aLbAlgorithm.SelectEntry( pColRes->GetTranslation( rSortData.aCollatorAlgorithm ) );

	if ( pDoc && !rSortData.bInplace )
	{
		String aStr;
		USHORT nFormat = (rSortData.nDestTab != pViewData->GetTabNo())
							? SCR_ABS_3D
							: SCR_ABS;

		theOutPos.Set( rSortData.nDestCol,
					   rSortData.nDestRow,
					   rSortData.nDestTab );

		theOutPos.Format( aStr, nFormat, pDoc, pDoc->GetAddressConvention() );
		aBtnCopyResult.Check();
		aLbOutPos.Enable();
		aEdOutPos.Enable();
		aEdOutPos.SetText( aStr );
		EdOutPosModHdl( &aEdOutPos );
		aEdOutPos.GrabFocus();
		aEdOutPos.SetSelection( Selection( 0, SELECTION_MAX ) );
	}
	else
	{
		aBtnCopyResult.Check( FALSE );
		aLbOutPos.Disable();
		aEdOutPos.Disable();
		aEdOutPos.SetText( EMPTY_STRING );
	}
}

// -----------------------------------------------------------------------

BOOL __EXPORT ScTabPageSortOptions::FillItemSet( SfxItemSet& rArgSet )
{
	ScSortParam theSortData = rSortData;
	if (pDlg)
	{
		const SfxItemSet* pExample = pDlg->GetExampleSet();
		const SfxPoolItem* pItem;
		if ( pExample && pExample->GetItemState( nWhichSort, TRUE, &pItem ) == SFX_ITEM_SET )
			theSortData = ((const ScSortItem*)pItem)->GetSortData();
	}

	theSortData.bByRow			= aBtnTopDown.IsChecked();
	theSortData.bHasHeader		= aBtnHeader.IsChecked();
	theSortData.bCaseSens		= aBtnCase.IsChecked();
	theSortData.bNaturalSort	= aBtnNaturalSort.IsChecked();
	theSortData.bIncludePattern = aBtnFormats.IsChecked();
	theSortData.bInplace		= !aBtnCopyResult.IsChecked();
	theSortData.nDestCol		= theOutPos.Col();
	theSortData.nDestRow		= theOutPos.Row();
	theSortData.nDestTab		= theOutPos.Tab();
	theSortData.bUserDef		= aBtnSortUser.IsChecked();
	theSortData.nUserIndex		= (aBtnSortUser.IsChecked())
									? aLbSortUser.GetSelectEntryPos()
									: 0;

	// get locale
	LanguageType eLang = aLbLanguage.GetSelectLanguage();
    theSortData.aCollatorLocale = MsLangId::convertLanguageToLocale( eLang, false );

	// get algorithm
	String sAlg;
	if ( eLang != LANGUAGE_SYSTEM )
	{
        uno::Sequence<rtl::OUString> aAlgos = pColWrap->listCollatorAlgorithms(
                theSortData.aCollatorLocale );
		USHORT nSel = aLbAlgorithm.GetSelectEntryPos();
		if ( nSel < aAlgos.getLength() )
			sAlg = aAlgos[nSel];
	}
	theSortData.aCollatorAlgorithm = sAlg;

#if !TEST_LAYOUT
	rArgSet.Put( ScSortItem( SCITEM_SORTDATA, &theSortData ) );
#endif /* TEST_LAYOUT */
	return TRUE;
}

// -----------------------------------------------------------------------

// fuer Datenaustausch ohne Dialog-Umweg: (! noch zu tun !)
// void ScTabPageSortOptions::ActivatePage( const SfxItemSet& rSet )
void __EXPORT ScTabPageSortOptions::ActivatePage()
{
	if ( pDlg )
	{
		if ( aBtnHeader.IsChecked() != pDlg->GetHeaders() )
		{
			aBtnHeader.Check( pDlg->GetHeaders() );
		}

		if ( aBtnTopDown.IsChecked() != pDlg->GetByRows() )
		{
			aBtnTopDown.Check( pDlg->GetByRows() );
			aBtnLeftRight.Check( !pDlg->GetByRows() );
		}

		aBtnHeader.SetText( (pDlg->GetByRows())
							? aStrColLabel
							: aStrRowLabel );
	}
}

// -----------------------------------------------------------------------

int __EXPORT ScTabPageSortOptions::DeactivatePage( SfxItemSet* pSetP )
{
	BOOL bPosInputOk = TRUE;

	if ( aBtnCopyResult.IsChecked() )
	{
		String		thePosStr = aEdOutPos.GetText();
		ScAddress	thePos;
		xub_StrLen	nColonPos = thePosStr.Search( ':' );

		if ( STRING_NOTFOUND != nColonPos )
			thePosStr.Erase( nColonPos );

		if ( pViewData )
		{
			//	visible table is default for input without table
			//	must be changed to GetRefTabNo when sorting has RefInput!
			thePos.SetTab( pViewData->GetTabNo() );
		}

		USHORT nResult = thePos.Parse( thePosStr, pDoc, pDoc->GetAddressConvention() );

		bPosInputOk = ( SCA_VALID == (nResult & SCA_VALID) );

		if ( !bPosInputOk )
		{
#if !ENABLE_LAYOUT
			ErrorBox( this, WinBits( WB_OK | WB_DEF_OK ),
					 ScGlobal::GetRscString( STR_INVALID_TABREF )
					).Execute();
#endif /* ENABLE_LAYOUT */
			aEdOutPos.GrabFocus();
			aEdOutPos.SetSelection( Selection( 0, SELECTION_MAX ) );
			theOutPos.Set(0,0,0);
		}
		else
		{
			aEdOutPos.SetText( thePosStr );
			theOutPos = thePos;
		}
	}

	if ( pDlg && bPosInputOk )
	{
		pDlg->SetHeaders( aBtnHeader.IsChecked() );
		pDlg->SetByRows ( aBtnTopDown.IsChecked() );
	}

    if ( pSetP && bPosInputOk )
        FillItemSet( *pSetP );

	return bPosInputOk ? SfxTabPage::LEAVE_PAGE : SfxTabPage::KEEP_PAGE;
}

// -----------------------------------------------------------------------

void ScTabPageSortOptions::FillUserSortListBox()
{
	ScUserList* pUserLists = ScGlobal::GetUserList();

	aLbSortUser.Clear();
	if ( pUserLists )
	{
		USHORT nCount = pUserLists->GetCount();
		if ( nCount > 0 )
			for ( USHORT i=0; i<nCount; i++ )
				aLbSortUser.InsertEntry( (*pUserLists)[i]->GetString() );
	}
}

// -----------------------------------------------------------------------
// Handler:

IMPL_LINK( ScTabPageSortOptions, EnableHdl, CheckBox *, pBox )
{
	if ( pBox == &aBtnCopyResult )
	{
		if ( pBox->IsChecked() )
		{
			aLbOutPos.Enable();
			aEdOutPos.Enable();
			aEdOutPos.GrabFocus();
		}
		else
		{
			aLbOutPos.Disable();
			aEdOutPos.Disable();
		}
	}
	else if ( pBox == &aBtnSortUser )
	{
		if ( pBox->IsChecked() )
		{
			aLbSortUser.Enable();
			aLbSortUser.GrabFocus();
		}
		else
			aLbSortUser.Disable();
	}
	return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK( ScTabPageSortOptions, SelOutPosHdl, ListBox *, pLb )
{
	if ( pLb == &aLbOutPos )
	{
		String	aString;
		USHORT	nSelPos = aLbOutPos.GetSelectEntryPos();

		if ( nSelPos > 0 )
			aString = *(String*)aLbOutPos.GetEntryData( nSelPos );

		aEdOutPos.SetText( aString );
	}
	return 0;
}

// -----------------------------------------------------------------------

IMPL_LINK( ScTabPageSortOptions, SortDirHdl, RadioButton *, pBtn )
{
	if ( pBtn == &aBtnTopDown )
	{
		aBtnHeader.SetText( aStrColLabel );
	}
	else if ( pBtn == &aBtnLeftRight )
	{
		aBtnHeader.SetText( aStrRowLabel );
	}
	return 0;
}

// -----------------------------------------------------------------------

void __EXPORT ScTabPageSortOptions::EdOutPosModHdl( Edit* pEd )
{
	if ( pEd == &aEdOutPos )
	{
		String	theCurPosStr = aEdOutPos.GetText();
		USHORT	nResult = ScAddress().Parse( theCurPosStr, pDoc, pDoc->GetAddressConvention() );

		if ( SCA_VALID == (nResult & SCA_VALID) )
		{
			String*	pStr	= NULL;
			BOOL	bFound	= FALSE;
			USHORT	i		= 0;
			USHORT	nCount	= aLbOutPos.GetEntryCount();

			for ( i=2; i<nCount && !bFound; i++ )
			{
				pStr = (String*)aLbOutPos.GetEntryData( i );
				bFound = (theCurPosStr == *pStr);
			}

			if ( bFound )
				aLbOutPos.SelectEntryPos( --i );
			else
				aLbOutPos.SelectEntryPos( 0 );
		}
	}
}

// -----------------------------------------------------------------------

IMPL_LINK( ScTabPageSortOptions, FillAlgorHdl, void *, EMPTYARG )
{
	aLbAlgorithm.SetUpdateMode( FALSE );
	aLbAlgorithm.Clear();

	LanguageType eLang = aLbLanguage.GetSelectLanguage();
	if ( eLang == LANGUAGE_SYSTEM )
	{
		//	for LANGUAGE_SYSTEM no algorithm can be selected because
		//	it wouldn't necessarily exist for other languages
		//	-> leave list box empty if LANGUAGE_SYSTEM is selected
        aFtAlgorithm.Enable( FALSE );           // nothing to select
		aLbAlgorithm.Enable( FALSE );			// nothing to select
	}
	else
	{
		lang::Locale aLocale( MsLangId::convertLanguageToLocale( eLang ));
		uno::Sequence<rtl::OUString> aAlgos = pColWrap->listCollatorAlgorithms( aLocale );

		long nCount = aAlgos.getLength();
		const rtl::OUString* pArray = aAlgos.getConstArray();
		for (long i=0; i<nCount; i++)
		{
			String sAlg = pArray[i];
			String sUser = pColRes->GetTranslation( sAlg );
			aLbAlgorithm.InsertEntry( sUser, LISTBOX_APPEND );
		}
		aLbAlgorithm.SelectEntryPos( 0 );		// first entry is default
        aFtAlgorithm.Enable( nCount > 1 );      // enable only if there is a choice
		aLbAlgorithm.Enable( nCount > 1 );		// enable only if there is a choice
	}

	aLbAlgorithm.SetUpdateMode( TRUE );
	return 0;
}


