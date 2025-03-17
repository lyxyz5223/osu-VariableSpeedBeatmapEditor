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
	string tmpLineStr;//һ�е��ı�
	while (getline(file, tmpLineStr))//���ж�ȡ
	{
		wstring tmpLineWStr = str2wstr_2UTF8(tmpLineStr);//����ת��
		beatmapContents += tmpLineWStr + L'\n';//׷�ӵ���������
		beatmapVector.push_back(tmpLineWStr);//���б���

		//��ǩrange����
		{
			tagIndexRangeMap[currentTag].end = beatmapVector.size() - 1;
			wstring newLineWStr;
			for (wchar_t c : tmpLineWStr)//ȥ�����ɼ��ַ�
			{
				if ((c >= 0 && c <= 32) || c == 127)//���ɼ��ַ���Χ0-31,127,�ո�ɼ�,ASCIIΪ32
					continue;
				else
					newLineWStr += c;
			}
			wstring::size_type pos1 = tmpLineWStr.find(L"[");//����Tag��ǩ
			wstring::size_type pos2 = tmpLineWStr.find_last_of(L']');//����Tag��ǩ
			if (newLineWStr.find(L"[") == 0 && newLineWStr.find_last_of(L']') == newLineWStr.size() - 1)
			{//��ǰ���Ǳ�ǩ
				wstring tag = tmpLineWStr.substr(pos1, pos2 - pos1 + 1);
				if (!tagIndexRangeMap.count(tag))//�Ҳ�����ǰtag
					tagIndexRangeMap[tag] = { beatmapVector.size() - 1, beatmapVector.size() - 1 };
				tagIndexRangeMap[currentTag].end = beatmapVector.size() - 2;
				currentTag = tag;
			}
		}

		//���ٲ�����������׷��
		wstring fullTag = L"[" + variableSpeedTag + L"]";
		wstring newLineWStr;
		for (wchar_t c : tmpLineWStr)
		{
			if ((c >= 0 && c <= 32) || c == 127)//���ɼ��ַ���Χ0-31,127,�ո�ɼ�,ASCIIΪ32
				continue;
			else
				newLineWStr += c;
		}
		wstring::size_type pos = newLineWStr.find(fullTag);//����Tag��ǩ
		if (pos != wstring::npos)
		{
			if (!FoundVariableSpeedTag)//�����һ�α�ǩ
				variableSpeedVector.push_back(tmpLineWStr);
			FoundVariableSpeedTag = true;
		}
		else
		{
			if (FoundVariableSpeedTag)
			{
				if (tmpLineWStr.find(L"[") != wstring::npos && tmpLineWStr.find(L"]") != wstring::npos)//��һ����ǩ
				{//�ҵ�
					//if (tmpLineWStr.find(fullTag) == wstring::npos)//�����Ǳ��ٱ�ǩ
					//	continue;
					//else//���Ҳ��Ǳ��ٱ�ǩ
					FoundVariableSpeedTag = false;//ֹͣ׷�ӱ��ٲ�����������
				}
				else//�Ҳ�����׷�ӱ��ٲ�����������
				{
					//ȥ���ո�Tab�������ַ����ж��Ƿ�Ϊ����
					wstring newLineWStr;
					for (wchar_t c : tmpLineWStr)
					{
						//if ((c >= 0 && c <= 31) || c == 127)//���ɼ��ַ���Χ
						//	continue;
						//else if (/*c == L'\t' ||*/  c == L' ') // '\t' == 9
						//	continue;
						//else
						//	newLineWStr += c;
						if ((c >= 0 && c <= 32) || c == 127)//���ɼ��ַ���Χ0-31,127,�ո�ɼ�,ASCIIΪ32
							continue;
						else
							newLineWStr += c;
					}
					if (newLineWStr != L"")// �ǿ���
					{
						variableSpeedVector.push_back(tmpLineWStr/*������Բ�����newLineWStr!!!*/);
					}
				}
			}
		}

	}//while(getline())
	if (beatmapContents != L"")
		beatmapContents.pop_back();//ȥ���ֶ���ӵ�ĩβ��\n���з�
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
	// TODO: ����ǩ������������
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
	//TODO: ���⴦�����󣺱�������
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

	//һ�����
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
	ull oriLen = tagIndexRange.end - tagIndexRange.start;//ԭ��������Ϣ������
	ull newLen = contents.size();
	//��ֵ��ȥԭ����,��new>ori,��Ϊ��Ҫ��ӵ�����,����Ϊ��Ҫɾ��������
	bool newLargerThanOri = newLen > oriLen;
	ull sub = (newLargerThanOri ? (newLen - oriLen) : (oriLen - newLen));
	ull minLen = (newLargerThanOri ? oriLen : newLen);//���е�����
	for (ull i = 0; i < minLen; i++)//�滻��ͬ��������
		beatmapVector[tagIndexRange.start + i + 1] = contents[i];
	if (newLargerThanOri)
	{
		//��������ڴ�
		for (ull i = 0; i < sub; i++)//�滻��ͬ��������
			beatmapVector.insert(beatmapVector.begin() + tagIndexRange.start + i + 1 + minLen, contents[i + minLen]);
	}
	else
	{
		//ԭ��������������ˣ���ɾ��������
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
