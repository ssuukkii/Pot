#include "..\Public\Texture.h"
#include "Shader.h"

CTexture::CTexture(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent { pDevice, pContext }
{
}

CTexture::CTexture(const CTexture & Prototype)
	: CComponent { Prototype }
	, m_iNumTextures { Prototype.m_iNumTextures }
	, m_SRVs { Prototype.m_SRVs }
{
	for (auto& iter : m_SRVs)
	{
		Safe_AddRef(iter);
	}
	//ID3D11ShaderResourceView* a;
	
	//m_SRVs.reserve(Prototype.m_iNumTextures);

	//ID3D11ShaderResourceView* pNewSRV = nullptr;
	//_tchar			szEXT[MAX_PATH] = {};
	//_wsplitpath_s(Prototype.m_szFullPath, nullptr, 0, nullptr, 0, nullptr, 0, szEXT, MAX_PATH);

	//HRESULT			hr = { 0 };

	//if (false == lstrcmp(szEXT, TEXT(".dds")))
	//	hr = CreateDDSTextureFromFile(m_pDevice, Prototype.m_szFullPath, nullptr, &pNewSRV);

	//else if (false == lstrcmp(szEXT, TEXT(".tga")))
	//{

	//}

	//else
	//{
	//	hr = CreateWICTextureFromFile(m_pDevice, Prototype.m_szFullPath, nullptr, &pNewSRV);
	//}

	//if (FAILED(hr))
	//{
	//	// HRESULT ���� ����Ͽ� ���� �ľ�
	//	wprintf(L"CreateWICTextureFromFile failed with HRESULT 0x%08X\n", hr);

	//}
	//wcscpy_s(m_szFullPath, MAX_PATH, Prototype.m_szFullPath);

	//m_SRVs.emplace_back(pNewSRV);


	//�ܼ��� SRV�� ���������Ѵٰ� �����ʾ��� �ٸ��ּҿ��� ����Ű�� �ִ� ���� �ؽ��� SRV�� ������
	//for (auto& pSRV : Prototype.m_SRVs)
	/*{
		ID3D11Resource* pOriginalResource = nullptr;
		pSRV->GetResource(&pOriginalResource);

		D3D11_TEXTURE2D_DESC textureDesc;
		ID3D11Texture2D* pOriginalTexture = nullptr;
		if (FAILED(pOriginalResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&pOriginalTexture)))
		{
			Safe_Release(pOriginalResource);
			continue;
		}

		pOriginalTexture->GetDesc(&textureDesc);

		ID3D11Texture2D* pNewTexture = nullptr;

		if (FAILED(m_pDevice->CreateTexture2D(&textureDesc, nullptr, &pNewTexture)))
		{
			Safe_Release(pOriginalTexture);
			Safe_Release(pOriginalResource);
			continue;
		}

		m_pContext->CopyResource(pNewTexture, pOriginalTexture);

		ID3D11ShaderResourceView* pNewSRV = nullptr;

		if (FAILED(m_pDevice->CreateShaderResourceView(pNewTexture, nullptr, &pNewSRV)))
		{
			Safe_Release(pNewTexture);
			Safe_Release(pOriginalTexture);
			Safe_Release(pOriginalResource);
			continue;
		}

		m_SRVs.emplace_back(pNewSRV);

		Safe_Release(pNewTexture);
		Safe_Release(pOriginalTexture);
		Safe_Release(pOriginalResource);
	}*/

}

HRESULT CTexture::Initialize_Prototype(const _tchar * pTextureFilePath, _uint iNumTextures)
{
	m_iNumTextures = iNumTextures;

	_tchar			szFullPath[MAX_PATH] = {};
	_tchar			szEXT[MAX_PATH] = {};
	
	for (size_t i = 0; i < iNumTextures; i++)
	{
		ID3D11ShaderResourceView*			pSRV = { nullptr };

		wsprintf(szFullPath, pTextureFilePath, i);

		/* D:\������\144\Framework\Client\Bin\Resources\Textures\Default.dds */
		/* ..\Bin\Resources\Textures\Default.dds */
		_wsplitpath_s(szFullPath, nullptr, 0, nullptr, 0, nullptr, 0, szEXT, MAX_PATH);

		HRESULT			hr = { 0 };

		if(false == lstrcmp(szEXT, TEXT(".dds")))
			hr = CreateDDSTextureFromFile(m_pDevice, szFullPath, nullptr, &pSRV);

		else if (false == lstrcmp(szEXT, TEXT(".tga")))
		{

		}

		else
		{
			hr = CreateWICTextureFromFile(m_pDevice, szFullPath, nullptr, &pSRV);
		}

		if (FAILED(hr))
		{
			// HRESULT ���� ����Ͽ� ���� �ľ�
			wprintf(L"CreateWICTextureFromFile failed with HRESULT 0x%08X\n", hr);

			return E_FAIL;
		}
		wcscpy_s(m_szFullPath, MAX_PATH, szFullPath);
		
		m_SRVs.emplace_back(pSRV);		
	}
	
	return S_OK;
}

HRESULT CTexture::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CTexture::Bind_ShaderResource(CShader * pShader, const _char * pConstantName, _uint iTextureIndex)
{
	if (iTextureIndex >= m_SRVs.size())
		return pShader->Bind_ShaderResourceView(pConstantName, m_SRVs.front());

	return pShader->Bind_ShaderResourceView(pConstantName, m_SRVs[iTextureIndex]);	
}

HRESULT CTexture::Bind_ShaderResourceArray(CShader* pShader, const _char* pConstantName, _uint iNumTextures)
{
	if (iNumTextures >= m_SRVs.size())
		return E_FAIL;

	return pShader->Bind_ShaderResourceViews(pConstantName, &m_SRVs.front(), iNumTextures);
}

_float2 CTexture::Get_TextureSize()
{
	_float2 fTextureSize = { 0.f,0.f };
	ID3D11Resource* pResource = nullptr;

	m_SRVs[0]->GetResource(&pResource);

	if (pResource)
	{
		// Resource�� ID3D11Texture2D�� ĳ�����մϴ�.
		ID3D11Texture2D* pTexture2D = nullptr;
		if (SUCCEEDED(pResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&pTexture2D)))
		{
			// �ؽ�ó�� ��ũ���͸� �����ɴϴ�.
			D3D11_TEXTURE2D_DESC desc;
			pTexture2D->GetDesc(&desc);


			// �ؽ�ó�� ũ�⸦ ����մϴ�.
			fTextureSize.x = desc.Width;
			fTextureSize.y = desc.Height;

			// ��� �� pTexture2D ����
			pTexture2D->Release();
		}

		// ��� �� pResource ����
		pResource->Release();
	}

	return fTextureSize;
}

HRESULT CTexture::Set_SRV(ID3D11ShaderResourceView* pSRV, _int iArray)
{
	if (pSRV == nullptr)
	{
		MSG_BOX(TEXT("Your SRV is nullptr"));
		return S_OK;
	}

	if (iArray < m_SRVs.size())
	{
		Safe_Release(m_SRVs[iArray]);
		m_SRVs[iArray] = pSRV;
		Safe_AddRef(m_SRVs[iArray]);
	}
	else
	{
		m_SRVs.push_back(pSRV);
		Safe_AddRef(m_SRVs[iArray]);
	}

	return S_OK;
}

CTexture * CTexture::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pTextureFilePath, _uint iNumTextures)
{
	CTexture*		pInstance = new CTexture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pTextureFilePath, iNumTextures)))
	{
		wstring Message{};
		Message = TEXT("Failed to Created : CTexture \nFile Path  : ") + wstring(pTextureFilePath);
		MessageBox(nullptr, Message.c_str(), L"System Message", MB_OK);
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CTexture::Clone(void * pArg)
{
	CTexture*		pInstance = new CTexture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CTexture"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTexture::Free()
{
	__super::Free();

	for (auto& pSRV : m_SRVs)
		Safe_Release(pSRV);

	m_SRVs.clear();
}
