#include "StdAfx.h"
#include "DUIStruct.h"

uint32_t hash<tagDuiImageSection>::operator()(const tagDuiImageSection &ImageSection) const
{
	CMMString strInfo;
	for (auto &strImageRes : ImageSection.vecImageResSwitch)
	{
		strInfo += strImageRes;
	}

	strInfo += CMMStrHelp::Format(_T("-%d-%d-%d-%d-%d-%d-%d-%d"),
		ImageSection.HorizImageAlign,
		ImageSection.VertImageAlign,
		(int)ImageSection.nHorizOffset,
		(int)ImageSection.nVertOffset,
		ImageSection.ImageSourceType,
		ImageSection.bPartVert,
		(int)ImageSection.cbPartAll,
		(int)ImageSection.cbPartSel);

	for (auto &SourceCustomScale : ImageSection.mapSourceCustomScale)
	{
		if (CDUIGlobal::GetInstance()->GetResVersion() < DuiResVersion_5)
		{
			strInfo += CMMStrHelp::Format(_T("-%d-%d-%d-%d"), SourceCustomScale.first, SourceCustomScale.second.left,
				SourceCustomScale.second.top, SourceCustomScale.second.right, SourceCustomScale.second.bottom);
		}
		else
		{
			strInfo += CMMStrHelp::Format(_T("-%d-%d-%d-%d-%d"), SourceCustomScale.first, SourceCustomScale.second.left,
				SourceCustomScale.second.top, SourceCustomScale.second.right, SourceCustomScale.second.bottom);
		}
	}

	strInfo += CMMStrHelp::Format(_T("-%d-%d-%d-%d-%d-%d-%d-%d-%d"), (int)ImageSection.cbAlpha, (int)ImageSection.dwMask,
		(int)ImageSection.rcCorner.left, (int)ImageSection.rcCorner.top, (int)ImageSection.rcCorner.right, (int)ImageSection.rcCorner.bottom,
		(int)ImageSection.bCornerHole, 0, 0);

	return CMMHash::GetHash(strInfo);
}