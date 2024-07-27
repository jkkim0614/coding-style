#pragma once

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	JKCharacter.h
//
//	ĳ���� �������� ���
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------
// ���
//--------------------------------------------------------------------------
#include "JKCharacterDefine.h"
#include "../JKEngine/JKDefine.h"
#include "../JKEngine/JKDraw.h"


//--------------------------------------------------------------------------
// Ŭ���� ���漱��
//--------------------------------------------------------------------------
class JKLight;


//--------------------------------------------------------------------------
// Ŭ���� ����
//--------------------------------------------------------------------------
namespace JKCharacterTool
{
	// ĳ���� ���� Ŭ����
	class AFX_EXT_CLASS	JKCharacter : public JKDraw
	{
	private:
		//--------------------------------------------------------------------------
		// �������
		//--------------------------------------------------------------------------
		static const int				m_nEffectFVFMaxCount = 2;			// ����Ʈ ���� ���� �ִ� ����

	private:
		LPDIRECT3DDEVICE9				m_pd3dDevice;						// D3D ����̽� �ڵ�
		LPD3DXEFFECT					m_pEffect;							// ����Ʈ �ڵ�
		LPDIRECT3DVERTEXDECLARATION9	m_pEffctFVF[m_nEffectFVFMaxCount];	// ���ؽ� ���� ��ü
		GeometryVector					m_vtGeometryList;					// ������Ʈ�� ����Ʈ
		stHeader						m_Header;							// ���(������ ����, ������Ʈ�� ����, �ؽ�ó ����)
		stTexture*						m_pTexture;							// �ؽ�ó �ڵ�
		int								m_nTextureCount;					// �ؽ�ó ����
		D3DXVECTOR3						m_vRotation;						// ȸ�� ����
		D3DXVECTOR3						m_vTranslation;						// �̵� ����
		D3DXVECTOR3						m_vScale;							// ũ�� ����
		D3DXMATRIX						m_matWorld;							// ���� ���
		D3DXMATRIX						m_matRecovery;						// ���� ���
		D3DXMATRIX						m_matCharacterWorld;				// ĳ���� ���� ���
		bool							m_bCharacterItemLoad;				// �������� �ε��ߴ���, �� �ߴ���
		WORD							m_wEffectPass;						// ���� ���̴� ����Ʈ �н�
		D3DXMATRIX						m_matRotation;						// ȸ�� ���
		D3DXMATRIX						m_matScale;							// ũ�� ���
		D3DXMATRIX						m_matTranslation;					// �̵� ���
		bool							m_bSetupFirstCall;					// �¾� �Լ� ó�� ȣ������, �ƴ���
		bool							m_bSetup;							// �¾� �Լ� ȣ���ߴ���, �� �ߴ���
		bool							m_bWireFrame;						// ���̾�����������, �ƴ���

		// �ִϸ��̼�
		D3DXMATRIX						m_matBlend[GEO_MAX_BLEND];			// ���� ��� ����
		DWORD							m_dwFrameBegin;						// ù��° ������
		DWORD							m_dwFrameEnd;						// ������ ������
		int								m_nCurrentFrame;					// ���� ������
		int								m_nMaxFrame;						// ��ü ������
		float							m_fAniSpeedFrame;					// �ִϸ��̼� ���ǵ� ������
		bool							m_bAniOn;							// �ִϸ��̼��� ������, ��������
		int								m_nAniIndex;						// �ִϸ��̼� �ε���
		float							m_fAniSpeed;						// �ִϸ��̼� �ӵ�
		
		// ����
		JKLight*						m_pLightInstance;					// ���� ��ü
		bool							m_bLight;							// ���� ����������, �ƴ���

		// �Ƿ翧 ������
		bool							m_bOutLine;							// ������ ����������, �ƴ���
		
	public:
		//--------------------------------------------------------------------------
		// ����Լ�
		//--------------------------------------------------------------------------
		/*
		�� ����		: �⺻ ������
		*/
		JKCharacter();

		/*
		�� ����		: �Ű����� ������
		�� param 1	: LPDIRECT3DDEVICE9 �ڵ�
		�� param 2	: ���� ��ü
		*/
		JKCharacter( LPDIRECT3DDEVICE9 _pd3dDevice, void* _pLightInstance );

		/*
		�� ����		: �Ҹ���
		*/
		virtual ~JKCharacter();

		/*
		�� ����		: ������� �ʱ�ȭ
		*/
		void Initialize();

		/*
		�� ����		: �޸��Ҵ�� ���� ����
		*/
		void Destroy();

		/*
		�� ����		: �⺻ �¾�
		*/
		void Setup();

		/*
		�� ����		: �⺻ �¾� + �ִϸ��̼� ������ ����
		�� param 1	: ù ������
		�� param 2	: ������ ������
		*/
		void Setup( DWORD _dwFirstFrame, DWORD _dwLastFrame );
		
		/*
		�� ����		: ĳ���� ����
		*/
		void Draw();

		/*
		�� ����		: ĳ���� �ε�
		�� param 1	: ���ϰ�� ���ڿ�
		*/
		bool LoadCharacter( char* _strFileName );

		/*
		�� ����		: ĳ���� ������ �ε�
		�� param 1	: ���ϰ�� ���ڿ�
		�� param 2	: ĳ���� �������� �ε�Ǵ���, �ȵǴ��� �˸��µ� ���
		*/
		bool LoadItem( char* _strFileName, bool _m_bCharacterItemLoad );

		/*
		�� ����		: �ִϸ��̼� �ε�
		�� param 1	: ���ϰ�� ���ڿ�
		*/
		bool LoadAni( char* _strFileName );

		/*
		�� ����		: ����� �޸� ����
		*/
		void Destroy();

		/*
		�� ����		: ����Ʈ���� �ε�
		�� param 1	: ���ϰ�� ���ڿ�
		*/
		bool LoadEffectFile( const wchar_t* _strFilePath );

		/*
		�� ����		: ����Ʈ���� ���ε�
		�� param 1	: ���ϰ�� ���ڿ�
		*/
		void ReloadEffectFile( const wchar_t* _strFilePath );

		/*
		�� ����		: �ִϸ��̼� �¾�
		*/
		void SetupAnimation();

		/*
		�� ����		: DrawIndexedPrimitiveUP ȣ��
		�� param 1	: ������Ʈ�� ��ü
		*/
		void CallDrawIndexedPrimitiveUP( stGeometry& _pGeometry );

		/*
		�� ����		: ��� ���� �о����
		�� param 1	: ����������
		�� param 2	: ��� ����
		�� param 3	: ������Ʈ�� ����
		*/
		void ReadHeaderInfo( FILE* _filePtr, stHeader* _pHeader, int& _nGeometryCount );

		/*
		�� ����		: �޽� ���� �о����
		�� param 1	: ����������
		�� param 2	: ������Ʈ�� ����
		*/
		void ReadMeshInfo( FILE* _filePtr, int& _nGeometryCount );

		/*
		�� ����		: ���ؽ� ����ġ �о����
		�� param 1	: ����������
		�� param 2	: ������Ʈ�� ����
		*/
		void ReadVertexWeightInfo( FILE* _filePtr, int& _nGeometryCount );

		/*
		�� ����		: ���� ��ְ� �о����
		�� param 1	: ����������
		*/
		void ReadBlendNormalInfo( FILE* _filePtr );

		/*
		�� ����		: �ؽ�ó ������ ����
		�� param 1	: �ؽ�ó �ڵ�
		�� param 2	: ������ ������
		�� param 3	: ���Ҵ��� ������ ����
		*/
		void SetTextureResize( stTexture& _pTexture, int _nReSize, int _nCopyCount );

		/*
		�� ����		: �ؽ�ó �����̸� �о����
		�� param 1	: ���� ������
		�� param 2	: ��� ����
		�� param 3	: ���Ҵ��� ������ ����
		�� param 4	: �ؽ�ó ����
		�� param 5	: ���� �ؽ�ó ����
		�� param 6	: ������Ʈ�� ����
		*/
		void ReadTextureFileNameInfo( FILE* _filePtr, stHeader& _pHeader, int& _nTextureCount, 
			int& _nResultTextureCount, int& _nGeometryCount );

		/*
		�� ����		: �ؽ�ó UV �о����
		�� param 1	: ����������
		�� param 2	: ������Ʈ�� ����
		*/
		void ReadTextureUVInfo( FILE* _filePtr, int& _nGeometryCount );

		/*
		�� ����		: ���ؽ� ����ġ ����
		�� param 1	: ������Ʈ�� ����
		*/
		void CopyVertexWeightInfo( int& _nGeometryCount );

		/*
		�� ����		: ���ؽ� ����ġ ����
		�� param 1	: ������Ʈ�� ����
		*/
		void DeleteVertexWeightInfo( int& _nGeometryCount );

		/*
		�� ����		: ������Ʈ�� �ؽ�ó ����
		�� param 1	: ���ϰ�� ���ڿ�
		�� param 2	: �ؽ�ó ����
		�� param 3	: ���� �ؽ�ó ����
		*/
		void CreateGeometryTexture( char* _strFileName, int& _nTextureCount, int& _nResultTextureCount );

		/*
		�� ����		: ���̾������� ����
		�� param 1	: ���̾��������� ����� ������, ��������
		*/
		void SetWireFrame( bool _bWireFrame )
		{
			m_bWireFrame = _bWireFrame;
		}
		
		//--------------------------------------------------------------------------
		// �ִϸ��̼� ����
		//--------------------------------------------------------------------------
		/*
		�� ����		: ù������ ����
		�� param 1	: ù��° �����Ӱ�
		*/
		void SetBeginFrame( DWORD _dwFrameBegin )
		{
			m_dwFrameBegin = _dwFrameBegin;
		}

		/*
		�� ����		: ������ ������ ����
		�� param 1	: ������ �����Ӱ�
		*/
		void SetBeginEnd( DWORD _dwFrameEnd )
		{
			m_dwFrameEnd = _dwFrameEnd;
		}

		/*
		�� ����		: �ִϸ��̼� �ӵ� ����
		�� param 1	: �ִϸ��̼� �ӵ���
		*/
		void SetFrameSpeed( float _fAniSpeed )
		{
			m_fAniSpeed = _fAniSpeed;
		}

		/*
		�� ����		: ù ������ ��ȯ
		*/
		int GetBeginFrame() const
		{
			return m_Header.nFrameBegin;
		}

		/*
		�� ����		: ������ ������ ��ȯ
		*/
		int GetEndFrame() const
		{
			return m_Header.nFrameEnd;
		}

		/*
		�� ����		: ���� ������ ��ȯ
		*/
		int GetCurrentFrame() const
		{
			return m_nCurrentFrame;
		}

		//--------------------------------------------------------------------------
		// ���� ����
		//--------------------------------------------------------------------------
		//void DrawLight();

		//--------------------------------------------------------------------------
		// �Ƿ翧 ������ ����
		//--------------------------------------------------------------------------
		//void DrawOutLine();

	private:
		/*
		�� ����		: ĳ���� �޽� �� ĳ���� ������ �޽� �ε�
		�� param 1	: ���ϰ�� ���ڿ�
		*/
		bool Load( char* _strFileName );

	};	// class JKCharacter

} // namespace JKCharacterTool