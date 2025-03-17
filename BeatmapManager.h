#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <unordered_map>
//tag����Я�� [ �� ] ����
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
	std::wstring beatmapContents;//��������
	std::vector<std::wstring> beatmapVector;//�����������������飩
	std::wstring variableSpeedTag = L"TimingPoints";
	std::vector<std::wstring> variableSpeedVector;//������ٲ����������������飩����һ��Ԫ��Ϊ��ǩ
	struct Range {
		unsigned long long start;
		unsigned long long end;
	};
	std::unordered_map<std::wstring/*tag*/, Range/*line index range*/> tagIndexRangeMap;

	void updateBeatmapContents();
};

