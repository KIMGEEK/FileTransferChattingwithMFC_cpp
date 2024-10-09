#pragma once
// LayerManager.cpp: implementation of the CLayerManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pch.h"
#include "LayerManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//������: CLayerManager ��ü �ʱ�ȭ
CLayerManager::CLayerManager()
	: m_nLayerCount(0), //���̾� �� 0���� �ʱ�ȭ
	mp_sListHead(NULL), //����Ʈ�� ��带 NULL�� �ʱ�ȭ
	mp_sListTail(NULL), //����Ʈ�� ������ NULL�� �ʱ�ȭ
	m_nTop(-1) //������ top�� -1�� �ʱ�ȭ(������ ��������� �ǹ���)
{

}

//�Ҹ���: CLayerManager ��ü�� �Ҹ�� �� ȣ��
CLayerManager::~CLayerManager()
{

}

//AddLayer �Լ�: ���̾ ���̾� �迭�� �߰�
void CLayerManager::AddLayer(CBaseLayer* pLayer)
{
	mp_aLayers[m_nLayerCount++] = pLayer;
}

//GetLayer �Լ�: �ε����� ���̾� ��ȯ
CBaseLayer* CLayerManager::GetLayer(int nindex)
{
	return mp_aLayers[nindex]; //nindex�� �ش��ϴ� ���̾ ��ȯ
}

//GetLayer �Լ�: �̸����� ���̾ ã�� ��ȯ
CBaseLayer* CLayerManager::GetLayer(char* pName)
{
	for (int i = 0; i < m_nLayerCount; i++)
	{
		//��� ���̾ ��ȸ�ϸ� pName�� ��ġ�ϴ� ���̾� ã��
		if (!strcmp(pName, mp_aLayers[i]->GetLayerName())) //�̸� ��
			return mp_aLayers[i]; //��ġ�ϴ� ���̾� ��ȯ
	}

	return NULL; //��ġ�ϴ� ���̾� ���� �� NULL ��ȯ
}

//ConnectLayer �Լ�: ���̾� ������ ���� ����Ʈ ���� �� ���̾� ����
void CLayerManager::ConnectLayers(char* pcList)
{
	MakeList(pcList); //pcList���� ����Ʈ ����
	LinkLayer(mp_sListHead); //����Ʈ�� ������� ���̾�� ����;
	int arr;
	arr = 3;
}

//MakeList �Լ�: pcList ���ڿ� �Ľ��Ͽ� ����Ʈ ����
void CLayerManager::MakeList(char* pcList)
{
	while (pcList != (char*)0x01) //0x01 ���� �� ������ ����
	{
		char sBuff[100]; //��ū�� ������ ����
		sscanf_s(pcList, "%s", sBuff, sizeof(sBuff)); //pcList���� ���ڿ� �����Ͽ� sBuff�� ����
		pcList = strchr(pcList, ' ') + 1; //���� ��ū���� �̵�

		PNODE pNode = AllocNode(sBuff); //sBuff ������ ���� ���ο� ��� ����
		AddNode(pNode); //��带 ����Ʈ�� �߰�
	}
}

//���ο� ��� �����Ͽ� ��ȯ
CLayerManager::PNODE CLayerManager::AllocNode(char* pcName)
{
	PNODE node = new NODE; //���ο� ��� �Ҵ�
	ASSERT(node); //��尡 NULL�� �ƴ��� Ȯ��

	strcpy_s(node->token, pcName); //����� token�� pcName�� ����
	node->next = NULL; //����� next�� NULL�� �ʱ�ȭ

	return node; //���� ������ ��带 ��ȯ
}

void CLayerManager::AddNode(PNODE pNode)
{
	//����Ʈ�� ��尡 NULL�� ���(����Ʈ�� ����ִ� ���)
	if (!mp_sListHead)
	{
		//���� �߰��ϴ� ��带 ����Ʈ�� ��� �� ���Ϸ� ����
		mp_sListHead = mp_sListTail = pNode;
	}
	else
	{
		//����Ʈ�� ������� ������
		//���� ���� ����� next �����͸� �� ���� ����
		mp_sListTail->next = pNode;
		//�� ��带 ���Ϸ� ������Ʈ
		mp_sListTail = pNode;
	}
}

// ���̾ ���ÿ� �߰��ϴ� �Լ�
void CLayerManager::Push(CBaseLayer* pLayer)
{
	// ������ ���� ���� �ʾҴ��� Ȯ��
	if (m_nTop >= MAX_LAYER_NUMBER)
	{
#ifdef _DEBUG
		TRACE("The Stack is full.. so cannot run the push operation.. \n");
#endif
		return; // ���� ���� �߰����� ����
	}

	// ������ ���� ��ġ�� ���̾ �߰��ϰ� top �ε����� ����
	mp_Stack[++m_nTop] = pLayer;
}

// ������ ���� �� ���̾ �����ϰ� ��ȯ�ϴ� �Լ�
CBaseLayer* CLayerManager::Pop()
{
	// ������ ����ִ��� Ȯ��
	if (m_nTop < 0)
	{
#ifdef _DEBUG
		TRACE("The Stack is empty.. so cannot run the pop operation.. \n");
#endif
		return NULL; // ��������� NULL ��ȯ
	}

	CBaseLayer* pLayer = mp_Stack[m_nTop]; // ���� �� ���̾� ����
	mp_Stack[m_nTop] = NULL; // �ش� ��ġ�� NULL�� �ʱ�ȭ
	m_nTop--; // top �ε��� ����

	return pLayer; // ������ ���̾� ��ȯ
}

// ������ ���� �� ���̾ ��ȯ�ϴ� �Լ�
CBaseLayer* CLayerManager::Top()
{
	// ������ ����ִ��� Ȯ��
	if (m_nTop < 0)
	{
#ifdef _DEBUG
		TRACE("The Stack is empty.. so cannot run the top operation.. \n");
#endif
		return NULL; // ��������� NULL ��ȯ
	}

	return mp_Stack[m_nTop]; // ���� �� ���̾� ��ȯ
}

// ���̾� ���� �Լ�: ����Ʈ�� �� ��带 ������� ���̾ ����
void CLayerManager::LinkLayer(PNODE pNode)
{
	CBaseLayer* pLayer = NULL; // ���̾� ������ �ʱ�ȭ

	// ��尡 �����ϴ� ���� �ݺ�
	while (pNode)
	{
		if (!pLayer) // pLayer�� NULL�� ���
			pLayer = GetLayer(pNode->token); // ù ��° ����� ���̾� ��������
		else
		{
			// ���� ����� ù ��° ���ڰ� '('�� ���
			if (*pNode->token == '(')
				Push(pLayer); // ���� ���̾ ���ÿ� Ǫ��
			else if (*pNode->token == ')') // ���� ����� ù ��° ���ڰ� ')'�� ���
				Pop(); // ���ÿ��� ���̾� ����
			else // �� ���� ���
			{
				char cMode = *pNode->token; // ù ��° ����(mode)�� ����
				char* pcName = pNode->token + 1; // ���̾� �̸� ����

				pLayer = GetLayer(pcName); // �̸��� �ش��ϴ� ���̾� ��������

				// ��忡 ���� ���� �� ���� ���̾� ����
				switch (cMode)
				{
				case '*': Top()->SetUpperUnderLayer(pLayer); break; // ���� ���̾�� ����
				case '+': Top()->SetUpperLayer(pLayer); break; // ���� ���̾�� ����
				case '-': Top()->SetUnderLayer(pLayer); break; // �� �ٸ� ���� ���̾�� ����
				}
			}
		}

		// ���� ���� �̵�
		pNode = pNode->next;
	}
}

// ���̾� �޸� ���� �Լ�
void CLayerManager::DeAllocLayer()
{
	// ���� ���̾� �� ��ŭ �ݺ��Ͽ� �� ���̾� �޸� ����
	for (int i = 0; i < this->m_nLayerCount; i++)
		delete this->mp_aLayers[i]; // ���� �Ҵ�� ���̾� ����
}