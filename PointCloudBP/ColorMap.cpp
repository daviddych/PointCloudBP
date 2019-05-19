#include "stdafx.h"
#include "ColorMap.h"
#include <algorithm>
#include <iterator>

CColorMap::CColorMap()
{
}


CColorMap::~CColorMap()
{
}

std::vector<glm::vec3> CColorMap::colorMap(const std::vector<float> & data)
{
	assert(!_lut.empty()); 
	std::vector<glm::vec3> res(data.size());
	
	const auto pts = std::minmax_element(std::begin(data), std::end(data));
	unsigned int n = _lut.size();
	float delta = (*pts.second - *pts.first) / (n - 1);
	for (size_t i = 0; i < data.size(); ++i)
	{
		res[i] = _lut[floor((data[i] - *pts.first) / delta)];
	}

	return std::move(res);
}

