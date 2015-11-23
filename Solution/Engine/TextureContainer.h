#pragma once
#include <unordered_map>
namespace Prism
{
	class Texture;

	class TextureContainer
	{
	public:
		~TextureContainer();

		Texture* GetTexture(const std::string& aFileName);

	private:
		void LoadTexture(const std::string& aFileName);
		void ReloadTexture(const std::string& aFileName);

		std::unordered_map<std::string, Texture*> myTextures;
	};
}