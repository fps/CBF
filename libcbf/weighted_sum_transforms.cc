/*
    This file is part of CBF.

    CBF is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    CBF is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with CBF.  If not, see <http://www.gnu.org/licenses/>.


    Copyright 2009, 2010 Florian Paul Schmidt
*/

#include <cbf/weighted_sum_transforms.h>

#include <cbf/plugin_impl_macros.h>
#include <cbf/utilities.h>

namespace CBF {
	#ifdef CBF_HAVE_XSD
		WeightedSumSensorTransform::WeightedSumSensorTransform(const WeightedSumSensorTransformType &xml_instance)
		{
			CBF_DEBUG("yay!!!");
		
			std::vector<SensorTransformPtr> transforms;
		
			//! Instantiate the subordinate transforms
			SensorTransformChainType::SensorTransform_const_iterator it;
			for (
				it = xml_instance.SensorTransform().begin(); 
				it != xml_instance.SensorTransform().end();
				++it
			)
			{
				SensorTransformPtr tr(PluginPool<SensorTransform>::get_instance()->create_from_xml(*it));
				transforms.push_back(tr);
				//tr->set_resource(ResourcePtr(new DummyResource(tr->get_resource_dim())));
			}
		
			if (transforms.size() < 1) 
				throw std::runtime_error("[WeightedSumSensorTransform]: Number of transforms < 1");

			set_transforms(transforms);

			m_Weights = create_vector(xml_instance.Weights());
			if (m_Weights.size() != m_Transforms.size())
				throw std::runtime_error ("[WeightedSumSensorTransform]: Mismatching number of weights and transforms");
		}
		
	#endif

	CBF_PLUGIN_CLASS(WeightedSumSensorTransform, SensorTransform)

} // namespace



