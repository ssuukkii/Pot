#include "stdafx.h"
#include "IMGUI_Animation_Tab.h"
#include "GameInstance.h"
#include "RenderInstance.h"

#include "imgui.h"

#include <fstream>  
#include <iostream> 
#include <vector>   
#include <string>   

//#include "Windows.h"
//#include "shellapi.h"

#include "Animation.h"
#include "Channel.h"




CIMGUI_Animation_Tab::CIMGUI_Animation_Tab(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CIMGUI_Tab{ pDevice,pContext }
{
}


//static FrameEventMap FrameEvent;// [CHARACTER_INDEX_END] [100] [2] ;

void CIMGUI_Animation_Tab::Effect_Menu()
{
}

void CIMGUI_Animation_Tab::Camera_Set(_uint iNumCameraModel)
{
}

void CIMGUI_Animation_Tab::Effect_Transform()
{
}

HRESULT CIMGUI_Animation_Tab::Initialize()
{

    m_pFrameEvent = CFrameEvent_Manager::Get_Instance()->Get_pFrameEventMap();




    m_iTestModelIndex = PLAY_FRN;

  

    //static FrameEventMap FrameEvent[CHARACTER_INDEX_END][100][2];
  //  FrameEvent[SELECT_HIT][0][5].emplace("TEST");
  //  FrameEvent[SELECT_HIT][0][50].push_back("MotionLock,1,10");
  //  FrameEvent[SELECT_HIT][1][0].push_back("ZERO");


    //FrameEvent[PlAYER_GOKU][ATTACK][50].push_back("TEST");

    //�ִϸ��̼� ����ӵ� ��ü�� 0���� �ٲٰ�  0.1�� �ڿ�  Ư�� ���������� �����δٴ��� �ϴ� �Լ��� �ʿ��ҵ�
   
    /*
    animation lock and jump(_float fLockTime, _float fNextPosition);   
    {
        m_fAccLockTime= 0.f;
        m_fLockTime = fLockTime;
        _bool AnimationLock = true;
        m_fNextPosition = fNextPosition;
    }

    if(m_fAccLockTime > fLockTime)
    {
        AnimationLock = false;
        m_fCurrentPosition = m_fNextPosition;
       
       m_fAccLockTime=0.f;
       m_fLockTime=0.f;
       m_fNextPosition=0.f;
    }

    //������ٰ� ġ�µ� �� �����Ѱ� ��� string���� ó����?
    */


   // CFrameEvent_Manager::Get_Instance()->Add_Event(TEXT("MotionLock,1.02f,100"));



   //ù �����϶�, ������������ ZeroFrame�� ��� �����ϴ°�
  

   
    //�̰� ModelPreview�� �̻簨
    //CFrameEvent_Manager::Get_Instance()->Initalize_NameMap();
   // CFrameEvent_Manager::Get_Instance()->LoadFile2("../Bin/FrameEventData/EventData_Frieza.txt");



	return S_OK;
}


static int g_iCurrentAnimationIndex = 999;
static bool g_isLoop = false;

void CIMGUI_Animation_Tab::Render(_float fTimeDelta)
{
    



    ImGui::InputText("Label", buffer, IM_ARRAYSIZE(buffer));

    if (ImGui::Button("Label Load"))
    {

        //../Bin/ModelData\Hit_Select.bin

        if (m_pSelectedModelCom != nullptr)
            Safe_Release(m_pSelectedModelCom);

        _fmatrix preMatrix = XMMatrixIdentity();
       m_pSelectedModelCom = CModel::Create(m_pDevice, m_pContext, buffer, preMatrix);

    }

    ImGui::SameLine();
    if (ImGui::Button("Object Load"))
    {


        if (m_pSelectedModelCom != nullptr)
            Safe_Release(m_pSelectedModelCom);

       
        m_pSelectedObject = m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Model_Preview"), 0);
        m_pSelectedModelCom = static_cast<CModel*>(m_pSelectedObject->Get_Component(TEXT("Com_Model")));

        m_pSelectedObjectTransformCom = static_cast<CTransform*>(m_pSelectedObject->Get_Component(TEXT("Com_Transform")));

       
        m_pSelectedModelCom->m_bNoMoveXZ = true;

    }

    //ImGui::NewLine();
    if (ImGui::Button("Event ReLoad"))
    {
        CFrameEvent_Manager::Get_Instance()->ReLoadFrameEvent(m_EventTXTBuffer);
    }

    ImGui::SameLine();
    if (ImGui::Button("Event Delete"))
    {
        CFrameEvent_Manager::Get_Instance()->ClearFrameEvent();
    }


    ImGui::NewLine();
    if (m_pSelectedModelCom != nullptr)
    {

         if (ImGui::Button("AnimationName Export"))
         {
             ExportAllAnimationToTXT();
         }
         ImGui::SameLine();
         if (ImGui::Button("Select Position Reset"))
         {
             CTransform* pTrasnform = static_cast<CTransform*>(m_pSelectedObject->Get_Component(TEXT("Com_Transform")));
             pTrasnform->Set_State(CTransform::STATE_POSITION, { 0,0,0,1 });

         }
         ImGui::SameLine();
         if (ImGui::Button("Toggle NoMoveXZ"))
         {

             // Layer_Camera
             //CGameObject* pObject = m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Camera"));
             //CTransform* pTrasnform = static_cast<CTransform*>(pObject->Get_Component(TEXT("Com_Transform")));
             //
             ////_vector Debug = pTrasnform->Get_State(CTransform::STATE_POSITION);
             ////_vector Debug2 = pTrasnform->Get_State(CTransform::STATE_LOOK);
             //
             //
             //pTrasnform->Set_State(CTransform::STATE_POSITION, { 0,1,-5,1 });
             //pTrasnform->LookAt({0, 0, 0, 1});



             m_pSelectedModelCom->m_bNoMoveXZ = !m_pSelectedModelCom->m_bNoMoveXZ;

         }
         Info_Anim();

        

         if (m_bMotionPlaying)
         {

             _float fPrePosition = m_pSelectedModelCom->m_fCurrentAnimPosition;

             _int iOneFrameTeest = 0;

             if (fPrePosition == 0)
             {
                 ProcessEventsFramesZero(m_iTestModelIndex, g_iCurrentAnimationIndex);
                 fPrePosition += 0.001;

                 iOneFrameTeest++;
             }

       

             //  if (m_pSelectedModelCom->Play_Animation_Lick(fTimeDelta))
             if (m_pSelectedModelCom->Play_Animation_Lick2(fTimeDelta, m_pSelectedObjectTransformCom))
             {
                 //����� ��������, ������    (�Ʊ���� ������ �ƴϿ��µ� �̹��� ������ ��¼��?)
                 if(m_pSelectedModelCom->m_isLoopAnim)
                 {
                     fPrePosition = 0.001;
                     ProcessEventsFramesZero(m_iTestModelIndex, g_iCurrentAnimationIndex);
                     iOneFrameTeest++;
                 }
             }

             iOneFrameTeest;
             
             if (iOneFrameTeest > 1)
             {
                 _bool bDebug = false;
             }

             _float fCurPosition = m_pSelectedModelCom->m_fCurrentAnimPosition;
             
            
             ProcessEventsBetweenFrames2(0, m_pSelectedModelCom->m_iCurrentAnimationIndex, fPrePosition, fCurPosition);

         }

         if (m_bCurrentPositionSlide)
         {
             _float fCurrentPosition =(m_pSelectedModelCom->m_fCurrentAnimPosition);

             ImGui::Text("CurrentAnimPosition :%f ", (m_pSelectedModelCom->m_fCurrentAnimPosition));
             float maxValue = m_pSelectedModelCom->m_Animations[g_iCurrentAnimationIndex]->m_fDuration;       // �����̴� �ִ밪
             if (ImGui::SliderFloat("Current Position", &(m_pSelectedModelCom->m_fCurrentAnimPosition), 0, maxValue))
             {
                 
                 //�پ��� 0���� �ٿ��ٰ� �����?
                 if (fCurrentPosition > m_pSelectedModelCom->m_fCurrentAnimPosition)
                 {
                     _float fSavePosition = m_pSelectedModelCom->m_fCurrentAnimPosition;

                     m_pSelectedModelCom->SetUp_Animation(0, false, 0);
                     m_pSelectedModelCom->SetUp_Animation(g_iCurrentAnimationIndex, g_isLoop);
                     //m_pSelectedModelCom->Play_Animation(fSavePosition / 25.f);
                     m_pSelectedModelCom->Play_Animation(fSavePosition/ m_pSelectedModelCom->Get_pCurrentAnimation()->m_fTickPerSecond);  


                 }
                 //�þ�ٸ�   ������� ������ if �߰��ؾߵ�
                 else if (fCurrentPosition < m_pSelectedModelCom->m_fCurrentAnimPosition)
                 {
                     //���̸�ŭ PlayAnimation
                     _float fdifference = m_pSelectedModelCom->m_fCurrentAnimPosition - fCurrentPosition;
                     m_pSelectedModelCom->Play_Animation(fdifference / m_pSelectedModelCom->Get_pCurrentAnimation()->m_fTickPerSecond);

                 }

             }


         }

    }

    _bool DebugPoint = false;

    
    SHORT currKeyState = GetAsyncKeyState(VK_SPACE);  // ���÷� �����̽��ٸ� ���

    // Ű�� ������ ������ ����
    if ((currKeyState & 0x8000) && !(prevKeyState & 0x8000))
    {
        // Ű�� ������ ����
        m_bMotionPlaying = !m_bMotionPlaying;
    }

    // ���� ���¸� ���� ���·� ������Ʈ
    prevKeyState = currKeyState;
}

void CIMGUI_Animation_Tab::Info_Anim()
{
    //999�� �� ������ 0���� �ϸ� ������ 
   // static int g_iCurrentAnimationIndex = 999;


    vector<const char*> animNames;

    //�¿�
   // for (const auto& anim : m_pSelectedModelCom->m_ModelData.animations) {
   //     animNames.push_back(anim.szName);
   // }

    //���� ��
    for (auto AnimData : m_pSelectedModelCom->m_Animations)
    {
        animNames.push_back(AnimData->m_szName);
    }


    ImGui::Text("Selected Animation");
    ImGui::Combo("##", &g_iCurrentAnimationIndex, animNames.data(), animNames.size());

    if (g_iCurrentAnimationIndex < animNames.size())
    {
        // ���� ���õ� �ִϸ��̼� �����͸� ������
         
        //Ver.�¿�
        //const auto& selectedAnim = m_pSelectedModelCom->m_ModelData.animations[g_iCurrentAnimationIndex];


        const auto& selectedAnim = m_pSelectedModelCom->m_Animations[g_iCurrentAnimationIndex];

        // �ִϸ��̼��� �� ����� ImGui::Text�� ǥ��
        ImGui::Text("AnimIndex: %d", g_iCurrentAnimationIndex);
        ImGui::Text("Name: %s", selectedAnim->m_szName);
        ImGui::Text("Duration: %f", selectedAnim->m_fDuration);
        ImGui::Text("Ticks Per Second: %f", selectedAnim->m_fTickPerSecond);
        ImGui::Text("Number of Channels: %u", selectedAnim->m_iNumChannels);

        // �� ä���� �ϳ��� ���� Ʈ�� ���� ǥ��
        if (ImGui::TreeNode("Channels"))
        {
            // �� ä���� ���� ǥ��
            for (size_t i = 0; i < selectedAnim->m_Channels.size(); ++i)
            {
                const auto& channel = selectedAnim->m_Channels[i];
                
               //const auto channel = selectedAnim->m_Channels[i];


                //if (ImGui::TreeNode((void*)(intptr_t)i, "Channel %u: %s", static_cast<unsigned>(i), channel->m_szName))
                if (ImGui::TreeNode((void*)(intptr_t)i, "Channel %u: %s", static_cast<unsigned>(i), channel->GetName()))
                {
                   //ImGui::Text("  Name: %s", channel.m_szName);
                   //ImGui::Text("  Bone Index: %u", channel.iBoneIndex);
                   //ImGui::Text("  Number of KeyFrames: %u", channel.iNumKeyFrames);

                    ImGui::Text("  Name: %s", channel->GetName());
                    ImGui::Text("  Bone Index: %u", channel->m_iBoneIndex);
                    ImGui::Text("  Number of KeyFrames: %u", channel->m_iNumKeyFrames);

                    // �� Ű�������� ���� ǥ��
                    if (ImGui::TreeNode("KeyFrames"))
                    {
                        for (size_t j = 0; j < channel->m_KeyFrames.size(); ++j)
                        {
                            const auto& keyFrame = channel->m_KeyFrames[j];
                            if (ImGui::TreeNode((void*)(intptr_t)j, "KeyFrame %u", static_cast<unsigned>(j)))
                            {
                                ImGui::Text("    Time: %f", keyFrame.fTime);
                                ImGui::Text("    Scale: [%f, %f, %f]", keyFrame.vScale.x, keyFrame.vScale.y, keyFrame.vScale.z);
                                ImGui::Text("    Rotation: [%f, %f, %f, %f]", keyFrame.vRotation.x, keyFrame.vRotation.y, keyFrame.vRotation.z, keyFrame.vRotation.w);
                                ImGui::Text("    Position: [%f, %f, %f]", keyFrame.vPosition.x, keyFrame.vPosition.y, keyFrame.vPosition.z);
                                ImGui::TreePop();
                            }
                        }
                        ImGui::TreePop();
                    }
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }

        ImGui::Checkbox("Loop", &g_isLoop);
        if (ImGui::Button("Play"))
        {

           //if (m_pSelectedModelCom->m_fCurrentAnimPosition == m_pSelectedModelCom->m_Animations[g_iCurrentAnimationIndex]->m_fDuration)
           //{
           //    m_pSelectedModelCom->m_fCurrentAnimPosition = 0.f;
           //}
            m_pSelectedModelCom->SetUp_Animation(g_iCurrentAnimationIndex, g_isLoop);
            
           // ProcessEventsFramesZero(m_iTestModelIndex, g_iCurrentAnimationIndex);
           

            //ProcessEventsFramesZero((int)SELECT_HIT, g_iCurrentAnimationIndex);
            m_bMotionPlaying = true;
            m_bCurrentPositionSlide = true;

        }

        ImGui::SameLine();
        if (ImGui::Button("Pause"))
        {
            m_bMotionPlaying = false;

        }
        
        if (ImGui::Button("Toggle"))
        {
            m_bMotionPlaying = !m_bMotionPlaying;

        }
        ImGui::SameLine();
        if (ImGui::Button("Stop"))
        {

            m_bMotionPlaying = false;
            m_bCurrentPositionSlide = false;
            m_pSelectedModelCom->SetUp_Animation(g_iCurrentAnimationIndex, false, 0);
            m_pSelectedModelCom->m_fCurrentAnimPosition = 0.f;
        }

        if (ImGui::SliderFloat("AnimationUpdateTime", &m_fAnimationUpdateTime, 0.005, 1.f))
        {
            m_pSelectedModelCom->Set_MaxAnimationUpdate_Time(m_fAnimationUpdateTime);
        }
        if (ImGui::SliderFloat("TickPerSecond", &m_fTool_TickPerSecond, 0, 120.f))
        {
            m_pSelectedModelCom->m_Animations[g_iCurrentAnimationIndex]->m_fTickPerSecond = m_fTool_TickPerSecond;
        }

        if (ImGui::Button("Save Time&Speed  to this Animation&Position"))
        {
            SaveEventTXT(m_EventTXTBuffer, Get_strAnimationEvent());

        }

        ImGui::InputText("Target Position", m_PostionBuffer, IM_ARRAYSIZE(m_PostionBuffer));
        if (ImGui::Button("Position Move"))
        {

            _float fTarget_Position = stof(m_PostionBuffer);


            m_pSelectedModelCom->SetUp_Animation(0, false, 0);
            m_pSelectedModelCom->SetUp_Animation(g_iCurrentAnimationIndex, g_isLoop);
            //m_pSelectedModelCom->Play_Animation(fSavePosition / 25.f);
            m_pSelectedModelCom->Play_Animation(fTarget_Position / m_pSelectedModelCom->Get_pCurrentAnimation()->m_fTickPerSecond);

        }
    }
}


 
void CIMGUI_Animation_Tab::SelectedModelLoad(std::ifstream& inFile, BoneData& bone)
{
    m_pSelectedModelCom->LoadBoneFromBinary(inFile, bone);
}



//void CIMGUI_Animation_Tab::LoadBoneFromBinary_Tool(std::ifstream& inFile, BoneData& bone) {
//    // BoneData�� �̸� �б�
//    size_t nameLength;
//    inFile.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
//    bone.name.resize(nameLength);
//    inFile.read(&bone.name[0], nameLength);
//    
//    // BoneData�� ��ȯ ��� �б�
//    inFile.read(reinterpret_cast<char*>(&bone.transformationMatrix), sizeof(XMFLOAT4X4));
//    
//    // BoneData�� �ڽ� ���� �б�
//    uint32_t numChildren;
//    inFile.read(reinterpret_cast<char*>(&numChildren), sizeof(numChildren));
//    bone.children.resize(numChildren);
//    for (auto& child : bone.children) {
//        LoadBoneFromBinary_Tool(inFile, child);
//    }
//}

void CIMGUI_Animation_Tab::LoadFromFile(const std::string& filename)
{
    std::ifstream inFile(filename, std::ios::binary);
    if (!inFile) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    // ModelHeader �б�
    ModelHeader header;
    inFile.read(reinterpret_cast<char*>(&header), sizeof(header));
    std::cout << "Loaded header: " << header.numMeshes << " meshes, "
        << header.numMaterials << " materials, "
        << header.numAnimations << " animations" << std::endl;

    std::vector<MeshData> meshes(header.numMeshes);
    for (auto& mesh : meshes) {
        // Mesh �̸� �б�
        size_t nameLength;
        inFile.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
        mesh.name.resize(nameLength);
        inFile.read(&mesh.name[0], nameLength);

        // Vertex�� Index ���� �б�
        inFile.read(reinterpret_cast<char*>(&mesh.vertexCount), sizeof(mesh.vertexCount));
        inFile.read(reinterpret_cast<char*>(&mesh.indexCount), sizeof(mesh.indexCount));

        // Anim ���ο� ���� Vertex ������ �б�
        if (header.isAnim) {
            mesh.animVertices.resize(mesh.vertexCount);
            inFile.read(reinterpret_cast<char*>(mesh.animVertices.data()), mesh.vertexCount * sizeof(VertexAnim));
        }
        else {
            mesh.vertices.resize(mesh.vertexCount);
            inFile.read(reinterpret_cast<char*>(mesh.vertices.data()), mesh.vertexCount * sizeof(Vertex));
        }

        // Index ������ �б�
        mesh.indices.resize(mesh.indexCount);
        inFile.read(reinterpret_cast<char*>(mesh.indices.data()), mesh.indexCount * sizeof(unsigned int));

        // Material index �б�
        inFile.read(reinterpret_cast<char*>(&mesh.materialIndex), sizeof(mesh.materialIndex));

        if (header.isAnim) {
            // ���� ���� �б�
            uint32_t numBones;
            inFile.read(reinterpret_cast<char*>(&numBones), sizeof(numBones));
            mesh.bones.resize(numBones);
            for (auto& bone : mesh.bones) {
                //LoadBoneFromBinary(inFile, bone);
               m_pSelectedModelCom->LoadBoneFromBinary(inFile, bone);

            }

           //SelectedModelLoad(inFile, bone);
           //m_pSelectedModelCom->LoadBoneFromBinary(inFile, bone);


            // ������ ��� �б�
            uint32_t numOffsetMatrices;
            inFile.read(reinterpret_cast<char*>(&numOffsetMatrices), sizeof(numOffsetMatrices));
            mesh.offsetMatrices.resize(numOffsetMatrices);
            inFile.read(reinterpret_cast<char*>(mesh.offsetMatrices.data()), numOffsetMatrices * sizeof(XMFLOAT4X4));
        }
    }

    // MaterialData �б�
    std::vector<MaterialData> materials(header.numMaterials);
    for (auto& material : materials) {
        for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; ++i) {
            size_t pathLength;
            inFile.read(reinterpret_cast<char*>(&pathLength), sizeof(pathLength));
            material.texturePaths[i].resize(pathLength);
            inFile.read(&material.texturePaths[i][0], pathLength);

            std::cout << "Loaded texture path: " << material.texturePaths[i] << std::endl;
        }
    }

    // AnimationData �б�
    std::vector<AnimationData> animations(header.numAnimations);
    if (header.isAnim) {
        for (auto& anim : animations) {
            // Animation �̸� �б�
            size_t nameLength;
            inFile.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
            anim.name.resize(nameLength);
            inFile.read(&anim.name[0], nameLength);

            // Animation duration�� ticks per second �б�
            inFile.read(reinterpret_cast<char*>(&anim.duration), sizeof(anim.duration));
            inFile.read(reinterpret_cast<char*>(&anim.ticksPerSecond), sizeof(anim.ticksPerSecond));

            // Animation ä�� �б�
            uint32_t numChannels;
            inFile.read(reinterpret_cast<char*>(&numChannels), sizeof(numChannels));
            anim.channels.resize(numChannels);
            for (auto& channel : anim.channels) {
                // ä�� �̸� �б�
                size_t channelNameLength;
                inFile.read(reinterpret_cast<char*>(&channelNameLength), sizeof(channelNameLength));
                channel.name.resize(channelNameLength);
                inFile.read(&channel.name[0], channelNameLength);

                // Keyframes �б�
                uint32_t numKeyframes;
                inFile.read(reinterpret_cast<char*>(&numKeyframes), sizeof(numKeyframes));
                channel.keyframes.resize(numKeyframes);
                inFile.read(reinterpret_cast<char*>(channel.keyframes.data()), numKeyframes * sizeof(Keyframe));
            }
        }
    }

    inFile.close();
}

void CIMGUI_Animation_Tab::ProcessEventsBetweenFrames2(int characterIndex, int animationIndex, float prevFrame, float currentFrame)
{
    //Frame�� ���缭 �ߺ�ó���Ǵ°���?
   // �������� 0�ΰ�츦 �����ϰ� �������� ����µ� ��Ȯ�ϰ� �̺�Ʈ�� ���� �� �ֳ�?
       // 0.001�ʴ����� ���� �Ұ����Ѱ� �ƴ�

   //�ذ���
   // 1. �� �Լ��� play animation �ֺ��� �־, �ִϸ��̼��� �����Ѱ�� �� �Լ� ȣ�� ��ü�� ����.  << ����
   // 2. bool���� ���� 1ȸ�� ó��

   //�ϴ� ����.




   //frame�� 0�ΰ��� �ִϸ��̼� ���� ����, �ִϸ��̼� ����� 0�� �̺�Ʈ�� ã�Ƽ� �ϴ°ɷ� ó��.   (ProcessEventsFramesZero)
    if (prevFrame == 0)
    {

        //����,���� �������� ��� 0���� �ִϸ��̼� ��ü�� ������ ��� �Լ� ��
        if (currentFrame == 0)
            return;

        //�ƴϸ� 0������ �̺�Ʈ �����ϰ� �����ϱ� ����  ���������� ���� ����
        else
            prevFrame += 0.001;
    }





    // ĳ���� �ε��� Ž��
 
     auto characterIt = m_pFrameEvent->find(m_iTestModelIndex);   //�׽�Ʈ��
    if (characterIt != m_pFrameEvent->end())
    {
        auto& animationMap = characterIt->second;
        auto animationIt = animationMap.find(animationIndex);
        if (animationIt != animationMap.end())
        {
            auto& frameMap = animationIt->second;


            //0�϶��� ���� ��������� �����ؾ��ϳ�?
            for (auto frameIt = frameMap.lower_bound(prevFrame); frameIt != frameMap.end() && frameIt->first <= currentFrame; ++frameIt)
            {
                // �ش� �����ӿ����� �̺�Ʈ ����Ʈ�� ���
                for (string event : frameIt->second)
                {
                    //MSG_BOX(TEXT("������ " + frameIt->first + "���� �̺�Ʈ �߻�: " + event));

                    _bool bdebug1 = false;

                    //m_pSelectedModelCom->m_Animations[m_pSelectedModelCom->m_iCurrentAnimationIndex]->m_fTickPerSecond = 100.f;
                    //���� 25���� ���ƿ��� �ʴ� ������ ����...?


                    CFrameEvent_Manager::Get_Instance()->UseEvent_Test(event, m_pSelectedObject);

                }
            }

        }
    }

}




/*
void CIMGUI_Animation_Tab::ProcessEventsBetweenFrames(int characterIndex, int animationIndex, float prevFrame, float currentFrame)
{



    //Frame�� ���缭 �ߺ�ó���Ǵ°���?
    // �������� 0�ΰ�츦 �����ϰ� �������� ����µ� ��Ȯ�ϰ� �̺�Ʈ�� ���� �� �ֳ�?
        // 0.001�ʴ����� ���� �Ұ����Ѱ� �ƴ�

    //�ذ���
    // 1. �� �Լ��� play animation �ֺ��� �־, �ִϸ��̼��� �����Ѱ�� �� �Լ� ȣ�� ��ü�� ����.  << ����
    // 2. bool���� ���� 1ȸ�� ó��
    
    //�ϴ� ����.






    //frame�� 0�ΰ��� �ִϸ��̼� ���� ����, �ִϸ��̼� ����� 0�� �̺�Ʈ�� ã�Ƽ� �ϴ°ɷ� ó��.   (ProcessEventsFramesZero)
    if (prevFrame == 0)
    {
        
        //����,���� �������� ��� 0���� �ִϸ��̼� ��ü�� ������ ��� �Լ� ��
        if (currentFrame == 0)
            return;

        //�ƴϸ� 0������ �̺�Ʈ �����ϰ� �����ϱ� ����  ���������� ���� ����
        else
            prevFrame += 0.001;
    }


    


    // ĳ���� �ε��� Ž��

    auto characterIt = FrameEvent.find(SELECT_HIT);   //�׽�Ʈ��
    if (characterIt != FrameEvent.end()) 
    {
        auto& animationMap = characterIt->second;
        auto animationIt = animationMap.find(animationIndex);
        if (animationIt != animationMap.end()) 
        {
            auto& frameMap = animationIt->second;


            //0�϶��� ���� ��������� �����ؾ��ϳ�?
            for (auto frameIt = frameMap.lower_bound(prevFrame); frameIt != frameMap.end() && frameIt->first <= currentFrame; ++frameIt) 
            {
                // �ش� �����ӿ����� �̺�Ʈ ����Ʈ�� ���
                for (string event : frameIt->second) 
                {
                    //MSG_BOX(TEXT("������ " + frameIt->first + "���� �̺�Ʈ �߻�: " + event));

                    _bool bdebug1 = false;

                    //m_pSelectedModelCom->m_Animations[m_pSelectedModelCom->m_iCurrentAnimationIndex]->m_fTickPerSecond = 100.f;
                    //���� 25���� ���ƿ��� �ʴ� ������ ����...?



                }
            }

        }
    }
}
*/


void CIMGUI_Animation_Tab::ProcessEventsFramesZero(int characterIndex, int animationIndex)
{
    //�ִϸ��̼� ����� 0��° �������̺�Ʈ�� �����ϱ� ����

    auto characterIt = m_pFrameEvent->find(m_iTestModelIndex);   //�׽�Ʈ��
    if (characterIt != m_pFrameEvent->end())
    {
        auto& animationMap = characterIt->second;
        auto animationIt = animationMap.find(animationIndex);
        if (animationIt != animationMap.end())
        {
            auto& frameMap = animationIt->second;

            auto frameIt = frameMap[0];

            for (auto event : frameIt)
            {

                CFrameEvent_Manager::Get_Instance()->UseEvent_Test(event, m_pSelectedObject);
            }
            
        }
    }

}


void CIMGUI_Animation_Tab::SaveEventTXT(const _char* TextFilePath, string strNeweventText)
{
    ifstream file(TextFilePath);

    // ������ ����� �������� Ȯ��
    if (!file.is_open())
    {

        MSG_BOX(TEXT("CIMGUI_Animation_Tab:: SaveEventTXT ���� txt ���� �ε� ����\nPath:"));
        return;
    }



    // ���� ���� �б�
    string fileContent((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    // ���ο� �̺�Ʈ �ؽ�Ʈ �߰�
    fileContent += "\n" + strNeweventText;

    // ������ ���� ���� �ٽ� ���� (�����)
    std::ofstream outFile(TextFilePath);

    if (!outFile.is_open())
    {
        MSG_BOX(TEXT("CIMGUI_Animation_Tab:: SaveEventTXT ���� txt ���� ���� ����\nPath:"));
        return;
    }

    // ������ ���� ������ ���Ͽ� �ٽ� ����
    outFile << fileContent;
    outFile.close();

    MSG_BOX(TEXT("�̺�Ʈ�� ���������� �߰��Ǿ����ϴ�."));


}

string CIMGUI_Animation_Tab::Get_strAnimationEvent()
{
    string strEventTEXT{};

    //�Ҽ��� 6�ڸ����� ������
   strEventTEXT += Get_CharacterName();
   strEventTEXT += "," + g_iCurrentAnimationIndex;
   strEventTEXT += "," + Get_AnimationName();// to_string(m_pSelectedModelCom->m_iCurrentAnimationIndex);
   strEventTEXT += "," + to_string(m_pSelectedModelCom->m_fCurrentAnimPosition);
   strEventTEXT += ",AnimSpeedChange,";
   strEventTEXT += to_string(m_fAnimationUpdateTime);
   strEventTEXT += ",";
   strEventTEXT += to_string(m_fTool_TickPerSecond);




    return strEventTEXT;
}

string CIMGUI_Animation_Tab::Get_CharacterName()
{

    string strCharacterTEXT;

    switch (m_iTestModelIndex)
    {
    case Client::SELECT_GOKU:
        strCharacterTEXT += "SELECT_GOKU";
        break;

    case Client::SELECT_FRN:
        strCharacterTEXT += "SELECT_FRN";
        break;

    case Client::SELECT_21:
        strCharacterTEXT += "SELECT_21";
        break;

    case Client::SELECT_HIT:
        strCharacterTEXT += "SELECT_HIT";
        break;

    case Client::PLAY_GOKU:
        strCharacterTEXT += "PLAY_GOKU";
        break;

    case Client::PLAY_FRN:
        strCharacterTEXT += "PLAY_FRN";
        break;

    case Client::PLAY_21:
        strCharacterTEXT += "PLAY_21";
        break;

    case Client::PLAY_HIT:
        strCharacterTEXT += "PLAY_HIT";
        break;

    case Client::CHARACTER_INDEX_END:
    default:
        MSG_BOX(TEXT("CIMGUI_Animation_Tab::Get_strAnimationEvent() m_iTestModelIndex ����"));
        break;
    }

    return strCharacterTEXT;
}

string CIMGUI_Animation_Tab::Get_AnimationName()
{
    
    std::vector<const char*> animNames;

    CAnimation*  AnimData = m_pSelectedModelCom->m_Animations[g_iCurrentAnimationIndex];

    string strFullName = AnimData->m_szName;
    string AnimName{};

    size_t pos = strFullName.find_last_of('|');

    // ������ '|'�� �����ϸ� �� ���� �κи� ����
    if (pos != std::string::npos) 
    {
        AnimName = strFullName.substr(pos + 1);
    }
    else 
    {
        AnimName = strFullName;
    }

    return AnimName;

}

void CIMGUI_Animation_Tab::ExportAllAnimationToTXT()
{

    std::ofstream outFile("../Bin/FrameEventData/ExportAnimation.txt");

    if (!outFile) {
        std::cerr << "������ �� �� �����ϴ�!" << std::endl;
        return;
    }


    _int iAnimationIndex = 0;

    //���� ��
    for (auto AnimData : m_pSelectedModelCom->m_Animations)
    {

        string strFullName = AnimData->m_szName;
        string AnimName{};
       
        size_t pos = strFullName.find_last_of('|');

        // ������ '|'�� �����ϸ� �� ���� �κи� ����
        if (pos != std::string::npos)
        {
            AnimName = strFullName.substr(pos + 1);
        }
        else
        {
            AnimName = strFullName;
        }


        string FinalText = "[\"" + AnimName +"\"] = ";
        FinalText += to_string(iAnimationIndex);

        outFile << FinalText << std::endl;  // ���Ͽ� �� �پ� ���

        iAnimationIndex++;
    }

    // ���� �ݱ�
    outFile.close();

    MSG_BOX(TEXT("���⼺��"));

}




CIMGUI_Animation_Tab* CIMGUI_Animation_Tab::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CIMGUI_Animation_Tab* pInstance = new CIMGUI_Animation_Tab(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CIMGUI_Animation_Tab"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CIMGUI_Animation_Tab::Free()
{
	__super::Free();

    if (m_pSelectedModelCom != nullptr)
    {
        Safe_Release(m_pSelectedModelCom);
    }
}
