========================================================================
    Static Library : netserverLib Project
========================================================================

1. This library using Platform SDK
   Must Install Platform SDK
   Visit and install http://www.microsoft.com/msdownload/platformsdk/sdkupdate/
   
2. This library using safe string function 
   If you already install Platform SDK, you can find it

   #include "strsafe.h" // Safe string function

3. DirectX head files and lib files need.
   Must set path of DirectX SDK, include path and lib path
   
4. Some library must link with netserverLib.
   If you can't auto linking them, you  must add some library yourself.

   // Auto Link Library Files
   #pragma comment (lib,"ws2_32.lib")
   #pragma comment (lib,"Winmm.lib")
   #pragma comment (lib,"strsafe.lib")
   #pragma comment (lib,"ntwdblib.lib")
   
5. Database library.
   netserverLib using ODBC library and SQL Server native library.
   ntwdblib.lib and odbc32.lib
   ntwdblib and it's head files will be find Microsoft SQL Server Install CD
   Must set path it's head file path and lib file path
   
   Link with Odbcinst.lib for 16-bit applications.
   Link with Odbccp32.lib for 32-bit applications. 

   #include <sqlfront.h> // SQL server native library
   #include <sqldb.h>    // SQL server native library

   #include <sqlext.h> // ODBC library
   
   #pragma comment (lib,"ntwdblib.lib") // SQL Server native library
   #pragma comment (lib,"Odbccp32.lib") // ODBC library
   
6. Database Setting.

   SQL Server packet size setting.
   
   Use T-SQL Query
   
   EXEC sp_configure 'network packet size', 8192
   RECONFIGURE WITH OVERRIDE
   GO

--
If you have any question, plz contact via email.

email : maincoms@mincoms.co.kr 
email : jgkim@mincoms.co.kr
MSN   : jgkim999@hotmail.com

Thanks.