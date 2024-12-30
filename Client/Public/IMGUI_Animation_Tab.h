#pragma once
#include "IMGUI_Tab.h"
#include "Model.h"

//#include "../../AssimpSaveAndLoad/Data.h"

#include "AnimationEvent_Defines.h"
#include "FrameEvent_Manager.h"
BEGIN(Client)

struct VertexAnim {
    XMFLOAT3 vPosition;
    XMFLOAT3 vNormal;
    XMFLOAT2 vTexcoord;
    XMFLOAT3 vTangent;
    XMUINT4 vBlendIndex;
    XMFLOAT4 vBlendWeight;
};

struct Vertex {
    XMFLOAT3 vPosition;
    XMFLOAT3 vNormal;
    XMFLOAT2 vTexcoord;
    XMFLOAT3 vTangent;
};

struct Keyframe {
    XMFLOAT3 scale;
    XMFLOAT4 rotation;
    XMFLOAT3 position;
    float time;
};

//struct MODEL_DATA {
//    struct ANIMATION {
//        const char* szName;       // �ִϸ��̼� �̸�
//        float fDuration;          // �ִϸ��̼��� ���� �ð�
//        float fTickPerSecond;     // �ʴ� ƽ �� (�ִϸ��̼� �ӵ�)
//        unsigned int iNumChannels;// ä��(��)�� ����
//
//        struct CHANNEL {
//            const char* szName;         // �� �Ǵ� ����� �̸�
//            unsigned int iBoneIndex;    // �� �ε���
//            unsigned int iNumKeyFrames; // Ű�������� ����
//
//            struct KEYFRAME {
//                float fTime;           // Ű�������� �ð�
//                XMFLOAT3 vScale;       // ������ ��
//                XMFLOAT4 vRotation;    // ȸ�� �� (���ʹϾ�)
//                XMFLOAT3 vPosition;    // ��ġ ��
//            };
//
//            std::vector<KEYFRAME> keyFrames;  // Ű������ ����Ʈ
//        };
//
//        std::vector<CHANNEL> channels;   // ä�� ����Ʈ
//    };
//
//    std::vector<ANIMATION> animations;  // �ִϸ��̼� ����Ʈ
//};


class CIMGUI_Animation_Tab : public CIMGUI_Tab
{

protected:
	CIMGUI_Animation_Tab(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CIMGUI_Animation_Tab() = default;

public:
    virtual void Effect_Menu() override;
    virtual void Camera_Set(_uint iNumCameraModel) override;
    virtual void Effect_Transform() override;
	HRESULT Initialize() override;
	void Render(_float fTimeDelta) override;

	virtual const _char* GetTabName() const { return "Animation"; };

    void Info_Anim();

private:
	//void LoadModelDataFromBinary();
	//void LoadBoneFromBinary_Tool(ifstream& inFile, BoneData& bone);
    void SelectedModelLoad(ifstream& inFile, BoneData& bone);

    void LoadFromFile(const string& filename);
    //void ProcessEventsBetweenFrames(int characterIndex, int animationIndex, float prevFrame, float currentFrame);
    void ProcessEventsBetweenFrames2(int characterIndex, int animationIndex, float prevFrame, float currentFrame);

    void ProcessEventsFramesZero(int characterIndex, int animationIndex);
    void SaveEventTXT(const _char* TextFilePath, string strNeweventText);

    string Get_strAnimationEvent();
    string Get_CharacterName();
    string Get_AnimationName();

    void ExportAllAnimationToTXT();


    short prevKeyState;
public:
     
private:

   // _bool m_bDrag = false;
    CModel*         m_pSelectedModelCom = { nullptr };
    CGameObject*    m_pSelectedObject = { nullptr };
    CTransform* m_pSelectedObjectTransformCom = { nullptr };
    _char buffer[128] = "../Bin/ModelData/Frieza_Select.bin";
    _bool m_bMotionPlaying = false;
    _bool m_bCurrentPositionSlide = false;

    _float fSliderDebug = 0.f;

    FrameEventMap* m_pFrameEvent = { nullptr };


    CHARACTER_INDEX m_iTestModelIndex = { SKILL_GOKU };
    _float m_fAnimationUpdateTime = {0.005f};
    _float m_fTool_TickPerSecond = { 25.f };
    _char m_PostionBuffer[30] = "0.0";


   // _char m_EventTXTBuffer[128] = "../Bin/FrameEventData/EventData_Hit.txt";
    //_char m_EventTXTBuffer[128] = "../Bin/FrameEventData/EventData_21.txt";
   // _char m_EventTXTBuffer[128] = "../Bin/FrameEventData/EventData_Goku.txt";
    _char m_EventTXTBuffer[128] = "../Bin/FrameEventData/EventData_Frieza.txt";


 public:
	static CIMGUI_Animation_Tab* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
