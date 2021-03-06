/*
Copyright (C) 2010-2011 Tom Schoonjans, Philip Brondeel and Laszlo Vincze

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "xmi_xslt.h"
#include "xmi_xml.h"
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxml/xmlmemory.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/catalog.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>
#include <glib.h>

#ifdef G_OS_WIN32
#include "xmi_registry_win.h"
#elif defined(MAC_INTEGRATION)
#include "xmi_resources_mac.h"
#endif

extern int xmlLoadExtDtdDefaultValue;

static void update_xpath_nodes(xmlNodeSetPtr nodes, const xmlChar* value) {
	int size;
	int i;
	   
	size = (nodes) ? nodes->nodeNr : 0;
	for(i = size - 1; i >= 0; i--) {
		xmlNodeSetContent(nodes->nodeTab[i], value);

		if (nodes->nodeTab[i]->type != XML_NAMESPACE_DECL)
	    		nodes->nodeTab[i] = NULL;
	}
}		


int xmi_xmso_to_xmsi_xslt(char *xmsofile, char *xmsifile , char *outputfile  ) {

	xsltStylesheetPtr cur = NULL;
	xmlDocPtr doc, res;
	const char *params[1] = {NULL};
	xmlXPathContextPtr xpathCtx; 
	xmlXPathObjectPtr xpathObj;

#ifdef G_OS_WIN32
	xmlChar *xsltfile;

	if (xmi_registry_win_query(XMI_REGISTRY_WIN_XMSO2XMSI,(char **) &xsltfile) == 0)
		return 0;
#elif defined(MAC_INTEGRATION)
	xmlChar *xsltfile;

	if (xmi_resources_mac_query(XMI_RESOURCES_MAC_XMSO2XMSI,(char **) &xsltfile) == 0)
		return 0;
#else
	const xmlChar xsltfile[] = XMI_XMSO2XMSI_XSLT;
#endif

	xsltInit();

	xmlSubstituteEntitiesDefault(1);
	xmlLoadExtDtdDefaultValue = 1;

	cur = xsltParseStylesheetFile(xsltfile);
	if (cur == NULL)
		return 0;
#if defined(G_OS_WIN32) || defined (MAC_INTEGRATION)
	free(xsltfile);
#endif

	doc = xmlParseFile(xmsofile);
	if (doc == NULL)
		return 0;


	res = xsltApplyStylesheet(cur, doc, params);
	if (res == NULL)
		return 0;

	if (outputfile != NULL) {
    		xpathCtx = xmlXPathNewContext(res);
        	if(xpathCtx == NULL) {
	        	fprintf(stderr,"Error: unable to create new XPath context\n");
		        return 0;
		}
		xpathObj = xmlXPathEvalExpression((const xmlChar *) "/xmimsim/general/outputfile", xpathCtx);
		    if(xpathObj == NULL) {
		        fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n","xmimsim/general/outputfile" );
			xmlXPathFreeContext(xpathCtx); 
			return 0;
		}
		update_xpath_nodes(xpathObj->nodesetval, (const xmlChar *) outputfile);
		xmlXPathFreeObject(xpathObj);
	        xmlXPathFreeContext(xpathCtx);
	}


	xsltSaveResultToFilename(xmsifile, res, cur, 0);

	xsltFreeStylesheet(cur);
	xmlFreeDoc(res);
	xmlFreeDoc(doc);

        xsltCleanupGlobals();

	return 1;

}


int xmi_xmso_to_svg_xslt(char *xmsofile, char *xmsifile, unsigned convoluted) {

	xsltStylesheetPtr cur = NULL;
	xmlDocPtr doc, res;
	const char *params[3];
	char catalog[] = XMI_CATALOG;
	xmlXPathContextPtr xpathCtx; 
	xmlXPathObjectPtr xpathObj;

	char parm_name[] = "type1";
        char s_convoluted[] = "'convoluted'";
        char s_unconvoluted[] = "'unconvoluted'";


#ifdef G_OS_WIN32
	xmlChar *xsltfile;

	if (xmi_registry_win_query(XMI_REGISTRY_WIN_XMSO2SVG,(char **) &xsltfile) == 0)
		return 0;
#elif defined(MAC_INTEGRATION)
	xmlChar *xsltfile;

	if (xmi_resources_mac_query(XMI_RESOURCES_MAC_XMSO2SVG,(char **) &xsltfile) == 0)
		return 0;
#else
	const xmlChar xsltfile[] = XMI_XMSO2SVG_XSLT;
#endif


	xsltInit();

	params[0] = parm_name;
        if ( convoluted )
         params[1] = s_convoluted;
        else
         params[1] = s_unconvoluted;
        params[2] = NULL;

       	//fprintf(stdout, "parm 0 = %s \n", params[0] ); 
	//fprintf(stdout, "parm 1 = %s \n", params[1] );
	//fprintf(stdout, "parm 2 = %s \n", params[2] );  

	xmlSubstituteEntitiesDefault(1);
	xmlLoadExtDtdDefaultValue = 1;

	cur = xsltParseStylesheetFile(xsltfile);
	if (cur == NULL)
		return 0;

#if defined(G_OS_WIN32) || defined (MAC_INTEGRATION)
	free(xsltfile);
#endif


	doc = xmlParseFile(xmsofile);
	if (doc == NULL)
		return 0;       

	res = xsltApplyStylesheet(cur, doc, params);
	if (res == NULL)
		return 0;

	xsltSaveResultToFilename(xmsifile, res, cur, 0);

	xsltFreeStylesheet(cur);
	xmlFreeDoc(res);
	xmlFreeDoc(doc);

        xsltCleanupGlobals();

	return 1;

}

int xmi_xmso_to_spe_xslt(char *xmsofile, char *spefile, unsigned convoluted, int interaction_number) {

	xsltStylesheetPtr cur = NULL;
	xmlDocPtr doc, res;
	const char *params[5];
	char catalog[] = XMI_CATALOG;
	xmlXPathContextPtr xpathCtx; 
	xmlXPathObjectPtr xpathObj;

	char parm_name1[] = "type";
	char parm_name2[] = "interaction";
        char s_convoluted[] = "'spectrum_conv'";
        char s_unconvoluted[] = "'spectrum_unconv'";
	char interaction[10];


#ifdef G_OS_WIN32
	xmlChar *xsltfile;

	if (xmi_registry_win_query(XMI_REGISTRY_WIN_XMSO2SPE,(char **) &xsltfile) == 0)
		return 0;
#elif defined(MAC_INTEGRATION)
	xmlChar *xsltfile;

	if (xmi_resources_mac_query(XMI_RESOURCES_MAC_XMSO2SPE,(char **) &xsltfile) == 0)
		return 0;
#else
	const xmlChar xsltfile[] = XMI_XMSO2SPE_XSLT;
#endif


	xsltInit();

	params[0] = parm_name1;
        if ( convoluted )
         params[1] = s_convoluted;
        else
         params[1] = s_unconvoluted;
	params[2] = parm_name2;
	sprintf(interaction,"'%i'",interaction_number);
	params[3] = interaction;
        params[4] = NULL;

       	//fprintf(stdout, "parm 0 = %s \n", params[0] ); 
	//fprintf(stdout, "parm 1 = %s \n", params[1] );
	//fprintf(stdout, "parm 2 = %s \n", params[2] );  

	xmlSubstituteEntitiesDefault(1);
	xmlLoadExtDtdDefaultValue = 1;

	cur = xsltParseStylesheetFile(xsltfile);
	if (cur == NULL)
		return 0;

#if defined(G_OS_WIN32) || defined (MAC_INTEGRATION)
	free(xsltfile);
#endif


	doc = xmlParseFile(xmsofile);
	if (doc == NULL)
		return 0;       

	res = xsltApplyStylesheet(cur, doc, params);
	if (res == NULL)
		return 0;

	xsltSaveResultToFilename(spefile, res, cur, 0);

	xsltFreeStylesheet(cur);
	xmlFreeDoc(res);
	xmlFreeDoc(doc);

        xsltCleanupGlobals();

	return 1;

}

int xmi_xmso_to_csv_xslt(char *xmsofile, char *csvfile, unsigned convoluted) {


	xsltStylesheetPtr cur = NULL;
	xmlDocPtr doc, res;
	const char *params[3];
	char catalog[] = XMI_CATALOG;
	xmlXPathContextPtr xpathCtx; 
	xmlXPathObjectPtr xpathObj;

	char parm_name[] = "type";
        char s_convoluted[] = "'spectrum_conv'";
        char s_unconvoluted[] = "'spectrum_unconv'";


#ifdef G_OS_WIN32
	xmlChar *xsltfile;

	if (xmi_registry_win_query(XMI_REGISTRY_WIN_XMSO2CSV,(char **) &xsltfile) == 0)
		return 0;
#elif defined(MAC_INTEGRATION)
	xmlChar *xsltfile;

	if (xmi_resources_mac_query(XMI_RESOURCES_MAC_XMSO2CSV,(char **) &xsltfile) == 0)
		return 0;
#else
	const xmlChar xsltfile[] = XMI_XMSO2CSV_XSLT;
#endif


	xsltInit();

	params[0] = parm_name;
        if ( convoluted )
         params[1] = s_convoluted;
        else
         params[1] = s_unconvoluted;
        params[2] = NULL;

       	//fprintf(stdout, "parm 0 = %s \n", params[0] ); 
	//fprintf(stdout, "parm 1 = %s \n", params[1] );
	//fprintf(stdout, "parm 2 = %s \n", params[2] );  

	xmlSubstituteEntitiesDefault(1);
	xmlLoadExtDtdDefaultValue = 1;

	cur = xsltParseStylesheetFile(xsltfile);
	if (cur == NULL)
		return 0;

#if defined(G_OS_WIN32) || defined (MAC_INTEGRATION)
	free(xsltfile);
#endif


	doc = xmlParseFile(xmsofile);
	if (doc == NULL)
		return 0;       

	res = xsltApplyStylesheet(cur, doc, params);
	if (res == NULL)
		return 0;

	xsltSaveResultToFilename(csvfile, res, cur, 0);

	xsltFreeStylesheet(cur);
	xmlFreeDoc(res);
	xmlFreeDoc(doc);

        xsltCleanupGlobals();

	return 1;

}

int xmi_xmso_to_htm_xslt(char *xmsofile, char *xmsifile, unsigned convoluted) {

	xsltStylesheetPtr cur = NULL;
	xmlDocPtr doc, res;
	const char *params[3];
	char catalog[] = XMI_CATALOG;
	xmlXPathContextPtr xpathCtx; 
	xmlXPathObjectPtr xpathObj;

	char parm_name[] = "type1";
        char s_convoluted[] = "'convoluted'";
        char s_unconvoluted[] = "'unconvoluted'";

#ifdef G_OS_WIN32
	xmlChar *xsltfile;

	if (xmi_registry_win_query(XMI_REGISTRY_WIN_XMSO2HTM,(char **) &xsltfile) == 0)
		return 0;
#elif defined(MAC_INTEGRATION)
	xmlChar *xsltfile;

	if (xmi_resources_mac_query(XMI_RESOURCES_MAC_XMSO2HTM,(char **) &xsltfile) == 0)
		return 0;
#else
	const xmlChar xsltfile[] = XMI_XMSO2HTM_XSLT;
#endif



	xsltInit();

	params[0] = parm_name;
        if ( convoluted )
         params[1] = s_convoluted;
        else
         params[1] = s_unconvoluted;
        params[2] = NULL;

       	//fprintf(stdout, "parm 0 = %s \n", params[0] ); 
	//fprintf(stdout, "parm 1 = %s \n", params[1] );
	//fprintf(stdout, "parm 2 = %s \n", params[2] );  

	xmlSubstituteEntitiesDefault(1);
	xmlLoadExtDtdDefaultValue = 1;

	cur = xsltParseStylesheetFile(xsltfile);
	if (cur == NULL)
		return 0;

#if defined(G_OS_WIN32) || defined (MAC_INTEGRATION)
	free(xsltfile);
#endif


	doc = xmlParseFile(xmsofile);
	if (doc == NULL)
		return 0;       

	res = xsltApplyStylesheet(cur, doc, params);
	if (res == NULL)
		return 0;

	xsltSaveResultToFilename(xmsifile, res, cur, 0);

	xsltFreeStylesheet(cur);
	xmlFreeDoc(res);
	xmlFreeDoc(doc);

        xsltCleanupGlobals();

	return 1;

}
