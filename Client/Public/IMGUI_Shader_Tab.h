#pragma once
#include "IMGUI_Tab.h"
#include "Shader_Texture.h"
#include "Shader_Tab_Defines.h"
#include "Effect.h"
#include "Effect_Layer.h"

BEGIN(Engine)
class CGameObject;
END
//#include "GameObject.h"
BEGIN(Client)

class CIMGUI_Shader_Tab : public CIMGUI_Tab
{
public:
	enum ENDNODE_TYPE { END_DIFFUSE, END_ALPHA };
	enum NODE_TYPE { NODE_BASIC, NODE_FUNCTION, NODE_LINK, NODE_END};
	enum FUNCTION_TYPE { FUNCTION_TEXMOVE, FUNCTION_SPRITE, FUNCTION_END };
public:
	typedef struct
	{
		_int MoveTex_node_id = { 1500 };
		_float2 fDirection = { 0.f,0.f };
		_float fSpeed = { 0.f };
	}MoveTex_Node;

	typedef struct
	{
		_bool isLoop = { false };
		_int Sprite_node_id = { 3000 };
		_float2 fSpriteSizeNumber = { 0.f,0.f };
		_float fSpeed = { 0.f };
	}Sprite_Node;

	typedef struct
	{
		_int iID;
		ImTextureID Texture;
		ImTextureID Alpha;
	}SRV_Texture;


protected:
	CIMGUI_Shader_Tab(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CTexture* pTexture, CEffect* pEffect);
	CIMGUI_Shader_Tab(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CTexture* pTexture);


	virtual ~CIMGUI_Shader_Tab() = default;
	
public:
	virtual void Effect_Menu() override;
	virtual void Effect_Transform() override;
	virtual void Camera_Set(_uint iNumCameraModel) override;
	HRESULT Initialize() override;
	HRESULT Load_Initialize(string strFilename);

	
	void Update(_float fTimeDelta);
	void Render(_float fTimeDelta) override;
	void TabPos_Init(); 
	void Create_NodeTexture(string szPath);
	void Push_ShaderTexture(CShader_Texture* pShaderTexture) {
		m_NodeTextures.push_back(pShaderTexture);
	}
	virtual const _char* GetTabName() const { return "Shader"; };
	void Render_MainNode();
	void Render_TextureNode();
	void Render_MoveTexNode();
	void Render_SpriteNode();
	void Render_Link();
	void Check_Create_Link();
	void Check_Delete_Link();
	void Draw_MusicButton(CShader_Texture* pShaderTexture);

	void Click_Save_Shader_Tab(string fileName);
	void Click_Load_Shader_Tab(string fileName);
	void Create_Link(_int start_attr, _int end_attr);
	void Load_NodeTextures(vector<Save_Key>& PrototypeKeys);

	void Save_Shader_Tab(string fileName, const Shader_Tab_Save& shaderTabSave, _int version = 2);
	void Load_Shader_Tab(string fileName, Shader_Tab_Save& shaderTabSave);

	void Update_TestToLayer_TextureCom(CTexture* pTexture, CEffect* pEffect) { m_TestEffectModel_Texture = pTexture; 
	m_pEffect = pEffect;
	}

	void Add_Clone_EffectToShader_Texture(CEffect* pEffect);
	_int Update_Clone_EffectToShader_Texture(CEffect* pEffect, _float fTimeDelta);
	void Delete_Clone_EffectToShader_Texture(CEffect* pEffect);
private:
	unordered_map<CEffect*, vector<CShader_Texture*>*> m_CopyClones;

public:
	_int m_iNumberId = { -1 };
	_bool m_TabPick = { false };

	string m_FileName;
private: /* �� m_PrototypeKeys �� Load_NodeTextures() �Լ�ȣ��� ��ü���� �� �����Ѵ� */
	//vector<*>			m_ShaderTexture_TexCoords;
	vector<Save_Key>			m_PrototypeKeys;							
	/*----------------------------m_PrototypeKeys-----------------------------------------------------------------------------------------*/
	CTexture*					m_pDefaultTexture = { nullptr };			// ���� ���� �Ҷ� �ʿ��� ����ؽ�����
	vector<CShader_Texture*>	m_NodeTextures;								// �ؽ��� ��忡 �ؽ��İ�ü��
	vector<SRV_Texture>			m_NodeTextureSRVs;							// �� �ؽ��İ�ü�� �׸��� ����Ÿ���� �׷��� ��� ShaderReasouceView*
public:
	CTexture*					m_TestEffectModel_Texture = { nullptr };	// ������ ������ �׽�Ʈ����Ʈ��ü�� �ؽ��� ������Ʈ
	/*------------------------------------------------------------------------------------------------------------------------------------*/
private:
	_int						m_iNodeTextureCount = 0;		// ��� �ؽ����� ����
	_bool						isStart = false;				// �ܺο��� �����ִ� �Ҷ� ���� ����

private: /* ImNodes �� ���õ� ��� ���� */
	vector<pair<_int, _int>>	links;							// ���� ��� ���̸� �̾��ֱ� ���� ���� ( �ܼ��� int�� ID�� ������ ���� )

	const _int					m_iMain_node_id = -1;			// ���� ��� id
	_int						m_iMain_Input_Diffuse_id = 0;	// ��ǻ� ����� id

	_int						m_iAttributeCount = 4;			// ��� Attribute ���� �̰� �þ�� ��� �ִ� ������ �پ��

	_int						unique_node_id = 1;				// ��� �ִ� ���� 500��
	vector<_int>				node_ids;						// �̰� �ؽ��� ���� ����

	_int						m_MoveTex_node_id = 1501;		// �ӽ÷� ������ �����ؽ� ��� ���̵� ������ 
	list<MoveTex_Node>			m_MoveTex_Node_ids;				// �����ؽ� ����

	_int						m_Sprite_node_id = 3001;		// �ӽ÷� ������ ��������Ʈ ��� ���̵� ������ 
	list<Sprite_Node>			m_Sprite_Node_ids;				// ��������Ʈ ����
	
	unordered_map<int, ImVec2>	node_positions;					// �� ��ȯ�� ��� ��ġ�� �ʱ�ȭ�Ǵµ� �װ� �������� �׻� ������ ��ġ�� ��� ����

public:
	void Save_ClientBinary();

public:
	void RemoveSubstring(std::wstring& wstr, const std::wstring& toRemove);
	string WStringToString(const std::wstring& wstr);
	void ShowTextWithoutPrefix(const std::wstring& wstr);
	size_t GetRemainingFileSize(ifstream& file);
	size_t GetFileSize(ifstream& file);
public:
	CEffect* m_pEffect = { nullptr };
	static CIMGUI_Shader_Tab* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CTexture* pTexture, CEffect* pEffect);
	static CIMGUI_Shader_Tab* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CTexture* pTexture);
	static CIMGUI_Shader_Tab* Create_Load(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CTexture* pTexture, string strFilename, CEffect* pEffect);

	virtual void Free() override;
};

END
