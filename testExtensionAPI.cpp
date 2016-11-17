//-
// ==================================================================
// Copyright 2016 Autodesk, Inc.  All rights reserved.
// 
// This computer source code  and related  instructions and comments are
// the unpublished confidential and proprietary information of Autodesk,
// Inc. and are  protected  under applicable  copyright and trade secret
// law. They may not  be disclosed to, copied or used by any third party
// without the prior written consent of Autodesk, Inc.
// ==================================================================
//
//
//

#include <maya/MIOStream.h>
#include <maya/MObject.h> 
#include <maya/MDGModifier.h> 
#include <maya/MGlobal.h>
#include <maya/MSelectionList.h>
#include <maya/MItSelectionList.h> 
#include <maya/MString.h>
#include <maya/MStatus.h>
#include <maya/MArgDatabase.h> 
#include <maya/MSyntax.h> 
#include <maya/MPxCommand.h> 
#include <maya/MFnPlugin.h> 
#include <maya/MNodeClass.h> 
#include <maya/MObjectArray.h> 
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnNumericAttribute.h>

#define MEL_COMMAND_NAME        "testExtensionAPI" 
#define VENDOR_TAG		"Autodesk - Test Plugin" 
#define PLUGIN_VERSION		"1.0"

#define kCreateExtensionsLong	"-createExtensions"
#define kCreateExtensions	"-ce"
#define kRemoveExtensionsLong	"-removeExtensions"
#define kRemoveExtensions	"-re"
#define kListExtensionsLong	"-listExtensions"
#define kListExtensions		"-le"

// Create extension attribute Name one parent and two childs 
#define kParentNameLong		"extParent"
#define kParentNameShort	"ep"
#define kChild1NameLong		"extChild1"
#define kChild1NameShort	"ex1"
#define kChild2NameLong		"extChild2"
#define kChild2NameShort	"ex2"

/////////
// Class Definition 
// 
class testExtensionAPI : public MPxCommand 
{
public: 
					testExtensionAPI( ); 
	virtual				~testExtensionAPI( ); 
	
	MStatus				doIt( const MArgList & ); 
	bool				isUndoable( ) const; 
	MStatus				undoIt( ); 
	MStatus				redoIt( ); 
	void addExtensionToPlugin(MObject &object) { pluginObject = object;}
    MStatus				parseArgs( const MArgList &args ); 
	static MSyntax		        newSyntax( );  	

	// Allocate a class instance for Maya. 
    static void*		creator( ); 
	
	enum OperationList { 
		kInvalid = 0, 
		kDoCreate,       // Create a set of extension attributes using MDGModifier
		kDoRemove,	// Remove a set of extension attributes using MDGModifier
		kDoListExtensions // list only extension attributes
	};   

private: 
	MDGModifier	        fCmd; 
	OperationList		fOperation; 
	MNodeClass*	        fNodeType;
	MObject 		pluginObject;
}; 

// Standard API entry and exit points. 
//
MStatus initializePlugin( MObject obj );
MStatus uninitializePlugin( MObject obj );

//======================================================================
//
testExtensionAPI::testExtensionAPI() : fOperation(kInvalid), fNodeType(NULL)
{
}

//======================================================================
//
testExtensionAPI::~testExtensionAPI()
{
	if( fNodeType ) delete fNodeType;
}

//======================================================================
//
void *testExtensionAPI::creator( )
{
	return new testExtensionAPI; 
}

//======================================================================
//
bool testExtensionAPI::isUndoable() const
{ 
	return false; 
}

//======================================================================
//
MStatus testExtensionAPI::parseArgs( const MArgList &args )
{
	MStatus status;
	MArgDatabase argData( syntax(), args ); 

	// Default values ... 
	// 
	fOperation = kInvalid; 
	
	bool listExtensions = true;

	if( argData.isFlagSet( kListExtensions ) )
	{
		MString nodeTypeName;
		status = argData.getFlagArgument( kListExtensions, 0, nodeTypeName); 
		CHECK_MSTATUS_AND_RETURN_IT(status);
		fOperation = kDoListExtensions; 
		fNodeType = new MNodeClass(nodeTypeName);
	}
	else if ( argData.isFlagSet( kCreateExtensions ) )
	{ 
		MString nodeTypeName;
		status = argData.getFlagArgument( kCreateExtensions, 0, nodeTypeName ); 
		CHECK_MSTATUS_AND_RETURN_IT(status);
		fOperation =  kDoCreate; 
		fNodeType = new MNodeClass(nodeTypeName);
	}
	else if ( argData.isFlagSet( kRemoveExtensions ) )
	{ 
		MString nodeTypeName;
		status = argData.getFlagArgument( kRemoveExtensions, 0, nodeTypeName ); 
		CHECK_MSTATUS_AND_RETURN_IT(status);
		fOperation = kDoRemove; 
		fNodeType = new MNodeClass(nodeTypeName);
	}
	else
	{
		MGlobal::displayError("You must specify at least one flag when using this command.");
		status = MS::kFailure;
	}

	return status; 
} 
		
//======================================================================
//
MSyntax testExtensionAPI::newSyntax( )
{
	MSyntax syntax; 
	
	// begin-syntax
	syntax.addFlag( kCreateExtensions, kCreateExtensionsLong, MSyntax::kString ); 
	syntax.addFlag( kRemoveExtensions, kRemoveExtensionsLong, MSyntax::kString ); 
	syntax.addFlag( kListExtensions, kListExtensionsLong, MSyntax::kString ); 
	// end-syntax 

	return syntax; 
}

//======================================================================
//
MStatus testExtensionAPI::doIt( const MArgList& args )
{
	MStatus status; 
	
	if( !parseArgs( args ) )
	{ 
		status = MS::kFailure; 
	}
	else if( ! fNodeType || (fNodeType->typeId() == MTypeId(MFn::kInvalid)) )
	{
		MGlobal::displayError( "Invalid node type specified" ); 
		status = MS::kFailure; 
	}
	else
	{ 
		switch( fOperation )
		{
			case kDoCreate:
			{
				// Use MDGModifier to add a stock set of extensions to the node type
				MFnCompoundAttribute compAttr;
				MFnNumericAttribute numAttr1;
				MFnNumericAttribute numAttr2;
				MObject extensionChild1 = numAttr1.create( kChild1NameLong, kChild1NameShort, MFnNumericData::kFloat, 0, &status );
				CHECK_MSTATUS(status);
				MObject extensionChild2 = numAttr2.create( kChild2NameLong, kChild2NameShort, MFnNumericData::kFloat, 0, &status );
				CHECK_MSTATUS(status);
				MObject extensionParent = compAttr.create( kParentNameLong, kParentNameShort, &status );
				CHECK_MSTATUS(status);
				compAttr.addChild( extensionChild1 );
				compAttr.addChild( extensionChild2 );
				status = fCmd.addExtensionAttribute( *fNodeType, extensionParent );
				CHECK_MSTATUS(status);
				status = fCmd.linkExtensionAttributeToPlugin(pluginObject, extensionParent);
				CHECK_MSTATUS(status);
			}
			break;

			case kDoRemove:
			{
				// Use MDGModifier to remove the already added extension
				MObject parentAttr = fNodeType->attribute( kParentNameLong, &status );
				CHECK_MSTATUS(status);
				status = fCmd.removeExtensionAttribute( *fNodeType, parentAttr );
				CHECK_MSTATUS(status);
				status = fCmd.unlinkExtensionAttributeFromPlugin(pluginObject, parentAttr);
				CHECK_MSTATUS(status);
			}
			break;

                        case kDoListExtensions:
			{
				MObjectArray allAttrs;
				status = fNodeType->getAttributes(allAttrs);
				CHECK_MSTATUS(status);
				int attrCount = fNodeType->attributeCount();
				for (int i=0; i < attrCount; i++)
				{
					MFnAttribute attr;
					attr.setObject(allAttrs[i]);
					bool isExtension = attr.isExtension();
					if ( isExtension )
					cout << "Attribute is extension, name is <" << attr.name() << ">"<< endl;
				}
					
			}
			break;

			default:
			{
				MGlobal::displayError( "Invalid command operation" ); 
				status = MS::kFailure; 
				break;
			}
		}

		status = redoIt();
	}

	return status; 
}

//======================================================================
//
MStatus testExtensionAPI::undoIt() 
// 
// Description: 
//  Adding and removing extensions is not undoable so don't try
// 
{
	return MS::kFailure;
}

//======================================================================
//
MStatus testExtensionAPI::redoIt() 
// 
// Description: 
//  Do the commands in the MDGModifier
// 
{
	MStatus status = fCmd.doIt(); 
	CHECK_MSTATUS(status);
	return status; 
}

//======================================================================
//
MStatus initializePlugin( MObject obj )
// 
// Description: 
//  Start-up point. 
//
{
	MStatus status;
	
	MFnPlugin plugin( obj, VENDOR_TAG, PLUGIN_VERSION, "Any" ); 

	status = plugin.registerCommand( MEL_COMMAND_NAME, 
									 testExtensionAPI::creator, 
									 testExtensionAPI::newSyntax ); 

	testExtensionAPI::addExtensionToPlugin(obj);

	return status; 
}

//======================================================================
//
MStatus uninitializePlugin( MObject obj )
//
// Description: 
//  Called just before the DSO is unloaded. 
//
{
	MFnPlugin plugin( obj ); 
	
	MStatus status; 
	status = plugin.deregisterCommand( MEL_COMMAND_NAME ); 

	return status; 
}

