/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



import java.io.*;

import com.sun.star.comp.helper.RegistryServiceFactory;
import com.sun.star.comp.servicemanager.ServiceManager;
import com.sun.star.lang.XMultiComponentFactory;
import com.sun.star.lang.XServiceInfo;
import com.sun.star.lang.XComponent;
import com.sun.star.bridge.XUnoUrlResolver;
import com.sun.star.uno.UnoRuntime;
import com.sun.star.uno.XComponentContext;
import com.sun.star.beans.XPropertySet;
import com.sun.star.container.XNameAccess;
import com.sun.star.sdbc.*;
import com.sun.star.sdbcx.Privilege;
import com.sun.star.sdb.CommandType;
import com.sun.star.sdb.XRowSetApproveBroadcaster;
    
public class RowSet
{
    private static XComponentContext xContext = null;
    private static XMultiComponentFactory xMCF = null;
	public static void main(String argv[]) throws java.lang.Exception
	{
        try {
            // get the remote office component context
            xContext = com.sun.star.comp.helper.Bootstrap.bootstrap();
            System.out.println("Connected to a running office ...");
            xMCF = xContext.getServiceManager();
        }
        catch( Exception e) {
            System.err.println("ERROR: can't get a component context from a running office ...");
            e.printStackTrace(System.out);
            System.exit(1);
        }

        try{
			showRowSetEvents();
			showRowSetRowCount();
			showRowSetPrivileges();
			useRowSet();
		}
		catch(com.sun.star.uno.Exception e)
		{
			System.err.println(e);
			e.printStackTrace();
		}
		System.exit(0);
	}

	public static void printDataSources() throws com.sun.star.uno.Exception
	{
		// create a DatabaseContext and print all DataSource names
		XNameAccess xNameAccess = (XNameAccess)UnoRuntime.queryInterface(
            XNameAccess.class,
            xMCF.createInstanceWithContext("com.sun.star.sdb.DatabaseContext",
                                           xContext));
		String aNames [] = xNameAccess.getElementNames();
		for(int i=0;i<aNames.length;++i)
			System.out.println(aNames[i]);
	}

	public static void useRowSet() throws com.sun.star.uno.Exception
	{
		// first we create our RowSet object
		XRowSet xRowRes = (XRowSet)UnoRuntime.queryInterface(
            XRowSet.class,
            xMCF.createInstanceWithContext("com.sun.star.sdb.RowSet", xContext));

		System.out.println("RowSet created!");
		// set the properties needed to connect to a database
		XPropertySet xProp = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class,xRowRes);
		xProp.setPropertyValue("DataSourceName","Bibliography");
		xProp.setPropertyValue("Command","biblio");
		xProp.setPropertyValue("CommandType",new Integer(com.sun.star.sdb.CommandType.TABLE));

		xRowRes.execute();
		System.out.println("RowSet executed!");


		XComponent xComp = (XComponent)UnoRuntime.queryInterface(XComponent.class,xRowRes);
		xComp.dispose();
		System.out.println("RowSet destroyed!");
	}

	public static void showRowSetPrivileges() throws com.sun.star.uno.Exception
	{
		// first we create our RowSet object
		XRowSet xRowRes = (XRowSet)UnoRuntime.queryInterface(
            XRowSet.class,
            xMCF.createInstanceWithContext("com.sun.star.sdb.RowSet", xContext));

		System.out.println("RowSet created!");
		// set the properties needed to connect to a database
		XPropertySet xProp = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class,xRowRes);
		xProp.setPropertyValue("DataSourceName","Bibliography");
		xProp.setPropertyValue("Command","biblio");
		xProp.setPropertyValue("CommandType",new Integer(com.sun.star.sdb.CommandType.TABLE));

		xRowRes.execute();
		System.out.println("RowSet executed!");

		Integer aPriv = (Integer)xProp.getPropertyValue("Privileges");
		int nPriv  = aPriv.intValue();
		if( (nPriv & Privilege.SELECT) == Privilege.SELECT)
			System.out.println("SELECT");
		if( (nPriv & Privilege.INSERT) == Privilege.INSERT)
			System.out.println("INSERT");
		if( (nPriv & Privilege.UPDATE) == Privilege.UPDATE)
			System.out.println("UPDATE");
		if( (nPriv & Privilege.DELETE) == Privilege.DELETE)
			System.out.println("DELETE");

		// now destroy the RowSet
		XComponent xComp = (XComponent)UnoRuntime.queryInterface(XComponent.class,xRowRes);
		xComp.dispose();
		System.out.println("RowSet destroyed!");
	}

	public static void showRowSetRowCount() throws com.sun.star.uno.Exception
	{
		// first we create our RowSet object
		XRowSet xRowRes = (XRowSet)UnoRuntime.queryInterface(
            XRowSet.class,
            xMCF.createInstanceWithContext("com.sun.star.sdb.RowSet", xContext));

		System.out.println("RowSet created!");
		// set the properties needed to connect to a database
		XPropertySet xProp = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class,xRowRes);
		xProp.setPropertyValue("DataSourceName","Bibliography");
		xProp.setPropertyValue("Command","biblio");
		xProp.setPropertyValue("CommandType",new Integer(com.sun.star.sdb.CommandType.TABLE));

		xRowRes.execute();
		System.out.println("RowSet executed!");

		// now look if the RowCount is already final
		System.out.println("The RowCount is final: " + xProp.getPropertyValue("IsRowCountFinal"));

		XResultSet xRes = (XResultSet)UnoRuntime.queryInterface(XResultSet.class,xRowRes);
		xRes.last();

		System.out.println("The RowCount is final: " + xProp.getPropertyValue("IsRowCountFinal"));
		System.out.println("There are " + xProp.getPropertyValue("RowCount") + " rows!");

		// now destroy the RowSet
		XComponent xComp = (XComponent)UnoRuntime.queryInterface(XComponent.class,xRowRes);
		xComp.dispose();
		System.out.println("RowSet destroyed!");
	}

	public static void showRowSetEvents() throws com.sun.star.uno.Exception
	{
		// first we create our RowSet object
		XRowSet xRowRes = (XRowSet)UnoRuntime.queryInterface(
            XRowSet.class,
            xMCF.createInstanceWithContext("com.sun.star.sdb.RowSet", xContext));

		System.out.println("RowSet created!");
		// add our Listener
		System.out.println("Append our Listener!");
		RowSetEventListener pRow = new RowSetEventListener();
		XRowSetApproveBroadcaster xApBroad = (XRowSetApproveBroadcaster)UnoRuntime.queryInterface(XRowSetApproveBroadcaster.class,xRowRes);
		xApBroad.addRowSetApproveListener(pRow);
		xRowRes.addRowSetListener(pRow);

		// set the properties needed to connect to a database
		XPropertySet xProp = (XPropertySet)UnoRuntime.queryInterface(XPropertySet.class,xRowRes);
		xProp.setPropertyValue("DataSourceName","Bibliography");
		xProp.setPropertyValue("Command","biblio");
		xProp.setPropertyValue("CommandType",new Integer(com.sun.star.sdb.CommandType.TABLE));

		xRowRes.execute();
		System.out.println("RowSet executed!");

		// do some movements to check if we got all notifications
		XResultSet xRes = (XResultSet)UnoRuntime.queryInterface(XResultSet.class,xRowRes);
		System.out.println("beforeFirst");
		xRes.beforeFirst(); 
		// this should lead to no notifications because 
		// we should stand before the first row at the beginning
		System.out.println("We stand before the first row: " + xRes.isBeforeFirst());

		System.out.println("next");
		xRes.next();
		System.out.println("next");
		xRes.next();
		System.out.println("last");
		xRes.last();
		System.out.println("next");
		xRes.next();
		System.out.println("We stand after the last row: " + xRes.isAfterLast());
		System.out.println("first");
		xRes.first();
		System.out.println("previous");
		xRes.previous();
		System.out.println("We stand before the first row: " + xRes.isBeforeFirst());
		System.out.println("afterLast");
		xRes.afterLast();
		System.out.println("We stand after the last row: " + xRes.isAfterLast());

		// now destroy the RowSet
		XComponent xComp = (XComponent)UnoRuntime.queryInterface(XComponent.class,xRowRes);
		xComp.dispose();
		System.out.println("RowSet destroyed!");
	}
}

