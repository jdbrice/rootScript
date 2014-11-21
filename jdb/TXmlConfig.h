#ifndef TXML_CONFIG_H
#define TXML_CONFIG_H


/*
	STD
 */
#include <string>
#include <vector>
#include <exception>
#include <sstream>
#include <map>
#include <algorithm>
#include <iomanip>
/*
	ROOT
 */
#include "TXMLAttr.h"
#include "TXMLNode.h"
#include "TDOMParser.h"
#include "TXMLDocument.h"



namespace jdb {
	using namespace std;
	cout << "Including TXmlConfig.h" << endl;
	/*jdoc{
		"class" : "TXmlConfig",
		"desc" : "A lot goes here"
	}*/
	class TXmlConfig
	{
	protected:

		//jdoc{ "name" : "string filename", "desc" : "Filename of the config file"}
		string filename;

		//jdoc{ "name" : "TXMLDocument *doc", "desc" : "TXMLDocument returned by the DOM parser"}
		TXMLDocument * doc;

		//jdoc{ "name" : "TXMLNode *rootNode", "desc" : "The XML DOM's root node"}
		TXMLNode * rootNode;

		//jdoc{ "name" : "char attrDelim", "desc" : "The delimiter used for attributes - Default is \":\""}
		char attrDelim;

		//jdoc{ "name" : "char pathDelim", "desc" : "The delimiter used for paths - Default is \".\""}
		char pathDelim;

		//jdoc{ "name" : "indexOpenDelim", "desc" : "The delimeter for index open - Default is \"[\""}
		string indexOpenDelim;
		
		//jdoc{ "name" : "indexCloseDelim", "desc" : "The delimeter for index open - Default is \"]\""}
		string indexCloseDelim;



	public:

		/*jdoc{
			"name" : "TXmlConfig( string filename )",
			"params" : [
				"filename"
			],
			"paramDesc" : [
				"The filename of the xml file to load"
			],
			"returns" : [
				
			],
			"desc" : "Loads the XML file in and parses it using the TDOMParser. Successfully parsed xml will produce a document and a root node."
		}*/
		TXmlConfig( string fname ){

			// currently set may change
			pathDelim = '.';
			attrDelim = ':';
			indexOpenDelim = "[";
			indexCloseDelim = "]";

			try {
				TDOMParser * dp = new TDOMParser();
				dp->SetValidate( false );
				dp->ParseFile( fname.c_str() );

				doc = dp->GetXMLDocument();
				rootNode = doc->GetRootNode();
			} catch( exception &e ) {
				cout << "ERROR : " << e.what() << endl;
			}

		}
		~TXmlConfig();

		/*jdoc{
			"name" : "TXMLNode * getNode( string nodePath )",
			"params" : [
				"nodePath"
			],
			"paramDesc" : [
				"Path to node in xml file. Delimeted by '.' "
			],
			"returns" : [
				"TXMLNode * to the node if found, NULL otherwise"
			],
			"desc" : ""
		}*/
		TXMLNode * getNode( string nodePath ) {

			TXMLNode * ret = NULL;
			try {

				// split into individual nodes to find
				vector<string> ntf = split( nodePath, pathDelim );

				if ( ntf.size() <= 0 )
					return NULL;

				// look for an attribute off the final node
			  	vector<string> attr = split( ntf[ ntf.size() - 1 ], attrDelim );

			  	// Remove the attribute from the final node name
			  	if ( attr.size() >= 2 ){
					ntf[ ntf.size() - 1 ] = ntf[ ntf.size() - 1 ].substr( 0, ntf[ ntf.size() - 1 ].length() - (attr[ 1].length() + 1) );
				}

				// loop through the nodes tree
				TXMLNode * cNode = rootNode;
				for ( int iNode = 0; iNode < ntf.size(); iNode ++ ){

					string nodeName = ntf[ iNode ];
					int nodeIndex = 0;

					std::size_t pos1 = nodeName.find( indexOpenDelim );
					std::size_t pos2 = nodeName.find( indexCloseDelim );
					if ( pos1 < nodeName.length() && pos2 <= nodeName.length() ){
						string index = nodeName.substr( pos1+1, pos2 - (pos1+1) );
						
						nodeIndex = atoi( index.c_str() );
						nodeName = nodeName.substr( 0, pos1 );
					}
					
					cNode = findChild( cNode, nodeName, nodeIndex );
				}
				
				ret = cNode;

			} catch ( exception &e ){
				cout << " Error : " << e.what() << endl;
			}

			
			return ret;
		
		}

		/*jdoc{
			"name" : "TXMLAttr * getAttribute( string nodePath )",
			"params" : [
				"nodePath"
			],
			"paramDesc" : [
				"Path to the node of interest"
			],
			"returns" : [
				"NULL - Specified attribute was found",
				"TXMLAttr * to the attribute if found"
			],
			"desc" : "Since Root uses separate classes for encapsulating nodes and attributes, this method provides lookup of paths with attribute delimeters included. "
		}*/
		TXMLAttr * getAttribute( string nodePath ){
			TXMLNode * node = getNode( nodePath );
			if ( node ){
				vector<string> attrs = split( nodePath, attrDelim );
				if ( 2 == attrs.size()  ){
					return attributeByName( node, attrs[ 1 ] );
				}
			}
			return NULL;
		}

		
		string getString( string nodePath, string def = "" ){
			
			vector<string> attrs = split( nodePath, attrDelim );
			if ( 2 == attrs.size()  ){
				TXMLAttr * a = getAttribute( nodePath );
				if ( a && a->GetValue())
					return  trim( std::string(a->GetValue()) );
			}
			
			TXMLNode * n = getNode( nodePath );
			if ( n && n->GetText())
				return trim( std::string(n->GetText()) );

			return def;
		}
		
		vector<string> stringVector( string nodePath ){
			string value = getString( nodePath );
			return vectorFromString( value );
		}

		int getInt( string nodePath, int def = 0 ){
			string str = getString( nodePath, "" );
			if ( "" != str && str.length() >= 1 )
				return atoi( str.c_str() );
			return def;
		}
		vector<int> intVector( string nodePath ){
			vector<string> vec = stringVector( nodePath );
			vector<int> d;
			for ( int i = 0; i < vec.size(); i++  ){
				d.push_back( atoi( vec[ i ].c_str() ) );
			}
			return d;
		}

		double getDouble( string nodePath, double def = 0 ){
			string str = getString( nodePath, "" );
			if ( "" != str && str.length() >= 1 )
				return atof( str.c_str() );
			return def;
		}
		vector<double> doubleVector( string nodePath ){
			vector<string> vec = stringVector( nodePath );
			vector<double> d;
			for ( int i = 0; i < vec.size(); i++  ){
				d.push_back( atof( vec[ i ].c_str() ) );
			}
			return d;
		}

		float getFloat( string nodePath, float def = 0 ){
			return (float) getDouble( nodePath, (double)def );
		}


		bool getBool( string nodePath, bool def = false ) {

			string str = getString( nodePath );

			// first check for string literal "true" or "false"
			// push to lower case
			//std::transform( str.begin(), str.end(), str.begin(), std::tolower );
			str = manualToLower( str );
			if ( str == "false" )
				return false;
			if ( str == "true" )
				return true;

			// if not look for an integer
			// 0 or negative = false
			// any positive = true
			if ( str.length() >= 1 ){
				return ( atoi( str.c_str() ) >= 1 );
			}
			return def;
		}

		string manualToLower( string str ){
			string str2 = str;
			for ( int i = 0; i < str.length(); i++ ){
				str2[ i ] = std::tolower( str[ i ] );
			}
			return str2;
		}

		vector<string> childPaths( string parentPath, string filterNodeName = "" ){
			TXMLNode * parent = getNode( parentPath );

			TXMLNode * c = parent->GetChildren();
			
			std::map< string, int > index;
			vector< string > paths;

			while ( c && c->HasNextNode() ){
				string name = c->GetNodeName();
				string nodeName = c->GetNodeName();
				if ( name == "text" || name == "comment" ){
					c = c->GetNextNode();	
					continue;
				}

				if ( !index[ nodeName ] )
					index[ nodeName ] = 0;
				if ( "" != filterNodeName && nodeName != filterNodeName ){
					c = c->GetNextNode();
					continue;
				}

				stringstream sstr;
				sstr << index[ nodeName ];
				paths.push_back(  parentPath + pathDelim + nodeName + indexOpenDelim + sstr.str() + indexCloseDelim );

				index[ nodeName ]++;
				
				c = c->GetNextNode();
			}

			//cout << " Could not find child : " << child << " in node named " << node->GetName() << endl;
			return paths;

		}


		

		bool exists( string nodePath ){
			return nodeExists( nodePath );
		}
		bool nodeExists ( string nodePath ){
			TXMLNode * n = getNode( nodePath );
			if ( NULL != n )
				return true;
			return false;
		}

		/**
		 * Helper Methods
		 */
		vector<string> split(const string &s, char delim) {
		    vector<string> elems;
		    split(s, delim, elems);
		    return elems;
		}

		std::string trim(const std::string& str, const std::string& whitespace = " \t\n") {
		    std::size_t strBegin = str.find_first_not_of(whitespace);
		    if (strBegin == std::string::npos)
		        return ""; // no content

		    std::size_t strEnd = str.find_last_not_of(whitespace);
		    std::size_t strRange = strEnd - strBegin + 1;

		    return str.substr(strBegin, strRange);
		}

	protected:

		vector<string> vectorFromString( string data ){
				
			vector<string> d = split( data, ',' );
			
			for ( int i = 0; i < d.size(); i++ ){
				d[ i ] =  trim( d[ i ] );
			}
			return d;

		}

		vector<string> &split(const string &s, char delim, vector<string> &elems) {
		    stringstream ss(s);
		    string item;
		    while (std::getline(ss, item, delim)) {
		        elems.push_back(item);
		    }
		    return elems;
		}

		TXMLNode * findChild( TXMLNode * node, string child, int index = 0 ) {
			
			TXMLNode * c = node->GetChildren();
			
			int n = 0;

			while ( c && c->HasNextNode() ){
				string name = c->GetNodeName();
				string nodeName = c->GetNodeName();
				if ( name == "text" || name == "comment" ){
					c = c->GetNextNode();	
					continue;
				}
				if ( nodeName == child ){
					if ( n == index )
						return c;
					n++;
				}
				
				c = c->GetNextNode();
			}

			//cout << " Could not find child : " << child << " in node named " << node->GetName() << endl;
			return NULL;
		}

		TXMLAttr * attributeByName( TXMLNode * node, string name  ){

			if ( node && node->HasAttributes() ){
				TIter i( node->GetAttributes() );
				
				TXMLAttr * cAttr;

				while ( cAttr = (TXMLAttr*)i.Next() ){
					if ( cAttr->GetName() == name  ){
						return cAttr;
					}
				}
			}

			return NULL;

		}
		
	};
}





#endif