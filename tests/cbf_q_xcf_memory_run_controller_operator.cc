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


    Copyright 20011 Viktor Richter
*/

#include <cbf/config.h>
#include <cbf/controller.h>
#include <cbf/control_basis.h>
#include <cbf/xsd_error_handler.h>
#include <cbf/schemas.hxx>

#include <cbf_q_xcf_memory_run_controller_operator.h> 

#include <string>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <time.h>

#include <memory>

#include <boost/program_options.hpp>


#include <QtGui/QFileDialog>
#include <QtGui/QInputDialog>
#include <QtGui/QLabel>
#include <QMessageBox>


namespace po = boost::program_options;

int main(int argc, char *argv[]) {
	po::options_description options_description("Allowed options");
	options_description.add_options() 
		(
			"help", 
			"produce help message"
		)
		(
			"rcname",
			po::value<std::string>(), 
			"The name of the XCFMemoryRunContoller to operate"
		)
		(
			"am",
			po::value<std::string>(), 
			"Name of the active-memory on which to publish"
		)
		;

	po::variables_map variables_map;

	po::store(
		po::parse_command_line(
			argc, 
			argv, 
			options_description
		), 
		variables_map
	);

	po::notify(variables_map);

	if (variables_map.count("help")) {
		std::cout << options_description << std::endl;
		return(EXIT_SUCCESS);
	}

	if (!variables_map.count("am")) {
		std::cout << "No active-memory specified" << std::endl;
		std::cout << options_description << std::endl;
		return(EXIT_FAILURE);
	}

	// getting the name of the actice_memory
	std::string active_memory_name = 
		variables_map["am"].as<std::string>();

	if (!variables_map.count("rcname")) {
		std::cout << "No run-controller-name specified" << std::endl;
		std::cout << options_description << std::endl;
		return(EXIT_FAILURE);
	}

	// getting the name of the run_controller
	std::string run_controller_name = 
		variables_map["rcname"].as<std::string>();

	//Initializing the QApplication.
	QApplication *app = new QApplication(argc, argv);

	// initializing an XcfMemoryRunControllerOperator so we can connect it to buttons.
	XcfMemoryRunControllerOperator op(active_memory_name, run_controller_name);

	//Initializing the Applictaions main-window.
	QWidget *window = new QWidget();
	QPushButton send("send descriptions");
	QPushButton create("create namespace");
	QPushButton execute("execute controller");
	QPushButton stop("stop controller");
	QPushButton sleep_time("sleep time");
	QPushButton steps("steps");
	QPushButton quit("quit");

	// connecting buttons to the corresponding slots.
	QObject::connect(&send, SIGNAL(clicked()), &op, SLOT(send_descriptions()));
	QObject::connect(&create, SIGNAL(clicked()), &op, SLOT(create_namespace()));
	QObject::connect(&execute, SIGNAL(clicked()), &op, SLOT(execute()));
	QObject::connect(&stop, SIGNAL(clicked()), &op, SLOT(stop()));
	QObject::connect(&sleep_time, SIGNAL(clicked()), &op, SLOT(set_time()));
	QObject::connect(&steps, SIGNAL(clicked()), &op, SLOT(set_steps()));
	QObject::connect(&quit, SIGNAL(clicked()), app, SLOT(quit()));

	// layouting the main window
	QVBoxLayout *windowLayout = new QVBoxLayout(window);
	windowLayout -> addWidget(&send);
	windowLayout -> addWidget(&create);
	windowLayout -> addWidget(&execute);
	windowLayout -> addWidget(&stop);
	windowLayout -> addWidget(&sleep_time);
	windowLayout -> addWidget(&steps);
	windowLayout -> addWidget(&quit);

	window -> setLayout(windowLayout);
	window -> show();

	//Starting the Application
	app -> exec();
}

void XcfMemoryRunControllerOperator::send_descriptions(){
	// getting the name of the xml file.
	QStringList files =
			QFileDialog::getOpenFileNames(this, tr("Open File"), m_DirPath.c_str(), tr("XML (*.xml);;ALL (*)"));
	QStringList files2 = files;

	if(files2.size() > 0) {m_DirPath = files2.at(0).toStdString();}

	memory::interface::Attachments att;

	// creating the XCFMemoryRunControllerAdd document
	CBFSchema::XCFMemoryRunControllerAdd v(m_RunControllerName);

	//creating buffers and setting attachment-names
	for(QStringList::const_iterator it = files2.begin();
			it != files2.end(); ++it)
	{
		std::string filename = it -> toStdString();

		// create Buffer and fill it with data from file.
		memory::interface::Buffer buffer;

		std::ifstream docFile(filename.c_str());
		if (docFile.is_open()){
			while (!docFile.eof()) {
				buffer.push_back(docFile.get());
			}
			docFile.close();
		} else {
			CBF_DEBUG("could not open file: " << filename);
			break;
		}

		//TODO: Memory server needs unique ID's. UUID when avaliable.
		// set the Buffer with (hopefully unique) id
		std::ostringstream id;
		id << rand() << rand() << rand();

		att[id.str()] = buffer;

		v.AttachmentName().push_back(id.str());

		//map the filename to the id.
		m_AttachmentNames[filename] = id.str();
	}

	try {

		std::ostringstream s;
		CBFSchema::XCFMemoryRunControllerAdd_ (s, v);
		// sending the document to the active_memory

		m_MemoryInterface -> insert(s.str(), &att);

	} catch (const xml_schema::exception& e) {
		std::cerr << "Error during parsing: " << e << std::endl;
	} catch (const std::exception& e) {
		std::cerr << "Unexpected exception: " << e.what() << std::endl;
	} catch (...) {
		std::cerr << "Unknown unexpected exception." << std::endl;
	}
}

void XcfMemoryRunControllerOperator::create_namespace(){
	// getting the names of the controllers to load.
	bool ok;
	XcfMemoryRunControllerDocumentDialog tmp(m_AttachmentNames, this);
	std::vector<std::string> ids = tmp.exec();
	if (!ids.empty()){
		// creating the XCFMemoryRunControllerLoadControllers document.
		CBFSchema::XCFMemoryRunControllerLoadNamespace v(m_RunControllerName);

		// adding all attachment id's to the document
		std::vector<std::string>::const_iterator it;
		for(it = ids.begin(); it != ids.end(); it++)
		{
			v.AttachmentName().push_back(*it);
		}

		std::ostringstream s;
		CBFSchema::XCFMemoryRunControllerLoadNamespace_ (s, v);
		// sending the document to the active_memory
		m_MemoryInterface -> insert(s.str());
	}
}

void XcfMemoryRunControllerOperator::set_time(){
	// getting the new sleep time.
	bool ok;
	int i = QInputDialog::getInt(this, tr("enter new sleep time (ms)"),
			tr("sleep time:"), 0, 0, 2147483647, 100, &ok);

	if (ok){
		// creating the XCFMemoryRunControllerOptions document
		CBFSchema::XCFMemoryRunControllerOptions v(m_RunControllerName);
		// setting the sleep-time
		v.SleepTime(i);

		std::ostringstream s;
		CBFSchema::XCFMemoryRunControllerOptions_ (s, v);
		// sending the document to the active_memory
		m_MemoryInterface -> insert(s.str());		
	}
}

void XcfMemoryRunControllerOperator::set_steps(){
	// getting the new step count.
	bool ok;
	int i = QInputDialog::getInt(this, tr("enter new step count"),
			tr("steps:"), 0, 0, 2147483647, 100, &ok);

	if (ok){
		// creating the XCFMemoryRunControllerOptions document
		CBFSchema::XCFMemoryRunControllerOptions v(m_RunControllerName);
		// setting the step count
		v.Steps(i);

		std::ostringstream s;
		CBFSchema::XCFMemoryRunControllerOptions_ (s, v);
		// sending the document to the active_memory
		m_MemoryInterface -> insert(s.str());
	}
}

void XcfMemoryRunControllerOperator::execute(){
	// getting the name of the controller to execute.
	bool ok;
	QString text = QInputDialog::getText(this, tr("enter controller name to execute"),
				tr("controller name:"), QLineEdit::Normal, "", &ok);
	if (ok && !text.isEmpty()){
		// creating the XCFMemoryRunControllerExecute document with the controller name.
		CBFSchema::XCFMemoryRunControllerExecute v(m_RunControllerName, text.toStdString());

		std::ostringstream s;
		CBFSchema::XCFMemoryRunControllerExecute_ (s, v);
		// sending the document to the active_memory
		m_MemoryInterface -> insert(s.str());
	}
}

void XcfMemoryRunControllerOperator::stop(){
	// creating a XCFMemoryRunControllerStop.
	CBFSchema::XCFMemoryRunControllerStop v(m_RunControllerName);
	std::ostringstream s;
	CBFSchema::XCFMemoryRunControllerStop_ (s, v);
	// sending the document to the active_memory
	m_MemoryInterface -> insert(s.str());
}

void XcfMemoryRunControllerDocumentDialog::init(std::map<std::string, std::string> attachment_map){
	QGridLayout* windowLayout = new QGridLayout(this);

	QLabel* title = new QLabel("Choose the source files for the namespace", this);
	windowLayout -> addWidget(title, 0, 0, 1, 2);

	std::map<std::string, std::string>::const_iterator it;
	int i = 1;
	for(it = attachment_map.begin(); it != attachment_map.end(); ++it, ++i){
		std::stringstream filenameAndID;
		filenameAndID << (it -> first) << "(" << (it -> second) << ")";
		std::string shortFilename = (it -> first);
		size_t slashPos = shortFilename.find_last_of('\\');
		if (slashPos == shortFilename.npos) slashPos = 0;
		shortFilename = shortFilename.substr(slashPos, shortFilename.npos);
		QCheckBox* cb = new QCheckBox((it -> first).c_str(), this);
		//setting the attachment-id as tooltip.
		cb -> setToolTip(filenameAndID.str().c_str());
 		m_QCheckBoxes.push_back(cb);
		windowLayout ->  addWidget(cb, i, 0, 1, 2);
	}

	QPushButton* accept = new QPushButton("okay", this);
	QPushButton* reject = new QPushButton("cancel", this);

	QObject::connect(accept, SIGNAL(clicked()), this, SLOT(accept()));
	QObject::connect(reject, SIGNAL(clicked()), this, SLOT(reject()));

	windowLayout ->  addWidget(reject, i + 1, 0, 1, 1);
	windowLayout ->  addWidget(accept, i + 1, 1 , 1, 1);
}

std::vector<std::string> XcfMemoryRunControllerDocumentDialog::exec(){
	CBF_DEBUG("running the dialog");
	int i = QDialog::exec();
	std::vector<std::string> ret;
	if (i == QDialog::Accepted) {
		std::vector<QCheckBox*>::const_iterator it;
		for (it = m_QCheckBoxes.begin(); it != m_QCheckBoxes.end(); ++it){
			if((*it) -> isChecked()){
				ret.push_back(((*it) -> toolTip()).toStdString());
			}
		}
	}
	return ret;
}
