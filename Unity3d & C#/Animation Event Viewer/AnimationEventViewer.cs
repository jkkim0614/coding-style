using UnityEditor;
using UnityEditor.Animations;
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;

public class AnimationEventViewer : EditorWindow
{
	#region [Class]
	///////////////////////////////////////////////////////////////////////////////////////
	private class AnimClipInfo 
	{
		public AnimationClip clip = null;
		public bool isSelected = false;

		public AnimClipInfo( AnimationClip clip )
		{
			this.clip = clip;
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////
	#endregion [Class]


	#region [Field]
	///////////////////////////////////////////////////////////////////////////////////////
	//-----------------------------------------------------------------
	// GUI
	//-----------------------------------------------------------------
	private bool m_isInitOnce = false;
	private bool m_foldAnimEvent = true;
	private bool m_foldBase = true;
	private bool m_foldModel = true;

	private GUIStyle m_playBtnStyle = null;
	private GUIStyle m_playPlayingBtnStyle = null;
	private GUIStyle m_pauseBtnStyle = null;
	private GUIStyle m_pausePlayingBtnStyle = null;
	private GUIStyle m_stopBtnStyle = null;
	private GUIStyle m_prevBtnStyle = null;
	private GUIStyle m_nextBtnStyle = null;
	private GUIStyle m_resetBtnStyle = null;

	//-----------------------------------------------------------------
	// Default
	//-----------------------------------------------------------------
	private float m_deltaTime = 0.0f;
	private int m_beforeTickCount = 0;	
	private int m_drawCall = 0;
	private int m_prevWindowWidth = 0;
	private int m_prevWindowHeight = 0;
	
	//-----------------------------------------------------------------
	// Root
	//-----------------------------------------------------------------
	private GameObject m_rootObject = null;
	private Vector3 m_rootObjectPivot = new Vector3(0.0f, 1000.0f, 0.0f);

	//-----------------------------------------------------------------
	// Light
	//-----------------------------------------------------------------
	private GameObject m_lightObject = null;

	//-----------------------------------------------------------------
	// Grid
	//-----------------------------------------------------------------
	private GameObject m_gridObject = null;
	private Texture2D m_gridTexture = null;
	private bool m_showGrid = true;

	//-----------------------------------------------------------------
	// Camera
	//-----------------------------------------------------------------
	private GameObject m_baseCamObject = null;
	private Camera m_baseCamera = null;
	private RenderTexture m_baseCamRenderTex = null;
	private Rect m_baseCamRect;
	private Transform m_baseCamTransform = null;
	private Color m_baseCamBGColor = Color.white;
	private float m_baseCamFov = 0.0f;

	//-----------------------------------------------------------------
	// Model
	//-----------------------------------------------------------------
	private GameObject m_modelPrefab = null;
	private GameObject m_modelObject = null;
	private Transform m_modelTransform = null;
	private Renderer[] m_modelRenderers = null;
	private Dictionary<string, AnimClipInfo> m_animClipInfoDic = null;
	private AnimClipInfo m_selectedAnimClipInfo = null;
	private int m_currModelPrefabInstID = 0;

	///////////////////////////////////////////////////////////////////////////////////////
	#endregion [Field]


	#region [Property]
	///////////////////////////////////////////////////////////////////////////////////////
	private bool ShowGrid
	{
		set
		{
			if( m_showGrid == value )
				return;

			m_showGrid = value;
			m_gridObject.SetActive(m_showGrid);
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////
	#endregion [Property]


	#region [Function]
	///////////////////////////////////////////////////////////////////////////////////////
	//-----------------------------------------------------------------
	// Unity Function
	//-----------------------------------------------------------------
	[MenuItem("Viewer/Animation Event Viewer")]
	public static void OpenWindow()
	{
		AnimationEventViewer window = EditorWindow.GetWindow<AnimationEventViewer>(false, "Animation Event Viewer", true);
		window.autoRepaintOnSceneChange = true;
	}

	void OnEnable()
	{
		InitOnce();
	}

	void OnDisable()
	{
		StopSampleAnimation();
		Clear();
	}

	void Update()
	{
		UpdateFrameRate();
		UpdateAnimation();
	}

	void OnGUI()
	{
		UpdateSize();
		DrawCamera();
		if( true == m_isShowLeftPanel )
			DrawLeftPanel();
		DrawRightPanel();
		ProcessEvent();
	}

	//-----------------------------------------------------------------
	// Clear Function
	//-----------------------------------------------------------------
	private void ClearModel()
	{
		ClearEffect();
		ClearModelRenderer();
		ClearAnimation();
	}

	//-----------------------------------------------------------------
	// Init Function
	//-----------------------------------------------------------------
	private void InitOnce()
	{
		if( true == m_isInitOnce )
			return;

		CreateRoot();
		CreateLight();
		CreateCamera();
		CreateGrid();
		InitGUI();
		InitKeyEvent();

		m_isInitOnce = true;
	}

	//-----------------------------------------------------------------
	// Update Function
	//-----------------------------------------------------------------
	private void UpdateAnimation()
	{
		if( true == m_animController.isPlay &&
			false == m_animController.isPause )
		{
			PlaySampleAnimation();
			PlaySimulateEffect();
			UpdateDrawCall();
		}
	}

	//-----------------------------------------------------------------
	// Draw Function
	//-----------------------------------------------------------------
	private void DrawLeftPanel()
	{
		GUILayout.BeginArea(m_leftPanelRect);

		DrawGUIAnimEvent();

		GUILayout.EndArea();
	}

	private void DrawRightPanel()
	{
		GUILayout.BeginArea(m_rightPanelRect);

		DrawGUIBase();
		DrawGUIModel();

		GUILayout.EndArea();
	}

	private void DrawGUIBase()
	{
		EditorGUILayout.BeginHorizontal();
		EditorGUILayout.FloatField("Frame Rate", m_frameRate);
		EditorGUILayout.EndHorizontal();

		EditorGUILayout.BeginHorizontal();
		EditorGUILayout.IntField("Draw Call", m_drawCall);
		EditorGUILayout.EndHorizontal();

		EditorGUILayout.BeginHorizontal();
		ShowLeftPanel = EditorGUILayout.Toggle("Show Animation Event", m_isShowLeftPanel);
		EditorGUILayout.EndHorizontal();

		EditorGUILayout.BeginHorizontal();
		ShowGrid = EditorGUILayout.Toggle("Show Grid", m_showGrid);
		EditorGUILayout.EndHorizontal();

		EditorGUILayout.BeginHorizontal();
		BaseCamBGColor = EditorGUILayout.ColorField("Camera BG Color", m_baseCamBGColor);
		EditorGUILayout.EndHorizontal();

		EditorGUILayout.BeginHorizontal();
		BaseCamFov = EditorGUILayout.FloatField("Camera Fov", m_baseCamFov);
		EditorGUILayout.EndHorizontal();
	}

	//-----------------------------------------------------------------
	// Animation Function
	//-----------------------------------------------------------------
	private void PlayAnimation( ref AnimationClip clip )
	{
		if( m_animController.isPlay &&
			m_animController.IsEqualClip(clip.name) )
				return;

		m_animController.Set(clip);
		m_animController.isPlay = true;
		m_animController.isPause = false;

		if( true == m_isShowLeftPanel )
			PlayAnimationEvent();
	}

	//-----------------------------------------------------------------
	// Set Function
	//-----------------------------------------------------------------
	private void SelectAnimClip( AnimClipInfo info )
	{
		if( null != m_selectedAnimClipInfo )
		{
			m_selectedAnimClipInfo.isSelected = false;
			StopAnimation();
		}

		info.isSelected = true;
		m_selectedAnimClipInfo = info;
		PlayAnimation(ref info.clip);
	}

	//-----------------------------------------------------------------
	// Event Function
	//-----------------------------------------------------------------
	private void ProcessEvent()
	{
		ProcessMouseEvent();
		ProcessKeyEvent();
	}

	///////////////////////////////////////////////////////////////////////////////////////
	#endregion [Function]
}