#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <unordered_map>
//tag必须携带 [ 和 ] 符号
class BeatmapManager
{
public:
	~BeatmapManager() {}
	BeatmapManager();
	BeatmapManager(std::string fileWithPath);
	BeatmapManager(std::wstring fileWithPath);
	void setFileWithPath(std::wstring fileWithPath) {
		this->fileWithPath = fileWithPath;
	}
	std::wstring getFileWithPath() const {
		return this->fileWithPath;
	}
	std::wstring getFileName() const {
		return this->fileName;
	}
	void setFileName(std::wstring fileName) {
		this->fileName = fileName;
	}
	std::wstring getBeatmapContents() const {
		return beatmapContents;
	}
	std::vector<std::wstring> getBeatmapVector() const {
		return beatmapVector;
	}
	bool openBeatmap();
	bool openBeatmap(std::wstring fileWithPath);
	std::vector<std::wstring> getBeatmapVectorByTag(std::wstring tag);
	std::vector<std::wstring> getVariableSpeedVector() const {
		return variableSpeedVector;
	}
	void changeVariableSpeedVector(std::vector<std::wstring> contents);
	bool save();
	bool saveAs(std::wstring fileWithPath);
	bool changeContentsByTag(std::wstring tag, std::vector<std::wstring> contents, bool bAddTagIfUncontained = false);
	inline bool isTagInBeatmap(std::wstring tag);
	bool addTagAndContents(std::wstring tag, std::vector<std::wstring> contents, bool replaceExistedTag = false);
	
private:
	std::wstring fileWithPath;//UTF8 Text
	std::wstring fileName;//UTF8 Text
	std::wstring beatmapContents;//谱面内容
	std::vector<std::wstring> beatmapVector;//谱面内容向量（数组）
	std::wstring variableSpeedTag = L"TimingPoints";
	std::vector<std::wstring> variableSpeedVector;//谱面变速部分内容向量（数组），第一个元素为标签
	struct Range {
		unsigned long long start;
		unsigned long long end;
	};
	std::unordered_map<std::wstring/*tag*/, Range/*line index range*/> tagIndexRangeMap;

	void updateBeatmapContents();
};

