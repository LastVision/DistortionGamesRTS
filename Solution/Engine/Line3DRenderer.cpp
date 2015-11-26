#include "stdafx.h"
//
//#include "EffectContainer.h"
//#include "IndexBufferWrapper.h"
//#include "Line3D.h"
//#include "Line3DRenderer.h"
//namespace Prism
//{
//	Line3DRenderer::Line3DRenderer()
//		: myEffect(nullptr)
//		, myVertexBuffer(nullptr)
//		, myIndexBuffer(nullptr)
//		, myVertexBufferDesc(new D3D11_BUFFER_DESC())
//		, myIndexBufferDesc(new D3D11_BUFFER_DESC())
//	{
//		InitVertexBuffer();
//		//InitIndexBuffer();
//		myPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
//
//		myEffect = EffectContainer::GetInstance()->
//	}
//
//
//	Line3DRenderer::~Line3DRenderer()
//	{
//		SAFE_DELETE(myEffect);
//		SAFE_DELETE(myVertexBuffer);
//		SAFE_DELETE(myIndexBuffer);
//	}
//
//	void Line3DRenderer::InitVertexBuffer()
//	{
//		myVertexBuffer = new VertexBufferWrapper();
//		myVertexBuffer->myStride = sizeof(Line3D);;
//		myVertexBuffer->myByteOffset = 0;
//		myVertexBuffer->myStartSlot = 0;
//		myVertexBuffer->myNumberOfBuffers = 1;
//
//
//		ZeroMemory(myVertexBufferDesc, sizeof(myVertexBufferDesc));
//		myVertexBufferDesc->Usage = D3D11_USAGE_IMMUTABLE;
//		myVertexBufferDesc->BindFlags = D3D11_BIND_VERTEX_BUFFER;
//		myVertexBufferDesc->CPUAccessFlags = 0;
//		myVertexBufferDesc->MiscFlags = 0;
//		myVertexBufferDesc->StructureByteStride = 0;
//	}
//
//	//void Line3DRenderer::InitIndexBuffer()
//	//{
//	//	myIndexBuffer = new IndexBufferWrapper();
//	//	myIndexBuffer->myIndexBufferFormat = DXGI_FORMAT_R32_UINT;
//	//	myIndexBuffer->myByteOffset = 0;
//
//
//	//	ZeroMemory(myIndexBufferDesc, sizeof(myIndexBufferDesc));
//	//	myIndexBufferDesc->Usage = D3D11_USAGE_IMMUTABLE;
//	//	myIndexBufferDesc->BindFlags = D3D11_BIND_INDEX_BUFFER;
//	//	myIndexBufferDesc->CPUAccessFlags = 0;
//	//	myIndexBufferDesc->MiscFlags = 0;
//	//	myIndexBufferDesc->StructureByteStride = 0;
//	//}
//}