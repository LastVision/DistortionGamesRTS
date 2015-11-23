#pragma once
#include <Matrix.h>
#include <GrowingArray.h>
#include <Vector.h>
#include <string>
#include <thread>
#include <Windows.h>

struct ID3D11Device;
struct ID3D11DeviceChild;
struct ID3D11DeviceContext;
struct ID3D11DepthStencilView;
struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;
struct ID3D11Texture2D;

namespace Prism
{
	struct TextCommand
	{
		std::string myText;
		CU::Vector2<float> myPosition;
		CU::Vector4<float> myColor;
		float myScale;
	};

	enum class eTextType
	{
		DEBUG_TEXT,
		RELEASE_TEXT,
	};


	class Camera;
	class DirectX;
	class EffectContainer;
	class Effect;
	class EmitterDataContainer;
	class FBXFactory;
	class FileWatcher;
	class Font;
	class Model;
	class ModelLoader;
	class ModelProxy;
	class Sprite;
	class Text;
	class TextureContainer;

	struct SetupInfo;

	class Engine
	{
	public:
		static bool Create(HWND& aHwnd, WNDPROC aWndProc, SetupInfo& aSetupInfo);
		static void Destroy();
		static Engine* GetInstance();
		void Render();
		void OnResize(int aWidth, int aHeigth);
		bool IsFullscreen() const;
		void SetFullscreen(bool aFullscreen);

		ID3D11Device* GetDevice();
		ID3D11DeviceContext* GetContex();
		ID3D11DepthStencilView* GetDepthView();
		ID3D11RenderTargetView* GetDepthBuffer();
		ID3D11ShaderResourceView* GetBackbufferView();
		ID3D11Texture2D* GetDepthBufferTexture();
		void SetDebugName(ID3D11DeviceChild* aChild, const std::string& aName);

		TextureContainer* GetTextureContainer();
		EffectContainer* GetEffectContainer();
		EmitterDataContainer* GetEmitterDataContainer();
		FileWatcher* GetFileWatcher();
		ModelLoader* GetModelLoader();


		Model* DLLLoadModel(const std::string& aModelPath, Effect* aEffect);

		const CU::Vector2<int>& GetWindowSize() const;
		const CU::Matrix44<float>& GetOrthogonalMatrix() const;

		void PrintText(const std::string& aText, const CU::Vector2<float>& aPosition, eTextType aTextType, float aScale = 1.f, CU::Vector4<float> aColor = { 1.f, 1.f, 1.f, 1.f });
		void PrintText(float aNumber, const CU::Vector2<float>& aPosition, eTextType aTextType, float aScale = 1.f, CU::Vector4<float> aColor = { 1.f, 1.f, 1.f, 1.f });
		void PrintText(int aNumber, const CU::Vector2<float>& aPosition, eTextType aTextType, float aScale = 1.f, CU::Vector4<float> aColor = { 1.f, 1.f, 1.f, 1.f });

		void RestoreViewPort();
		void SetBackBufferAsTarget();

		void EnableZBuffer();
		void DisableZBuffer();

		void ToggleWireframe();

		void EnableWireframe();
		void DisableWireframe();

		void EnableCulling();
		void DisableCulling();

		void SetShowDebugText(bool aShowDebug);
		void SetClearColor(const CU::Vector4<float>& aClearColor);

		bool UsePBLPixelShader();
		void TogglePBLPixelShader();

		void StartFade(float aDuration);

		bool myWireframeShouldShow;
		volatile bool myIsLoading;

	private:
		struct FadeData
		{
			Sprite* mySprite;
			float myCurrentTime;
			float myTotalTime;
			bool myIsFading;
		};

		Engine();
		~Engine();
		bool Init(HWND& aHwnd, WNDPROC aWndProc);
		bool WindowSetup(HWND& aHwnd, WNDPROC aWindowProc);
		
		bool myWireframeIsOn;
		bool myUsePBLPixelShader;
		

		DirectX* myDirectX;
		SetupInfo* mySetupInfo;
		TextureContainer* myTextureContainer;
		EffectContainer* myEffectContainer;
		EmitterDataContainer* myEmitterDataContainer;
		FBXFactory* myModelFactory;
		FileWatcher* myFileWatcher;
		Font* myFont;
		Text* myText;
		Text* myDebugText;
		FadeData myFadeData;

		CU::Vector4<float> myClearColor;
		CU::Vector2<int> myWindowSize;
		CU::Matrix44<float> myOrthogonalMatrix;


		ModelLoader* myModelLoader;
		std::thread* myModelLoaderThread;
		std::thread::id myModelLoaderThreadID;
		std::thread::id myMainThreadID;

		static Engine* myInstance;

		CU::GrowingArray<TextCommand> myTexts;
		CU::GrowingArray<TextCommand> myDebugTexts;
		bool myShowDebugText;
	};
}

inline Prism::TextureContainer* Prism::Engine::GetTextureContainer()
{
	return myTextureContainer;
}

inline Prism::EffectContainer* Prism::Engine::GetEffectContainer()
{
	return myEffectContainer;
}

inline Prism::EmitterDataContainer* Prism::Engine::GetEmitterDataContainer()
{
	DL_ASSERT_EXP(myEmitterDataContainer != nullptr, "EmitterDataContainer were nullptr! Couldn't get it.");
	return myEmitterDataContainer;
}

inline Prism::FileWatcher* Prism::Engine::GetFileWatcher()
{
	return myFileWatcher;
}

inline Prism::ModelLoader* Prism::Engine::GetModelLoader()
{
	return myModelLoader;
}

inline const CU::Vector2<int>& Prism::Engine::GetWindowSize() const
{
	return myWindowSize;
}

inline const CU::Matrix44<float>& Prism::Engine::GetOrthogonalMatrix() const
{
	return myOrthogonalMatrix;
}

inline void Prism::Engine::SetClearColor(const CU::Vector4<float>& aClearColor)
{
	myClearColor = aClearColor;
}

inline void Prism::Engine::SetShowDebugText(bool aShowDebugText)
{
	myShowDebugText = aShowDebugText;
}

inline bool Prism::Engine::UsePBLPixelShader()
{
	return myUsePBLPixelShader;
}

inline void Prism::Engine::TogglePBLPixelShader()
{
	myUsePBLPixelShader = !myUsePBLPixelShader;
}