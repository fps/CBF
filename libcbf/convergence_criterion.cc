#include <cbf/convergence_criterion.h>
#include <cbf/xml_object_factory.h>
#include <cbf/primitive_controller.h>

namespace CBF {

	bool TaskSpaceDistanceThreshold::check_convergence(const SubordinateController &p) {
		return p.m_GradientStep.norm() < m_Threshold;
	}
	
	bool ResourceStepNormThreshold::check_convergence(const SubordinateController &p) {
		return p.m_ResourceStep.norm() < m_Threshold;
	}
	
	
	#ifdef CBF_HAVE_XSD
		ConvergenceCriterion::ConvergenceCriterion(const CBFSchema::ConvergenceCriterion &xml_instance, ObjectNamespacePtr object_namespace) :
			Object(xml_instance, object_namespace)
		{

		}

		TaskSpaceDistanceThreshold::TaskSpaceDistanceThreshold(
			const CBFSchema::TaskSpaceDistanceThreshold &xml_instance, 
			ObjectNamespacePtr object_namespace
		) :
			ConvergenceCriterion(xml_instance, object_namespace)
		{
			m_Threshold = xml_instance.Threshold();
			CBF_DEBUG("task space threshold: " << m_Threshold);
		}

 		ResourceStepNormThreshold::ResourceStepNormThreshold(
			const CBFSchema::ResourceStepNormThreshold &xml_instance, 
			ObjectNamespacePtr object_namespace
		) :
			ConvergenceCriterion(xml_instance, object_namespace)
		{
			m_Threshold = xml_instance.Threshold();
			CBF_DEBUG("resource space threshold: " << m_Threshold);

		}

		XMLDerivedFactory<TaskSpaceDistanceThreshold, CBFSchema::TaskSpaceDistanceThreshold> x1;
		XMLDerivedFactory<ResourceStepNormThreshold, CBFSchema::ResourceStepNormThreshold> x2;
	#endif
	
} // namespace 
