
#include "jdb/TXmlConfig.h"
using namespace jdb;

void configTest( string fname = "cfg.xml" ){
	

	TXmlConfig * cfg = new TXmlConfig( fname );

	// get an attribute
	cout << cfg->getString( "test.sub[ 1 ]:a3" ) << endl;

	// get the value in the node
	cout << cfg->getString( "test.sub[ 0 ]" ) << endl;

	// leave off the [] to get the first one
	cout << cfg->getString( "test.sub" ) << endl;

	// get the paths of all children of a given node
	// optionally filter on type of child node
	vector<string> vec = cfg->childPaths( "test", "red" );
	for ( int i = 0; i < vec.size(); i++ )
		cout << vec[ i ] << endl;

	// white space in index [] dont matter
	cout << "Get an array of strings " << endl;
	vector<string> vec2 = cfg->stringVector( "test.sub[ 2    ]" );
	for ( int i = 0; i < vec2.size(); i++ )
		cout << vec2[ i ] << endl;

	cout << "now as doubles " << endl;
	vector<double> vec3 = cfg->doubleVector( "test.sub[ 2    ]" );
	for ( int i = 0; i < vec3.size(); i++ )
		cout << vec3[ i ] << endl;

	cout << "now as integers " << endl;
	vector<int> vec4 = cfg->intVector( "test.sub[ 2    ]" );
	for ( int i = 0; i < vec4.size(); i++ )
		cout << vec4[ i ] << endl;

	cout << "bools can be parsed from <node>true</node> : " << cfg->getBool( "test.red[2]" ) << endl;
	cout << "Case doesnt matter : bools can be parsed from <node>trUE</node>" << endl;
	cout << "also from attributes <node  use=\"FalSE\"/>" << endl;
	cout << "<red false=\"FalSE\" true=\"TRuE\"></red> = " << cfg->getBool( "test.red[3]:false" ) << " is false" << endl;
	cout << cfg->getBool( "test.red[3]:true" ) << " is true" << endl;

}