#pragma once


#define WATCH_FILE(FILE, FUNCTION) (Prism::Engine::GetInstance()->GetFileWatcher()->WatchFile(FILE, std::bind(&FUNCTION, this, FILE)))
#define UNWATCH_FILE(FILE) (Prism::Engine::GetInstance()->GetFileWatcher()->UnWatchFile(FILE))

#include <functional>
#include <string>

namespace Prism
{
	class FileWatcher
	{
	public:
		FileWatcher();

		void WatchFile(const std::string& aFile, std::function<void()> aCallBack);
		void UnWatchFile(const std::string& aFile);

		void CheckFiles();
		void Clear();

	private:
		struct FileData
		{
			std::string myFilePath;
			std::function<void()> myCallBack;
			FILETIME myFileTime;
		};

		CU::GrowingArray<FileData> myFileDatas;
	};
}