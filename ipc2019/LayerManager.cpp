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

//생성자: CLayerManager 객체 초기화
CLayerManager::CLayerManager()
	: m_nLayerCount(0), //레이어 수 0으로 초기화
	mp_sListHead(NULL), //리스트의 헤드를 NULL로 초기화
	mp_sListTail(NULL), //리스트의 테일을 NULL로 초기화
	m_nTop(-1) //스택의 top을 -1로 초기화(스택이 비어있음을 의미함)
{

}

//소멸자: CLayerManager 객체가 소멸될 때 호출
CLayerManager::~CLayerManager()
{

}

//AddLayer 함수: 레이어를 레이어 배열에 추가
void CLayerManager::AddLayer(CBaseLayer* pLayer)
{
	mp_aLayers[m_nLayerCount++] = pLayer;
}

//GetLayer 함수: 인덱스로 레이어 반환
CBaseLayer* CLayerManager::GetLayer(int nindex)
{
	return mp_aLayers[nindex]; //nindex에 해당하는 레이어를 반환
}

//GetLayer 함수: 이름으로 레이어를 찾아 반환
CBaseLayer* CLayerManager::GetLayer(char* pName)
{
	for (int i = 0; i < m_nLayerCount; i++)
	{
		//모든 레이어를 순회하며 pName과 일치하는 레이어 찾음
		if (!strcmp(pName, mp_aLayers[i]->GetLayerName())) //이름 비교
			return mp_aLayers[i]; //일치하는 레이어 반환
	}

	return NULL; //일치하는 레이어 없을 시 NULL 반환
}

//ConnectLayer 함수: 레이어 연결을 위한 리스트 생성 및 레이어 연결
void CLayerManager::ConnectLayers(char* pcList)
{
	MakeList(pcList); //pcList에서 리스트 생성
	LinkLayer(mp_sListHead); //리스트를 기반으로 레이어들 연결;
	int arr;
	arr = 3;
}

//MakeList 함수: pcList 문자열 파싱하여 리스트 구성
void CLayerManager::MakeList(char* pcList)
{
	while (pcList != (char*)0x01) //0x01 값이 될 때까지 루프
	{
		char sBuff[100]; //토큰을 저장할 버퍼
		sscanf_s(pcList, "%s", sBuff, sizeof(sBuff)); //pcList에서 문자열 추출하여 sBuff에 저장
		pcList = strchr(pcList, ' ') + 1; //다음 토큰으로 이동

		PNODE pNode = AllocNode(sBuff); //sBuff 내용을 담은 새로운 노드 생성
		AddNode(pNode); //노드를 리스트에 추가
	}
}

//새로운 노드 생성하여 반환
CLayerManager::PNODE CLayerManager::AllocNode(char* pcName)
{
	PNODE node = new NODE; //새로운 노드 할당
	ASSERT(node); //노드가 NULL이 아닌지 확인

	strcpy_s(node->token, pcName); //노드의 token에 pcName을 복사
	node->next = NULL; //노드의 next를 NULL로 초기화

	return node; //새로 생성한 노드를 반환
}

void CLayerManager::AddNode(PNODE pNode)
{
	//리스트의 헤드가 NULL인 경우(리스트가 비어있는 경우)
	if (!mp_sListHead)
	{
		//새로 추가하는 노드를 리스트의 헤드 및 테일로 설정
		mp_sListHead = mp_sListTail = pNode;
	}
	else
	{
		//리스트가 비어있지 않으면
		//현재 테일 노드의 next 포인터를 새 노드로 설정
		mp_sListTail->next = pNode;
		//새 노드를 테일로 업데이트
		mp_sListTail = pNode;
	}
}

// 레이어를 스택에 추가하는 함수
void CLayerManager::Push(CBaseLayer* pLayer)
{
	// 스택이 가득 차지 않았는지 확인
	if (m_nTop >= MAX_LAYER_NUMBER)
	{
#ifdef _DEBUG
		TRACE("The Stack is full.. so cannot run the push operation.. \n");
#endif
		return; // 가득 차면 추가하지 않음
	}

	// 스택의 다음 위치에 레이어를 추가하고 top 인덱스를 증가
	mp_Stack[++m_nTop] = pLayer;
}

// 스택의 가장 위 레이어를 제거하고 반환하는 함수
CBaseLayer* CLayerManager::Pop()
{
	// 스택이 비어있는지 확인
	if (m_nTop < 0)
	{
#ifdef _DEBUG
		TRACE("The Stack is empty.. so cannot run the pop operation.. \n");
#endif
		return NULL; // 비어있으면 NULL 반환
	}

	CBaseLayer* pLayer = mp_Stack[m_nTop]; // 가장 위 레이어 저장
	mp_Stack[m_nTop] = NULL; // 해당 위치를 NULL로 초기화
	m_nTop--; // top 인덱스 감소

	return pLayer; // 제거한 레이어 반환
}

// 스택의 가장 위 레이어를 반환하는 함수
CBaseLayer* CLayerManager::Top()
{
	// 스택이 비어있는지 확인
	if (m_nTop < 0)
	{
#ifdef _DEBUG
		TRACE("The Stack is empty.. so cannot run the top operation.. \n");
#endif
		return NULL; // 비어있으면 NULL 반환
	}

	return mp_Stack[m_nTop]; // 가장 위 레이어 반환
}

// 레이어 연결 함수: 리스트의 각 노드를 기반으로 레이어를 연결
void CLayerManager::LinkLayer(PNODE pNode)
{
	CBaseLayer* pLayer = NULL; // 레이어 포인터 초기화

	// 노드가 존재하는 동안 반복
	while (pNode)
	{
		if (!pLayer) // pLayer가 NULL인 경우
			pLayer = GetLayer(pNode->token); // 첫 번째 노드의 레이어 가져오기
		else
		{
			// 현재 노드의 첫 번째 문자가 '('인 경우
			if (*pNode->token == '(')
				Push(pLayer); // 현재 레이어를 스택에 푸시
			else if (*pNode->token == ')') // 현재 노드의 첫 번째 문자가 ')'인 경우
				Pop(); // 스택에서 레이어 제거
			else // 그 외의 경우
			{
				char cMode = *pNode->token; // 첫 번째 문자(mode)를 저장
				char* pcName = pNode->token + 1; // 레이어 이름 저장

				pLayer = GetLayer(pcName); // 이름에 해당하는 레이어 가져오기

				// 모드에 따라 상위 및 하위 레이어 연결
				switch (cMode)
				{
				case '*': Top()->SetUpperUnderLayer(pLayer); break; // 하위 레이어로 설정
				case '+': Top()->SetUpperLayer(pLayer); break; // 상위 레이어로 설정
				case '-': Top()->SetUnderLayer(pLayer); break; // 또 다른 하위 레이어로 설정
				}
			}
		}

		// 다음 노드로 이동
		pNode = pNode->next;
	}
}

// 레이어 메모리 해제 함수
void CLayerManager::DeAllocLayer()
{
	// 현재 레이어 수 만큼 반복하여 각 레이어 메모리 해제
	for (int i = 0; i < this->m_nLayerCount; i++)
		delete this->mp_aLayers[i]; // 동적 할당된 레이어 해제
}