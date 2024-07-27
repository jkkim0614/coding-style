/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	JKCharacter.cpp
//
//	ĳ���� �������� ���
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------
// ���
//--------------------------------------------------------------------------
#include "JKCharacter.h"
#include "../JKBase/JKLight.h"
#include <memory>


namespace JKCharacterTool
{
	//--------------------------------------------------------------------------
	// Ŭ���� ����Լ� ����
	//--------------------------------------------------------------------------
	JKCharacter::JKCharacter()
	{
		Initialize();
	}

	JKCharacter::JKCharacter( LPDIRECT3DDEVICE9 _pd3dDevice, void* _pLightInstance )
	{
		Initialize();

		m_pd3dDevice = _pd3dDevice;
		m_pLightInstance = (JKLight*)_pLightInstance;
	}

	JKCharacter::~JKCharacter()
	{
		Destroy();
	}

	void JKCharacter::Initialize()
	{
		m_pd3dDevice = NULL;
		m_pEffect = NULL;
		m_pLightInstance = NULL;
		m_nAniIndex = 0;
		m_pTexture = NULL;
		m_bCharacterItemLoad = false;
		m_nTextureCount = 0;
		m_wEffectPass = 1;
		m_vScale = D3DXVECTOR3( 1.0f, 1.0f, 1.0f );
		m_vRotation = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		m_vTranslation = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		m_bWireFrame = false;
		m_bSetupFirstCall = true;
		m_bSetup = false;
		for ( int i = 0; i < m_nEffectFVFMaxCount; ++i )
		{
			m_pEffctFVF[i] = NULL;
		}
		D3DXMatrixIdentity( &m_matWorld );
		D3DXMatrixIdentity( &m_matRecovery );
		D3DXMatrixIdentity( &m_matCharacterWorld );
		D3DXMatrixIdentity( &m_matScale );
		D3DXMatrixIdentity( &m_matRotation );
		D3DXMatrixIdentity( &m_matTranslation );

		// ����
		m_bLight = false;

		// �Ƿ翧 ������
		m_bOutLine = false;

		// �ִϸ��̼�
		m_bAniOn = false;
		m_dwFrameBegin = 0;
		m_dwFrameEnd = 0;
		m_nCurrentFrame = 0;
		m_nMaxFrame = 0;
		m_fAniSpeed = 0.573f;
		m_fAniSpeedFrame = 0.0f;
		for ( int i = 0; i < GeometryType::GEO_MAX_BLEND; ++i )
		{
			D3DXMatrixIdentity( &m_matBlend[i] );
		}
	}

	void JKCharacter::Destroy()
	{
		for ( int i = 0; i < m_nEffectFVFMaxCount; ++i )
		{
			SAFE_RELEASE( m_pEffctFVF );
		}
		SAFE_RELEASE( m_pEffect );

		for ( int i = 0; i < sizeof( m_pTexture ) / sizeof( m_pTexture[0] ); ++i )
		{
			SAFE_RELEASE( m_pTexture[i].pTexture )
		}
	}

	void JKCharacter::Setup( DWORD _dwFirstFrame, DWORD _dwLastFrame )
	{
		// �¾� �Լ� ȣ���� ó���̸�
		if ( m_bSetupFirstCall )
		{
			m_fAniSpeedFrame = _dwFirstFrame;
			m_bSetupFirstCall = false;
		}

		m_nAniIndex	 =	static_cast< int >( m_fAniSpeedFrame );
		m_fAniSpeedFrame +=	m_fAniSpeed;

		// �ִϸ��̼� ���ǵ� �������� ������ �����Ӻ��� ũ��
		if ( static_cast< int >( m_fAniSpeedFrame ) > _dwLastFrame )
			m_fAniSpeedFrame = _dwFirstFrame;

		SetupAnimation();
	}

	void JKCharacter::Setup()
	{
		// �¾��� ������ ȣ���� ���� ���ٸ�
		if ( !m_bSetup )
		{
			m_fAniSpeedFrame = m_Header.nFrameBegin;
			m_bSetup = true;
		}

		m_nAniIndex = static_cast< int >( m_fAniSpeedFrame );
		m_fAniSpeedFrame +=	m_fAniSpeed;

		// �ִϸ��̼� ���ǵ� �������� ������ �����Ӻ��� ũ��
		if ( static_cast< int >( m_fAniSpeedFrame ) > m_Header.nFrameEnd )
			m_fAniSpeedFrame = m_Header.nFrameBegin;

		SetupAnimation();
	}

	void JKCharacter::Draw()
	{
		// �ִϸ��̼� ��, ������ ����, �׸��� ���� �����̸�
		if ( m_bAniOn && !m_bOutLineOn && !m_bShadowOn )
			Setup( m_dwFrameBegin, m_dwFrameEnd );

		// ���̾� ���������� ���ϰ���, ������
		if ( m_bWireFrame )
			m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );

		m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
		m_pd3dDevice->SetSoftwareVertexProcessing( TRUE );

		D3DXMATRIX matView;
		D3DXMATRIX matProj;
		// �� ��İ�  ���� ����� ���´�.
		m_pd3dDevice->GetTransform( D3DTS_VIEW, &matView );
		m_pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj );

		// ������ ����������
		if ( m_bLight )
			DrawLight();

		// �������� ����������
		if ( m_bOutLine )
			DrawOutLine();

		// �ؽ�ó ����
		m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

		for ( int i = 0; i < m_vtGeometryList.size(); ++i )
		{
			if ( m_vtGeometryList[i].nFaceNumber < 1 ||
				m_vtGeometryList[i].nVertexNumber < 1 ||
				m_vtGeometryList[i].nGeoType == GeometryType::GEO_BONE )
				continue;

			m_pEffect->SetTechnique( "T_Character" );
			m_pEffect->SetMatrix( "matView", &matView );
			m_pEffect->SetMatrix( "matProj", &matProj );

			switch ( m_vtGeometryList[i].nGeoType )
			{
			case GeometryType::GEO_MESH	:
				{
					m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matCharacterWorld );

					m_pEffect->SetMatrix( "matWorld", &m_matCharacterWorld );
					m_pEffect->SetMatrixArray( "matBlend", m_matBlend, GeometryType::GEO_MAX_BLEND );

					m_pEffect->Begin( NULL, 0 );
					m_pEffect->BeginPass( m_wEffectPass );

					// �ؽ�ó ���� 
					LPDIRECT3DTEXTURE9 pTexture = NULL;
					if ( 0 <= m_vtGeometryList[i].nTextureCount )
						pTexture = m_pTexture[m_vtGeometryList[i].nTextureCount].pTexture;

					m_pd3dDevice->SetTexture( 0, pTexture );
					m_pd3dDevice->SetVertexDeclaration( m_pEffctFVF[1] );	
				}
				break;
			case GeometryType::GEO_BONE	:
				{
					D3DXMATRIX	matWorld = m_matBlend[0] * m_matCharacterWorld;

					m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
					m_pEffect->SetMatrix( "matWorld", &matWorld );

					m_pEffect->Begin( NULL, 0 );
					m_pEffect->BeginPass( 0 );

					m_pd3dDevice->SetTexture( 0, NULL );
					m_pd3dDevice->SetVertexDeclaration( m_pEffctFVF[0] );
				}
				break;
			}

			CallDrawIndexedPrimitiveUP( m_vtGeometryList[i] );

			m_pEffect->EndPass();
			m_pEffect->End();
		}

		m_pd3dDevice->SetTexture( 0, NULL );
		m_pd3dDevice->SetVertexDeclaration( NULL );
		m_pd3dDevice->SetVertexShader( NULL );
		m_pd3dDevice->SetPixelShader( NULL );
		m_pd3dDevice->SetSoftwareVertexProcessing( FALSE );
		m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
		m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_matRecovery )		// ���� ��� ����
		m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );	// �ʸ�� ����
	}

	bool JKCharacter::LoadCharacter( char* _strFileName )
	{
		if ( !Load( _strFileName ) )
			return false;

		D3DXMatrixScaling( &m_matScale, m_vScale.x, m_vScale.y, m_vScale.z );

		m_matCharacterWorld = m_matScale;

		LoadEffectFile( g_strCharacterEffectFilePath );

		return true;
	}

	bool JKCharacter::LoadItem( char* _strFileName, bool _m_bCharacterItemLoad )
	{
		m_bCharacterItemLoad = _m_bCharacterItemLoad;

		if ( !Load( _strFileName ) )
			return false;

		m_bCharacterItemLoad = !m_bCharacterItemLoad;

		return true;
	}

	bool JKCharacter::LoadAni( char* _strFileName )
	{
		std::tr1::shared_ptr<FILE> fileSmartPtr(fopen(_strFileName, "rb"), [](FILE *fp) { fclose(fp); });
		FILE* filePtr = fileSmartPtr.get();

		if ( !filePtr )
			return false;

		for ( int i = 0; i < m_vtGeometryList.size(); ++i )
		{
			fread( &m_vtGeometryList[i].nAniCount, 1, sizeof( int ), filePtr );

			if ( m_vtGeometryList[i].nAniCount < 1 )
				continue;

			m_vtGeometryList[i].pAniMatrix = new D3DXMATRIX[ m_vtGeometryList[i].nAniCount ];
			fread( m_vtGeometryList[i].pAniMatrix, sizeof( D3DXMATRIX ), m_vtGeometryList[i].nAniCount, filePtr );
		}

		m_bAniOn = true;

		return true;
	}

	void JKCharacter::Destroy()
	{
	}

	bool JKCharacter::RoadEffectFile( const wchar_t* _strFilePath )
	{
		D3DVERTEXELEMENT9				vertexElement[MAX_FVF_DECL_SIZE] = { 0 };
		LPDIRECT3DVERTEXDECLARATION9	pEffectFVF = NULL;

		// EffectFVF0
		{
			D3DXDeclaratorFromFVF( stVertexPos::FVF, vertexElement );

			if ( !m_pd3dDevice->CreateVertexDeclaration( vertexElement, &pEffectFVF ) )
				return false;

			SAFE_RELEASE( m_pEffctFVF[0] );
			m_pEffctFVF[0] = pEffectFVF;
		}
		// EffectFVF1
		{
			D3DXDeclaratorFromFVF( stVertexBlend::FVF, vertexElement );

			pEffectFVF = NULL;

			if( !m_pd3dDevice->CreateVertexDeclaration( vertexElement, &pEffectFVF ) )
				return false;

			SAFE_RELEASE( m_pEffctFVF[1] );
			m_pEffctFVF[1] = pEffectFVF;
		}

		if ( !D3DXCreateEffectFromFile( m_pd3dDevice, _strFilePath, 
			NULL, NULL, 0, NULL, &m_pEffect, NULL ) )
			return false;

		return true;
	}

	void JKCharacter::ReloadEffectFile( const wchar_t* _strFilePath )
	{
		SAFE_RELEASE( m_pEffect );
		
		D3DXCreateEffectFromFile( m_pd3dDevice, _strFilePath, 
			NULL, NULL, 0, NULL, &m_pEffect, NULL );
	}

	void JKCharacter::SetupAnimation()
	{
		int count = -1;

		for ( int i = 0; i < m_vtGeometryList.size(); ++i )
		{
			++count;

			D3DXMATRIX matAni;
			D3DXMatrixIdentity( &matAni );

			// �ִϸ��̼� ����� �ִٸ�
			if ( m_vtGeometryList[i].pAniMatrix )
			{
				// ���� �����ӿ� �ִϸ��̼� �ε����� �ְ�
				// �ӽ� �ִϸ��̼� ��Ŀ� �ִϸ��̼� ����� ����
				m_nCurrentFrame = m_nAniIndex;
				matAni = m_vtGeometryList[i].pAniMatrix[m_nAniIndex];
			}

			m_matBlend[count] = matAni;
		}
	}

	void JKCharacter::CallDrawIndexedPrimitiveUP( stGeometry& _pGeometry )
	{
		m_pd3dDevice->DrawIndexedPrimitiveUP(
			D3DPT_TRIANGLELIST,
			0,
			_pGeometry.nVertexCount,
			_pGeometry.nIndexCount,
			_pGeometry.pIndexList,
			(D3DFORMAT)stVerterIndex::FVF,
			_pGeometry.pVertexList,
			_pGeometry.uStride
		);
	}

	bool JKCharacter::Load( char* _strFileName )
	{
		// ���� �ڵ� ������
		std::tr1::shared_ptr<FILE> fileSmartPtr(fopen(_strFileName, "rb"), [](FILE *fp) { fclose(fp); });
		FILE* filePtr = fileSmartPtr.get();
		
		if ( filePtr == NULL )
			return false;

		stHeader	tHeader;
		int			nGeometryCount = 0;

		// ��� ���� �б�
		ReadHeaderInfo( filePtr, tHeader, nGeometryCount );

		if ( m_Header.nGeometryCount < 1 )
			return false;

		// ���� ������ ��� ���������� �̵�
		fseek( filePtr, GeometryType::GEO_HEADER_OFFSET, SEEK_SET );

		// ���� ������ ����
		m_vtGeometryList.reserve( m_Header.nGeometryCount );
		m_vtGeometryList.resize( m_Header.nGeometryCount );
		
		// ������ ����
		m_dwFrameBegin = m_Header.nFrameBegin;
		m_dwFrameEnd = m_Header.nFrameEnd;

		// �޽� ���� �б�
		ReadMeshInfo( filePtr, nGeometryCount );

		// ������ ����
		DWORD dwTick = m_Header.nTickFrame;
		DWORD dwTimeBegin = m_Header.nFrameBegin * dwTick;
		DWORD dwTimeEnd	= m_Header.nFrameEnd * dwTick;
		m_nMaxFrame	= m_Header.nFrameEnd - m_Header.nFrameBegin + 1;

		// ���ؽ� ����ġ ���� �б�
		ReadVertexWeightInfo( filePtr, nGeometryCount );

		// ���� ��ְ� ���� �б�
		ReadBlendNormalInfo( filePtr );

		int nTextureCount = 0;
		int nResultTextureCount = 0;

		// �ؽ�ó �����̸� ���� �б�
		ReadTextureFileNameInfo( filePtr, tHeader, nTextureCount, nResultTextureCount, nGeometryCount );

		// �ؽ�ó UV ���� �б�
		ReadTextureUVInfo( filePtr, nGeometryCount );

		// ���ؽ� ����ġ ���� ����
		CopyVertexWeightInfo( nGeometryCount );

		// ���ؽ� ����ġ ���� ����
		DeleteVertexWeightInfo( nGeometryCount );

		// ������Ʈ�� �ؽ�ó ����
		CreateGeometryTexture( _strFileName, nTextureCount, nResultTextureCount );

		return true;
	}

	void JKCharacter::ReadHeaderInfo( FILE* _filePtr, stHeader* _pHeader, int& _nGeometryCount )
	{
		// ĳ���� �������� �ε���� �ʾҴٸ�
		if ( !m_bCharacterItemLoad )
		{
			fread( &m_Header, 1, sizeof( stHeader ), _filePtr );
		}
		// ĳ���� �������� �ε�Ǿ��ٸ�
		else
		{
			fread( &_pHeader, 1, sizeof( stHeader ), _filePtr );

			_nGeometryCount = m_Header.nGeometryCount;

			m_Header.nGeometryCount += _pHeader.nGeometryCount;
			m_Header.nTexFileCount += _pHeader.nTexFileCount;
		}
	}

	void JKCharacter::ReadMeshInfo( FILE* _filePtr, int& _nGeometryCount )
	{
		for ( int i = 0; i < m_vtGeometryList.size(); ++i )
		{
			if ( m_bCharacterItemLoad && i < _nGeometryCount )
				continue;

			fread( m_vtGeometryList[i].strName,			32, sizeof( char ), _filePtr	);
			fread( m_vtGeometryList[i].nGeoType,			1,	sizeof( int ),	_filePtr	);			
			fread( m_vtGeometryList[i].nParentIndex,	1,	sizeof( int ),	_filePtr	);	
			fread( m_vtGeometryList[i].nIndexCount,		1,	sizeof( int ),	_filePtr	);
			fread( m_vtGeometryList[i].nVertexCount,	1,	sizeof( int ),	_filePtr	);
			fread( m_vtGeometryList[i].nTextureCount,	1,	sizeof( int ),	_filePtr	);

			if ( m_nTextureCount < m_vtGeometryList[i].nTextureCount )
				m_nTextureCount = m_vtGeometryList[i].nTextureCount;

			if ( m_bCharacterItemLoad )
			{
				m_vtGeometryList[i].nTextureCount = m_nTextureCount;
				++m_vtGeometryList[i].nTextureCount;
			}

			// �θ� ��� ����
			if ( m_vtGeometryList[i].nParentIndex != -1 )
			{
				m_vtGeometryList[i].pParent = &m_vtGeometryList[ m_vtGeometryList[i].nParentIndex ];
			}

			// �ε��� ���� �� ���� ������ 1���� ������ continue
			if ( m_vtGeometryList[i].nIndexCount < 1 ||
				m_vtGeometryList[i].nVertexCount < 1 )
				continue;

			switch ( m_vtGeometryList[i].nGeoType )
			{
			case GeometryType::GEO_MESH	:
				{
					m_vtGeometryList[i].dwFVF = stVertexBlend::FVF;
					m_vtGeometryList[i].uStride	= sizeof( stVertexBlend );
				}
				break;
			case GeometryType::GEO_BONE	:
				{
					m_vtGeometryList[i].dwFVF = stVertexPos::FVF;
					m_vtGeometryList[i].uStride	= sizeof( stVertexPos );
				}
				break;
			default:
				{
					m_vtGeometryList[i].nIndexCount	= 0;
					m_vtGeometryList[i].nVertexCount = 0;
					continue;
				}
				break;
			}

			// �ε��� ����Ʈ ����
			m_vtGeometryList[i].pIndexList = new stVerterIndex[ m_vtGeometryList[i].nIndexCount ];
			fread( m_vtGeometryList[i].pIndexList, sizeof( stVerterIndex ), m_vtGeometryList[i].nIndexCount, _filePtr );

			// ���� ����Ʈ ����
			m_vtGeometryList[i].pVertexList = malloc( m_vtGeometryList[i].uStride * m_vtGeometryList[i].nVertexCount );

			// �޽� Ÿ���̸�
			if ( m_vtGeometryList[i].nGeoType == GeometryType::GEO_MESH )
			{
				stVertexBlend* pVertexList (stVertexBlend*)m_vtGeometryList[i].pVertexList;

				for ( int j = 0; j < m_vtGeometryList[i].nVertexCount; ++j )
					fread( &pVertexList[j], sizeof( D3DXVECTOR3 ), 1, _filePtr );
			}
			// �� Ÿ���̸�
			else if ( m_vtGeometryList[i].nGeoType == GeometryType::GEO_BONE )
			{
				fread( m_vtGeometryList[i].pVertexList, 
					m_vtGeometryList[i].uStride * m_vtGeometryList[i].nVertexCount, 
					_filePtr );
			}
		}
	}

	void JKCharacter::ReadVertexWeightInfo( FILE* _filePtr, int& _nGeometryCount )
	{
		for ( int i = 0; i < m_vtGeometryList.size(); ++i )
		{
			if ( m_bCharacterItemLoad && i < _nGeometryCount )
				continue;

			fread( &m_vtGeometryList[i].nBlendCount, sizeof( int ), 1, _filePtr );

			if ( m_vtGeometryList[i].nBlendCount <= 0 )
				continue;

			m_vtGeometryList[i].pBlendIndexList = new stBoneIndexList[m_vtGeometryList[i].nVertexCount];

			for ( int j = 0; j < m_vtGeometryList[i].nVertexCount; ++j )
			{
				stBoneIndexList* pBlendIndexList = m_vtGeometryList[i].pBlendIndexList[j];
				int nBoneCount = 0;

				// �� ���� �о����
				fread( &nBoneCount, sizeof( int ), 1, _filePtr );

				for ( int k = 0; k < nBoneCount; ++k )
				{
					int		nBoneIndex	= -1; // �� �ε���
					float	fWeight	= 0.0f;	// ����ġ

					fread( &nBoneIndex, sizeof( int ),		1,	_filePtr );
					fread( &fWeight,	sizeof( float ),	1,	_filePtr );

					pBlendIndexList->vtBoneIndex.push_back( stBone( nBoneIndex, fWeight ) );
				}
			}
		}
	}

	void JKCharacter::ReadBlendNormalInfo( FILE* _filePtr )
	{
		for ( int i = 0; i < m_vtGeometryList.size(); ++i )
		{
			if ( m_bCharacterItemLoad && i < nGeometryCount )
				continue;

			if ( m_vtGeometryList[I].nGeoType == GeometryType::GEO_MESH )
				continue;

			stVertexBlend* pVertexList = static_cast< stVertexBlend* >( m_vtGeometryList[I].pVertexList );

			for ( int j = 0; j < m_vtGeometryList[I].nVertexCount; ++j )
			{
				fread( &pVertexList[j].vNormal, sizeof( D3DXVECTOR3 ), 1, _filePtr );
			}
		}
	}

	void JKCharacter::SetTextureResize( stTexture& _pTexture, int _nReSize, int _nCopyCount )
	{
		stTexture* pTexture = _pTexture;
		_pTexture = NULL;
		_pTexture = new stTexture[ _nReSize ];
		memcpy( _pTexture, pTexture, (sizeof( stTexture ) * _nCopyCount) );
		SAFE_DELETE_ARRAY( pTexture );
	}

	void JKCharacter::ReadTextureFileNameInfo( FILE* _filePtr, stHeader& _pHeader, int& _nTextureCount, 
		int& _nResultTextureCount, int& _nGeometryCount )
	{
		// ĳ���� �������� �ε�Ǿ��ٸ�
		if ( m_bCharacterItemLoad )
		{
			_nTextureCount = _pHeader.nTexFileCount;
			_nResultTextureCount = m_Header.nTexFileCount - _pHeader.nTexFileCount;
		}
		// ĳ���� �������� �ε���� �ʾҴٸ�
		else
		{
			_nTextureCount = m_Header.nTexFileCount;
			_nResultTextureCount = 0;
		}

		// �Ͼ���� �ؽ�ó�� �ִٸ�
		if ( _nTextureCount > 0 )
		{
			// ĳ���� �������� �ε�Ǿ��ٸ�
			if ( m_bCharacterItemLoad )
			{
				// �ؽ�ó ������ ����
				SetTextureResize( m_pTexture, m_Header.nTexFileCount, _nResultTextureCount );
			}
			// ĳ���� �������� �ε���� �ʾҴٸ�
			else
			{
				m_pTexture = new stTexture[ m_Header.nTexFileCount ];
			}

			for ( int i = _nResultTextureCount; i < m_Header.nTexFileCount; ++i )
				fread( m_pTexture[i].strFileName, sizeof( char ), MAX_PATH, _filePtr );

			// �ؽ�ó �ߺ� üũ
			if ( m_bCharacterItemLoad )
			{
				int tCount = 0;

				for ( int i = 0; i < (m_Header.nTexFileCount - 1); ++i )
				{
					// ���� �����̸��� ���� �����̸��� ���ٸ�
					if ( strcmp( m_pTexture[i].strFileName, m_pTexture[i+1].strFileName ) == 0 )
						++tCount;
				}

				m_Header.nTexFileCount -= tCount;

				// �ߺ��� �ؽ�ó�� �ִٸ�
				if ( tCount > 0 )
				{
					// �ؽ�ó ������ ����
					SetTextureResize( m_pTexture, m_Header.nTexFileCount, _nResultTextureCount );

					for ( int i = 0; i < m_vtGeometryList.size(); ++i )
					{
						if ( i < _nGeometryCount )
							continue;

						// �ߺ��� �ؽ�ó���� ���ش�.
						--m_vtGeometryList[i]._nTextureCount;
					}
				}
			}
		}
	}

	void JKCharacter::ReadTextureUVInfo( FILE* _filePtr, int& _nGeometryCount )
	{
		for ( int i = 0; i < m_vtGeometryList.size(); ++i )
		{
			if ( m_bCharacterItemLoad && i < _nGeometryCount )
				continue;

			if ( m_vtGeometryList[i].nGeoType == GeometryType::GEO_MESH )
				continue;

			stVertexBlend* pVertexList = static_cast< stVertexBlend* >( m_vtGeometryList[i].pVertexList );

			// �ؽ�ó UV �б�
			for ( int j = 0; j < m_vtGeometryList[i].nVertexCount; ++j )
				fread( &pVertexList[j].vTexCoord, sizeof( D3DXVECTOR2 ), 1 _filePtr );
		}
	}

	void JKCharacter::CopyVertexWeightInfo( int& _nGeometryCount )
	{
		for ( int i = 0; i < m_vtGeometryList.size(); ++i )
		{
			if ( m_bCharacterItemLoad && i < _nGeometryCount )
				continue;

			if ( m_vtGeometryList[i].nBlendCount <= 0 )
				continue;

			stVertexBlend* pVertexList = static_cast< stVertexBlend* >( m_vtGeometryList[i].pVertexList );

			for ( int j = 0; j < m_vtGeometryList[i].nVertexCount; ++j )
			{
				// ���� ����
				stBoneIndexList* pBlendIndexList = m_vtGeometryList[i].pBlendIndexList[j];
				int	nBoneCount = pBlendIndexList->vtBoneIndex.size();

				float	fWeight[8]			= { 0 }; // ���� ����ġ
				byte	btIndexMatrix[8]	= { 0 }; // �ε��� ���

				for ( int k = 0; k < nBoneCount; ++k )
				{
					fWeight[k] = pBlendIndexList->vtBoneIndex[k].fWeight;
					btIndexMatrix[k] = pBlendIndexList->vtBoneIndex[k].nBoneIndex;
				}

				pVertexList[j].fWeight[0] = fWeight[0];
				pVertexList[j].fWeight[1] = fWeight[1];
				pVertexList[j].fWeight[2] = fWeight[2];

				pVertexList[j].btIndexMatrix[0] = btIndexMatrix[0];
				pVertexList[j].btIndexMatrix[1] = btIndexMatrix[1];
				pVertexList[j].btIndexMatrix[2] = btIndexMatrix[2];
				pVertexList[j].btIndexMatrix[3] = btIndexMatrix[3];
			}
		}
	}

	void JKCharacter::DeleteVertexWeightInfo( int& _nGeometryCount )
	{
		for ( int i = 0; i < m_vtGeometryList.size(); ++i )
		{
			if ( m_bCharacterItemLoad && i < _nGeometryCount )
				continue;

			if ( m_vtGeometryList[i].nBlendCount <= 0 )
				continue;

			SAFE_DELETE_ARRAY( m_vtGeometryList[i].pBlendIndexList );
		}
	}

	void JKCharacter::CreateGeometryTexture( char* _strFileName, int& _nTextureCount, int& _nResultTextureCount )
	{
		if ( _nTextureCount > 0 )
		{
			char path[MAX_PATH] = { 0 };
			char drive[_MAX_DRIVE] = { 0 };
			char dir[MAX_PATH] = { 0 };

			_splitpath( _strFileName, drive, dir, NULL, NULL );
			_makepath( path, drive, dir, NULL, NULL );

			for ( int i = _nResultTextureCount; i < m_Header.nTexFileCount; ++i )
			{
				char strFileName[MAX_PATH] = { 0 };

				sprintf( strFileName, "%s%s", g_strTextureFilePath, m_pTexture[i].strFileName );
				D3DXCreateTextureFromFileA( m_pd3dDevice, strFileName, &m_pTexture[i].pTexture );
			}
		}
	}

} // namespace JKCharacterTool