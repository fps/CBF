/* -*- mode: c-non-suck; -*- */

#ifndef CBF_C_BINDINGS_1212121_HH
#define CBF_C_BINDINGS_1212121_HH

/**
	NOTE: Deprecated. See c_api.h

	This struct is intended to be an opaque container holding
	a handle to the real controller. At this time only PrimitiveControllers
	are supported.
*/
struct primitive_controller {
	/**
		NOTE: Deprecated. See c_api.h

		This is a pointer to a boost::shared_ptr<PrimitiveController>. Do not mess 
		with this from within C.
	*/
	void *controller_ptr;
};

#ifdef __cplusplus
extern "C" {
#endif

void init_cbf();

/**
	NOTE: Deprecated. See c_api.h

	This function creates a controller from a file representing an XML infoset. The parameter
	c needs to be pointing to a preallocated struct primitive_controller.

	Return 0 on failure. Else the memory pointed to by c is filled with appropriate data 
	and returned.
*/
struct primitive_controller*
create_controller_from_file(
	struct primitive_controller *c, 
	const char *filename
);

/**
	NOTE: Deprecated. See c_api.h

	This function creates a file from a memory region holding a text representation
	of an XML infoset.

	Otherwise this function behaves as create_controller_from_file().

	TODO: Implement!!
*/
struct primitive_controller*
create_controller_from_memory(
	struct primitive_controller *c, 
	const char *mem
);


/**
	NOTE: Deprecated. See c_api.h

	Fills a preallocated struct primitive_controller with a pointer that refers
	to the index-th subordinate controller.

	Arguments:
		pc - Pointer to a struct primitive controller of which a subordinate controller
		spc - Pointer to a struct primitive_controller that will hold a reference to the subordinate controller

	CAUTION:
	
	The pointer is only legal to use as long as the original controller is
	legal to use.

	Returns the pointer to the struct primitve_controller that was passed in as spc..
*/
struct primitive_controller*
controller_get_subordinate_controller(
	struct primitive_controller *pc, 
	struct primitive_controller *spc, 
	int index
);


/**
	NOTE: Deprecated. See c_api.h

	Returns 1 if the controller is finished (converged). Returns 0 if the controller
	is not converged. 

	CAUTION:

	Precondition: The controller has to have been stepped at least once before thie test returns
	sensible values
*/
int
controller_is_finished(struct primitive_controller *pc);

/**
	NOTE: Deprecated. See c_api.h

	Fills out with the current task space position represented as a double[]. Make
	sure out points to valid memory!

	NOTE: The controller has to be run (step() or do_step()) at least once before this function returns
	sensible values.

	Returns -1 on error..
*/
int 
controller_get_current_task_position(
	struct primitive_controller *c, 
	double *out
);

/**
	NOTE: Deprecated. See c_api.h

	Returns the dimension of the resource of the controller..

	Returns -1 on error
*/
int
controller_get_resource_dim(
	struct primitive_controller *c
);

/**
	NOTE: Deprecated. See c_api.h

	Sets the reference of the controller (the point in task space that the controller
	eventually should converge to.. The C++ interface allows to set several references
	and the controller is expected to converge to the closest one, but for simplicity
	this function only exposes a single reference point via this C interface.

	Returns -1 on error..
*/
int
controller_set_reference(
	struct primitive_controller* c, 
	double *reference
);

/**
	NOTE: Deprecated. See c_api.h

*/
int 
controller_get_reference(
	struct primitive_controller* c, 
	double *reference
);


/**
	NOTE: Deprecated. See c_api.h

	@brief Set current resource values (e.g. joint angles of a robotic arm) corresponding to the sensor transform

	Arguments:

	c - The struct primitive_controller thhat is referenced
	resource_out - an array of doubles holding values to be passed

	Returns -1 on failure
*/
int 
controller_set_resource(
	struct primitive_controller *c, 
	double *resource_in
);

/**
	NOTE: Deprecated. See c_api.h

	@brief Set current resource values (e.g. joint angles of a robotic arm) corresponding to the sensor transform

	Arguments:

	c - The struct primitive_controller thhat is referenced
	resource_out - a preallocated array of doubles to be filled with the resource values

	Returns -1 on failure
*/
int 
controller_get_resource(
	struct primitive_controller *c, 
	double *resource_out
);


/**
	NOTE: Deprecated. See c_api.h

	Run a single step of a controller. This function reads the current resource values from 
	the parameter in and writes the result into the memory pointed to by out.

	This function does not update the resource contained in the controller..

	Returns -1 on failure.
*/
int
step_controller(
	struct primitive_controller *c, 
	double *in, 
	double *out
);


/**
	NOTE: Deprecated. See c_api.h

	This function steps the controller.. This function is self contained in the sense
	that the controller's resource is updated automatically. 

	TODO: Implement!!
*/
int
step_controller_internal(
	struct primitive_controller *c
);

/**
	NOTE: Deprecated. See c_api.h

	Destroy the controller wrapped by the struct primitive_controller pointed to by c.

	This does not free() the struct itself. This is left to the user.
*/
struct primitive_controller*
destroy_controller(
	struct primitive_controller *c
);

#ifdef __cplusplus
}
#endif


#endif

