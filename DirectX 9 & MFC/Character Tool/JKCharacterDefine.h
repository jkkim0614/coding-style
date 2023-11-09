/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	JKCharacterDefine.h
//
//	캐릭터 렌더링에 필요한 정보를 정의
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------
// 헤더
//--------------------------------------------------------------------------
#include <stdio.h>
#include <windows.h>
#include <vector>

namespace JKCharacterTool
{
	//--------------------------------------------------------------------------
	// 열거체
	//--------------------------------------------------------------------------
	enum GeometryType
	{
		GEO_ETC				= 0,
		GEO_MESH			= 1,
		GEO_BONE			= 2,
		GEO_HEADER_OFFSET	= 64,
		GEO_MAX_BLEND		= 128,
	};


	//--------------------------------------------------------------------------
	// 구조체
	//--------------------------------------------------------------------------
	struct stVerterIndex
	{
		WORD a, b, c;

		stVerterIndex() : a( 0 ), b( 0 ), c( 0 )
		{
		}

		stVerterIndex( WORD _a, WORD _b, WORD _c ) : a( _a ), b( _b ), c( _c )
		{
		}

		enum
		{
			FVF = (D3DFMT_INDEX16),
		};
	};

	struct stVertexPos
	{
		D3DXVECTOR3 vPos;

		stVertexPos() : vPos( 0.0f, 0.0f, 0.0f )
		{
		}

		stVertexPos( float x_, float y_, float z_ ) : vPos( 0.0f, 0.0f, 0.0f )
		{
		}

		enum
		{
			FVF = (D3DFVF_XYZ),
		};
	};

	struct stVertexBlend
	{
		D3DXVECTOR3 vPos;
		D3DXVECTOR3	vNormal;			// 노멀 벡터
		D3DXVECTOR2	vTexCoord;			// 텍스처 좌표
		float		fWeight[3];			// 블렌드 가중치
		byte		btIndexMatrix[4];	// 인덱스 행렬

		stVertexBlend() : vPos( 0.0f, 0.0f, 0.0f )
		{
			fWeight[0] = fWeight[1] = fWeight[2] = 0.0f;
			btIndexMatrix[0] = btIndexMatrix[1] = btIndexMatrix[2] = btIndexMatrix[3] = 0;
		}

		enum
		{
			FVF = (D3DFVF_XYZB4 | D3DFVF_LASTBETA_UBYTE4 | D3DFVF_NORMAL | D3DFVF_TEX1),
		};
	};

	struct stBone
	{
		int		nBoneIndex;
		float	fWeight;

		stBone() : nBoneIndex( -1 ), fWeight( 0.0f )
		{
		}

		stBone( int _nBoneIndex, float _fWeight ) : nBoneIndex( _nBoneIndex ), fWeight( _fWeight )
		{
		}
	};
	
	struct stBoneIndexList
	{
		std::vector< stBone >	vtBoneIndex;
	};

	struct stGeometry
	{
		char				strName[32];		// 노드 이름
		int					nGeoType;			// GeometryType
		int					nParentIndex;		// 부모 인덱스
		stGeometry*			pParent;			// 부모 노드
		int					nIndexCount;		// 인덱스 개수
		int					nVertexCount;		// 정점 개수
		DWORD				dwFVF;				// 정점 포멧
		UINT				uStride;			// Stride
		stVerterIndex*		pIndexList;			// 인덱스 리스트
		void*				pVertexList;		// 정점 리스트
		int					nBlendCount;		// Bone에 영향받는 버텍스 수
		stBoneIndexList*	pBlendIndexList;	// Bone 인덱스
		int					nAniCount;			// 애니메이션 개수
		D3DXMATRIX*			pAniMatrix;			// 애니메이션 행렬
		int					nTextureCount;		// 텍스처 개수

		stGeometry()
		{
			memset( strName, 0, sizeof( strName ) );
			nType = 0;
			nParentIndex = -1;
			pParent = NULL;
			nIndexCount = 0;
			nVertexCount = 0;
			dwFVF = 0;
			uStride	= 0;
			pIndexList = NULL;
			pVertexList = NULL;
			nBlendCount = 0;
			pBlendIndexList	= NULL;
			nAniCount = 0;
			pAniMatrix = NULL;
			nTextureCount = -1;
		}

		~stGeometry()
		{
			if ( pAniMatrix )
				SAFE_DELETE_ARRAY( pAniMatrix );

			if ( pBlend)
				SAFE_DELETE_ARRAY( pBlend );

			if ( pFaceList )
				SAFE_DELETE_ARRAY( pIndexList );

			if ( pVertexList )
				SAFE_FREE( pVertexList );	
		}
	};

	struct stHeader
	{
		int nFrameBegin;
		int nFrameEnd;
		int nFrameRate;
		int nTickFrame;
		int nGeometryCount;
		int nTexFileCount;

		stHeader()
		{
			nFrameBegin = 0;
			nFrameEnd = 0;
			nFrameRate = 0;
			nTickFrame = 0;
			nGeometryCount = 0;
			nTexFileCount = 0;
		}
	};

	struct stTexture
	{
		char				strFileName[MAX_PATH];
		LPDIRECT3DTEXTURE9	pTexture;

		stTexture() : pTexture( NULL )
		{
			memset( strFileName, 0, MAX_PATH );
		}

		stTexture( char* _strFileName, void* _pTexture )
		{
			strcpy( strFileName, _strFileName );
			pTexture = (LPDIRECT3DTEXTURE9)_pTexture;
		}
	};

	struct stBoundingBox
	{
		D3DXVECTOR3 vMin;
		D3DXVECTOR3 vMax;

		stBoundingBox()
		{
			vMin = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
			vMax = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		}
	};

	struct stBoundingBoxList
	{
		stBoundingBox* pBoundingBox;

		stBoundingBoxList() : pBoundingBox( NULL )
		{
		}

		~stBoundingBoxList()
		{
			SAFE_DELETE_ARRAY( pBoundingBox );
		}
	};

	struct stRay
	{
		D3DXVECTOR3	vOrigin;
		D3DXVECTOR3 vDirection;

		stRay()
		{
			vOrigin	= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
			vDirection = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		}
	};

	struct stBoundingSphere
	{
		D3DXVECTOR3 vCenter;
		float		fRadius;

		stBoundingSphere()
		{
			vCenter	= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
			fRadius	= 0.0f;
		}
	};

	struct stBoundingSphereArrayXYZ
	{
		stBoundingSphere* x, y, z;
		
		stBoundingSphereArrayXYZ() : x( NULL ), y( NULL ), z( NULL )
		{
		}

		~stBoundingSphereArrayXYZ()
		{
			SAFE_DELETE_ARRAY( x );
			SAFE_DELETE_ARRAY( y );
			SAFE_DELETE_ARRAY( z );
		}
	};

	struct stMeshXYZ
	{
		LPD3DXMESH	x, y, z;

		stMeshXYZ() : x( NULL ), y( NULL ), z( NULL )
		{
		}

		~stMeshXYZ()
		{
			SAFE_RELEASE( x );
			SAFE_RELEASE( y );
			SAFE_RELEASE( z );
		}
	};

	struct stBoolXYZ
	{
		bool x, y, z;

		stBoolXYZ() : x( false ), y( false ), z( false )
		{
		}
	};

	struct stMatrixXYZ
	{
		D3DXMATRIX	x, y, z;

		stMatrixXYZ()
		{
			D3DXMatrixIdentity( x );
			D3DXMatrixIdentity( y );
			D3DXMatrixIdentity( z );
		}
	};

	//--------------------------------------------------------------------------
	// typedef 정의
	//--------------------------------------------------------------------------
	typedef std::vector< stGeometry >	GeometryVector;


	//--------------------------------------------------------------------------
	// 텍스처 파일 경로
	//--------------------------------------------------------------------------
	const char*		g_strCharacterTextureFilePath = "../Resource/Texture/Character/";
	const wchar_t*	g_strCharacterEffectFilePath = L"../Resource/Shader/JKCharacter.fx";

} //namespace JKCharacterTool