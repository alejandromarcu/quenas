<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:template match="/">

  <html>
  <body>
    <center>
      <h2>Test Application</h2>
            <table border="1" cellspacing="0">    
                <tr bgcolor="#808080">
                  <th>Source</th>
                  <th>Destination</th>
                  <th>Time Sent</th>
                  <th>Time Received</th>
                  <th>Elapsed Time</th>
                </tr>

      <xsl:for-each select="simulation/Notification">
          <xsl:if test="@id='node.testApplication.received'">
              <tr>
                <td><xsl:value-of select="Data/source" /></td>
                <td><xsl:value-of select="Data/destination" /></td>
                <td><xsl:value-of select="Data/sentTime" /></td>
                <td><xsl:value-of select="Data/receivedTime" /></td>
                <td><xsl:value-of select="Data/elapsedTime" /></td>
              </tr>

          </xsl:if>
      </xsl:for-each>
    </table>

   </center>
    
    
  </body>
  </html>
</xsl:template>
</xsl:stylesheet>