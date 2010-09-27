#include <cbf/robotinterface_resource.h>
#include <cbf/xml_factory.h>

namespace CBF {

#ifdef CBF_HAVE_XSD
	RobotInterfaceResource::RobotInterfaceResource(const CBFSchema::RobotInterfaceResource &xml_instance) {
		init(xml_instance.SendMemoryName(), xml_instance.RecvMemoryName(), xml_instance.RobotName(), xml_instance.NumberOfJoints());
	}

	static XMLDerivedFactory<RobotInterfaceResource, CBFSchema::RobotInterfaceResource> x;
#endif

} // namespace

