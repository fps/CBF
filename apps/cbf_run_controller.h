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


    Copyright 2011 Viktor Richter
*/

#ifndef CBF_RUN_CONTROLLER_HH
#define CBF_RUN_CONTROLLER_HH

#include <cbf/config.h>
#include <cbf/controller.h>
#include <cbf/control_basis.h>
#include <cbf/debug_macros.h>
#include <cbf/namespace.h>

#ifdef CBF_HAVE_QT
	#include <QApplication>
#endif

#include <IceUtil/Monitor.h> 
#include <IceUtil/RecMutex.h>

#include <boost/shared_ptr.hpp>

#include <stdexcept>

namespace CBF {

/**
	@brief An exception that will be thrown, when an action is called,
	which can not be done while the controller is running.
*/
struct ControllerRunningException : std::runtime_error {

	public:
		ControllerRunningException() 
			: 
			std::runtime_error ("action impossible while controller is running")
		{ }	
};

/**
	@brief An exception that will be thrown, when an action is called,
	which needs the namespace to be set first.
*/
struct ObjectNamespaceNotSetException : std::runtime_error {

	public:
		ObjectNamespaceNotSetException()
			: 
			std::runtime_error ("action impossible while the object namespace is not sets")
		{ }	
};

/**
	@brief An exception that will be thrown, when start_controller() is called with a
	conntroller_name, that does not exist in the namespace
*/
struct ControllerNotFoundExcepption : std::runtime_error {

	public:
		ControllerNotFoundExcepption()
			: 
			std::runtime_error ("could not find the controller in the object namespace")
		{ }
};






/**
	@brief A struct that runs a controller from a controller 
	name and a ObjectNamespacePtr. The execution of the controller
	and all functions are (meant to be) threadsafe.
*/
struct CBFRunController {
	public:

	/**
		@brief Creates a CBFRunController struct, does not start to run the controller.
		Execution can be started with start_controller() after an ObjectNamespace was set.

		@param sleep_time The sleep time after each step of execution in milliseconds.
		@param steps The count of steps to execute. Setting steps to 0 lets the controller run
		until convergence.
		@param verbosity_level Sets the verbosity level.s
				
	*/	
	CBFRunController(
				unsigned int sleep_time = 0,
				unsigned int steps = 0,
				unsigned int verbosity_level = 0
				#ifdef CBF_HAVE_QT
					,bool qt_support = false
				#endif
				);

	/**
		A virtual destructor, so polymorphic cleanup 
		works nicely
	*/
	virtual ~CBFRunController() {}

	/**
		@brief Sets the ObjectNamespace when the controller is not running (thread safe).
	*/
	void setObjectNamespace(CBF::ObjectNamespacePtr object_namespace) throw(ControllerRunningException);

	/**
		@brief Returns an ObjectNamespacePtr with a copy of the object namespace.
	*/
	const CBF::ObjectNamespacePtr getObjectNamespace() throw(ObjectNamespaceNotSetException);

	/**
		@brief Changes the sleep-time (thread-save).
	*/
	void setSleepTime(unsigned int time);

	/**
		@brief Returns the sleep-time (thread-save).
	*/
	unsigned int sleepTime();

	/**
		@brief Changes the amount of steps (thread-save).

		If at start_controller() the amount of steps was greater then 0
		a value less or equal 0 will stop the execution.
	*/
	void setStepCount(unsigned int steps);

	/**
		@brief Returns the step count (thread-save).
	*/
	unsigned int stepCount();

	/**
		@brief Decreases the step count by 1 (thread-save).
	*/
	void decStepCount();

	/**
		@brief Changes the verbosity level (thread-save).
	*/
	void setVerbosityLevel(unsigned int verbosity_level);

	/**
		@brief Returns the verbosity level (thread-save).
	*/
	unsigned int verbosityLevel();
	#ifdef CBF_HAVE_QT

		/**
			@brief Switches the Qt-support (thread-save). Only works 
			before start_controller() was called.

			qt_support must not be set to true without an initialized QApplication.
		*/
		void setQTSupport(bool qt_support);

		/**
			@brief Returns whether Qt-support is on (thread-save).
		*/
		bool qtSupport();
	#endif	

	/**
		@brief Starts to run the controller 'controller_name' from the m_ObjectNamespace.
		m_ObjectNamespace must be set before. Waits for m_SleepTime milliseconds after each
		step of execution. Execution can be stopped through stop_controller().

		If m_Steps == 0 the controller runs until convergence.
		If m_Steps > 0 this amount of steps will be performed.
	*/
	void start_controller(std::string controller_name) 
		throw(ObjectNamespaceNotSetException, ControllerNotFoundExcepption, ControllerRunningException);

	/**
		@brief Stops the controller from execution before the next step is performed.
	*/
	void stop_controller();
	
	/**
		@brief a thread safe way to check whether the controller is running.

		@return Whether the controller is running.
	*/
	bool checkControllerRuns();

	/**
		@brief a thread safe way to check whether the ObjectNamespace is already set.

		@return Whether the ObjectNamespace is set.
	*/
	bool checkObjectNamespaceSet();
	
	/**
		@brief a thread safe way to check whether a controller is in the ObjectNamespace.

		@return Whether the controller exists.
	*/
	bool checkControllerExists(std::string controller_name)
		throw(ObjectNamespaceNotSetException, ControllerNotFoundExcepption);
	
	/**
		@brief a thread safe way to check whether the controller
		converged.

		@return Whether the controller converged in the last step().
	*/
	bool checkConverged();


	private:

	/**
		@brief Holds the ObjectnamespacePtr.
	*/
	ObjectNamespacePtr m_ObjectNamespace;

	/**
		@brief Holds the sleep-time in milliseconds.
	*/
	unsigned int m_SleepTime;

	/**
		@brief Holds the amount of steps to perform.
	*/
	unsigned int m_Steps;

	/**
		@brief Switches the verbosity level.
	*/
	unsigned int m_VerbosityLevel;

	#ifdef CBF_HAVE_QT
		/**
			@brief Switches the Qt-support.
		*/
		bool m_QtSupport;
	#endif

	/**
		@brief Tells whether the controller is running.
	*/
	bool m_ControllerRunning;

	/**
		@brief Tells whether the ObjectNamespace is set.
	*/
	bool m_ObjectNamespaceSet;
	
	/**
		@brief Tells whether the controller converged.
	*/
	bool m_Converged;

	/**
		@brief The mutex-lock that is used for the thread 
		syncronization of the sleep-time resource.
	*/
	IceUtil::Monitor<IceUtil::RecMutex> m_SleepTimeMonitor;

	/**
		@brief The mutex-lock that is used for the thread 
		syncronization of the steps resource.
	*/
	IceUtil::Monitor<IceUtil::RecMutex> m_StepsMonitor;

	/**
		@brief The mutex-lock that is used for the thread 
		syncronization of the verbosity-level resource.
	*/
	IceUtil::Monitor<IceUtil::RecMutex> m_VerbosityLevelMonitor;

	/**
		@brief The mutex-lock that is used for the thread 
		syncronization of the controllerRunning resource.
	*/
	IceUtil::Monitor<IceUtil::RecMutex> m_ControllerRunningMonitor;
	
	/**
		@brief The mutex-lock that is used for the thread 
		syncronization of the converged resource.
	*/
	IceUtil::Monitor<IceUtil::RecMutex> m_ConvergedMonitor;

	#ifdef CBF_HAVE_QT
		/**
			@brief The mutex-lock that is used for the thread 
			syncronization of the qt-support resource.
		*/
		IceUtil::Monitor<IceUtil::RecMutex> m_QtSupportMonitor;
	#endif

	/**
		@brief a thread safe way to check whether the controller
		is already running and set it.

		@param running A boolean to set the controller to.
		@return Whether the controller was running before.
	*/
	bool checkControllerRuns(bool running);
	
	/**
		@brief a thread safe way to set whether the controller
		converged.

		@param converged A boolean whether the controller converged.
		@return Returns the same boolean that is passed.
	*/
	bool setConverged(bool converged);

};

/**
	@brief A shared pointer to a CBFRunController.
*/
typedef boost::shared_ptr<CBFRunController> CBFRunControllerPtr;

} //namespace
#endif
