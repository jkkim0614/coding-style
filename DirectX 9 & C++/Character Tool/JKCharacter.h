#pragma once

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	JKCharacter.h
//
//	캐릭터 렌더링을 담당
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------
// 헤더
//--------------------------------------------------------------------------
#include "JKCharacterDefine.h"
#include "../JKEngine/JKDefine.h"
#include "../JKEngine/JKDraw.h"


//--------------------------------------------------------------------------
// 클래스 전방선언
//--------------------------------------------------------------------------
class JKLight;


//--------------------------------------------------------------------------
// 클래스 선언
//--------------------------------------------------------------------------
namespace JKCharacterTool
{
	// 캐릭터 렌더 클래스
	class AFX_EXT_CLASS	JKCharacter : public JKDraw
	{
	private:
		//--------------------------------------------------------------------------
		// 멤버변수
		//--------------------------------------------------------------------------
		static const int				m_nEffectFVFMaxCount = 2;			// 이펙트 정점 포멧 최대 개수

	private:
		LPDIRECT3DDEVICE9				m_pd3dDevice;						// D3D 디바이스 핸들
		LPD3DXEFFECT					m_pEffect;							// 이펙트 핸들
		LPDIRECT3DVERTEXDECLARATION9	m_pEffctFVF[m_nEffectFVFMaxCount];	// 버텍스 선언 객체
		GeometryVector					m_vtGeometryList;					// 지오메트리 리스트
		stHeader						m_Header;							// 헤더(프레임 정보, 지오메트리 개수, 텍스처 개수)
		stTexture*						m_pTexture;							// 텍스처 핸들
		int								m_nTextureCount;					// 텍스처 개수
		D3DXVECTOR3						m_vRotation;						// 회전 벡터
		D3DXVECTOR3						m_vTranslation;						// 이동 벡터
		D3DXVECTOR3						m_vScale;							// 크기 벡터
		D3DXMATRIX						m_matWorld;							// 월드 행렬
		D3DXMATRIX						m_matRecovery;						// 복구 행렬
		D3DXMATRIX						m_matCharacterWorld;				// 캐릭터 월드 행렬
		bool							m_bCharacterItemLoad;				// 아이템을 로드했는지, 안 했는지
		WORD							m_wEffectPass;						// 현재 셰이더 이펙트 패스
		D3DXMATRIX						m_matRotation;						// 회전 행렬
		D3DXMATRIX						m_matScale;							// 크기 행렬
		D3DXMATRIX						m_matTranslation;					// 이동 행렬
		bool							m_bSetupFirstCall;					// 셋업 함수 처음 호출인지, 아닌지
		bool							m_bSetup;							// 셋업 함수 호출했는지, 안 했는지
		bool							m_bWireFrame;						// 와이어프레임인지, 아닌지

		// 애니메이션
		D3DXMATRIX						m_matBlend[GEO_MAX_BLEND];			// 블렌딩 행렬 버퍼
		DWORD							m_dwFrameBegin;						// 첫번째 프레임
		DWORD							m_dwFrameEnd;						// 마지막 프레임
		int								m_nCurrentFrame;					// 현재 프레임
		int								m_nMaxFrame;						// 전체 프레임
		float							m_fAniSpeedFrame;					// 애니메이션 스피드 프레임
		bool							m_bAniOn;							// 애니메이션이 온인지, 오프인지
		int								m_nAniIndex;						// 애니메이션 인덱스
		float							m_fAniSpeed;						// 애니메이션 속도
		
		// 광원
		JKLight*						m_pLightInstance;					// 광원 객체
		bool							m_bLight;							// 광원 렌더중인지, 아닌지

		// 실루엣 윤곽선
		bool							m_bOutLine;							// 윤곽선 렌더중인지, 아닌지
		
	public:
		//--------------------------------------------------------------------------
		// 멤버함수
		//--------------------------------------------------------------------------
		/*
		☞ 설명		: 기본 생성자
		*/
		JKCharacter();

		/*
		☞ 설명		: 매개변수 생성자
		☞ param 1	: LPDIRECT3DDEVICE9 핸들
		☞ param 2	: 광원 객체
		*/
		JKCharacter( LPDIRECT3DDEVICE9 _pd3dDevice, void* _pLightInstance );

		/*
		☞ 설명		: 소멸자
		*/
		virtual ~JKCharacter();

		/*
		☞ 설명		: 멤버변수 초기화
		*/
		void Initialize();

		/*
		☞ 설명		: 메모리할당된 변수 해제
		*/
		void Destroy();

		/*
		☞ 설명		: 기본 셋업
		*/
		void Setup();

		/*
		☞ 설명		: 기본 셋업 + 애니메이션 프레임 조정
		☞ param 1	: 첫 프레임
		☞ param 2	: 마지막 프레임
		*/
		void Setup( DWORD _dwFirstFrame, DWORD _dwLastFrame );
		
		/*
		☞ 설명		: 캐릭터 렌더
		*/
		void Draw();

		/*
		☞ 설명		: 캐릭터 로드
		☞ param 1	: 파일경로 문자열
		*/
		bool LoadCharacter( char* _strFileName );

		/*
		☞ 설명		: 캐릭터 아이템 로드
		☞ param 1	: 파일경로 문자열
		☞ param 2	: 캐릭터 아이템이 로드되는지, 안되는지 알리는데 사용
		*/
		bool LoadItem( char* _strFileName, bool _m_bCharacterItemLoad );

		/*
		☞ 설명		: 애니메이션 로드
		☞ param 1	: 파일경로 문자열
		*/
		bool LoadAni( char* _strFileName );

		/*
		☞ 설명		: 사용한 메모리 해제
		*/
		void Destroy();

		/*
		☞ 설명		: 이펙트파일 로드
		☞ param 1	: 파일경로 문자열
		*/
		bool LoadEffectFile( const wchar_t* _strFilePath );

		/*
		☞ 설명		: 이펙트파일 리로드
		☞ param 1	: 파일경로 문자열
		*/
		void ReloadEffectFile( const wchar_t* _strFilePath );

		/*
		☞ 설명		: 애니메이션 셋업
		*/
		void SetupAnimation();

		/*
		☞ 설명		: DrawIndexedPrimitiveUP 호출
		☞ param 1	: 지오메트리 객체
		*/
		void CallDrawIndexedPrimitiveUP( stGeometry& _pGeometry );

		/*
		☞ 설명		: 헤더 정보 읽어오기
		☞ param 1	: 파일포인터
		☞ param 2	: 헤더 정보
		☞ param 3	: 지오메트리 개수
		*/
		void ReadHeaderInfo( FILE* _filePtr, stHeader* _pHeader, int& _nGeometryCount );

		/*
		☞ 설명		: 메시 정보 읽어오기
		☞ param 1	: 파일포인터
		☞ param 2	: 지오메트리 개수
		*/
		void ReadMeshInfo( FILE* _filePtr, int& _nGeometryCount );

		/*
		☞ 설명		: 버텍스 가중치 읽어오기
		☞ param 1	: 파일포인터
		☞ param 2	: 지오메트리 개수
		*/
		void ReadVertexWeightInfo( FILE* _filePtr, int& _nGeometryCount );

		/*
		☞ 설명		: 블렌드 노멀값 읽어오기
		☞ param 1	: 파일포인터
		*/
		void ReadBlendNormalInfo( FILE* _filePtr );

		/*
		☞ 설명		: 텍스처 사이즈 변경
		☞ param 1	: 텍스처 핸들
		☞ param 2	: 변경할 사이즈
		☞ param 3	: 재할당할 사이즈 개수
		*/
		void SetTextureResize( stTexture& _pTexture, int _nReSize, int _nCopyCount );

		/*
		☞ 설명		: 텍스처 파일이름 읽어오기
		☞ param 1	: 파일 포인터
		☞ param 2	: 헤더 정보
		☞ param 3	: 재할당할 사이즈 개수
		☞ param 4	: 텍스처 개수
		☞ param 5	: 최종 텍스처 개수
		☞ param 6	: 지오메트리 개수
		*/
		void ReadTextureFileNameInfo( FILE* _filePtr, stHeader& _pHeader, int& _nTextureCount, 
			int& _nResultTextureCount, int& _nGeometryCount );

		/*
		☞ 설명		: 텍스처 UV 읽어오기
		☞ param 1	: 파일포인터
		☞ param 2	: 지오메트리 개수
		*/
		void ReadTextureUVInfo( FILE* _filePtr, int& _nGeometryCount );

		/*
		☞ 설명		: 버텍스 가중치 복사
		☞ param 1	: 지오메트리 개수
		*/
		void CopyVertexWeightInfo( int& _nGeometryCount );

		/*
		☞ 설명		: 버텍스 가중치 제거
		☞ param 1	: 지오메트리 개수
		*/
		void DeleteVertexWeightInfo( int& _nGeometryCount );

		/*
		☞ 설명		: 지오메트리 텍스처 생성
		☞ param 1	: 파일경로 문자열
		☞ param 2	: 텍스처 개수
		☞ param 3	: 최종 텍스처 개수
		*/
		void CreateGeometryTexture( char* _strFileName, int& _nTextureCount, int& _nResultTextureCount );

		/*
		☞ 설명		: 와이어프레임 설정
		☞ param 1	: 와이어프레임을 사용할 것인지, 말것인지
		*/
		void SetWireFrame( bool _bWireFrame )
		{
			m_bWireFrame = _bWireFrame;
		}
		
		//--------------------------------------------------------------------------
		// 애니메이션 관련
		//--------------------------------------------------------------------------
		/*
		☞ 설명		: 첫프레임 설정
		☞ param 1	: 첫번째 프레임값
		*/
		void SetBeginFrame( DWORD _dwFrameBegin )
		{
			m_dwFrameBegin = _dwFrameBegin;
		}

		/*
		☞ 설명		: 마지막 프레임 설정
		☞ param 1	: 마지막 프레임값
		*/
		void SetBeginEnd( DWORD _dwFrameEnd )
		{
			m_dwFrameEnd = _dwFrameEnd;
		}

		/*
		☞ 설명		: 애니메이션 속도 설정
		☞ param 1	: 애니메이션 속도값
		*/
		void SetFrameSpeed( float _fAniSpeed )
		{
			m_fAniSpeed = _fAniSpeed;
		}

		/*
		☞ 설명		: 첫 프레임 반환
		*/
		int GetBeginFrame() const
		{
			return m_Header.nFrameBegin;
		}

		/*
		☞ 설명		: 마지막 프레임 반환
		*/
		int GetEndFrame() const
		{
			return m_Header.nFrameEnd;
		}

		/*
		☞ 설명		: 현재 프레임 반환
		*/
		int GetCurrentFrame() const
		{
			return m_nCurrentFrame;
		}

		//--------------------------------------------------------------------------
		// 광원 관련
		//--------------------------------------------------------------------------
		//void DrawLight();

		//--------------------------------------------------------------------------
		// 실루엣 윤곽선 관련
		//--------------------------------------------------------------------------
		//void DrawOutLine();

	private:
		/*
		☞ 설명		: 캐릭터 메시 및 캐릭터 아이템 메시 로드
		☞ param 1	: 파일경로 문자열
		*/
		bool Load( char* _strFileName );

	};	// class JKCharacter

} // namespace JKCharacterTool