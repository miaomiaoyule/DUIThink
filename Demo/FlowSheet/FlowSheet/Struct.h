#pragma once

struct tagLinkTarget
{
	CDUIContainerCtrl *					pLinkCtrl = NULL;
	enLinkType							LinkType = Link_Top;
};

struct tagLinkData
{
	tagLinkTarget						LinkLeft;
	tagLinkTarget						LinkTop;
	tagLinkTarget						LinkRight;
	tagLinkTarget						LinkBottom;
};

typedef std::unordered_map<CDUIContainerCtrl*, tagLinkData> MapLinkData;