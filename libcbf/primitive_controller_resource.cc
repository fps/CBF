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

#include <cbf/primitive_controller_resource.h>

#include <cbf/debug_macros.h>
#include <cbf/xml_object_factory.h>


namespace CBF {
	
	#ifdef CBF_HAVE_XSD
		PrimitiveControllerResource::PrimitiveControllerResource(
			const CBFSchema::PrimitiveControllerResource &xml_instance,
			ObjectNamespacePtr object_namespace) :
			Resource(xml_instance, object_namespace)
		{
			m_PrimitiveController = XMLObjectFactory::instance()->create<PrimitiveController>
				(xml_instance.PrimitiveController(), object_namespace);
		}	
	
	static XMLDerivedFactory<PrimitiveControllerResource, 
									 CBFSchema::PrimitiveControllerResource> x;
	#endif


} // namespace

