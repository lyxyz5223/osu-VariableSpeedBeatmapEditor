#include "pch.h"
#include "BeatmapManager.h"
#include "stringProcess.h"

BeatmapManager::BeatmapManager()
{

}

BeatmapManager::BeatmapManager(std::string fileWithPath)
{
	this->fileWithPath = str2wstr_2UTF8(fileWithPath);
}

BeatmapManager::BeatmapManager(std::wstring fileWithPath)
{
	this->fileWithPath = fileWithPath;
}

bool BeatmapManager::openBeatmap()
{
	using namespace std;
	if (this->fileWithPath == L"")
		return false;
	fstream file(this->fileWithPath, ios::in);
	if (!file.is_open())
		return false;
	wstring currentTag;
	bool FoundVariableSpeedTag = false;
	string tmpLineStr;//一行的文本
	while (getline(file, tmpLineStr))//逐行读取
	{
		wstring tmpLineWStr = str2wstr_2UTF8(tmpLineStr);//编码转换
		beatmapContents += tmpLineWStr + L'\n';//追加到完整内容
		beatmapVector.push_back(tmpLineWStr);//逐行保存

		//标签range保存
		{
			tagIndexRangeMap[currentTag].end = beatmapVector.size() - 1;
			wstring newLineWStr;
			for (wchar_t c : tmpLineWStr)//去除不可见字符
			{
				if ((c >= 0 && c <= 32) || c == 127)//不可见字符范围0-31,127,空格可见,ASCII为32
					continue;
				else
					newLineWStr += c;
			}
			wstring::size_type pos1 = tmpLineWStr.find(L"[");//查找Tag标签
			wstring::size_type pos2 = tmpLineWStr.find_last_of(L']');//查找Tag标签
			if (newLineWStr.find(L"[") == 0 && newLineWStr.find_last_of(L']') == newLineWStr.size() - 1)
			{//当前行是标签
				wstring tag = tmpLineWStr.substr(pos1, pos2 - pos1 + 1);
				if (!tagIndexRangeMap.count(tag))//找不到当前tag
					tagIndexRangeMap[tag] = { beatmapVector.size() - 1, beatmapVector.size() - 1 };
				tagIndexRangeMap[currentTag].end = beatmapVector.size() - 2;
				currentTag = tag;
			}
		}

		//变速部分谱面容器追加
		wstring fullTag = L"[" + variableSpeedTag + L"]";
		wstring newLineWStr;
		for (wchar_t c : tmpLineWStr)
		{
			if ((c >= 0 && c <= 32) || c == 127)//不可见字符范围0-31,127,空格可见,ASCII为32
				continue;
			else
				newLineWStr += c;
		}
		wstring::size_type pos = newLineWStr.find(fullTag);//查找Tag标签
		if (pos != wstring::npos)
		{
			if (!FoundVariableSpeedTag)//仅添加一次标签
				variableSpeedVector.push_back(tmpLineWStr);
			FoundVariableSpeedTag = true;
		}
		else
		{
			if (FoundVariableSpeedTag)
			{
				if (tmpLineWStr.find(L"[") != wstring::npos && tmpLineWStr.find(L"]") != wstring::npos)//下一个标签
				{//找到
					//if (tmpLineWStr.find(fullTag) == wstring::npos)//并且是变速标签
					//	continue;
					//else//并且不是变速标签
					FoundVariableSpeedTag = false;//停止追加变速部分谱面内容
				}
				else//找不到，追加变速部分谱面内容
				{
					//去除空格，Tab等特殊字符，判断是否为空行
					wstring newLineWStr;
					for (wchar_t c : tmpLineWStr)
					{
						//if ((c >= 0 && c <= 31) || c == 127)//不可见字符范围
						//	continue;
						//else if (/*c == L'\t' ||*/  c == L' ') // '\t' == 9
						//	continue;
						//else
						//	newLineWStr += c;
						if ((c >= 0 && c <= 32) || c == 127)//不可见字符范围0-31,127,空格可见,ASCII为32
							continue;
						else
							newLineWStr += c;
					}
					if (newLineWStr != L"")// 非空行
					{
						variableSpeedVector.push_back(tmpLineWStr/*这里绝对不能是newLineWStr!!!*/);
					}
				}
			}
		}

	}//while(getline())
	if (beatmapContents != L"")
		beatmapContents.pop_back();//去除手动添加的末尾的\n换行符
	file.close();
	return true;
}
bool BeatmapManager::openBeatmap(std::wstring fileWithPath)
{
	this->fileWithPath = fileWithPath;
	return openBeatmap();
}

std::vector<std::wstring> BeatmapManager::getBeatmapVectorByTag(std::wstring tag)
{
	// TODO: 按标签查找谱面容器
	std::vector<std::wstring> res;
	if (!tagIndexRangeMap.count(tag))
		return res;
	const Range& tagIndexRange = tagIndexRangeMap[tag];
	for (size_t i = tagIndexRange.start; i <= tagIndexRange.end; i++)
		res.push_back(beatmapVector[i]);
	return res;
}

void BeatmapManager::changeVariableSpeedVector(std::vector<std::wstring> contents)
{
	changeContentsByTag(L"[" + variableSpeedTag + L"]", contents, true);
}

bool BeatmapManager::save()
{
	return saveAs(fileWithPath);
}

bool BeatmapManager::saveAs(std::wstring fileWithPath)
{
	using namespace std;
	ofstream f(fileWithPath, ios::out);
	if (!f.is_open())
		return false;
	f << wstr2str_2UTF8(beatmapContents);
	f.close();
	return true;
}

bool BeatmapManager::changeContentsByTag(std::wstring tag, std::vector<std::wstring> contents, bool bAddTagIfUncontained)
{
	//TODO: 特殊处理需求：变速铺面
	if (tag == (L"[" + variableSpeedTag + L"]"))
	{
		std::wstring firstElem;
		if (variableSpeedVector.size())
			firstElem = variableSpeedVector[0];
		else
			firstElem = L"[" + variableSpeedTag + L"]";
		variableSpeedVector.clear();
		variableSpeedVector.push_back(firstElem);
		for (auto i = contents.begin(); i != contents.end(); i++)
			variableSpeedVector.push_back(*i);
	}

	//一般情况
	if (!tagIndexRangeMap.count(tag))
	{
		if (bAddTagIfUncontained)
		{
			beatmapVector.push_back(tag);
			tagIndexRangeMap[tag] = {
				beatmapVector.size() - 1,
				beatmapVector.size() - 1
			};
		}
		else
			return false;
	}
	Range& tagIndexRange = tagIndexRangeMap[tag];
	typedef unsigned long long ull;
	ull oriLen = tagIndexRange.end - tagIndexRange.start;//原来谱面信息的行数
	ull newLen = contents.size();
	//新值减去原行数,若new>ori,则为需要添加的行数,否则为需要删除的行数
	bool newLargerThanOri = newLen > oriLen;
	ull sub = (newLargerThanOri ? (newLen - oriLen) : (oriLen - newLen));
	ull minLen = (newLargerThanOri ? oriLen : newLen);//共有的行数
	for (ull i = 0; i < minLen; i++)//替换相同部分铺面
		beatmapVector[tagIndexRange.start + i + 1] = contents[i];
	if (newLargerThanOri)
	{
		//需分配新内存
		for (ull i = 0; i < sub; i++)//替换相同部分铺面
			beatmapVector.insert(beatmapVector.begin() + tagIndexRange.start + i + 1 + minLen, contents[i + minLen]);
	}
	else
	{
		//原来铺面的行数多了，需删除多余行
		beatmapVector.erase(beatmapVector.begin() + tagIndexRange.start + minLen, beatmapVector.begin() + tagIndexRange.start + minLen + sub);
	}
	tagIndexRange.end = tagIndexRange.start + (newLargerThanOri ? newLen : oriLen) + 1;
	updateBeatmapContents();

	return true;
}

bool BeatmapManager::isTagInBeatmap(std::wstring tag)
{
	return tagIndexRangeMap.count(tag);
}

bool BeatmapManager::addTagAndContents(std::wstring tag, std::vector<std::wstring> contents, bool replaceExistedTag)
{
	if ((!replaceExistedTag) && tagIndexRangeMap.count(tag))
		return false;
	return changeContentsByTag(tag, contents, true);
}

void BeatmapManager::updateBeatmapContents()
{
	beatmapContents.clear();
	for (auto iter = beatmapVector.begin(); iter != beatmapVector.end(); iter++)
		beatmapContents += *iter + L"\n";
	if (beatmapContents != L"")
		beatmapContents.pop_back();
}
