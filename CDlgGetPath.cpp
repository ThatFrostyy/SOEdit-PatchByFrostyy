// Copyright (C) 2005 by Brian A. Kay
// Modification by TonaTor
//
// See additional copyright information by John McTainsh below
//
//-----------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
//
// DlgGetPath.cpp : implementation file
//
//PURPOSE
//	Display a dialog to allow the user to select a file path.
//
//AUTHOR
//	John McTainsh (mctainsh@bit.net.au)
//
//THING TO DO
//	1)	Display the recycle bin appropiatly.
//	2)	Display the contents of the recycle bin with out S- numbers.
//	3)	Sort items when addes except for "Entire Network"
//	4)	Check on a more complex network.
//	5)	Get icons for the network items.
//	6)	Determine why icon disappears from the ALT-TAB bar when active?
//	7)	Handle WNetEnumResource for more than 16384 bytes of data.
//
//Copyright 1999 (C) John McTainsh.
//I am happy for any developer to use this dialog. I think it is pretty
//cool and suites my purposes. Feel free to copy and alter whatever
//you please. Keep me informed of any good things you do to it
//and also what it has been used for (for my ego).
//
//This is shareware and can be copied or distrabuted without my concent,
//permision or knowledge. It would be uncool to use it without putting
//my name in the credits (ego) but that is up to you.
//
//This code is provide as is. If it dont work or dont do what it should
//then dont come crying to me. It is full of bugs (undocumented features)
//which you the user/developer must find to use it.
//
//Remember, you to can become a Jedi master (VC++ developer) if you can
//resist the temptations of the dark side of the force (VB).
//
//PS
//	Comments regarding my spelling will NOT be kindly recieved.
//PSS
//	All the best software is written by New Zealanders.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SOEdit.h"
#include "CDlgGetPath.h"								//Definition to get path
#include "general.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//Handy local functions.
TCHAR* MakeObjectDynamic( LPTSTR szData );
CString WindowName( CString sName );


/////////////////////////////////////////////////////////////////////////////
// CDlgGetPath dialog
CDlgGetPath::CDlgGetPath(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgGetPath::IDD, pParent)
{
	TRACE( _T("CDlgGetPath::CDlgGetPath()\n") );
	m_sTopNote = _T(" John McTainsh wrote this.(Ego)");//Remove this if you don't like me.
    // Au contraire!  John McTainsh is a GENIUS!!!
	m_sTitle = _T("Поиск папки");
	//{{AFX_DATA_INIT(CDlgGetPath)
	#ifdef ALTERNATIVE_LANG
		m_sTopNote = _T("Select the folder where the data is extracted.");
	#else
		m_sTopNote = _T("Выбери папку, в которую извлечены данные.");
	#endif
	//}}AFX_DATA_INIT
	m_hNetworkRoot = NULL;
}


void CDlgGetPath::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgGetPath)
	DDX_Control(pDX, IDC_TREE, m_Tree);
	DDX_Text(pDX, IDC_STATIC_NOTE, m_sTopNote);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgGetPath, CDialog)
	//{{AFX_MSG_MAP(CDlgGetPath)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, OnSelchangedTree)
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_TREE, OnItemexpandingTree)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
//FUNCTION
//	Setup inital Dialog Data.
//CREATED
//	8-5-1999  19:57:09
//PARAMS
//	
//RETURNS
//
BOOL CDlgGetPath::OnInitDialog() 
{
	TRACE( _T("CDlgGetPath::OnInitDialog()\n") );
	CDialog::OnInitDialog();
	SetWindowText( m_sTitle );
	//
	//Setup the Image lists
	//
	if(!m_ImageListTree.Create( IDB_FILE_TYPES, 17, 1, RGB(255, 255, 255)))
		TRACE(_T("Image list creation fault") );
	m_Tree.SetImageList(&m_ImageListTree, TVSIL_NORMAL);

	
	
	//
	//List the local drives
	//
	for( TCHAR cLetter = _T('A'); cLetter <= _T('Z'); cLetter++ )
	{
		CString sDrive = cLetter;
		sDrive += _T(":");
		UINT nType = GetDriveType( sDrive );
		if( DRIVE_REMOVABLE <= nType && nType <= DRIVE_RAMDISK )
			InsertItem( TVI_ROOT, NULL, sDrive, nType );
	}
	#ifdef ALTERNATIVE_LANG
		SetWindowTextA("Selecting the Game Data catalog");
		CWnd *pWnd = (CWnd *)GetDlgItem(IDC_STATIC_NOTE);
		pWnd -> SetWindowText("Game data is stored in:");
		pWnd = (CWnd *)GetDlgItem(IDCANCEL);
		pWnd -> SetWindowText("Cancel");
		pWnd = (CWnd *)GetDlgItem(IDOK);
		pWnd -> SetWindowText("OK");
	#endif
	//Network neighbour hood
	///m_hNetworkRoot = InsertItem( TVI_ROOT, NULL, _T("Network Neighborhood"), DRIVE_RAMDISK+1 );
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


///////////////////////////////////////////////////////////////////////////////
//DESCRIPTION:
//	Insert an Item in to the List
//CREATED:
//	10-5-1999  13:49:20
//PARAMS:
//	hParent		Parent to Attach to.
//	sText		Text to Add.
//	iImage		Image.
//RETURN:
//	Handle of Item created
HTREEITEM CDlgGetPath::InsertItem( HTREEITEM hParent, NETRESOURCE *const pNetResource, CString sText, int iImage, int iImageSelected /* = -1 */ )
{
	TRACE( _T("CDlgGetPath::InsertItem(%p,%p,%s +++ )\n"), hParent, pNetResource, sText );
	sText = WindowName( sText );
	TVINSERTSTRUCT InsertStruct;
	InsertStruct.hParent		= hParent;	
	InsertStruct.hInsertAfter	= TVI_LAST;
	InsertStruct.itemex.mask	= TVIF_IMAGE | TVIF_TEXT | TVIF_CHILDREN | 
								  TVIF_SELECTEDIMAGE | TVIF_PARAM;
	InsertStruct.itemex.pszText	= sText.GetBuffer( sText.GetLength() );  sText.ReleaseBuffer();
	InsertStruct.itemex.iImage	= iImage;
	InsertStruct.itemex.cChildren= 1;
	InsertStruct.itemex.lParam	= (LPARAM)pNetResource;
	if( iImageSelected == -1 )
		InsertStruct.itemex.iSelectedImage = iImage;
	else
		InsertStruct.itemex.iSelectedImage = iImageSelected;
	return m_Tree.InsertItem( &InsertStruct );
}


/////////////////////////////////////////////////////////////////////////////
//FUNCTION
//	Called in request to expand an item in the tree.
//CREATED
//	8-5-1999  19:57:09
//PARAMS
//	sPath	Path to use for populating from
//	hParent	Where to add the data
//RETURNS
//	True if some children were added
bool CDlgGetPath::PopulateTree( CString sPath, HTREEITEM hParent ) 
{
	TRACE( _T("CDlgGetPath::PopulateTree( %s )\n"), sPath );
	bool bGotChildren = false;						//True if a child is added.	
	//
	//Populate Network neighbourhood tree (Entire network and Local-Computers)
	//
	if( hParent == m_hNetworkRoot )
	{
		bGotChildren = EnumNetwork( hParent );
	}
	//
	//Network item(Search deep into the network)
	//
	else if( m_Tree.GetItemData( hParent ) )
	{
		bGotChildren = EnumNetwork( hParent );
	}
	
	//
	//Search for files and populate the CTreeCtrl
	//
	else
	{
		CFileFind finder;
		BOOL bWorking = finder.FindFile( sPath+_T("*.*") );
		while (bWorking)   
		{
			bWorking = finder.FindNextFile();
			if( finder.IsDots() )
				continue;
			if( finder.IsDirectory() )
			{
				InsertItem( hParent, NULL, finder.GetFileName(), DRIVE_NO_ROOT_DIR, DRIVE_UNKNOWN );
				bGotChildren = true;
			}
		}
	}

	//
	//Remove the [+] if no children
	//
	if( !bGotChildren )
	{
		TVITEM item		= { 0 };
		item.mask		= TVIF_HANDLE | TVIF_CHILDREN;
		item.hItem		= hParent;
		item.cChildren	= 0;
		m_Tree.SetItem( &item );
	}
	return bGotChildren;
}


/////////////////////////////////////////////////////////////////////////////
//FUNCTION
//	Get the Part from a given selection point.
//CREATED
//	8-5-1999  22:29:00
//PARAMS
//	hItem	Item to Get Path for.
//RETURNS
//	Full path as a string.
CString CDlgGetPath::GetItemPath( HTREEITEM hItem ) 
{
	TRACE( _T("CDlgGetPath::GetItemPath(%p)\n"), hItem );
	CString sRet;
	do
	{
		//End with a share name.
		NETRESOURCE *const pNetResource = (NETRESOURCE *)(m_Tree.GetItemData( hItem ) );
		if( pNetResource )
		{
			sRet = CString(pNetResource->lpRemoteName) + _T('\\')+ sRet; 		
			break;
		}
		//Add the directory name to the path.
		sRet = m_Tree.GetItemText( hItem ) + _T('\\')+ sRet; 
		hItem = m_Tree.GetParentItem( hItem );
	} while( hItem );
	return sRet;
}



/////////////////////////////////////////////////////////////////////////////
//FUNCTION
//	Node on the tree is about to expand
//	Called when some bozo tries to expand the tree.
//CREATED
//	8-5-1999  19:56:55
//PARAMS
//	See help.
//RETURNS
//
void CDlgGetPath::OnItemexpandingTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TRACE( _T("CDlgGetPath::OnItemexpandingTree(%p)\n"), pNMHDR );
	CWaitCursor CursorWaiting;						//Show the wait cursor while expanding
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	//Only action 2 notifications
	if( pNMTreeView->action == 2 )
	{
		//Update location display
		CString sPath = GetItemPath( pNMTreeView->itemNew.hItem );
		//Refresh children
		if( !m_Tree.GetChildItem( pNMTreeView->itemNew.hItem ) )
		{
			PopulateTree( sPath, pNMTreeView->itemNew.hItem );
			if( m_Tree.GetSelectedItem( ) != pNMTreeView->itemNew.hItem )
				m_Tree.SelectItem( pNMTreeView->itemNew.hItem );
		}
	}
	*pResult = 0;
}


/////////////////////////////////////////////////////////////////////////////
//FUNCTION
//	Update the Edit dox with the new selection path
//CREATED
//	9-5-1999  12:54:48
//PARAMS
//	
//RETURNS
//
void CDlgGetPath::OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TRACE( _T("CDlgGetPath::OnSelchangedTree(%p)\n"), pNMHDR );
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	//Disable search on Workstation roots
	if( m_Tree.GetItemData( pNMTreeView->itemNew.hItem ) || 
			pNMTreeView->itemNew.hItem == m_hNetworkRoot )
		GetDlgItem( IDOK )->EnableWindow( false );
	else
		GetDlgItem( IDOK )->EnableWindow( true );
	*pResult = 0;
}

					
/////////////////////////////////////////////////////////////////////////////
//FUNCTION
//	Convert the 
//CREATED
//	23-5-1999  15:55:53
//PARAMS
//	
//RETURNS
//
TCHAR* MakeObjectDynamic( LPTSTR szData )
{
	TRACE( _T("MakeObjectDynamic( %s )\n"), szData );
	//Assume a NULL empty string
	TCHAR * szRet = NULL;
	int nLength	  = 0;
	if( szData )
		nLength = _tcslen( szData )+1;
	if( nLength > 0 )
	{
		szRet = new TCHAR[nLength];
		_tcscpy( szRet, szData );
	}
	return szRet;
}


/////////////////////////////////////////////////////////////////////////////
//FUNCTION
//	To display the same was window explorer
//	..names all uppercase letters are converted
//	..to inital caps.
//CREATED
//	23-5-1999  16:10:34
//PARAMS
//	sName		sName to convert.
//RETURNS
//	Converted name
CString WindowName( CString sName )
{
	CString sRet;
	//Scan all charactors to determine if their are any lower case items
	for( int n = 0; n < sName.GetLength(); n++ )
	{
		TCHAR ch = sName[n];
		if ((ch >= 'a') && (ch <= 'z') )
			return sName;
	}
	sName.MakeLower();
	if( sName.GetLength() > 0 )
	{
		CString sFirstChar = sName[0];
		sFirstChar.MakeUpper();
		sName = sFirstChar + sName.Mid( 1 );
	}
	return sName;
}

///////////////////////////////////////////////////////////////////////////////
//DESCRIPTION:
//	Enumerate the given network resource. This is where the magic happens.
//CREATED:
//	10-5-1999  15:19:02
//PARAMS:
//	hParent	Item to create the child items from
//			..(item.lParam) == NULL if finding the NETWORK root.
//RETURN:
//	ture if one or more items were found
bool CDlgGetPath::EnumNetwork( HTREEITEM hParent )
{
	TRACE( _T("CDlgGetPath::EnumNetwork( %p )\n"), hParent );
	bool bGotChildren = false;						//True if a child is added.	

	//Check if the item already has a network resource and use it.
	NETRESOURCE *const pNetResource = (NETRESOURCE *)(m_Tree.GetItemData( hParent ) );
	//
	//Setup
	//
	DWORD dwResult;      
	HANDLE hEnum;      
	DWORD cbBuffer = 16384;
	DWORD cEntries = 0xFFFFFFFF;      
	LPNETRESOURCE lpnrDrv;      
	DWORD i;
	dwResult = WNetOpenEnum(	pNetResource ?  RESOURCE_GLOBALNET  : RESOURCE_CONTEXT,
								RESOURCETYPE_ANY,//RESOURCETYPE_DISK,
								0,                               
								pNetResource ? 	pNetResource		: NULL,
								&hEnum );      
	//Was the read sucessfull
	if (dwResult != NO_ERROR)      
	{
		TRACE( _T("*** ERROR %d - Cannot enumerate network drives.\n"), dwResult );
		return false;
	} 
	
	//
	//Get items until no more remain.
	//
	do      
	{
		lpnrDrv = (LPNETRESOURCE) GlobalAlloc( GPTR, cbBuffer );
		dwResult = WNetEnumResource( hEnum, &cEntries, lpnrDrv, &cbBuffer   );
		if (dwResult == NO_ERROR)         
		{
			//Scann through the results
			for( i = 0; i < cEntries; i++ )            
			{
				CString sNameRemote = lpnrDrv[i].lpRemoteName;
				int nType = 9;
				if( sNameRemote.IsEmpty() )
				{
					sNameRemote = lpnrDrv[i].lpComment;
					nType = 8;
				}
				//
				//Remove leading back slashes 
				//
				if( sNameRemote.GetLength() > 0 && sNameRemote[0] == _T('\\') )
					sNameRemote = sNameRemote.Mid( 1 );
				if( sNameRemote.GetLength() > 0 && sNameRemote[0] == _T('\\') )
					sNameRemote = sNameRemote.Mid( 1 );

				//
				//Display a share or the appropiate icon
				//
				if( lpnrDrv[i].dwDisplayType == RESOURCEDISPLAYTYPE_SHARE )
				{
					//Display only the share name
					int nPos = sNameRemote.Find( _T('\\') );
					if( nPos >= 0 )
						sNameRemote = sNameRemote.Mid( nPos+1 );
					InsertItem( hParent, NULL, sNameRemote, DRIVE_NO_ROOT_DIR, DRIVE_UNKNOWN );
				}
				else
				{
					NETRESOURCE* pResource = new NETRESOURCE;
					*pResource = lpnrDrv[i];
					pResource->lpLocalName	= MakeObjectDynamic( pResource->lpLocalName );
					pResource->lpRemoteName = MakeObjectDynamic( pResource->lpRemoteName );
					pResource->lpComment	= MakeObjectDynamic( pResource->lpComment );
					pResource->lpProvider	= MakeObjectDynamic( pResource->lpProvider );
					InsertItem( hParent, pResource, sNameRemote, pResource->dwDisplayType+7 );
				}
				bGotChildren = true;
			}
		}         
		GlobalFree( (HGLOBAL) lpnrDrv );      
		if( dwResult != ERROR_NO_MORE_ITEMS )
		{            
			TRACE( _T("*** ERROR %d - Cannot complete network drive enumeration\n"), dwResult );
			break;         
		}
	}
	while( dwResult != ERROR_NO_MORE_ITEMS );      

	//
	//Let go and go home
	//
	WNetCloseEnum(hEnum);   
	return bGotChildren;
}


/////////////////////////////////////////////////////////////////////////////
//FUNCTION
//	Release the dynamically created items in the list.
//CREATED
//	22-5-1999  16:59:56
//PARAMS
//	
//RETURNS
//
BOOL CDlgGetPath::DestroyWindow() 
{
	TRACE( _T("CDlgGetPath::DestroyWindow()\n") );
	//Get the current selection before heading home
	m_sPath = GetItemPath( m_Tree.GetSelectedItem( ) );

	//Start at the root deleting.
	HTREEITEM hItemCur = m_Tree.GetRootItem( );		//Current item under inspection

	//
	//While their is still data in the tree
	//
	while( hItemCur )
	{
		HTREEITEM hItem = m_Tree.GetChildItem( hItemCur );
		//Has children then make the child current
		if( hItem )
		{
			hItemCur = hItem;
			continue;
		}

		//Item has not children so we shall destroy it.
		//but first we must decide who is to take its place.
		HTREEITEM hNextSibItem = m_Tree.GetNextSiblingItem( hItemCur );
		HTREEITEM hPrevSibItem = m_Tree.GetPrevSiblingItem( hItemCur );
		HTREEITEM hParentItem  = m_Tree.GetParentItem( hItemCur );
		//Get item data to check if lparam is to be destroyed
		NETRESOURCE *const pNetResource = (NETRESOURCE *)m_Tree.GetItemData( hItemCur );//(item.lParam);
		if( pNetResource )
		{
			delete [] (pNetResource->lpLocalName);
			delete [] (pNetResource->lpRemoteName);
			delete [] (pNetResource->lpComment);
			delete [] (pNetResource->lpProvider);
			delete pNetResource;
		}
		m_Tree.DeleteItem( hItemCur );
		hItemCur = NULL;
		//Determine which item is next to recieve the focus
		if( hParentItem )
			hItemCur = hParentItem;
		if( hPrevSibItem )
			hItemCur = hPrevSibItem;
		if( hNextSibItem )
			hItemCur = hNextSibItem;
	}	
	//All items removed from list now. Lets trash this place and go home.
	return CDialog::DestroyWindow();
}