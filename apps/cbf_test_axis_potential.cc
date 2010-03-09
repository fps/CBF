#include <cbf/primitive_controller.h>
#include <cbf/orientation_potentials.h>
#include <cbf/dummy_resource.h>
#include <cbf/dummy_reference.h>
#include <cbf/difference_sensor_transform.h>
#include <cbf/kdl_transforms.h>
#include <cbf/generic_transform.h>

#include <boost/shared_ptr.hpp>
#include <kdl/chain.hpp>
#include <algorithm>

#define NUM_OF_JOINT_TRIPLES 3

int main() {
	//! Build up a fairly simple KDL::Chain (made up of 
	//! NUM_OF_JOINT_TRIPLES * 3 segments and joints)
	boost::shared_ptr<KDL::Chain> chain1 (new KDL::Chain);
	for (unsigned int i = 0; i < NUM_OF_JOINT_TRIPLES; ++i) {
		chain1->addSegment(
			KDL::Segment(
				KDL::Joint::RotX, KDL::Frame(KDL::Vector(0,0,0.1))));

		chain1->addSegment(
			KDL::Segment(
				KDL::Joint::RotY, KDL::Frame(KDL::Vector(0,0,0.1))));

		chain1->addSegment(
			KDL::Segment(
				KDL::Joint::RotZ, KDL::Frame(KDL::Vector(0,0,0.1))));
	}

	//! Build up a second chain with one additional fixed link
	boost::shared_ptr<KDL::Chain> chain2 (new KDL::Chain);
	for (unsigned int i = 0; i < NUM_OF_JOINT_TRIPLES; ++i) {
		chain2->addSegment(
			KDL::Segment(
				KDL::Joint::RotX, KDL::Frame(KDL::Vector(0,0,0.1))));

		chain2->addSegment(
			KDL::Segment(
				KDL::Joint::RotY, KDL::Frame(KDL::Vector(0,0,0.1))));

		chain2->addSegment(
			KDL::Segment(
				KDL::Joint::RotZ, KDL::Frame(KDL::Vector(0,0,0.1))));
	}
	//! On chain2 we add a final link with a fixed joint. This
	//! allows our difference sensor transform to do its thing
	chain2->addSegment(
		KDL::Segment(
			KDL::Joint::None, KDL::Frame(KDL::Vector(0,0,0.1))));


	CBF::KDLChainPositionSensorTransformPtr st1
		(new CBF::KDLChainPositionSensorTransform(chain1));

	CBF::KDLChainPositionSensorTransformPtr st2
		(new CBF::KDLChainPositionSensorTransform(chain2));

	std::vector<CBF::SensorTransformPtr> v;
	v.push_back(st1);
	v.push_back(st2);

	CBF::DifferenceSensorTransformPtr st
		(new CBF::DifferenceSensorTransform(v));

	CBF::GenericEffectorTransformPtr et(new CBF::GenericEffectorTransform(st));

	//! An AxisPotential for R^3
	CBF::AxisPotentialPtr p(new CBF::AxisPotential(3, 0.01));

	//! Initialize Reference with a vector that points along
	//! The X-Direction..
	//! Gah, screw c++.. This could be so much easier :D
	CBF::DummyReferencePtr dref(new CBF::DummyReference(1,3));
	double ref[] = {1, 0, 0};
	CBF::FloatVector vref(3);
	std::copy(ref, ref+3, vref.begin());
	dref->set_reference(vref);

	CBF::DummyResourcePtr dres(new CBF::DummyResource(NUM_OF_JOINT_TRIPLES * 3, 1));

	//! Create our primitive controller
	CBF::PrimitiveControllerPtr pc
		(new CBF::PrimitiveController);

	pc->set_sensor_transform(st);
	pc->set_effector_transform(et);

	pc->set_resource(dres);
	pc->set_potential(p);
	pc->set_reference(dref);

	do {
		pc->step();	
	} while(pc->finished() == false);
}
